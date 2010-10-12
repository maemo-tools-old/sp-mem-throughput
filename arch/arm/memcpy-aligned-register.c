#ifdef __ARM_NEON__

#include "routine.h"

void *memcpy_vld1_vst1_32(void *, const void *, size_t);
void *memcpy_vld1_vst1_64(void *, const void *, size_t);
void *memcpy_vld1_vst1_128(void *, const void *, size_t);
void *memcpy_ldm_stm_32(void *, const void *, size_t);
void *memcpy_neon_arm_128(void *, const void *, size_t);

ROUTINE_REGISTER_MEMCPY(memcpy_vld1_vst1_32,  "VLD1+VST1 memcpy, 32 bytes per loop")
ROUTINE_REGISTER_MEMCPY(memcpy_vld1_vst1_64,  "VLD1+VST1 memcpy, 64 bytes per loop")
ROUTINE_REGISTER_MEMCPY(memcpy_vld1_vst1_128, "VLD1+VST1 memcpy, 128 bytes per loop")
ROUTINE_REGISTER_MEMCPY(memcpy_ldm_stm_32,    "LDM+STM memcpy, 32 bytes per loop")
ROUTINE_REGISTER_MEMCPY(memcpy_neon_arm_128,  "NEON+ARM memcpy, 128 bytes per loop")

#endif /* __ARM_NEON__ */
