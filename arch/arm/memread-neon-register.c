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

void read_vldm_8(const void *, size_t);
void read_vldm_16(const void *, size_t);
void read_vldm_32(const void *, size_t);
void read_vldm_64(const void *, size_t);
void read_vldm_128(const void *, size_t);
void read_vldm_256(const void *, size_t);

void read_vld1_8(const void *, size_t);
void read_vld1_16(const void *, size_t);
void read_vld1_32(const void *, size_t);
void read_vld1_64(const void *, size_t);
void read_vld1_128(const void *, size_t);
void read_vld1_256(const void *, size_t);

void read_vldm_pld_8(const void *, size_t);
void read_vldm_pld_16(const void *, size_t);
void read_vldm_pld_32(const void *, size_t);
void read_vldm_pld_64(const void *, size_t);
void read_vldm_pld_128(const void *, size_t);
void read_vldm_pld_256(const void *, size_t);

void read_vld1_pld_8(const void *, size_t);
void read_vld1_pld_16(const void *, size_t);
void read_vld1_pld_32(const void *, size_t);
void read_vld1_pld_64(const void *, size_t);
void read_vld1_pld_128(const void *, size_t);
void read_vld1_pld_256(const void *, size_t);

ROUTINE_REGISTER_MEMREAD(read_vldm_8,   "NEON VLDM, 8 byte reads")
ROUTINE_REGISTER_MEMREAD(read_vldm_16,  "NEON VLDM, 16 byte reads")
ROUTINE_REGISTER_MEMREAD(read_vldm_32,  "NEON VLDM, 32 byte reads")
ROUTINE_REGISTER_MEMREAD(read_vldm_64,  "NEON VLDM, 64 byte reads")
ROUTINE_REGISTER_MEMREAD(read_vldm_128, "NEON VLDM, 128 byte reads")
ROUTINE_REGISTER_MEMREAD(read_vldm_256, "NEON VLDM, 256 byte reads")

ROUTINE_REGISTER_MEMREAD(read_vld1_8,   "NEON VLD1, 8 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(read_vld1_16,  "NEON VLD1, 16 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(read_vld1_32,  "NEON VLD1, 32 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(read_vld1_64,  "NEON VLD1, 64 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(read_vld1_128, "NEON VLD1, 128 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(read_vld1_256, "NEON VLD1, 256 byte aligned reads")

ROUTINE_REGISTER_MEMREAD(read_vldm_pld_8,   "NEON VLDM+PLD, 8 byte reads")
ROUTINE_REGISTER_MEMREAD(read_vldm_pld_16,  "NEON VLDM+PLD, 16 byte reads")
ROUTINE_REGISTER_MEMREAD(read_vldm_pld_32,  "NEON VLDM+PLD, 32 byte reads")
ROUTINE_REGISTER_MEMREAD(read_vldm_pld_64,  "NEON VLDM+PLD, 64 byte reads")
ROUTINE_REGISTER_MEMREAD(read_vldm_pld_128, "NEON VLDM+PLD, 128 byte reads")
ROUTINE_REGISTER_MEMREAD(read_vldm_pld_256, "NEON VLDM+PLD, 256 byte reads")

ROUTINE_REGISTER_MEMREAD(read_vld1_pld_8,   "NEON VLD1+PLD, 8 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(read_vld1_pld_16,  "NEON VLD1+PLD, 16 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(read_vld1_pld_32,  "NEON VLD1+PLD, 32 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(read_vld1_pld_64,  "NEON VLD1+PLD, 64 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(read_vld1_pld_128, "NEON VLD1+PLD, 128 byte aligned reads")
ROUTINE_REGISTER_MEMREAD(read_vld1_pld_256, "NEON VLD1+PLD, 256 byte aligned reads")

#endif /* __ARM_NEON__ */
