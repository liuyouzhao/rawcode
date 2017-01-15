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
# Prefix platform
PLATFORM=arm
BUILD=build

# Makefile main logic
ROOT=$(shell pwd)
PRE_ROOT=$(ROOT)/../
CC    =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-gcc
AS    =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-as
LD    =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-ld
OC    =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-objcopy
LDIR  =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/$(PLATFORM)-none-eabi/lib
LDIR2 =$(ROOT)/../$(BUILD)/toolchain/$(PLATFORM)/lib/gcc/$(PLATFORM)-none-eabi/4.9.3

OUT_DIR=../out/$(PLATFORM)

include filelist.mk
include config.mk

AFLAGS=-mcpu=$(CPU)
CFLAGS=-mcpu=$(CPU)  \
       -nostdlib -g \
       $(C_CFLAGS)

LDFLAGS = -L$(LDIR) \
          -L$(LDIR2) \
	  -lc -lgcc -lm 


C_OBJS =
S_OBJS =

$(shell mkdir $(OUT_DIR) -p)
$(foreach c, $(C_FILES), \
	$(shell mkdir $(OUT_DIR)/$(c:%c=%dump) -p) \
	$(shell $(CC) -o $(OUT_DIR)/$(c:%c=%o) -c $c $(CFLAGS)) \
	$(eval C_OBJS += $(OUT_DIR)/$(c:%c=%o)) )
$(foreach s, $(S_FILES), \
	$(shell mkdir $(OUT_DIR)/$(s:%s=%dump) -p) \
	$(shell $(AS) $(AFLAGS) $(s) -o $(OUT_DIR)/$(s:%s=%o)) \
	$(eval S_OBJS += $(OUT_DIR)/$(s:%s=%o)) )
$(shell cp $(LD_LINKER) $(OUT_DIR)/$(LD_LINKER))



all: compile raw
compile: $(C_OBJS)
raw:
	$(LD) -T $(OUT_DIR)/$(LD_LINKER) $(C_OBJS) $(S_OBJS) -o $(OUT_DIR)/output.elf $(LDFLAGS)
	$(OC) -O binary $(OUT_DIR)/output.elf $(PRE_ROOT)/output.bin


.PHONY: clean
clean:
	rm -rf ./out ./output.bin
