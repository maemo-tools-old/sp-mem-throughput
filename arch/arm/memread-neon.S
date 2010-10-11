/* Naive memory reading routines that utilize ARM NEON instructions. Only
 * useful for benchmarking purposes.
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

/* C prototypes: void memread_neon_{16,32,64,128}(const void *, size_t); */

/* read memory 128 bytes per loop. */
	.global memread_neon_128
	.type   memread_neon_128, %function
memread_neon_128:
	cmp	r1, #127
	bls	memread_neon_64
	vpush	{d8-d15}
.L1:	subs	r1, r1, #128
	vldmia	r0!, {d0-d15}
	bne	.L1
	vpop	{d8-d15}
	bx	lr
	.size	memread_neon_128, .-memread_neon_128

/* read memory 64 bytes per loop. */
	.global memread_neon_64
	.type   memread_neon_64, %function
memread_neon_64:
	cmp	r1, #63
	bls	memread_neon_32
.L2:	subs	r1, r1, #64
	vldmia	r0!, {d0-d7}
	bne	.L2
	bx	lr
	.size	memread_neon_64, .-memread_neon_64

/* read memory 32 bytes per loop. */
	.global memread_neon_32
	.type   memread_neon_32, %function
memread_neon_32:
	cmp	r1, #31
	bls	memread_neon_16
.L3:	subs	r1, r1, #32
	vldmia	r0!, {d0-d3}
	bne	.L3
	bx	lr
	.size	memread_neon_32, .-memread_neon_32

/* read memory 16 bytes per loop. */
	.global memread_neon_16
	.type   memread_neon_16, %function
memread_neon_16:
	cmp	r1, #15
	bls	memread_neon_8
.L4:	subs	r1, r1, #16
	vldmia	r0!, {d0-d1}
	bne	.L4
	bx	lr
	.size	memread_neon_16, .-memread_neon_16

/* read memory 8 bytes per loop. */
	.global memread_neon_8
	.type   memread_neon_8, %function
memread_neon_8:
.L5:	subs	r1, r1, #8
	vldmia	r0!, {d0}
	bne	.L5
	bx	lr
	.size	memread_neon_8, .-memread_neon_8

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
