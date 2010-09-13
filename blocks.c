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

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

long long perf_atoll(const char *);

unsigned block_sizes_largest;
unsigned block_sizes_cnt;
static struct block_size {
	unsigned b;
	unsigned e;
} *block_sizes;
static struct {
	unsigned bs_idx;
	unsigned lastret;
} block_size_parser_ctx;

static int
block_size_cmp(const void *a, const void *b)
{
	const struct block_size *aa = a;
	const struct block_size *bb = b;
	if (aa->e <  bb->b) return -1;
	if (aa->e == bb->b) return 0;
	return 1;
}

void
block_size_parse(char *s)
{
	char *range;
	long long beg, end;
	unsigned i;
	size_t slen, piecelen, items=0;
	slen = strlen(s);
	if (slen > (size_t)UINT_MAX) abort();
	/* We need to pass splitted strings to atoll(), otherwise the
	 * parsing will fail. */
	for (i=0; i < (unsigned)slen; ++i) {
		if (s[i] == ',') s[i] = '\0';
	}
	for (i=0; i < (unsigned)slen;) {
		piecelen = strlen(&s[i]);
		range = strchr(&s[i], '-');
		if (range) {
			*range = '\0';
			beg = perf_atoll(&s[i]);
			end = perf_atoll(range+1);
		} else {
			beg = end = perf_atoll(&s[i]);
		}
		if (beg == -1 || end == -1 || beg > end) {
			fprintf(stderr, "ERROR: invalid block size "
				"entry '%lld' > '%lld'.\n",
				beg, end);
			exit(1);
		}
		block_sizes = realloc(block_sizes,
				(items+1)*sizeof(struct block_size));
		if (!block_sizes) {
			fprintf(stderr,
				"ERROR: realloc() failure in %s().\n",
				__func__);
			exit(1);
		}
		block_sizes[items].b = beg;
		block_sizes[items].e = end;
		++items;
		i += piecelen+1;
	}
	if (items > (size_t)UINT_MAX) abort();
	for (i=0; i < (unsigned)items; ++i)
		block_sizes_cnt += (block_sizes[i].e - block_sizes[i].b + 1);
	qsort(block_sizes, items, sizeof(struct block_size), block_size_cmp);
	block_sizes_largest = block_sizes[items-1].e;
}

void
block_size_iter_reset(void)
{
	block_size_parser_ctx.bs_idx = 0;
	block_size_parser_ctx.lastret =
		block_sizes[block_size_parser_ctx.bs_idx].b - 1;
}

unsigned
block_size_iter_next(void)
{
	unsigned r = block_size_parser_ctx.lastret;
	if (r >= block_sizes[block_size_parser_ctx.bs_idx].e) {
		block_size_parser_ctx.bs_idx++;
		r = block_sizes[block_size_parser_ctx.bs_idx].b;
		block_size_parser_ctx.lastret = r;
		return r;
	}
	block_size_parser_ctx.lastret = ++r;
	return r;
}
