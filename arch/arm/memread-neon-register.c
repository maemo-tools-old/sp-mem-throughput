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

void memread_neon_16(const void *, size_t);
void memread_neon_32(const void *, size_t);
void memread_neon_64(const void *, size_t);
void memread_neon_128(const void *, size_t);

ROUTINE_REGISTER_MEMREAD(memread_neon_16,
	"ARM NEON memread, 16 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(memread_neon_32,
	"ARM NEON memread, 32 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(memread_neon_64,
	"ARM NEON memread, 64 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(memread_neon_128,
	"ARM NEON memread, 128 byte aligned reads")

#endif /* __ARM_NEON__ */
