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

#ifdef PLD
#define ENTRY(name, bytes) \
	.global	name##_pld_##bytes; \
	.func	name##_pld_##bytes; \
	name##_pld_##bytes:
#else
#define ENTRY(name, bytes) \
	.global	name##_##bytes; \
	.func	name##_##bytes; \
	name##_##bytes:
#endif
#define ENDPROC \
	.endfunc

ENTRY(memset_vstm, 256)
	mov	r3, r0
	vdup.8	q8, r1
	vmov	q9, q8
	vmov	q10, q8
	vmov	q11, q8
	vmov	q12, q8
	vmov	q13, q8
	vmov	q14, q8
	vmov	q15, q8
1:
#ifdef PLD
	pld	[r3, #320]
#endif
	subs	r2, r2, #256
	.rept	2
	vstmia	r3!, {d16-d31}
	.endr
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vstm, 128)
	mov	r3, r0
	vdup.8	q8, r1
	vmov	q9, q8
	vmov	q10, q8
	vmov	q11, q8
	vmov	q12, q8
	vmov	q13, q8
	vmov	q14, q8
	vmov	q15, q8
1:
#ifdef PLD
	pld	[r3, #64]
#endif
	subs	r2, r2, #128
	vstmia	r3!, {d16-d31}
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vstm, 64)
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
	vdup.8	q2, r1
	vdup.8	q3, r1
1:
#ifdef PLD
	pld	[r3, #64]
#endif
	subs	r2, r2, #64
	vstmia	r3!, {d0-d7}
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vstm, 32)
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
1:
#ifdef PLD
	pld	[r3, #64]
#endif
	subs	r2, r2, #32
	vstmia	r3!, {d0-d3}
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vstm, 16)
	mov	r3, r0
	vdup.8	q0, r1
1:
#ifdef PLD
	pld	[r3, #64]
#endif
	subs	r2, r2, #16
	vstmia	r3!, {d0-d1}
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vstm, 8)
	mov	r3, r0
	vdup.8	d0, r1
1:
#ifdef PLD
	pld	[r3, #64]
#endif
	subs	r2, r2, #8
	vstmia	r3!, {d0}
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vst1, 256)
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
1:
#ifdef PLD
	pld	[r3, #320]
#endif
	subs	r2, r2, #256
	.rept	8
	vst1.64	{d0-d3}, [r3,:256]!
	.endr
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vst1, 128)
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
1:
#ifdef PLD
	pld	[r3, #64]
#endif
	subs	r2, r2, #128
	.rept	4
	vst1.64	{d0-d3}, [r3,:256]!
	.endr
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vst1, 64)
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
1:
#ifdef PLD
	pld	[r3, #64]
#endif
	subs	r2, r2, #64
	.rept	2
	vst1.64	{d0-d3}, [r3,:256]!
	.endr
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vst1, 32)
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
1:
#ifdef PLD
	pld	[r3, #64]
#endif
	subs	r2, r2, #32
	vst1.64	{d0-d3}, [r3, :256]!
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vst1, 16)
	mov	r3, r0
	vdup.8	q0, r1
1:
#ifdef PLD
	pld	[r3, #64]
#endif
	subs	r2, r2, #16
	vst1.64	{d0-d1}, [r3, :128]!
	bgt	1b
	bx	lr
ENDPROC

ENTRY(memset_vst1, 8)
	mov	r3, r0
	vdup.8	d0, r1
1:
#ifdef PLD
	pld	[r3, #64]
#endif
	subs	r2, r2, #8
	vst1.64	{d0}, [r3, :64]!
	bgt	1b
	bx	lr
ENDPROC

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
