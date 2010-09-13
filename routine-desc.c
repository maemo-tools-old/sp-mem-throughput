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

#include <stdlib.h>

static const struct {
	const char *const name;
	const char *const desc;
} routine_type_desc[routine_types_count] = {
	[routine_memread] = {
		"memread",
		"Read bytes from buffer1.",
	},
	[routine_memchr] = {
		"memchr",
		"Scan for byte from buffer1.",
	},
	[routine_memset] = {
		"memset",
		"Fill buffer1 with byte pattern.",
	},
	[routine_memcpy] = {
		"memcpy",
		"Copy bytes from buffer1 to buffer2.",
	},
	[routine_strcpy] = {
		"strcpy",
		"Treat buffer1 as a string, copy to buffer2.",
	},
	[routine_strlen] = {
		"strlen",
		"Treat buffer1 as a string, calculate length.",
	},
};

const char *
rtype_name(enum routine_type t)
{
	if (t >= routine_types_count) abort();
	return routine_type_desc[t].name;
}

const char *
rtype_desc(enum routine_type t)
{
	if (t >= routine_types_count) abort();
	return routine_type_desc[t].desc;
}
