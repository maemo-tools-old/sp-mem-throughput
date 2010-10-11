/* This file is part of sp-mem-throughput.
 *
 * Copyright (C) 2004, 2010 by Nokia Corporation
 *
 * Authors: Tommi Rantala, Simo Piiroinen
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

#define _GNU_SOURCE

#include "blocks.h"
#include "cgroup-detect.h"
#include "csv.h"
#include "routine.h"
#include "sched-util.h"
#include "validate.h"
#include "version.h"

#include <ctype.h>
#include <getopt.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void print_cpu_info(FILE *);
int bind_to_one_cpu(void);

static struct routine **routines;
static unsigned routines_cnt;
static unsigned routine_now_running;
static char *buf1;
static char *buf2;
static size_t buf_len;
static int no_swap_buffers;
static unsigned repeats = 6;
static unsigned duration = 200;
static int mem_pattern;
static size_t align_src = 4096;
static size_t align_dst = 4096;
static size_t sliding_offset;
static size_t offset;
static size_t block_size;
static int want_mem_lock;
static int validation_mode;
static int no_banner;
static char default_block_sizes[] =
	"8,16,32,64,128,256,1kb,4kb,8kb,64kb,1mb,10mb";
static char *csv_filename, *argv_copy, *banner;

void
routine_register(struct routine *r)
{
	/* fprintf(stderr, "%s(): r->name='%s', r->desc='%s'.\n",
		__func__, r->name, r->desc); */
	routines = realloc(routines, (routines_cnt+1)*sizeof(struct routine *));
	if (!routines) {
		fprintf(stderr,
			"ERROR: realloc() failure in %s().\n", __func__);
		exit(1);
	}
	routines[routines_cnt] = r;
	++routines_cnt;
}

double
m_thr(const struct measurement *m, unsigned b)
{
	double megabytes = (b/1024.) * (m->calls/1024.);
	double secs = m->duration_us / 1000000.;
	return megabytes / secs;
}

static int
measurement_cmp(const void *a, const void *b)
{
	double thr1 = m_thr((struct measurement *)a, block_size);
	double thr2 = m_thr((struct measurement *)b, block_size);
	if (thr1 < thr2) return -1;
	if (thr1 > thr2) return 1;
	return 0;
}

static enum {
	TIMING_AUTODETECT,
	TIMING_CLOCK_PROCESS_CPUTIME,
	TIMING_GETTIMEOFDAY,
} timing_method;

/* Method 1 (preferred): CPU time reported by clock_gettime(). */
static unsigned long long
get_time_us_gettime(void)
{
#ifdef CLOCK_PROCESS_CPUTIME_ID
	struct timespec ts;
	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) < 0) return 0;
	return ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000ULL;
#else
	return 0;
#endif
}
/* Method 2: Wall clock.
 * Gives bad results if we're fighting for CPU time.
 */
static unsigned long long
get_time_us_gettimeofday(void)
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) < 0) return 0;
	return tv.tv_sec * 1000000ULL + tv.tv_usec;
}

static unsigned long long
get_time_us(void)
{
	unsigned long long ret;
	switch (timing_method) {
	case TIMING_CLOCK_PROCESS_CPUTIME:
		ret = get_time_us_gettime();
		if (ret) return ret;
		fprintf(stderr,
			"ERROR: clock_gettime(CLOCK_PROCESS_CPUTIME_ID) failure.\n");
		exit(1);
		break;
	case TIMING_GETTIMEOFDAY:
		ret = get_time_us_gettimeofday();
		if (ret) return ret;
		fprintf(stderr,
			"ERROR: gettimeofday() failure.\n");
		exit(1);
		break;
	case TIMING_AUTODETECT:
		ret = get_time_us_gettime();
		if (ret) { timing_method = TIMING_CLOCK_PROCESS_CPUTIME; return ret; }
		ret = get_time_us_gettimeofday();
		if (ret) { timing_method = TIMING_GETTIMEOFDAY; return ret; }
		fprintf(stderr,
			"ERROR: unable to determine working timing method.\n");
		exit(1);
		break;
	default:
		abort();
	};
	/* Not reached. */
	abort();
	return 0;
}

static void
print_measurement(struct routine *r)
{
	float throughput;
	unsigned i, j, b, rowcnt;
	char bs[32];
	printf("%-18s", r->name);
	block_size_iter_reset();
	for (i=0; i < block_sizes_cnt; ++i) {
		b = block_size_iter_next();
		if (i) printf("%18s", "");
		if (b >= 1024*1024) {
			sprintf(bs, " B=%uMB", b/(1024*1024));
		} else if (b >= 1024)      {
			sprintf(bs, " B=%uKB", b/1024);
		} else {
			sprintf(bs, " B=%u", b);
		}
		printf("%-9s", bs);
		for (j=0, rowcnt=0; j < repeats; ++j) {
			throughput = m_thr(&r->measurements[i*repeats+j], b);
			if (printf("|%7.1f", throughput) <= 8) {
				if (rowcnt < 5) printf(" ");
			}
			if (++rowcnt == 6 && j < repeats-1) {
				printf("\n%27s", "");
				rowcnt = 0;
			}
		}
		printf("\n");
	}
	fflush(stdout);
}

static void
print_testcase_headers(FILE *stream)
{
	fprintf(stream,
"Test case name    | Block  | Results (sorted): throughput in MB/s.\n"
"                  | size B | B=block size in bytes. (MB=1024*1024)\n");
}

static unsigned
handler_namelen(void)
{
	unsigned len;
	for (len=0; routines[routine_now_running]->name[len]; ++len) ;
	return len;
}

static void
sigsegv_handler(int sig)
{
	static const char errmsg1[] =
		"\n"
		"ERROR: SIGSEGV received while running routine '";
	static const char errmsg2[] = "'.\n";
	unsigned len = handler_namelen();
	if (len == 0) goto done;
	if (write(STDERR_FILENO, errmsg1, sizeof(errmsg1)-1) < 0) goto done;
	if (write(STDERR_FILENO, routines[routine_now_running]->name, len) < 0)
		goto done;
	if (write(STDERR_FILENO, errmsg2, sizeof(errmsg2)-1) < 0) goto done;
done:
	if (raise(sig)) _exit(1);
}

static void
sigbus_handler(int sig)
{
	static const char errmsg1[] =
		"\n"
		"ERROR: SIGBUS received while running routine '";
	static const char errmsg2[] =
		"'.\n"
		"****** This usually indicates unaligned memory access.\n"
		"****** Try adjusting the parameters related to alignment"
			" or block size.\n\n";
	unsigned len = handler_namelen();
	if (len == 0) goto done;
	if (write(STDERR_FILENO, errmsg1, sizeof(errmsg1)-1) < 0) goto done;
	if (write(STDERR_FILENO, routines[routine_now_running]->name, len) < 0)
		goto done;
	if (write(STDERR_FILENO, errmsg2, sizeof(errmsg2)-1) < 0) goto done;
done:
	if (raise(sig)) _exit(1);
}

static volatile sig_atomic_t timerflag;
static void alarm_handler(int sig)
{
	timerflag = sig;
}

static inline void
runner(struct routine *r)
{
	switch (r->type) {
	case routine_memread:
		r->fn.memread(&buf1[offset], block_size);
		return;
	case routine_memchr:
		r->fn.memchr(&buf1[offset], mem_pattern, block_size);
		return;
	case routine_memset:
		r->fn.memset(&buf1[offset], mem_pattern, block_size);
		return;
	case routine_memcpy:
		r->fn.memcpy(&buf2[offset], &buf1[offset], block_size);
		return;
	case routine_strcpy:
		r->fn.strcpy(&buf2[offset], &buf1[offset]);
		return;
	case routine_strlen:
		r->fn.strlen(&buf1[offset]);
		return;
	default:
		abort();
	}
}

static inline void
swap_bufs(void)
{
	char *tmp = buf1;
	buf1 = buf2;
	buf2 = tmp;
}

/* Initialize the contents of buffer1 (and buffer2) so that when benchmarking
 * the routine we get sane results.
 */
static void
prepare_buffers(struct routine *r)
{
	switch (r->type) {
	case routine_memchr:
		memset(&buf1[offset], ~(unsigned)mem_pattern, block_size);
		if (buf2) memset(&buf2[offset], ~(unsigned)mem_pattern,
				block_size);
		break;
	case routine_strcpy: /* fall through */
	case routine_strlen:
		memset(&buf1[offset], ~(unsigned)mem_pattern, block_size-1);
		buf1[offset+block_size-1] = 0;
		if (buf2) {
			memset(&buf2[offset], ~(unsigned)mem_pattern,
					block_size-1);
			buf2[offset+block_size-1] = 0;
		}
		break;
	default:
		break;
	}
}

static void
test_run(struct routine *r)
{
	struct itimerval dur;
	unsigned long long timing1, timing2;
	unsigned i, j, d=0, reps;
	r->measurements = calloc((block_sizes_cnt*repeats),
			sizeof(struct measurement));
	if (!r->measurements) {
		fprintf(stderr,
			"ERROR: calloc() failure in %s().\n",
			__func__);
		exit(1);
	}
	block_size_iter_reset();
	for (i=0; i < block_sizes_cnt; ++i) {
		block_size = block_size_iter_next();
		prepare_buffers(r);
		for (j=0; j < repeats; ++j) {
			memset(&dur, 0, sizeof(struct itimerval));
			dur.it_value.tv_sec  = (duration / 1000u);
			dur.it_value.tv_usec = (duration % 1000u) * 1000u;
			if (setitimer(ITIMER_REAL, &dur, 0) == -1) {
				fprintf(stderr,
					"ERROR: setitimer() failure in %s()"
					": '%m'.\n",
					__func__);
				exit(1);
			}
			reps = 0;
			timerflag = 0;
			offset = 0;
			runner(r);
			timing1 = get_time_us();
			if (sliding_offset) {
				if (no_swap_buffers==0 && buf2) {
					do {
						runner(r);
						++reps;
						offset += sliding_offset;
						offset %= 256;
						swap_bufs();
					} while (timerflag == 0);
				} else {
					do {
						runner(r);
						++reps;
						offset += sliding_offset;
						offset %= 256;
					} while (timerflag == 0);
				}
			} else {
				if (no_swap_buffers==0 && buf2) {
					do {
						runner(r);
						++reps;
						swap_bufs();
					} while (timerflag == 0);
				} else {
					do {
						runner(r);
						++reps;
					} while (timerflag == 0);
				}
			}
			timing2 = get_time_us();
			if (timing1 == 0 || timing2 <= timing1) {
				fprintf(stderr, "ERROR: insane timing results: "
						"time1: %llu, time2: %llu.\n",
						timing1, timing2);
				exit(1);
			}
			r->measurements[i*repeats+j].calls = reps;
			r->measurements[i*repeats+j].duration_us =
				timing2 - timing1;
		}
		qsort(&r->measurements[i*repeats], repeats,
				sizeof(struct measurement),
				measurement_cmp);
		if (block_sizes_cnt < 80 ||
		    (i % ((block_sizes_cnt / 80)+1) == 0)) {
			printf("-"); ++d;
		}
		fflush(stdout);
		usleep(2000);
	}
	for (; d < 80; ++d) printf("-");
	printf("\n");
	print_measurement(r);
}

static void
run_flagged_testcases(void)
{
	unsigned i;
	struct sigaction sigact;
	/* SIGBUS */
	memset(&sigact, 0, sizeof(struct sigaction));
	sigact.sa_handler = sigbus_handler;
	sigact.sa_flags = SA_RESETHAND;
	sigaction(SIGBUS, &sigact, NULL);
	/* SIGSEGV */
	memset(&sigact, 0, sizeof(struct sigaction));
	sigact.sa_handler = sigsegv_handler;
	sigact.sa_flags = SA_RESETHAND;
	sigaction(SIGSEGV, &sigact, NULL);
	/* SIGALRM */
	memset(&sigact, 0, sizeof(struct sigaction));
	sigact.sa_handler = alarm_handler;
	if (sigaction(SIGALRM, &sigact, NULL) < 0) {
		fprintf(stderr,
			"ERROR: unable to register SIGALRM handler: '%m'.\n");
		exit(1);
	}
	for (i=0; i < routines_cnt; ++i) {
		if (routines[i]->flagged) {
			routine_now_running = i;
			test_run(routines[i]);
		}
	}
	signal(SIGALRM, SIG_DFL);
	signal(SIGBUS,  SIG_DFL);
	signal(SIGSEGV, SIG_DFL);
}

static void
init_banner(const char *prog)
{
	const char *base;
	base = basename(prog);
	if (!base) return;
	if (asprintf(&banner,
		"%s: memory performance test suite v%d.%d",
		base, version_major, version_minor) < 0) {
		banner = NULL;
	}
}

static void
print_banner(FILE *stream)
{
	if (banner) {
		fprintf(stream, "%s\n\n", banner);
		fflush(stream);
	}
}

static void
print_available_routines(FILE *stream)
{
	unsigned i, j;
	fprintf(stream,
":: Categories and Routines :::::::::::::::::::::::::::::::::::::::::::::::::::::\n\n");
	for (i=0; i < routine_types_count; ++i) {
		fprintf(stream, "%-20s   %s\n",
			rtype_name(i), rtype_desc(i));
		for (j=0; j < routines_cnt; ++j) {
			if (routines[j]->type != i) continue;
			fprintf(stream, "- %-20s %s\n",
				routines[j]->name,
				routines[j]->desc);
		}
		fprintf(stream, "\n");
	}

}

static const struct option options[] = {
	{ "all", 0, NULL, 'A' },
	{ "duration", 1, NULL, 'd' },
	{ "rounds", 1, NULL, 'r' },
	{ "align1", 1, NULL, 1001 },
	{ "align2", 1, NULL, 1002 },
	{ "align", 1, NULL, 'a' },
	{ "mlock", 0, NULL, 1003 },
	{ "memlock", 0, NULL, 1003 },
	{ "validate", 0, NULL, 1004 },
	{ "no-banner", 0, NULL, 1005 },
	{ "list", 0, NULL, 'L' },
	{ "csv", 1, NULL, 1006 },
	{ "sliding-offset", 1, NULL, 1007 },
	{ "no-swap-buffers", 0, NULL, 1008 },
	{ "help", 0, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void
usage(const char *prog)
{
	char *base;

	base = basename(prog);

	/* Banner */
	print_banner(stderr);

	/* One line help */
	fprintf(stderr, "Usage: %s [OPT...] <test type>\n", base);

	/* Option listing */
	fprintf(stderr,
"\n"
":: Program Description and Testing Methodology :::::::::::::::::::::::::::::::::\n"
"\n"
"   This program is a benchmark for measuring memory throughput achieved by\n"
"   various memory access patterns:\n"
"        1) reads: using C library memchr() and similar functions.\n"
"        2) writes: using C library memset() and similar functions.\n"
"        3) reads & writes: using C library memcpy() and similar functions.\n"
"        ...\n"
"\n"
"   Throughput is reported in megabytes per second [1 MB = 1024*1024 bytes].\n"
"\n"
":: Options :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n"
"\n"
"   -L, --list          List available routines.\n"
"\n"
"   -A, --all           Run all benchmarks.\n"
"\n"
"   -d, --duration=N    For each function and every block size, the tested\n"
"                       function is repeatedly called for a minimum duration of N\n"
"                       milliseconds. Throughput is then calculated based on the\n"
"                       number of function calls that were made and the precise\n"
"                       CPU time it took. [default: %u ms]\n"
"\n"
"   -r, --rounds=N      Repeat each benchmark N times [default: %u].\n"
"\n"
"   -b, --blocks=N      This parameter sets the number of bytes that are\n"
"                       processed per function call. For example when\n"
"                       benchmarking a memcpy() call, this sets the last\n"
"                       parameter to N. Accepts multiple sizes, separated by\n"
"                       comma, and ranges separated by a dash.\n"
"                       [%s]\n"
"\n"
"   -a, --align=N       Align buffer1 and buffer2 to N byte boundaries.\n"
"       --align1=N      Align buffer1 to N byte boundary [default: %u bytes].\n"
"       --align2=N      Align buffer2 to N byte boundary [default: %u bytes].\n"
"\n"
"       --sliding-offset=N\n"
"                       For each function call, increment buffer1 (and buffer2)\n"
"                       positions by N bytes. This incrementation wraps over at\n"
"                       256 bytes [default: 0, max: 255].\n"
"\n"
"       --no-swap-buffers\n"
"                       By default, buffer1 and buffer2 pointers are swapped\n"
"                       between each function call, eg. memcpy() bechmarks will\n"
"                       copy 1->2, then 2->1, then 1->2 and so forth. This option\n"
"                       can be used to disable the swapping. Only has effect if\n"
"                       both of the buffers are allocated.\n"
"\n"
"       --csv=FILE      Write results in CSV format to FILE. Default file name is\n"
"                       'sp-mem-throughput-<device>-<year>-<week>-<build>.csv'\n"
"                       for Maemo platforms that have the sysinfo client\n"
"                       available, or 'sp-mem-throughput.csv' otherwise.\n"
"\n"
"       --memlock       Lock all memory to RAM. [default: no]\n"
"       --validate      Validate results from each function.\n"
"       --no-banner     Do not print banner & headers at program launch.\n"
		, duration, repeats, default_block_sizes, align_src, align_dst
);

	/* Examples */
	fprintf(stderr,
"\n"
":: Examples ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n"
"\n"
"   # List available routines:\n"
"   %s -L\n"
"\n"
"   # Run all benchmarks with default settings:\n"
"   %s -A\n"
"\n"
"   # Run each benchmark from the memset category:\n"
"   %s memset\n"
"\n"
"   # Run one benchmark 'memset_libc':\n"
"   %s memset_libc\n"
"\n"
"   # Run one benchmark 'memset_libc' for 100 rounds, 300ms each:\n"
"   %s memset_libc -r100 -d300\n"
"\n"
"   # Do not write a CSV file:\n"
"   %s --csv=/dev/null -A\n"
"\n"
"   # Benchmark each routine from the memcpy category with 1-256 bytes:\n"
"   %s -b1-256 -d50 -r5 memcpy\n"
#ifdef __ARM_NEON__
"\n"
"   # Benchmark reading 1MB, 4MB and 32MB of data using two NEON routines:\n"
"   %s -b1MB,4MB,32MB memread_neon_32 memread_neon_64\n"
, base
#endif /* __ARM_NEON__ */
, base, base, base, base, base, base, base
);
	fprintf(stderr, "\n");
}

static int
string_to_type(const char *s)
{
	unsigned i;
	for (i=0; i < routine_types_count; ++i) {
		if (strcmp(s, rtype_name(i)) == 0) {
			return i;
		}
	}
	return -1;
}

static void
flag_for_running(const char *name)
{
	unsigned i;
	int rtype;
	/* Check categories */
	rtype = string_to_type(name);
	if (rtype >= 0 && rtype < routine_types_count) {
		for (i=0; i < routines_cnt; ++i) {
			if (routines[i]->type == (unsigned)rtype) {
				routines[i]->flagged = 1;
			}
		}
		return;
	}
	/* Check individual test routines */
	for (i=0; i < routines_cnt; ++i) {
		if (strcmp(name, routines[i]->name) == 0) {
			routines[i]->flagged = 1;
			return;
		}
	}
	fprintf(stderr, "ERROR: unidentified test case '%s'.\n", name);
	exit(1);
}

static void
parse_args(int argc, char **argv)
{
	unsigned i;
	int c, t, flag_all, got_b;
	flag_all = got_b = 0;
	while (1) {
		c = getopt_long(argc, argv, "LAd:b:s:r:a:h", options, NULL);
		if (c == -1)
			break;
		switch (c) {
		case 'L':
			print_banner(stdout);
			print_available_routines(stdout);
			exit(0);
			break;
		case 'A':
			flag_all = 1;
			break;
		case 'd':
			t = atoi(optarg);
			if (t < 1) {
				fprintf(stderr, "ERROR: -d/--duration value"
					" must be greater than zero.\n");
				exit(1);
			}
			duration = t;
			break;
		case 'b':
			block_size_parse(optarg);
			got_b = 1;
			break;
		case 'r':
			t = atoi(optarg);
			if (t < 1) {
				fprintf(stderr, "ERROR: -r/--rounds value must"
					" be greater than zero.\n");
				exit(1);
			}
			if (t > 1000) {
				fprintf(stderr, "ERROR: -r/--rounds largest"
					" supported value is 1000.\n");
				exit(1);
			}
			repeats = t;
			break;
		case 1001:
			t = atoi(optarg);
			if (t <= 0) {
				fprintf(stderr, "ERROR: --align1 value"
					" must be greater than zero.\n");
				exit(1);
			}
			align_src = t;
			break;
		case 1002:
			t = atoi(optarg);
			if (t <= 0) {
				fprintf(stderr, "ERROR: --align1 value"
					" must be greater than zero.\n");
				exit(1);
			}
			align_dst = t;
			break;
		case 1003:
			want_mem_lock = 1;
			break;
		case 1004:
			validation_mode = 1;
			flag_all = 1;
			break;
		case 1005:
			no_banner = 1;
			break;
		case 1006:
			csv_filename = optarg;
			break;
		case 1007:
			t = atoi(optarg);
			if (t < 1) {
				fprintf(stderr, "ERROR: --sliding-offset value"
					" must be greater than zero.\n");
				exit(1);
			}
			if (t >= 256) {
				fprintf(stderr, "ERROR: --sliding-offset value"
					" must be less than 256.\n");
				exit(1);
			}
			sliding_offset = t;
			break;
		case 1008:
			no_swap_buffers = 1;
			break;
		case 'a':
			t = atoi(optarg);
			if (t <= 0) {
				fprintf(stderr, "ERROR: -a/--align value"
					" must be greater than zero.\n");
				exit(1);
			}
			align_src = align_dst = t;
			break;
		case 'h':
			usage(argv[0]);
			exit(1);
			break;
		default:
			break;
		}
	}
	if (align_src > 4096 || align_dst > 4096) {
		fprintf(stderr,
			"ERROR: largest supported alignment is 4096 bytes.\n");
		exit(1);
	}
	if (flag_all) {
		for (i=0; i < routines_cnt; ++i)
			routines[i]->flagged = 1;
	}
	if (!validation_mode && !flag_all && optind >= argc) {
		usage(argv[0]);
		exit(1);
	}
	while (optind < argc) {
		flag_for_running(argv[optind]);
		++optind;
	}
	if (!got_b) block_size_parse(default_block_sizes);
}

static unsigned
alignment(void *ptr)
{
	uintptr_t p;
	unsigned i;
	p = (uintptr_t)ptr;
	for (i=0; i < sizeof(void*)*8; ++i)
		if (p & (1 << i)) break;
	return 1 << i;
}

static void *
force_alignment(void *ptr, size_t al)
{
	uintptr_t p = (uintptr_t)ptr;
	if (p % al) p += al - (p % al);
	p = p | al;
	/*
	fprintf(stderr,
		"force_alignment(): orig: %p\n"
		"                    new: %p\n"
		"                   diff: %u\n"
		"          alignment req: %zd\n"
		"          alignment ret: %u\n"
		, ptr, (void *)p, p-(uintptr_t)ptr, al, alignment((void *)p));
	*/
	return (void *)p;
}

static void
allocate_arrays(void)
{
	void *p;
	unsigned i;
	int need1=1, need2=0;
	for (i=0; i < routines_cnt; ++i) {
		if (!routines[i]->flagged) continue;
		if (routines[i]->type == routine_memcpy) need2=1;
		if (routines[i]->type == routine_strcpy) need2=1;
	}
	buf_len = block_sizes_largest+(2*4096);
	if (sliding_offset) buf_len += 256;
	if (need1) {
		p = (void *)buf1;
		if (posix_memalign(&p, 4096, buf_len) != 0) {
			fprintf(stderr,
				"ERROR: posix_memalign() failed for %zd bytes.\n",
				buf_len);
			exit(1);
		}
		buf1 = p;
		memset(buf1, 0, buf_len);
	}
	if (need2) {
		p = (void *)buf2;
		if (posix_memalign(&p, 4096, buf_len) != 0) {
			fprintf(stderr,
				"ERROR: posix_memalign() failed for %zd bytes.\n",
				buf_len);
			exit(1);
		}
		buf2 = p;
		memset(buf2, 0, buf_len);
	}
	if (need1) buf1 = force_alignment(buf1, align_src);
	if (need2) buf2 = force_alignment(buf2, align_dst);
}

static unsigned
estimate_runtime_seconds(void)
{
	unsigned i, cnt=0;
	for (i=0; i < routines_cnt; ++i)
		cnt += routines[i]->flagged;
	return (cnt * block_sizes_cnt * repeats * duration) / 1000;
}

static void
print_testcase_setup(FILE *stream)
{
	unsigned est;
	fprintf(stream, "\nTest case information:\n");
	fprintf(stream, "    Duration for one measurement: %u ms\n", duration);
	fprintf(stream, "    How many times each measurement is repeated: %u\n",
			repeats);
	if (sliding_offset) {
		fprintf(stream,
			"    Sliding offset: %u bytes\n", sliding_offset);
	}
	fprintf(stream, "    Write/search byte pattern: 0x%02x\n", mem_pattern);
	fprintf(stream, "    buffer1: %p-%p [%u bytes, alignment: %u bytes]\n",
		buf1 ? : (void *)0, buf1 ? &buf1[buf_len] : (void *)0,
			buf1 ? buf_len : 0, buf1 ? alignment(buf1) : 0);
	if (buf2) {
		fprintf(stream,
			"    buffer2: %p-%p [%u bytes, alignment: %u bytes]\n",
			buf2 ? : (void *)0, buf2 ? &buf2[buf_len] : (void *)0,
				buf2 ? buf_len : 0, buf1 ? alignment(buf2) : 0);
	}
	fprintf(stream, "    Memory locked to RAM: %s\n",
			want_mem_lock ? "yes" : "no");
	est = estimate_runtime_seconds();
	if (est < 10) {
		fprintf(stream, "    Estimated run time: < 10 seconds\n");
	} else if (est < 60) {
		fprintf(stream, "    Estimated run time: < 1 minute\n");
	} else if (est < 3600) {
		fprintf(stream, "    Estimated run time: %u minute(s)\n",
				 est/60);
	} else {
		fprintf(stream, "    Estimated run time: %u hour(s)\n",
				 est/3600);
	}
	fprintf(stream, "\n");
}

static void
print_cgroup_info(FILE *stream)
{
	char *cg = current_cgroup();
	if (!cg) goto done;
	fprintf(stream,
		"WARNING: running under a resource Control Group!\n"
		"/proc/self/cgroup: '%s'\n\n",
		cg);
done:
	free(cg);
}

static void
memlock(void)
{
	if (mlockall(MCL_CURRENT | MCL_FUTURE) < 0) {
		fprintf(stderr,
			"ERROR: mlockall() failed: '%m'.\n");
		exit(1);
	}
}

static void
print_headers(void)
{
	if (no_banner) return;
	print_sched_info(stdout);
	print_cpu_info(stdout);
	print_testcase_setup(stdout);
	print_cgroup_info(stderr);
	print_testcase_headers(stdout);
	fflush(stdout);
}

/* Compare routines first by type, then by name.
 *
 * Check if the names end with a number, so that we get the sorted order:
 *    memset_neon_16
 *    memset_neon_32
 *    memset_neon_64
 *    memset_neon_128
 */
static int
routine_cmp(const void *a, const void *b)
{
	unsigned u1, u2;
	const char *an, *bn, *t;
	const struct routine *aa = *(struct routine **)a;
	const struct routine *bb = *(struct routine **)b;
	if (aa->type != bb->type) return aa->type - bb->type;
	an = aa->name;
	bn = bb->name;
	while (*an && *bn) {
		if (*an != *bn) break;
		++an; ++bn;
	}
	if (*an == 0) goto cmp_with_strcmp;
	if (*bn == 0) goto cmp_with_strcmp;
	for (t=an; *t; ++t) if (!isdigit(*t)) goto cmp_with_strcmp;
	for (t=bn; *t; ++t) if (!isdigit(*t)) goto cmp_with_strcmp;
	if (sscanf(an, "%u", &u1) != 1) goto cmp_with_strcmp;
	if (sscanf(bn, "%u", &u2) != 1) goto cmp_with_strcmp;
	return u1 - u2;
cmp_with_strcmp:
	return strcmp(aa->name, bb->name);
}

static char *
dup_argv(int argc, char **argv)
{
	int i;
	char *p, *pp, *t;
	size_t len;
	for (len=0, i=0; i < argc; ++i) len += strlen(argv[i]);
	p = malloc(len+argc+1);
	if (!p) return NULL;
	*p = 0;
	for (pp=p, i=0; i < argc; ++i) {
		t = argv[i];
		while (*t) *pp++ = *t++;
		if (i < argc-1) *pp++ = ' ';
	}
	*pp = 0;
	return p;
}

int main(int argc, char **argv)
{
	argv_copy = dup_argv(argc, argv);
	qsort(routines, routines_cnt, sizeof(struct routine *), routine_cmp);
	init_banner(argv[0]);
	parse_args(argc, argv);
	if (!no_banner) print_banner(stdout);
	allocate_arrays();
	if (want_mem_lock) memlock();
	if (validation_mode) {
		exit(!!validate_all(routines, routines_cnt, buf1, buf2));
	}
	raise_priority();
	bind_to_one_cpu();
	if (timing_method == TIMING_AUTODETECT) (void)get_time_us();
	print_headers();
	run_flagged_testcases();
	if (!csv_filename) csv_filename = csv_get_default_name();
	if (!csv_filename) csv_filename = "sp-mem-throughput.csv";
	if (output_csv(csv_filename, routines, routines_cnt,
			repeats, argv_copy, banner) < 0)
		exit(1);
	return 0;
}
