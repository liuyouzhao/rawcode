
##
 # Copyright (C) 2016 Rawcode Project. All rights reserved.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #   http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##
####
ROOT=$(shell pwd)/../

BUILD=build

CPU=arm926ej-s

BOARD=qemu
PLATFORM=arm

## toolchain bin path, you may download from here
## https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
TOOLCHAIN=$(ROOT)/$(BUILD)/toolchain

TOOLCHAIN_BIN=$(TOOLCHAIN)/$(PLATFORM)/bin


## toolchain declare
CC=$(TOOLCHAIN_BIN)/$(PLATFORM)-none-eabi-gcc
AS=$(TOOLCHAIN_BIN)/$(PLATFORM)-none-eabi-as
LD=$(TOOLCHAIN_BIN)/$(PLATFORM)-none-eabi-ld
OC=$(TOOLCHAIN_BIN)/$(PLATFORM)-none-eabi-objcopy
LDIR=$(TOOLCHAIN)/arm/arm-none-eabi/lib
LDIR2=$(TOOLCHAIN)/arm/lib/gcc/arm-none-eabi/4.9.3

## output dir
OUT_DIR=../$(BUILD)/out

# Makefile main logic
OUT_BIN=output

C_CFLAGS += -I$(ROOT)/source/arch/$(PLATFORM)/$(BOARD)/include
C_CFLAGS += -I./ -Os

AFLAGS=-mcpu=$(CPU)
CFLAGS=-mcpu=$(CPU)  \
       -nostdlib -g \
       $(C_CFLAGS)

LDFLAGS = -L$(LDIR) \
          -L$(LDIR2) \
	  -lc -lgcc -lm 

DEFS = -DCONFIG_ABTASK=0 -DCONFIG_DEBUG_ENABLE=1
C_CFLAGS += $(DEFS)
