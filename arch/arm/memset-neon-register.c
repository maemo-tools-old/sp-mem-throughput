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

void *memset_vstm_8(void *, int, size_t);
void *memset_vstm_16(void *, int, size_t);
void *memset_vstm_32(void *, int, size_t);
void *memset_vstm_64(void *, int, size_t);
void *memset_vstm_128(void *, int, size_t);
void *memset_vstm_256(void *, int, size_t);

void *memset_vstm_pld_8(void *, int, size_t);
void *memset_vstm_pld_16(void *, int, size_t);
void *memset_vstm_pld_32(void *, int, size_t);
void *memset_vstm_pld_64(void *, int, size_t);
void *memset_vstm_pld_128(void *, int, size_t);
void *memset_vstm_pld_256(void *, int, size_t);

void *memset_vst1_8(void *, int, size_t);
void *memset_vst1_16(void *, int, size_t);
void *memset_vst1_32(void *, int, size_t);
void *memset_vst1_64(void *, int, size_t);
void *memset_vst1_128(void *, int, size_t);
void *memset_vst1_256(void *, int, size_t);

void *memset_vst1_pld_8(void *, int, size_t);
void *memset_vst1_pld_16(void *, int, size_t);
void *memset_vst1_pld_32(void *, int, size_t);
void *memset_vst1_pld_64(void *, int, size_t);
void *memset_vst1_pld_128(void *, int, size_t);
void *memset_vst1_pld_256(void *, int, size_t);

ROUTINE_REGISTER_MEMSET(memset_vstm_8,   "NEON VSTM, 8 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vstm_16,  "NEON VSTM, 16 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vstm_32,  "NEON VSTM, 32 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vstm_64,  "NEON VSTM, 64 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vstm_128, "NEON VSTM, 128 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vstm_256, "NEON VSTM, 256 bytes per loop")

ROUTINE_REGISTER_MEMSET(memset_vstm_pld_8,   "NEON VSTM+PLD, 8 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vstm_pld_16,  "NEON VSTM+PLD, 16 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vstm_pld_32,  "NEON VSTM+PLD, 32 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vstm_pld_64,  "NEON VSTM+PLD, 64 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vstm_pld_128, "NEON VSTM+PLD, 128 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vstm_pld_256, "NEON VSTM+PLD, 256 bytes per loop")

ROUTINE_REGISTER_MEMSET(memset_vst1_8,   "NEON VST1, 8 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vst1_16,  "NEON VST1, 16 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vst1_32,  "NEON VST1, 32 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vst1_64,  "NEON VST1, 64 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vst1_128, "NEON VST1, 128 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vst1_256, "NEON VST1, 256 bytes per loop")

ROUTINE_REGISTER_MEMSET(memset_vst1_pld_8,   "NEON VST1+PLD, 8 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vst1_pld_16,  "NEON VST1+PLD, 16 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vst1_pld_32,  "NEON VST1+PLD, 32 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vst1_pld_64,  "NEON VST1+PLD, 64 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vst1_pld_128, "NEON VST1+PLD, 128 bytes per loop")
ROUTINE_REGISTER_MEMSET(memset_vst1_pld_256, "NEON VST1+PLD, 256 bytes per loop")

#endif /* __ARM_NEON__ */
