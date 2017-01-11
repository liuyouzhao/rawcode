#！/bin/bash
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

BUILD=build
TARGET=arm
PLATFORM=./$BUILD/toolchain/$TARGET

if [ -d "$PLATFORM" ];then
echo ""
echo [1]Got corresponding platform:[$TARGET] in $PLATFORM
else
echo ""
echo ""
echo [1]No platform, unzip default one.
echo Please wait...
sleep 1
tar jxvf ./$BUILD/$TARGET.tar.bz2
mv toolchain/* $BUILD/toolchain/
fi

cp $BUILD/Makefile ./source
echo ""
echo [2]Start make all...
cd source
make all
#./bin/w500/bin/arm-none-eabi-as -mcpu=arm926ej-s startup.s -o startup.o
#./bin/w500/bin/arm-none-eabi-as -mcpu=arm926ej-s iv.s -o iv.o
#./bin/w500/bin/arm-none-eabi-gcc -c -mcpu=arm926ej-s init.c -o init.o 
#./bin/w500/bin/arm-none-eabi-gcc -c -mcpu=arm926ej-s timer.c -o timer.o 
#./bin/w500/bin/arm-none-eabi-ld -T linker.ld  init.o  timer.o -o output.elf 
#./bin/w500/bin/arm-none-eabi-objcopy -O binary output.elf output.bin
##qemu-system-arm -M versatilepb -nographic -kernel output.bin
