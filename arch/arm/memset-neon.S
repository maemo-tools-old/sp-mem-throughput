/* Idealized memset routines that utilize ARM NEON instructions.
 *
 * This file is part of sp-mem-throughput.
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

/* C prototypes:
 *    void *memset_vstm_{8,16,32,64,128,256}(void *, int, size_t);
 *    void *memset_vstm_pld_{8,16,32,64,128,256}(void *, int, size_t);
 *    void *memset_vst1_{8,16,32,64,128,256}(void *, int, size_t);
 *    void *memset_vst1_pld_{8,16,32,64,128,256}(void *, int, size_t);
 */

/* 256 bytes per loop. */
	.global memset_vstm_256
	.func   memset_vstm_256
memset_vstm_256:
	mov	r3, r0    /* return original pointer in r0 */
	vdup.8	q8, r1
	vmov	q9, q8
	vmov	q10, q8
	vmov	q11, q8
	vmov	q12, q8
	vmov	q13, q8
	vmov	q14, q8
	vmov	q15, q8
1:	subs	r2, r2, #256
	vstmia	r3!, {d16-d31}
	vstmia	r3!, {d16-d31}
	bgt	1b
	bx	lr
	.endfunc

/* 128 bytes per loop. */
	.global memset_vstm_128
	.func   memset_vstm_128
memset_vstm_128:
	mov	r3, r0    /* return original pointer in r0 */
	vdup.8	q8, r1
	vmov	q9, q8
	vmov	q10, q8
	vmov	q11, q8
	vmov	q12, q8
	vmov	q13, q8
	vmov	q14, q8
	vmov	q15, q8
1:	subs	r2, r2, #128
	vstmia	r3!, {d16-d31}
	bgt	1b
	bx	lr
	.endfunc

/* 64 bytes per loop. */
	.global memset_vstm_64
	.func   memset_vstm_64
memset_vstm_64:
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
	vdup.8	q2, r1
	vdup.8	q3, r1
1:	subs	r2, r2, #64
	vst1.8	{d0-d3}, [r3, :256]!
	vst1.8	{d0-d3}, [r3, :256]!
	bgt	1b
	bx	lr
	.endfunc

/* 32 bytes per loop. */
	.global memset_vstm_32
	.func   memset_vstm_32
memset_vstm_32:
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
1:	subs	r2, r2, #32
	vstmia	r3!, {d0-d3}
	bgt	1b
	bx	lr
	.endfunc

/* 16 bytes per loop. */
	.global memset_vstm_16
	.func   memset_vstm_16
memset_vstm_16:
	vdup.8	q0, r1
	mov	r3, r0
1:	subs	r2, r2, #16
	vstmia	r3!, {d0-d1}
	bgt	1b
	bx	lr
	.endfunc

/* 8 bytes per loop. */
	.global memset_vstm_8
	.func   memset_vstm_8
memset_vstm_8:
	vdup.8	d0, r1
	mov	r3, r0
1:	subs	r2, r2, #8
	vstmia	r3!, {d0}
	bgt	1b
	bx	lr
	.endfunc

/* 256 bytes per loop. */
	.global memset_vstm_pld_256
	.func   memset_vstm_pld_256
memset_vstm_pld_256:
	mov	r3, r0    /* return original pointer in r0 */
	vdup.8	q8, r1
	vmov	q9, q8
	vmov	q10, q8
	vmov	q11, q8
	vmov	q12, q8
	vmov	q13, q8
	vmov	q14, q8
	vmov	q15, q8
1:	pld	[r3, #320]
	subs	r2, r2, #256
	vstmia	r3!, {d16-d31}
	vstmia	r3!, {d16-d31}
	bgt	1b
	bx	lr
	.endfunc

/* 128 bytes per loop. */
	.global memset_vstm_pld_128
	.func   memset_vstm_pld_128
memset_vstm_pld_128:
	mov	r3, r0    /* return original pointer in r0 */
	vdup.8	q8, r1
	vmov	q9, q8
	vmov	q10, q8
	vmov	q11, q8
	vmov	q12, q8
	vmov	q13, q8
	vmov	q14, q8
	vmov	q15, q8
1:	pld	[r3, #64]
	subs	r2, r2, #128
	vstmia	r3!, {d16-d31}
	bgt	1b
	bx	lr
	.endfunc

/* 64 bytes per loop. */
	.global memset_vstm_pld_64
	.func   memset_vstm_pld_64
memset_vstm_pld_64:
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
	vdup.8	q2, r1
	vdup.8	q3, r1
1:	pld	[r3, #64]
	subs	r2, r2, #64
	vstmia	r3!, {d0-d7}
	bgt	1b
	bx	lr
	.endfunc

/* 32 bytes per loop. */
	.global memset_vstm_pld_32
	.func   memset_vstm_pld_32
memset_vstm_pld_32:
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
1:	pld	[r3, #64]
	subs	r2, r2, #32
	vstmia	r3!, {d0-d3}
	bgt	1b
	bx	lr
	.endfunc

/* 16 bytes per loop. */
	.global memset_vstm_pld_16
	.func   memset_vstm_pld_16
memset_vstm_pld_16:
	vdup.8	q0, r1
	mov	r3, r0
1:	pld	[r3, #64]
	subs	r2, r2, #16
	vstmia	r3!, {d0-d1}
	bgt	1b
	bx	lr
	.endfunc

/* 8 bytes per loop. */
	.global memset_vstm_pld_8
	.func   memset_vstm_pld_8
memset_vstm_pld_8:
	mov	r3, r0
1:	pld	[r3, #64]
	subs	r2, r2, #8
	vstmia	r3!, {d0}
	bgt	1b
	bx	lr
	.endfunc

/* 256 bytes per loop. */
	.global memset_vst1_256
	.func   memset_vst1_256
memset_vst1_256:
	mov	r3, r0    /* return original pointer in r0 */
	vdup.8	q8, r1
	vdup.8	q9, r1
1:	subs	r2, r2, #256
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* 128 bytes per loop. */
	.global memset_vst1_128
	.func   memset_vst1_128
memset_vst1_128:
	mov	r3, r0    /* return original pointer in r0 */
	vdup.8	q8, r1
	vdup.8	q9, r1
1:	subs	r2, r2, #128
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* 64 bytes per loop. */
	.global memset_vst1_64
	.func   memset_vst1_64
memset_vst1_64:
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
	vdup.8	q2, r1
	vdup.8	q3, r1
1:	subs	r2, r2, #64
	vst1.64	{d0-d3}, [r3, :256]!
	vst1.64	{d0-d3}, [r3, :256]!
	bgt	1b
	bx	lr
	.endfunc

/* 32 bytes per loop. */
	.global memset_vst1_32
	.func   memset_vst1_32
memset_vst1_32:
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
1:	subs	r2, r2, #32
	vst1.64	{d0-d3}, [r3, :256]!
	bgt	1b
	bx	lr
	.endfunc

/* 16 bytes per loop. */
	.global memset_vst1_16
	.func   memset_vst1_16
memset_vst1_16:
	vdup.8	q0, r1
	mov	r3, r0
1:	subs	r2, r2, #16
	vst1.64	{d0-d1}, [r3, :128]!
	bgt	1b
	bx	lr
	.endfunc

/* 8 bytes per loop. */
	.global memset_vst1_8
	.func   memset_vst1_8
memset_vst1_8:
	vdup.8	d0, r1
	mov	r3, r0
1:	subs	r2, r2, #8
	vst1.64	{d0}, [r3, :64]!
	bgt	1b
	bx	lr
	.endfunc

/* 256 bytes per loop. */
	.global memset_vst1_pld_256
	.func   memset_vst1_pld_256
memset_vst1_pld_256:
	mov	r3, r0    /* return original pointer in r0 */
	vdup.8	q8, r1
	vdup.8	q9, r1
1:	pld	[r3, #320]
	subs	r2, r2, #256
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* 128 bytes per loop. */
	.global memset_vst1_pld_128
	.func   memset_vst1_pld_128
memset_vst1_pld_128:
	mov	r3, r0    /* return original pointer in r0 */
	vdup.8	q8, r1
	vdup.8	q9, r1
1:	pld	[r3, #64]
	subs	r2, r2, #128
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* 64 bytes per loop. */
	.global memset_vst1_pld_64
	.func   memset_vst1_pld_64
memset_vst1_pld_64:
	mov	r3, r0
	vdup.8	q8, r1
	vdup.8	q9, r1
1:	pld	[r3, #64]
	subs	r2, r2, #64
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* 32 bytes per loop. */
	.global memset_vst1_pld_32
	.func   memset_vst1_pld_32
memset_vst1_pld_32:
	mov	r3, r0
	vdup.8	q8, r1
1:	pld	[r3, #64]
	subs	r2, r2, #32
	vst1.64	{d16-d19}, [r3,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* 16 bytes per loop. */
	.global memset_vst1_pld_16
	.func   memset_vst1_pld_16
memset_vst1_pld_16:
	mov	r3, r0
	vdup.8	q0, r1
1:	pld	[r3, #64]
	subs	r2, r2, #16
	vst1.64	{d0-d1}, [r3, :128]!
	bgt	1b
	bx	lr
	.endfunc

/* 8 bytes per loop. */
	.global memset_vst1_pld_8
	.func   memset_vst1_pld_8
memset_vst1_pld_8:
	mov	r3, r0
	vdup.8	d0, r1
1:	pld	[r3, #64]
	subs	r2, r2, #8
	vst1.64	{d0}, [r3, :64]!
	bgt	1b
	bx	lr
	.endfunc

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
