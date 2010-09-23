/* Naive memset routines that utilize ARM NEON instructions.
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

/* C prototypes: void *memset_neon_{16,32,64,128}(void *, int, size_t); */

/* memset() that writes 128 bytes per loop. */
	.global memset_neon_128
	.type   memset_neon_128, %function
memset_neon_128:
	cmp	r2, #127
	bls	memset_neon_64
	mov	r3, r0    /* return original pointer in r0 */
	vpush	{d8-d15}
	vdup.8	q0, r1
	vmov	q1, q0
	vmov	q2, q0
	vmov	q3, q0
	vmov	q4, q0
	vmov	q5, q0
	vmov	q6, q0
	vmov	q7, q0
.L1:	subs	r2, r2, #128
	vstmia	r3!, {d0-d15}
	bne	.L1
	vpop	{d8-d15}
	bx	lr
	.size	memset_neon_128, .-memset_neon_128

/* memset() that writes 64 bytes per loop. */
	.global memset_neon_64
	.type   memset_neon_64, %function
memset_neon_64:
	cmp	r2, #63
	bls	memset_neon_32
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
	vdup.8	q2, r1
	vdup.8	q3, r1
.L2:	subs	r2, r2, #64
	vstmia	r3!, {d0-d7}
	bne	.L2
	bx	lr
	.size	memset_neon_64, .-memset_neon_64

/* memset() that writes 32 bytes per loop. */
	.global memset_neon_32
	.type   memset_neon_32, %function
memset_neon_32:
	cmp	r2, #31
	bls	memset_neon_16
	mov	r3, r0
	vdup.8	q0, r1
	vdup.8	q1, r1
.L3:	subs	r2, r2, #32
	vstmia	r3!, {d0-d3}
	bne	.L3
	bx	lr
	.size	memset_neon_32, .-memset_neon_32

/* memset() that writes 16 bytes per loop. */
	.global memset_neon_16
	.type   memset_neon_16, %function
memset_neon_16:
	vdup.8	q0, r1
	mov	r3, r0
.L4:	subs	r2, r2, #16
	vstmia	r3!, {d0-d1}
	bne	.L4
	bx	lr
	.size	memset_neon_16, .-memset_neon_16

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
