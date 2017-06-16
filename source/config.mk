# Prefix platform
PLATFORM    = arm
BOARD       = qemu
CPU         = arm926ej-s

BUILD       = build

# Makefile main logic
ROOT=$(shell pwd)
PRE_ROOT=$(ROOT)/../
CC    =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-gcc
AS    =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-as
LD    =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-ld
OC    =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-objcopy
LDIR  =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/$(PLATFORM)-none-eabi/lib
LDIR2 =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/lib/gcc/$(PLATFORM)-none-eabi/6.3.1

OUT_DIR=../out/$(PLATFORM)
OUT_BIN = output


C_CFLAGS += -Iarch/$(PLATFORM)/$(BOARD)/include
#C_CFLAGS += -I./
C_CFLAGS += -I./ -Os

AFLAGS=-mcpu=$(CPU)
CFLAGS=-mcpu=$(CPU)  \
       -nostdlib -g \
       $(C_CFLAGS)

LDFLAGS = -L$(LDIR) \
          -L$(LDIR2) \
	  -lc -lgcc -lm 
