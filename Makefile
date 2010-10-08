all: sp-mem-throughput

CCFLAGS := -I$(PWD) -Wall -Wextra -std=gnu99 -O2 -g $(CCFLAGS)
LDFLAGS += -lrt

COMMON_SRCS=
ARCH_SRCS=
ARCH_OBJS=

COMMON_SRCS += atoll.c
COMMON_SRCS += blocks.c
COMMON_SRCS += cpu.c
COMMON_SRCS += csv.c
COMMON_SRCS += libc-routine-register.c
COMMON_SRCS += main.c
COMMON_SRCS += routine-desc.c
COMMON_SRCS += routines.c
COMMON_SRCS += sched-util.c
COMMON_SRCS += validate.c
COMMON_SRCS += directfb-memcpy.c

MACHINE := $(shell uname -m)
ifeq ($(MACHINE),arm)
CCFLAGS := -mfpu=neon $(CCFLAGS)
ARCH_OBJS += arch/arm/linux-arm-memchr.o
ARCH_OBJS += arch/arm/linux-arm-memcpy.o
ARCH_OBJS += arch/arm/linux-arm-memmove.o
ARCH_OBJS += arch/arm/linux-arm-memset.o
ARCH_OBJS += arch/arm/arm-asm-routines.o
ARCH_OBJS += arch/arm/memset-neon.o
ARCH_OBJS += arch/arm/memread-neon.o
ARCH_OBJS += arch/arm/memcpy-neon.o
ARCH_OBJS += arch/arm/memcpy-neon-unaligned.o
ARCH_OBJS += arch/arm/directfb-memcpy.o
ARCH_SRCS += arch/arm/kernel-routines-register.c
ARCH_SRCS += arch/arm/memset-neon-register.c
ARCH_SRCS += arch/arm/memread-neon-register.c
ARCH_SRCS += arch/arm/memcpy-neon-register.c
ARCH_SRCS += arch/arm/directfb-memcpy.c

arch/arm/memcpy-neon-unaligned.o: arch/arm/memcpy-neon.S
	$(CC) $(CCFLAGS) -DENABLE_UNALIGNED_MEM_ACCESSES -c $< -o $@
else
endif

sp-mem-throughput: $(COMMON_SRCS) $(ARCH_SRCS) $(ARCH_OBJS)
	$(CC) $(CCFLAGS) $(LDFLAGS) $+ -o $@

arch/arm/arm-asm-routines.S: arch/arm/gen-arm-functions.py
	cd $(PWD)/arch/arm && ./gen-arm-functions.py

arch/arm/%.o: arch/arm/%.S
	$(CC) $(CCFLAGS) -D__ASSEMBLY__ -c $< -o $@

install: sp-mem-throughput
	install -D $< $(DESTDIR)/usr/bin/$<

clean:
	rm -f $(ARCH_OBJS) arch/arm/arm-asm-routines.h arch/arm/arm-asm-routines.S arch/arm/arm-asm-routines-register.c

distclean: clean
	rm -f sp-mem-throughput
