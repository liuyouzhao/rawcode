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

$(shell sleep 1)
$(info )
$(info Begin to compile...)
$(shell sleep 1)

# Makefile main logic
ROOT=$(shell pwd)
CC=$(ROOT)/bin/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-gcc
AS=$(ROOT)/bin/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-as
LD=$(ROOT)/bin/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-ld
OC=$(ROOT)/bin/$(PLATFORM)/bin/$(PLATFORM)-none-eabi-objcopy
LDIR=$(ROOT)/bin/$(PLATFORM)/$(PLATFORM)-none-eabi/lib
LDIR2=$(ROOT)/bin/$(PLATFORM)/lib/gcc/$(PLATFORM)-none-eabi/4.9.3

AFLAGS=-mcpu=arm926ej-s
CFLAGS=-mcpu=arm926ej-s  \
       -nostdlib -g

LDFLAGS = -L$(LDIR) \
          -L$(LDIR2) \
	  -lc -lgcc -lm 

OBJS = timer.o init.o

.PHONY: clean

clean:
	rm -rf ./*.o ./output.bin ./output.elf
	
$(shell $(AS) $(AFLAGS) startup.s -o startup.o)

all: COMPILE RAW
COMPILE: $(OBJS)
RAW:
	$(LD) -T linker.ld $(OBJS) -o output.elf $(LDFLAGS)
	$(OC) -O binary output.elf output.bin
