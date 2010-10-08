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

#ifdef __ARM_NEON__

#include "routine.h"

void *caf_memcpy(void *, const void *, size_t);
void *caf_memmove(void *, const void *, size_t);
void *caf_memset(void *, int, size_t);

ROUTINE_REGISTER_MEMCPY(caf_memcpy,
		"ARM NEON memcpy from Code Aurora Forum")
ROUTINE_REGISTER_MEMCPY(caf_memmove,
		"ARM NEON memmove from Code Aurora Forum")
ROUTINE_REGISTER_MEMSET(caf_memset,
		"ARM NEON memset from Code Aurora Forum")

#endif /* __ARM_NEON__ */
