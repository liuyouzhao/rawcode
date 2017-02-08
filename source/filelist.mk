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

### C files
C_UTIL_FILES = utils/debug.c

C_ARCH_FILES = \
               arch/$(PLATFORM)/$(BOARD)/init.c \
               arch/$(PLATFORM)/$(BOARD)/irq.c \
               arch/$(PLATFORM)/$(BOARD)/fiq.c \
               arch/$(PLATFORM)/$(BOARD)/timer.c \
               arch/$(PLATFORM)/$(BOARD)/arch.c \
               arch/$(PLATFORM)/$(BOARD)/exc.c \
               arch/$(PLATFORM)/$(BOARD)/swi.c

C_CORE_FILES = core/rc_mem.c \
               core/rc_task.c \
               core/rc_queue.c \
               core/rc_timer.c

C_TEST_FILES = test/test_mem.c

C_FILES += $(C_BOOT_FILES)
C_FILES += $(C_CORE_FILES)
C_FILES += $(C_UTIL_FILES)
C_FILES += $(C_ARCH_FILES)
C_FILES += $(C_TEST_FILES)

### ASM files
S_BOOT_FILES = arch/$(PLATFORM)/$(BOARD)/startup.s

S_CORE_FILES = arch/$(PLATFORM)/$(BOARD)/mpu.s \
               arch/$(PLATFORM)/$(BOARD)/port.s

S_FILES += $(S_BOOT_FILES)
S_FILES += $(S_CORE_FILES)

### linker file
LD_LINKER = arch/$(PLATFORM)/$(BOARD)/linker.ld
