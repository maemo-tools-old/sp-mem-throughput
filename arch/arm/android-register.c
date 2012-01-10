#ifdef __ARM_NEON__

#include "routine.h"

void *android_memset(void *, int, size_t);
void *android_memcpy(void *, const void *, size_t);
char *android_strcpy(char *, const char *);
int android_strcmp(const char *, const char *);
int android_memcmp(const void *, const void *, size_t);

ROUTINE_REGISTER_MEMSET(android_memset, "Android bionic memset")
ROUTINE_REGISTER_MEMCPY(android_memcpy, "Android bionic memcpy")
ROUTINE_REGISTER_STRCPY(android_strcpy, "Android bionic strcpy")
ROUTINE_REGISTER_STRCMP(android_strcmp, "Android bionic strcmp")

#ifndef __thumb__
size_t android_strlen(const char *);
ROUTINE_REGISTER_STRLEN(android_strlen, "Android bionic strlen")
#endif

#endif /* __ARM_NEON__ */
