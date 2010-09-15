#ifdef __arm__

#include "routine.h"

void *linux_arm_memchr(const void *, int, size_t);
void *linux_arm_memcpy(void *restrict, const void *restrict, size_t);
void *linux_arm_memmove(void *, const void *, size_t);
void *linux_arm_memset(void *, int, size_t);

ROUTINE_REGISTER_MEMCHR(linux_arm_memchr,
		"ARM assembler memchr from linux-2.6")
ROUTINE_REGISTER_MEMCPY(linux_arm_memcpy,
		"ARM assembler memcpy from linux-2.6")
ROUTINE_REGISTER_MEMCPY(linux_arm_memmove,
		"ARM assembler memmove from linux-2.6")
ROUTINE_REGISTER_MEMSET(linux_arm_memset,
		"ARM assembler memset from linux-2.6")

#endif /* __arm__ */
