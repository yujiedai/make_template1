CROSS_COMPILE=
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdum



CFLAGS= -I./tools/cunit/sim-x86/include

CPPFLAGS=

LD_PATH= tools/cunit/sim-x86/lib

LD_FLAGS= -lcunit

ifneq ($(LD_PATH),)
LD_FLAGS+=-L$(LD_PATH)
endif


OS=linux
ARCH=x86_64

