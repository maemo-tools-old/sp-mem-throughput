/* Naive memory reading routines that utilize ARM NEON instructions. Only
 * useful for benchmarking purposes.
 *
 * This file is part of sp-mem-throughput.
 *
 * Copyright (C) 2010 by Nokia Corporation
 *
 * Authors: Tommi Rantala, Siarhei Siamashka
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

/* C prototypes:
 *     void read_vldm_{8,16,32,64,128,256}(const void *, size_t);
 *     void read_vld1_{8,16,32,64,128,256}(const void *, size_t);
 *     void read_vldm_pld_{8,16,32,64,128,256}(const void *, size_t);
 *     void read_vld1_pld_{8,16,32,64,128,256}(const void *, size_t);
 */

/* read memory 256 bytes per loop. */
	.global	read_vldm_256
	.func	read_vldm_256
read_vldm_256:
1:	subs	r1, r1, #256
	vldmia	r0!, {d16-d31}
	vldmia	r0!, {d16-d31}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 128 bytes per loop. */
	.global	read_vldm_128
	.func	read_vldm_128
read_vldm_128:
1:	subs	r1, r1, #128
	vldmia	r0!, {d16-d31}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 64 bytes per loop. */
	.global	read_vldm_64
	.func	read_vldm_64
read_vldm_64:
1:	subs	r1, r1, #64
	vldmia	r0!, {d0-d7}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 32 bytes per loop. */
	.global	read_vldm_32
	.func	read_vldm_32
read_vldm_32:
1:	subs	r1, r1, #32
	vldmia	r0!, {d0-d3}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 16 bytes per loop. */
	.global	read_vldm_16
	.func	read_vldm_16
read_vldm_16:
1:	subs	r1, r1, #16
	vldmia	r0!, {d0-d1}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 8 bytes per loop. */
	.global	read_vldm_8
	.func	read_vldm_8
read_vldm_8:
1:	subs	r1, r1, #8
	vldmia	r0!, {d0}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 256 bytes per loop. */
	.global	read_vld1_256
	.func	read_vld1_256
read_vld1_256:
1:	subs	r1, r1, #256
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 128 bytes per loop. */
	.global	read_vld1_128
	.func	read_vld1_128
read_vld1_128:
1:	subs	r1, r1, #128
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 64 bytes per loop. */
	.global	read_vld1_64
	.func	read_vld1_64
read_vld1_64:
1:	subs	r1, r1, #64
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 32 bytes per loop. */
	.global	read_vld1_32
	.func	read_vld1_32
read_vld1_32:
1:	subs	r1, r1, #32
	vld1.64	{d16-d19}, [r0,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 16 bytes per loop. */
	.global	read_vld1_16
	.func	read_vld1_16
read_vld1_16:
1:	subs	r1, r1, #16
	vld1.64	{d16,d17}, [r0,:128]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 8 bytes per loop. */
	.global	read_vld1_8
	.func	read_vld1_8
read_vld1_8:
1:	subs	r1, r1, #8
	vld1.64	{d16}, [r0,:64]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 256 bytes per loop. */
	.global	read_vldm_pld_256
	.func	read_vldm_pld_256
read_vldm_pld_256:
1:
	pld	[r0, #64]
	pld	[r0, #256]
	pld	[r0, #512]
	subs	r1, r1, #256
	vldmia	r0!, {d16-d31}
	vldmia	r0!, {d16-d31}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 128 bytes per loop. */
	.global	read_vldm_pld_128
	.func	read_vldm_pld_128
read_vldm_pld_128:
1:
	pld	[r0, #64]
	pld	[r0, #256]
	pld	[r0, #320]
	subs	r1, r1, #128
	vldmia	r0!, {d16-d31}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 64 bytes per loop. */
	.global	read_vldm_pld_64
	.func	read_vldm_pld_64
read_vldm_pld_64:
1:
	pld	[r0, #320]
	subs	r1, r1, #64
	vldmia	r0!, {d0-d7}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 32 bytes per loop. */
	.global	read_vldm_pld_32
	.func	read_vldm_pld_32
read_vldm_pld_32:
1:
	pld	[r0, #320]
	subs	r1, r1, #32
	vldmia	r0!, {d0-d3}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 16 bytes per loop. */
	.global	read_vldm_pld_16
	.func	read_vldm_pld_16
read_vldm_pld_16:
1:
	pld	[r0, #320]
	subs	r1, r1, #16
	vldmia	r0!, {d0-d1}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 8 bytes per loop. */
	.global	read_vldm_pld_8
	.func	read_vldm_pld_8
read_vldm_pld_8:
1:
	pld	[r0, #320]
	subs	r1, r1, #8
	vldmia	r0!, {d0}
	bgt	1b
	bx	lr
	.endfunc

/* read memory 256 bytes per loop. */
	.global	read_vld1_pld_256
	.func	read_vld1_pld_256
read_vld1_pld_256:
1:
	pld	[r0, #64]
	pld	[r0, #256]
	pld	[r0, #320]
	subs	r1, r1, #256
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 128 bytes per loop. */
	.global	read_vld1_pld_128
	.func	read_vld1_pld_128
read_vld1_pld_128:
1:
	pld	[r0, #64]
	pld	[r0, #256]
	pld	[r0, #320]
	subs	r1, r1, #128
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 64 bytes per loop. */
	.global	read_vld1_pld_64
	.func	read_vld1_pld_64
read_vld1_pld_64:
1:
	pld	[r0, #320]
	subs	r1, r1, #64
	vld1.64	{d16-d19}, [r0,:256]!
	vld1.64	{d16-d19}, [r0,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 32 bytes per loop. */
	.global	read_vld1_pld_32
	.func	read_vld1_pld_32
read_vld1_pld_32:
1:
	pld	[r0, #320]
	subs	r1, r1, #32
	vld1.64	{d16-d19}, [r0,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 16 bytes per loop. */
	.global	read_vld1_pld_16
	.func	read_vld1_pld_16
read_vld1_pld_16:
1:
	pld	[r0, #320]
	subs	r1, r1, #16
	vld1.64	{d16,d17}, [r0,:128]!
	bgt	1b
	bx	lr
	.endfunc

/* read memory 8 bytes per loop. */
	.global	read_vld1_pld_8
	.func	read_vld1_pld_8
read_vld1_pld_8:
1:
	pld	[r0, #320]
	subs	r1, r1, #8
	vld1.64	{d16}, [r0,:64]!
	bgt	1b
	bx	lr
	.endfunc

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
