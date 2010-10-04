#!/usr/bin/python
#
# Generates ARM assembler memcpy, memset and memory read routines for
# benchmarking purposes.
#
# Copyright (C) 2005-2010 Nokia.
#
# Authors: Simo Piiroinen
#          Tommi Rantala
#
# Contact: Eero Tamminen <eero.tamminen@nokia.com>

import sys,string

prologue = "/* AUTOGENERATED CODE - DO NOT EDIT */\n"

# We do not need executable stack.
epilogue = """
#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
"""

c_header_read = "void %s(const void *, size_t);"
register_read = "ROUTINE_REGISTER_MEMREAD(%s,\n\t\"ARM assembler read routine with %s byte reads\")"
template_read = """
/*******************************************************************/
	.global	<FUNC>
	.type	<FUNC>, %function
<FUNC>:
	stmfd	sp!, {r4-r8, lr}
	mov	lr,r0
	mov	r8,r1
<LOOP>:	subs	r8,r8,#32
	@ldr	r7,[lr, #32]@prefetch
<BODY>
	bne	<LOOP>
	ldmfd	sp!, {r4-r8, pc}
	.size	<FUNC>, .-<FUNC>
"""

c_header_write = "void *%s(void *, int, size_t);"
register_write = "ROUTINE_REGISTER_MEMSET(%s,\n\t\"ARM assembler memset routine with %s byte writes\")"
template_write = """
/*******************************************************************/
	.global	<FUNC>
	.type	<FUNC>, %function
<FUNC>:
	stmfd	sp!, {r4-r8, lr}
	mov	ip,r0
	mov	r8,r2
<LOOP>:	subs	r8,r8,#32
	@ldr	r7,[lr, #32]@prefetch
<BODY>
	bne	<LOOP>
	ldmfd	sp!, {r4-r8, pc}
	.size	<FUNC>, .-<FUNC>
"""

c_header_copy = "void *%s(void *restrict, const void *restrict, size_t);"
register_copy = "ROUTINE_REGISTER_MEMCPY(%s,\n\t\"ARM assembler memcpy routine, %s byte processing\")"
template_copy = """
/*******************************************************************/
	.global <FUNC>
	.type   <FUNC>, %function
<FUNC>:
	stmfd   sp!, {r4-r8, lr}
	mov ip,r0
	mov lr,r1
	mov r8,r2
<LOOP>:	subs r8,r8,#32
	@ldr r7,[lr, #32]@prefetch
<BODY>
	bne <LOOP>
	ldmfd   sp!, {r4-r8, pc}
	.size   <FUNC>, .-<FUNC>
"""

# ----------------------------------------------------------------
# read & write ops
# ----------------------------------------------------------------

rd32 = "\tldmia lr!,{r0-r7}"
wr32 = "\tstmia ip!,{r0-r7}"

rd16 = "\tldmia lr!,{r0-r3}"
wr16 = "\tstmia ip!,{r0-r3}"

rd8 = "\tldmia lr!,{r0-r1}"
wr8 = "\tstmia ip!,{r0-r1}"

rd4 = "\tldr r0,[lr],#4"
wr4 = "\tstr r0,[ip],#4"

rd2 = "\tldrh r0,[lr],#2"
wr2 = "\tstrh r0,[ip],#2"

rd1 = "\tldrb r0,[lr],#1"
wr1 = "\tstrb r0,[ip],#1"

rd = { 32:rd32, 16:rd16, 8:rd8, 4:rd4, 2:rd2, 1:rd1 }
wr = { 32:wr32, 16:wr16, 8:wr8, 4:wr4, 2:wr2, 1:wr1 }


# ----------------------------------------------------------------
# actual code
# ----------------------------------------------------------------

if __name__ == "__main__":
    try:
        base = sys.argv[1]
    except IndexError:
        base = "arm-asm-routines"

    asrc = [prologue]
    hsrc = [prologue]
    hsrc.append("#ifndef ARM_ASM_ROUTINES_H\n#define ARM_ASM_ROUTINES_H\n\n#include <stddef.h>\n")
    regfile = [prologue]
    regfile.append('#ifdef __arm__')
    regfile.append('')
    regfile.append('#include "%s.h"' % base)
    regfile.append('#include "routine.h"')
    regfile.append('')

    l = 0

    for n in (32,16,8,4,2,1):
        r = [rd[n]]
        w = [wr[n]]
        c = r + w

        m = 0
        if n: m=32/n

        for f,b,r,s,h,reg in (("memcpy",c,2,template_copy,c_header_copy,register_copy),\
	                      ("memset",w,1,template_write,c_header_write,register_write),\
			      ("memread",r,1,template_read,c_header_read,register_read)):
            if m == 0: r = 0
            f = "arm_asm_%s_%d" % (f,n)
            l = l + 1
            b = string.join(b*m, "\n")
            s = string.replace(s, "<FUNC>", f)
            s = string.replace(s, "<LOOP>", ".L%d"%l)
            s = string.replace(s, "<BODY>", b)
            asrc.append(s)
	    hsrc.append(h % f)
	    regfile.append(reg % (f, n))

	hsrc.append('')

    asrc.append(epilogue)
    asrc = string.join(asrc,'')

    hsrc.append("#endif /* ARM_ASM_ROUTINES_H */")
    hsrc.append('')
    hsrc = string.join(hsrc,'\n')

    regfile.append('');
    regfile.append('#endif /* __arm__ */')
    regfile.append('');
    regfile = string.join(regfile,'\n')

    open(base + ".S", "w").write(asrc)
    open(base + ".h", "w").write(hsrc)
    open(base + "-register.c", "w").write(regfile)