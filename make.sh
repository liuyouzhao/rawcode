./bin/w500/bin/arm-none-eabi-as -mcpu=arm926ej-s startup.s -o startup.o
./bin/w500/bin/arm-none-eabi-as -mcpu=arm926ej-s iv.s -o iv.o
./bin/w500/bin/arm-none-eabi-gcc -c -mcpu=arm926ej-s init.c -o init.o 
./bin/w500/bin/arm-none-eabi-gcc -c -mcpu=arm926ej-s timer.c -o timer.o 
./bin/w500/bin/arm-none-eabi-ld -T linker.ld  init.o  timer.o -o output.elf 
./bin/w500/bin/arm-none-eabi-objcopy -O binary output.elf output.bin


##qemu-system-arm -M versatilepb -nographic -kernel output.bin
