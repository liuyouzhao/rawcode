PLATFORM    = arm
BOARD       = qemu
CPU         = arm926ej-s

C_CFLAGS += -Iarch/$(PLATFORM)/$(BOARD)/include
C_CFLAGS += -I./
