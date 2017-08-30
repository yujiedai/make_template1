ifeq ($(CROSS_COMPILE),)
CROSS_COMPILE=/auto/lootom/toolchain/usr/bin/arm-linux-
endif
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdum

CFLAGS=-I./tools/cunit/arm-linux/include

CPPFLAGS=

LD_PATH=tools/cunit/arm-linux/lib

LD_FLAGS=-lcunit

ifneq ($(LD_PATH),)
LD_FLAGS+=-L$(LD_PATH)
endif
