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
	unsigned buffers_used : 2;
	unsigned flagged : 1;
	struct measurement {
		unsigned calls;
		unsigned long long duration_us;
	} *measurements;
};

void routine_register(struct routine *);

#define ROUTINE_REGISTER(_f, _t, _b, _n, _d)\
	static struct routine _f##_routine =\
		{ .fn._t##_ = (_f),\
		  .type = routine_##_t,\
		  .buffers_used = _b,\
		  .name = _n,\
		  .desc = _d }; \
	static void _f##_register_hook(void) __attribute__((constructor));\
	static void _f##_register_hook(void)\
		{ routine_register(&_f##_routine); };

#define ROUTINE_REGISTER_MEMREAD(_func, _desc)\
	ROUTINE_REGISTER(_func, memread, 1, #_func, _desc)

#define ROUTINE_REGISTER_MEMCHR(_func, _desc)\
	ROUTINE_REGISTER(_func, memchr, 1, #_func, _desc)

#define ROUTINE_REGISTER_MEMSET(_func, _desc)\
	ROUTINE_REGISTER(_func, memset, 1, #_func, _desc)

#define ROUTINE_REGISTER_MEMCPY(_func, _desc)\
	ROUTINE_REGISTER(_func, memcpy, 2, #_func, _desc)

#define ROUTINE_REGISTER_STRCPY(_func, _desc)\
	ROUTINE_REGISTER(_func, strcpy, 2, #_func, _desc)

#define ROUTINE_REGISTER_STRNCPY(_func, _desc)\
	ROUTINE_REGISTER(_func, strncpy, 2, #_func, _desc)

#define ROUTINE_REGISTER_STRLEN(_func, _desc)\
	ROUTINE_REGISTER(_func, strlen, 1, #_func, _desc)

#define ROUTINE_REGISTER_STRCMP(_func, _desc)\
	ROUTINE_REGISTER(_func, strcmp, 2, #_func, _desc)

#define ROUTINE_REGISTER_STRNCMP(_func, _desc)\
	ROUTINE_REGISTER(_func, strncmp, 2, #_func, _desc)

#define ROUTINE_REGISTER_STRCHR(_func, _desc)\
	ROUTINE_REGISTER(_func, strchr, 1, #_func, _desc)

/* Throughput for one measurement as performed with the given block size and
 * with the number of buffers used by the routine.
 */
double m_thr(const struct measurement *, unsigned block_size,
		unsigned buffers_used);

#ifdef __cplusplus
}
#endif

#endif /* ROUTINE_H */
