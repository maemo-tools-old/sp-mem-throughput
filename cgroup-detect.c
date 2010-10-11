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

#include "cgroup-detect.h"

#include <stdio.h>
#include <stdlib.h>

char *
current_cgroup(void)
{
	FILE *fp;
	char *line = NULL;
	size_t line_n = 0;
	ssize_t ret;
	fp = fopen("/proc/self/cgroup", "r");
	if (!fp) goto error;
	ret = getline(&line, &line_n, fp);
	if (ret == -1) {
		line = NULL;
		goto error;
	}
	if (ret == 0) goto error;
	if (line[ret-1] == '\n')
		line[ret-1] = '\0';
	return line;
error:
	if (fp) fclose(fp);
	free(line);
	return NULL;
}
