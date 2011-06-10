/* This file is part of sp-mem-throughput.
 *
 * Copyright (C) 2010-2011 by Nokia Corporation
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
#include <string.h>

/* Expose C library functions with '_libc' appended to the names.
 * This way we can use 'memchr', 'memcpy' et al. as category names.
 */
ROUTINE_REGISTER(memchr, memchr, 1, "memchr_libc", "C library memchr")
ROUTINE_REGISTER(memset, memset, 1, "memset_libc", "C library memset")
ROUTINE_REGISTER(memcpy, memcpy, 2, "memcpy_libc", "C library memcpy")
ROUTINE_REGISTER(memmove, memcpy, 2, "memmove_libc", "C library memmove")
ROUTINE_REGISTER(strlen, strlen, 1, "strlen_libc", "C library strlen")
ROUTINE_REGISTER(strcpy, strcpy, 2, "strcpy_libc", "C library strcpy")
ROUTINE_REGISTER(strncpy, strncpy, 2, "strncpy_libc", "C library strncpy")
ROUTINE_REGISTER(strcmp, strcmp, 2, "strcmp_libc", "C library strcmp")
ROUTINE_REGISTER(strncmp, strncmp, 2, "strncmp_libc", "C library strncmp")
ROUTINE_REGISTER(strchr, strchr, 1, "strchr_libc", "C library strchr")
ROUTINE_REGISTER(strrchr, strchr, 1, "strrchr_libc", "C library strrchr")
