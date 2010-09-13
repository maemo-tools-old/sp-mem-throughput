/* This file is part of sp-mem-throughput.
 *
 * Copyright (C) 2010 by Nokia Corporation
 *
 * Authors: Tommi Rantala
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <inttypes.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* We want to avoid CPU migrations during execution, because that would cause
 * cache flushes etc. and that would perturbate our benchmarking results. Try
 * to bind to first available CPU.
 */
int
bind_to_one_cpu(void)
{
	int i, got, ret=-1;
	cpu_set_t c, mask;
	if (sched_getaffinity(0, sizeof(cpu_set_t), &c) < 0) goto done;
	CPU_ZERO(&mask);
	got = 0;
	for (i=0; i < CPU_SETSIZE; ++i) {
		if (CPU_ISSET(i, &c)) {
			CPU_SET(i, &mask);
			got = 1;
			break;
		}
	}
	if (!got) goto done;
	if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) < 0) goto done;
	ret = 0;
done:
	return ret;
}

static const char *
cpu_affinity(void)
{
	static char buf[256];
	int i, pos;
	cpu_set_t c;
	if (sched_getaffinity(0, sizeof(cpu_set_t), &c) < 0) goto err;
	buf[0] = 0;
	for (i=0, pos=0; i < CPU_SETSIZE; ++i) {
		if (CPU_ISSET(i, &c)) {
			pos += sprintf(&buf[pos], ",%d", i);
		}
	}
	return &buf[1];
err:
	return NULL;
}

static char *
readfile(const char *name)
{
	FILE *fp;
	char *line = NULL;
	size_t l, line_n = 0;
	fp = fopen(name, "r");
	if (fp) {
		l = getline(&line, &line_n, fp);
		fclose(fp);
		if (l < 1) line = NULL;
		else line[l-1]=0;
	}
	return line;
}

static char *
sys_possible_cpus(void)
{
	return readfile("/sys/devices/system/cpu/possible");
}

static void
cat(unsigned cpu, const char *path, const char *indent)
{
	char *l, *p = NULL;
	if (asprintf(&p,
		"/sys/devices/system/cpu/cpu%u/%s",
		cpu, path) < 0) goto error;
	l = readfile(p);
	if (!l) goto error;
	printf("%s%s: %s\n", indent, path, l);
	free(l);
error:
	return;
}

void
print_cpu_info(FILE *stream)
{
	unsigned c;
	char *cpu_possible;
	const char *cpu_aff;
	cpu_possible = sys_possible_cpus();
	cpu_aff = cpu_affinity();
	fprintf(stream, "CPU information:\n");
	fprintf(stream, "    Possible CPUs in system: [IDs: %s]\n", cpu_possible ? : "<unknown>");
	fprintf(stream, "    Possible CPUs for this process: [IDs: %s]\n", cpu_aff ? : "<unknown>");
	if (cpu_aff && sscanf(cpu_aff, "%u", &c) == 1) {
		fprintf(stream, "    CPU%u:\n", c);
		cat(c, "cpufreq/scaling_cur_freq", "       ");
		cat(c, "cpufreq/scaling_available_frequencies", "       ");
		cat(c, "cpufreq/scaling_governor", "       ");
		cat(c, "cpufreq/scaling_available_governors", "       ");
	}
	free(cpu_possible);
}
