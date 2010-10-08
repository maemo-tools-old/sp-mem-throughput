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

#include "blocks.h"
#include "routine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *
csv_get_default_name(void)
{
	FILE *q;
	char *p, *device_sw_release_ver, *component_product, *csv_filename;
	size_t n;
	unsigned year,week,build;
	csv_filename = NULL;
	device_sw_release_ver = NULL;
	component_product = NULL;
	q = popen("sysinfoclient -g /device/sw-release-ver 2>/dev/null", "r");
	if (!q) {
		q = popen("sysinfo-tool -g /device/sw-release-ver 2>/dev/null",
				"r");
		if (!q) goto out;
	}
	if (getline(&device_sw_release_ver, &n, q) < 0) goto out;
	pclose(q);
	q = popen("sysinfoclient -g /component/product 2>/dev/null", "r");
	if (!q) {
		q = popen("sysinfo-tool -g /component/product 2>/dev/null",
				"r");
		if (!q) goto out;
	}
	if (getline(&component_product, &n, q) < 0) goto out;
	pclose(q);
	p = strchr(device_sw_release_ver, '.');
	if (!p) goto out;
	if (sscanf(p, ".%u.%u-%u", &year, &week, &build) != 3) goto out;
	p = strchr(component_product, '=');
	if (p) {
		if (*p++ == 0 || *p++ == 0 || *p == 0) goto out;
		component_product = p;
	}
	n = strlen(component_product);
	if (component_product[n-1] == '\n')
		component_product[n-1] = 0;
	if (asprintf(&csv_filename, "sp-mem-throughput-%s-%u-w%u-%u.csv",
			component_product, year, week, build) < 0) {
		csv_filename = NULL;
	}
out:
	free(device_sw_release_ver);
	return csv_filename;
}

int
output_csv(const char *csv_filename,
           struct routine **routines,
           size_t routines_cnt,
           unsigned repeats,
           const char *argv,
           const char *banner)
{
	int fd, ret=-1;
	unsigned i, j, k, b;
	FILE *fp;
	fp = NULL;
	fprintf(stderr, "\n");
	if (!csv_filename) {
		fprintf(stderr, "WARNING: CSV output creation failed"
				" -- internal error.\n");
		goto error;
	}
	fp = fopen(csv_filename, "w");
	if (!fp) {
		fprintf(stderr, "WARNING: CSV output creation failed: "
				"unable to open '%s' for writing.\n",
				csv_filename);
		goto error;
	}
	fprintf(stderr, "Writing results in CSV format: %s\n", csv_filename);
	if (banner) fprintf(fp, "# %s\n", banner);
	if (argv) fprintf(fp, "# Command line: %s\n", argv);
	fprintf(fp,
		"Category,"
		"Function,"
		"Block_size_bytes,"
		"Throughput_megabytes_per_second,"
		"Calls,"
		"Duration_microsecs\n");
	for (i=0; i < routines_cnt; ++i) {
		if (!routines[i]->flagged) continue;
		block_size_iter_reset();
		for (j=0; j < block_sizes_cnt; ++j) {
			b = block_size_iter_next();
			for (k=0; k < repeats; ++k) {
				if (fprintf(fp, "%s,%s,%u,%f,%u,%llu\n",
				    rtype_name(routines[i]->type),
				    routines[i]->name,
				    b,
				    m_thr(&routines[i]->measurements[j*repeats+k], b),
				    routines[i]->measurements[j*repeats+k].calls,
				    routines[i]->measurements[j*repeats+k].duration_us) < 0) {
					goto output_done;
				}
			}
		}
	}
	ret = 0;
output_done:
	if (ret == -1) {
		fprintf(stderr,
			"WARNING: CSV result writing failure, aborted.\n");
		/* continue anyway */
	}
	if (fflush(fp)) {
		fprintf(stderr, "WARNING: fflush() failure at %s(): '%m'.\n",
				__func__);
		/* continue anyway */
	}
	fd = fileno(fp);
	if (fd != -1) fdatasync(fd); /* ignore errors */
	if (fclose(fp)) {
		fprintf(stderr, "WARNING: fclose() failure at %s(): '%m'.\n",
				__func__);
		/* continue anyway */
	}
error:
	return ret;
}
