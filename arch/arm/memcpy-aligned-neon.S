/* Best case memcpy() routines. */

	.fpu	neon
	.text

/* 128 bytes per loop */
	.global	memcpy_vld1_vst1_128
	.func	memcpy_vld1_vst1_128
memcpy_vld1_vst1_128:
	pld	[r1]
	mov	r3, r0
1:	pld	[r1, #64]
	pld	[r1, #256]
	pld	[r1, #320]
	vld1.64	{d0-d3},   [r1,:256]!
	vld1.64	{d4-d7},   [r1,:256]!
	vld1.64	{d16-d19}, [r1,:256]!
	vld1.64	{d20-d23}, [r1,:256]!
	subs	r2, r2, #128
	vst1.64	{d0-d3},   [r3,:256]!
	vst1.64	{d4-d7},   [r3,:256]!
	vst1.64	{d16-d19}, [r3,:256]!
	vst1.64	{d20-d23}, [r3,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* 64 bytes per loop */
	.global	memcpy_vld1_vst1_64
	.func	memcpy_vld1_vst1_64
memcpy_vld1_vst1_64:
	pld	[r1]
	mov	r3, r0
1:	pld	[r1, #256]
	pld	[r1, #320]
	vld1.64	{d0-d3},   [r1,:256]!
	vld1.64	{d4-d7},   [r1,:256]!
	subs	r2, r2, #64
	vst1.64	{d0-d3},   [r3,:256]!
	vst1.64	{d4-d7},   [r3,:256]!
	bgt	1b
	bx	lr
	.endfunc

/* 32 bytes per loop */
	.global	memcpy_vld1_vst1_32
	.func	memcpy_vld1_vst1_32
memcpy_vld1_vst1_32:
	pld	[r1]
	mov	r3, r0
1:	pld	[r1, #256]
	pld	[r1, #320]
	vld1.64	{d0-d3},   [r1,:256]!
	subs	r2, r2, #32
	vst1.64	{d0-d3},   [r3,:256]!
	bgt	1b
	bx	lr
	.endfunc

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
