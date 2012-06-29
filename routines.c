/* This file is part of sp-mem-throughput.
 *
 * Copyright (C) 2004, 2010 by Nokia Corporation
 *
 * Authors: Leonid Moiseichuk <leonid.moiseichuk@nokia.com>
 *          Tommi Rantala
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
#include <inttypes.h>
#include <stddef.h>

void *
memcpy_c_1(void *restrict d, const void *restrict s, size_t n)
{
	uint8_t *restrict d_8 = d;
	const uint8_t *restrict s_8 = s;
	while (n-- > 0) *d_8++ = *s_8++;
	return d;
}
ROUTINE_REGISTER_MEMCPY(memcpy_c_1, "C memcpy with byte-by-byte writes")

void *
memcpy_c_2(void *restrict d, const void *restrict s, size_t n)
{
	if (n < 16) return memcpy_c_1(d, s, n);
	uint16_t *restrict d_16 = d;
	const uint16_t *restrict s_16 = s;
	size_t c = n / sizeof(uint16_t);
	while (c-- > 0) *d_16++ = *s_16++;
	return d;
}
ROUTINE_REGISTER_MEMCPY(memcpy_c_2, "C memcpy with 2 byte writes")

void *
memcpy_c_4(void *restrict d, const void *restrict s, size_t n)
{
	if (n < 32) return memcpy_c_1(d, s, n);
	uint32_t *restrict d_32 = d;
	const uint32_t *restrict s_32 = s;
	unsigned c = n / sizeof(uint32_t);
	while (c-- > 0) *d_32++ = *s_32++;
	return d;
}
ROUTINE_REGISTER_MEMCPY(memcpy_c_4, "C memcpy with 4 byte writes")

void *
memcpy_c_8(void *restrict d, const void *restrict s, size_t n)
{
	uint64_t *restrict d_64 = d;
	const uint64_t *restrict s_64 = s;
	unsigned c = n / sizeof(uint64_t);
	while (c-- > 0) *d_64++ = *s_64++;
	return d;
}
ROUTINE_REGISTER_MEMCPY(memcpy_c_8, "C memcpy with 8 byte writes")

void
memread_c_4(const void *s, size_t n)
{
	const uint32_t *s_32 = s;
	size_t c = n / sizeof(uint32_t);
	volatile uint32_t temp __attribute__((unused));
	while (c-- > 0) { temp = *s_32++; }
}
ROUTINE_REGISTER_MEMREAD(memread_c_4,
	"Read through input with 4 byte aligned reads");

void
memread_c_8(const void *s, size_t n)
{
	const uint64_t *s_64 = s;
	size_t c = n / sizeof(uint64_t);
	volatile uint64_t temp __attribute__((unused));
	while (c-- > 0) { temp = *s_64++; }
}
ROUTINE_REGISTER_MEMREAD(memread_c_8,
	"Read through input with 8 byte aligned reads");

void *
memset_c_1(void *s, int c, size_t n)
{
	char *ss = s;
	while (n-- > 0) { *ss++ = c; }
	return s;
}
ROUTINE_REGISTER_MEMSET(memset_c_1, "C memset with byte-by-byte writes")

void *
memset_c_2(void *s, int c, size_t n)
{
	if (n < sizeof(uint16_t) || (uintptr_t)s % 2)
		return memset_c_1(s, c, n);
	const uint16_t pattern16 =
		((uint16_t)(uint8_t)c) << 8 | (uint16_t)(uint8_t)c;
	uint16_t *s_16 = (uint16_t *)s;
	size_t cnt = n / sizeof(uint16_t);
	while (cnt-- > 0) { *s_16++ = pattern16 ; }
	if (n % 2) *(char *)s_16 = c;
	return s;
}
ROUTINE_REGISTER_MEMSET(memset_c_2, "C memset with 2 byte writes")

void *
memset_c_4(void *s, int c, size_t n)
{
	if (n < sizeof(uint32_t) || (uintptr_t)s % 4)
		return memset_c_2(s, c, n);
	const uint16_t pattern16 =
		((uint16_t)(uint8_t)c) << 8 | (uint16_t)(uint8_t)c;
	const uint32_t pattern32 =
		((uint32_t)pattern16) << 16 | (uint32_t)pattern16;
	uint32_t *s_32 = (uint32_t *)s;
	size_t cnt = n / sizeof(uint32_t);
	while (cnt-- > 0) { *s_32++ = pattern32; }
	if (n % 4) memset_c_1(s_32, c, n % 4);
	return s;
}
ROUTINE_REGISTER_MEMSET(memset_c_4, "C memset with 4 byte writes")

void *
memset_c_8(void *s, int c, size_t n)
{
	if (n < sizeof(uint64_t) || (uintptr_t)s % 8)
		return memset_c_4(s, c, n);
	const uint16_t pattern16 =
		((uint16_t)(uint8_t)c) << 8 | (uint16_t)(uint8_t)c;
	const uint32_t pattern32 =
		((uint32_t)pattern16) << 16 | (uint32_t)pattern16;
	const uint64_t pattern64 =
		((uint64_t)pattern32) << 32 | (uint64_t)pattern32;
	uint64_t *s_64 = (uint64_t *)s;
	size_t cnt = n / sizeof(uint64_t);
	while (cnt-- > 0) { *s_64++ = pattern64; }
	if (n % 8) memset_c_1(s_64, c, n % 8);
	return s;
}
ROUTINE_REGISTER_MEMSET(memset_c_8, "C memset with 8 byte writes")

size_t
strlen_c_1(const char *s)
{
	const char *p = s;
	while (*p++) ;
	return (size_t)(p-s)-1;
}
ROUTINE_REGISTER_STRLEN(strlen_c_1, "C strlen with single byte reads")
