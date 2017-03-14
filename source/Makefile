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

include config.mk
include filelist.mk


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
LD_CMD =
OC_CMD =
ifeq ($(LD_LINKER), )
else
$(shell cp $(LD_LINKER) $(OUT_DIR)/$(LD_LINKER))
LD_CMD = -T $(OUT_DIR)/$(LD_LINKER)
OC_CMD = -O binary
endif


all: compile raw
compile: $(C_OBJS)
raw:
	$(LD) $(LD_CMD) $(C_OBJS) $(S_OBJS) -o $(OUT_DIR)/$(OUT_BIN).elf $(LDFLAGS)
	$(OC) $(OC_CMD) $(OUT_DIR)/$(OUT_BIN).elf $(PRE_ROOT)/$(OUT_BIN)

.PHONY: clean
clean:
	rm -rf ./out ./output.bin
