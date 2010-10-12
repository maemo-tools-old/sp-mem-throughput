/* Best case memcpy() routines. */

	.fpu		neon
	.text

	.global		memcpy_ldm_stm_32
	.func		memcpy_ldm_stm_32
memcpy_ldm_stm_32:
	push		{r4-r11}
	mov		r3, r0
1:	subs		r2, r2, #32
	pld		[r1, #64]
	pld		[r1, #256]
	pld		[r1, #320]
	ldm		r1!, {r4-r11}
	stm		r3!, {r4-r11}
	bgt		1b
	pop		{r4-r11}
	bx		lr
	.endfunc

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
