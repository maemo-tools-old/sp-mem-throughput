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

#include "cpu.h"

#include <errno.h>
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

char *
process_possible_cpus(void)
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

cpu_set_t *
system_possible_cpuset(void)
{
	cpu_set_t *ret = NULL;
	char *line, *p;
	long cpu1, cpu2;
	line = readfile("/sys/devices/system/cpu/possible");
	if (!line) goto error;
	ret = CPU_ALLOC(CPU_SETSIZE);
	if (!ret) goto error;
	while (*line) {
		errno = 0;
		cpu1 = strtol(line, &p, 10);
		if (errno) goto error;
		if (*p == '-') {
			line = p+1;
			errno = 0;
			cpu2 = strtol(line, &p, 10);
			line = p;
			if (errno) goto error;
			for (; cpu1 <= cpu2; ++cpu1) {
				CPU_SET(cpu1, ret);
			}
		} else {
			line = p;
			if (*line == ',')
				++line;
			CPU_SET(cpu1, ret);
		}
	}
	return ret;
error:
	if (ret) CPU_FREE(ret);
	return NULL;
}

cpu_set_t *
process_possible_cpuset(void)
{
	cpu_set_t *ret;
	ret = CPU_ALLOC(CPU_SETSIZE);
	if (!ret) goto error;
	if (sched_getaffinity(0, sizeof(cpu_set_t), ret) < 0) goto error;
	return ret;
error:
	if (ret) CPU_FREE(ret);
	return NULL;
}

char *
system_possible_cpus(void)
{
	return readfile("/sys/devices/system/cpu/possible");
}

static char *
get_string(const char *path)
{
	FILE *fp;
	char *line;
	size_t line_n;
	line = NULL;
	line_n = 0;
	fp = fopen(path, "r");
	if (!fp) goto done;
	if (getline(&line, &line_n, fp) < 0) {
		line = NULL;
		goto done;
	}
	if (line[strlen(line)-1] == '\n')
		line[strlen(line)-1] = '\0';
done:
	if (fp) fclose(fp);
	return line;
}

static unsigned
get_unsigned(const char *path)
{
	FILE *fp;
	unsigned ret = 0;
	fp = fopen(path, "r");
	if (!fp) goto done;
	if (fscanf(fp, "%u", &ret) != 1)
		ret = 0;
	fclose(fp);
done:
	return ret;
}

static void
get_avail_freqs(unsigned cpu, struct cpu_scaling *cs)
{
	FILE *fp;
	char *path;
	unsigned cnt, val, *ret;
	fp = NULL;
	path = NULL;
	if (asprintf(&path,
		"/sys/devices/system/cpu/cpu%u/cpufreq/"
		"scaling_available_frequencies",
		cpu) < 0) goto done;
	fp = fopen(path, "r");
	if (!fp) goto done;
	cnt = 0;
	ret = NULL;
	while (fscanf(fp, "%u", &val) == 1) {
		ret = realloc(ret, (cnt+1)*sizeof(unsigned));
		if (!ret) goto done;
		ret[cnt] = val;
		++cnt;
	}
	cs->avail_freqs = ret;
	cs->avail_freqs_cnt = cnt;
done:
	if (fp) fclose(fp);
	free(path);
}

static void
get_avail_governors(unsigned cpu, struct cpu_scaling *cs)
{
	FILE *fp;
	char *p, *line, *path, **ret;
	unsigned cnt, i;
	size_t line_n;
	int add;
	fp = NULL;
	path = NULL;
	if (asprintf(&path,
		"/sys/devices/system/cpu/cpu%u/cpufreq/"
		"scaling_available_governors",
		cpu) < 0) goto done;
	fp = fopen(path, "r");
	if (!fp) goto done;
	line = NULL;
	line_n = 0;
	/* userspace ondemand performance\n\0 */
	if (getline(&line, &line_n, fp) < 0) goto done;
	if (*line == '\0') goto done;
	for (cnt=0, p=line; *p; ++p) {
		if (*p == ' ') ++cnt;
	}
	/* userspace ondemand performance\0 */
	if (line[strlen(line)-1] == '\n')
		line[strlen(line)-1] = '\0';
	ret = calloc(cnt, sizeof(char *));
	/* userspace\0ondemand\0performance\0 */
	for (i=0, add=1, p=line; *p; ++p) {
		if (add) {
			ret[i++] = p;
			add = 0;
		}
		if (*p == ' ') {
			add = 1;
			*p = '\0';
		}
	}
	cs->avail_governors = ret;
	cs->avail_governors_cnt = cnt;
done:
	if (fp) fclose(fp);
	free(path);
}

#define GETTER(field, type)\
	static void get_##field(unsigned cpu, struct cpu_scaling *cs) {\
		char *path;\
		if (asprintf(&path,\
			"/sys/devices/system/cpu/cpu%u/cpufreq/scaling_"#field, cpu) < 0) {\
				path = NULL; goto done;\
		}\
		cs->field = get_##type(path);\
		done: free(path);\
	}

GETTER(cur_freq, unsigned)
GETTER(driver, string)
GETTER(governor, string)
GETTER(max_freq, unsigned)
GETTER(min_freq, unsigned)

struct cpu_scaling *
get_cpu_scaling(int cpu)
{
	struct cpu_scaling *c;
	c = calloc(1, sizeof(struct cpu_scaling));
	if (!c) goto error;
	get_avail_freqs(cpu, c);
	get_avail_governors(cpu, c);
	get_cur_freq(cpu, c);
	get_driver(cpu, c);
	get_governor(cpu, c);
	get_max_freq(cpu, c);
	get_min_freq(cpu, c);
	return c;
error:
	return NULL;
}
