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

#include "routine.h"
#include "blocks.h"

#include <stdio.h>

static unsigned
validate_memset_routine(struct routine *r, char *buf1)
{
	unsigned j, k, b, ret=0;
	for (j=0; j < block_sizes_cnt; ++j) {
		b = block_size_iter_next();
		for (k=0; k < b; ++k) buf1[k] = 0x11;
		r->fn.memset(buf1, 0x22, b);
		for (k=0; k < b; ++k) {
			if (buf1[k] != 0x22) {
				fprintf(stderr, "ERROR: %s() failed"
					" validation with B=%u.\n",
					r->name, b);
				fflush(stderr);
				++ret;
				break;
			}
		}
	}
	return ret;
}

static unsigned
validate_memcpy_routine(struct routine *r, char *buf1, char *buf2)
{
	unsigned j, k, b, ret=0;
	for (j=0; j < block_sizes_cnt; ++j) {
		b = block_size_iter_next();
		for (k=0; k < b; ++k) {
			buf1[k] = 0x33;
			buf2[k] = 0x44;
		}
		r->fn.memcpy(buf2, buf1, b);
		for (k=0; k < b; ++k) {
			if (buf2[k] != 0x33) {
				fprintf(stderr, "ERROR: %s() failed"
					" validation with B=%u, buf2[%u]==%#x.\n",
					r->name, b, k, buf2[k]);
				fflush(stderr);
				++ret;
				break;
			}
		}
	}
	return ret;
}

static unsigned
validate_strlen_routine(struct routine *r, char *buf1)
{
	size_t l;
	unsigned j, k, b, ret=0;
	for (j=0; j < block_sizes_cnt; ++j) {
		b = block_size_iter_next();
		for (k=0; k < b-1; ++k) {
			buf1[k] = 0x33;
		}
		buf1[b-1] = 0;
		l = r->fn.strlen(buf1);
		if (l != b-1) {
			fprintf(stderr, "ERROR: %s() failed validation"
					" [got: %zd, expected: %zd].\n",
				r->name, l, b);
			fflush(stderr);
			++ret;
			break;
		}
	}
	return ret;
}

static unsigned
validate_strcpy_routine(struct routine *r, char *buf1, char *buf2)
{
	unsigned j, b, k, ret=0;
	for (j=0; j < block_sizes_cnt; ++j) {
		b = block_size_iter_next();
		for (k=0; k < b; ++k) {
			buf1[k] = 'x';
			buf2[k] = 'y';
		}
		buf1[b-1] = 0;
		r->fn.strcpy(buf2, buf1);
		for (k=0; k < b-1; ++k) {
			if (buf2[k] != 'x') {
				fprintf(stderr,
					"ERROR: %s() failed validation"
					" [buf2[%zd]==%c].\n",
					r->name, k, buf2[k]);
				fflush(stderr);
				++ret;
				break;
			}
		}
		if (buf2[b-1] != 0) {
			fprintf(stderr,
				"ERROR: %s() failed validation"
				" [buf2[%zd]==%x].\n",
				r->name, b-1, buf2[b-1]);
			fflush(stderr);
			++ret;
			break;
		}
	}
	return ret;
}

unsigned
validate_all(struct routine **routines,
             unsigned routines_cnt,
             char *buf1, char *buf2)
{
	unsigned ret, i, failures;
	failures = 0;
	printf("Validating routines...\n");
	fflush(stdout);
	for (i=0; i < routines_cnt; ++i) {
		printf("- %-20s ...", routines[i]->name); fflush(stdout);
		ret = 0;
		block_size_iter_reset();
		if (routines[i]->type == routine_memset) {
			ret = validate_memset_routine(routines[i], buf1);
		} else if (routines[i]->type == routine_memcpy) {
			ret = validate_memcpy_routine(routines[i], buf1, buf2);
		} else if (routines[i]->type == routine_strlen) {
			ret = validate_strlen_routine(routines[i], buf1);
		} else if (routines[i]->type == routine_strcpy) {
			ret = validate_strcpy_routine(routines[i], buf1, buf2);
		} else {
			printf(" SKIPPED VALIDATION.\n"); fflush(stdout);
			continue;
		}
		if (ret == 0) {
			printf(" OK!\n"); fflush(stdout);
		}
		failures += ret;
	}
	printf("\n");
	if (failures) {
		fprintf(stderr,
			"ERROR: total %u validation failures!\n",
			failures);
	} else {
		printf("Validation complete: ALL OK!\n");
	}
	return failures;
}
