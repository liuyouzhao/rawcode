RC_PLATFORM = arm
RC_BOARD = qemu
C_CFLAGS += -Iarch/$(RC_PLATFORM)/$(RC_BOARD)/include
C_CFLAGS += -I./
