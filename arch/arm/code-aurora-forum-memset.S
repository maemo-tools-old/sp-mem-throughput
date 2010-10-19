/***************************************************************************
 Copyright (c) 2009, Code Aurora Forum. All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Code Aurora nor the names of its contributors may
       be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

/***************************************************************************
  Neon memset: Attempts to do a memset with Neon registers if possible,
     Inputs:
	s: The buffer to write to
	c: The integer data to write to the buffer
	n: The size_t count.
     Outputs:

***************************************************************************/

	.code 32
	.align 4
	.globl caf_memset
	.func

caf_memset:
#if defined __GNUC__ && (4 == __GNUC__ && 1 == __GNUC_MINOR__ && 1 == __GNUC_PATCHLEVEL__)
	stmdb		sp!, {r0}
#else
	push		{r0}
#endif
	/* If we have < 4 bytes, just do a quick loop to handle that */
	cmp		r2, #8
	bgt		memset_gt4
	cmp		r2, #0
	beq		memset_smallcopy_done
memset_smallcopy_loop:
	strb		r1, [r0], #1
	subs		r2, r2, #1
	bne		memset_smallcopy_loop
memset_smallcopy_done:
#if defined __GNUC__ && (4 == __GNUC__ && 1 == __GNUC_MINOR__ && 1 == __GNUC_PATCHLEVEL__)
	ldmia		sp!, {r0}
#else
	pop		{r0}
#endif
	bx		lr
	
memset_gt4:
	/* Check if the target is word-aligned. */
	ands		r12, r0, #0x3
	beq		memset_word_aligned
	/* Not word-aligned.  We'll scoot forward by the # of bytes needed
	 * to get to a word-aligned boundary.
	 */
	rsb		r12, r12, #4
	sub		r2, r2, r12
	lsls		r12, r12, #31
	strmib		r1, [r0], #1
	strcsb		r1, [r0], #1
	strcsb		r1, [r0], #1
	cmp		r2, #4
	blt		memset_1
memset_word_aligned:
	/* Duplicate the r1 lowest byte across r1.  The idea is to have
	 * a register with 4 byte-values we can copy.  We do this by
	 * copying the lowest 8-bits to r1, then duplicating that for
	 * the next 8, then the upper 16-bits.
	 */
	and		r1, r1, #0xff
	orr		r1, r1, r1, lsl #8
	orr		r1, r1, r1, lsl #16

	/*
	 * If we're copying > 16 bytes, then we may want to get
	 * onto a 16-byte boundary to improve speed even more.
	 */
	cmp		r2, #64
	blt		memset_route
	ands		r12, r0, #0xf
	beq		memset_route
	/* Determine the # of bytes to move forward to get to the 16-byte
	 * boundary.  Note that this will be a multiple of 4, since we
	 * already are word-aligned.
	 */
	rsb		r12, r12, #16
	sub		r2, r2, r12
	lsls		r12, r12, #29
	strmi		r1, [r0], #4
	strcs		r1, [r0], #4
	strcs		r1, [r0], #4
memset_route:
	/*
	 * Decide where to route for the maximum copy sizes.  Note that we
	 * build q0 and q1 depending on if we'll need it, so that's
	 * interwoven here as well.
	 */
	vdup.u32	d0, r1
	cmp		r2, #16
	blt		memset_8
	vmov		d1, d0
	cmp		r2, #64
	blt		memset_16
	vmov		q1, q0
	cmp		r2, #128
	blt		memset_32
memset_128:
	mov		r12, r2, lsr #7
memset_128_loop:
	vst1.32		{q0, q1}, [r0]!
	vst1.32		{q0, q1}, [r0]!
	vst1.32		{q0, q1}, [r0]!
	vst1.32		{q0, q1}, [r0]!
	subs		r12, r12, #1
	pld		[r0, #0]
	pld		[r0, #128]
	bne		memset_128_loop
	ands		r2, r2, #0x7f
	beq		memset_end
memset_32:
	movs		r12, r2, lsr #5
	beq		memset_16
memset_32_loop:
	subs		r12, r12, #1
	vst1.32		{q0, q1}, [r0]!
	bne		memset_32_loop
	ands		r2, r2, #0x1f
	beq		memset_end
memset_16:
	movs		r12, r2, lsr #4
	beq		memset_8
memset_16_loop:
	subs		r12, r12, #1
	vst1.32		{q0}, [r0]!
	bne		memset_16_loop
	ands		r2, r2, #0xf
	beq		memset_end
	/* memset_8 isn't a loop, since we try to do our loops at 16
	 * bytes and above.  We should loop there, then drop down here
	 * to finish the <16-byte versions.  Same for memset_4 and
	 * memset_1.
	 */
memset_8:
	cmp		r2, #8
	blt		memset_4
	subs		r2, r2, #8
	vst1.32		{d0}, [r0]!
memset_4:
	cmp		r2, #4
	blt		memset_1
	subs		r2, r2, #4
	str		r1, [r0], #4
memset_1:
	cmp		r2, #0
	beq		memset_end
	lsls		r2, #31
	strmib		r1, [r0], #1
	strcsb		r1, [r0], #1
	strcsb		r1, [r0], #1
memset_end:
#if defined __GNUC__ && (4 == __GNUC__ && 1 == __GNUC_MINOR__ && 1 == __GNUC_PATCHLEVEL__)
	ldmia		sp!, {r0}
#else
	pop		{r0}
#endif
	bx		lr

	.endfunc

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
