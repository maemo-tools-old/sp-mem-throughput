/* Copyright (C) 2004, 2010-2011 by Nokia Corporation
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

#ifndef ROUTINE_H
#define ROUTINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

enum routine_type {
	routine_memread,
	routine_memchr,
	routine_memset,
	routine_memcpy,
	routine_strcpy,
	routine_strncpy,
	routine_strlen,
	routine_strcmp,
	routine_strncmp,
	routine_strchr,
	routine_types_count
};

const char *rtype_name(enum routine_type);
const char *rtype_desc(enum routine_type);

struct routine {
	union {
		void (*memread_)(const void *, size_t);
		void *(*memchr_)(const void *, int, size_t);
		void *(*memset_)(void *, int, size_t);
		void *(*memcpy_)(void *restrict, const void *restrict, size_t);
		char *(*strcpy_)(char *, const char *);
		char *(*strncpy_)(char *, const char *, size_t);
		size_t (*strlen_)(const char *);
		int (*strcmp_)(const char *, const char *);
		int (*strncmp_)(const char *, const char *, size_t);
		char *(*strchr_)(const char *, int);
	} fn;
	const char *name;
	const char *desc;
	enum routine_type type;
	unsigned flagged : 1;
	struct measurement {
		unsigned calls;
		unsigned long long duration_us;
	} *measurements;
};

void routine_register(struct routine *);

#define ROUTINE_REGISTER(_f, _t, _n, _d)\
	static struct routine _f##_routine =\
		{ .fn._t##_ = (_f),\
		  .type = routine_##_t,\
		  .name = _n,\
		  .desc = _d }; \
	static void _f##_register_hook(void) __attribute__((constructor));\
	static void _f##_register_hook(void)\
		{ routine_register(&_f##_routine); };

#define ROUTINE_REGISTER_MEMREAD(_func, _desc)\
	ROUTINE_REGISTER(_func, memread, #_func, _desc)

#define ROUTINE_REGISTER_MEMCHR(_func, _desc)\
	ROUTINE_REGISTER(_func, memchr, #_func, _desc)

#define ROUTINE_REGISTER_MEMSET(_func, _desc)\
	ROUTINE_REGISTER(_func, memset, #_func, _desc)

#define ROUTINE_REGISTER_MEMCPY(_func, _desc)\
	ROUTINE_REGISTER(_func, memcpy, #_func, _desc)

#define ROUTINE_REGISTER_STRCPY(_func, _desc)\
	ROUTINE_REGISTER(_func, strcpy, #_func, _desc)

#define ROUTINE_REGISTER_STRNCPY(_func, _desc)\
	ROUTINE_REGISTER(_func, strncpy, #_func, _desc)

#define ROUTINE_REGISTER_STRLEN(_func, _desc)\
	ROUTINE_REGISTER(_func, strlen, #_func, _desc)

#define ROUTINE_REGISTER_STRCMP(_func, _desc)\
	ROUTINE_REGISTER(_func, strcmp, #_func, _desc)

#define ROUTINE_REGISTER_STRNCMP(_func, _desc)\
	ROUTINE_REGISTER(_func, strncmp, #_func, _desc)

#define ROUTINE_REGISTER_STRCHR(_func, _desc)\
	ROUTINE_REGISTER(_func, strchr, #_func, _desc)

double m_thr(const struct measurement *, unsigned block_size);

#ifdef __cplusplus
}
#endif

#endif /* ROUTINE_H */
