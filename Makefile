ROOT=$(shell pwd)
CC=$(ROOT)/bin/w500/bin/arm-none-eabi-gcc
AS=$(ROOT)/bin/w500/bin/arm-none-eabi-as
LD=$(ROOT)/bin/w500/bin/arm-none-eabi-ld
OC=$(ROOT)/bin/w500/bin/arm-none-eabi-objcopy
LDIR=$(ROOT)/bin/w500/arm-none-eabi/lib
LDIR2=$(ROOT)/bin/w500/lib/gcc/arm-none-eabi/4.9.3

AFLAGS=-mcpu=arm926ej-s
CFLAGS=-mcpu=arm926ej-s  \
       -nostdlib -g

#LDFLAGS=-L$(LDIR) -lc
LDFLAGS = -L$(LDIR) \
          -L$(LDIR2) \
	  -lc -lgcc -lm 

#LDFLAGS =


OBJS = timer.o init.o

#$(OBJS): %.o: %.c
#	$(CC) $(CFLAGS) $< -o $@


.PHONY: clean

clean:
	rm -rf ./*.o ./output.bin ./output.elf
	
$(shell $(AS) $(AFLAGS) startup.s -o startup.o)

all: COMPILE RAW
COMPILE: $(OBJS)
RAW:
	$(LD) -T linker.ld $(OBJS) -o output.elf $(LDFLAGS)
	$(OC) -O binary output.elf output.bin
#./bin/w500/bin/arm-none-eabi-ld -T linker.ld  init.o  timer.o -o output.elf -lm -lc -L/home/hujia/500G/#workspace/rawcode_git/rawcode/bin/w500/arm-none-eabi/lib
