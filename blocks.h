/* This file is part of sp-mem-throughput.
 *
 * Provide a simple API for parsing and iterating a block size definition:
 *     "32,64,128,1kb,1mb" (count: 5, largest: 1mb)
 *     "1-256" (count: 256, largest: 256)
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

#ifndef BLOCKS_H
#define BLOCKS_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned block_sizes_largest;
unsigned block_sizes_cnt;
void block_size_parse(char *);
void block_size_iter_reset(void);
unsigned block_size_iter_next(void);

#ifdef __cplusplus
}
#endif

#endif /* BLOCKS_H */
