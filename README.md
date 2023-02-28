# README #

### What is this project? ###
A simple RTOS kernel with arm board adaptation. It can be a starter of your custimized RTOS kernel.

### How to run? ###
Preparations:

[1] Find ARM cross-build toolchain for your building host here:

https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads

[2] Download the correct toolchain package. As I use linux-x64, my correct package is the below one.

arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz

[3] Copy the downloaded toolchain to build folder. Then unzip the package to build/toolchain

mkdir <Project Folder>/build/toolchain

cp <Your Download Folder>/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz <Project Folder>/build/toolchain

cd <Project Folder>/build/toolchain

tar -xvf ./arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz

[4] Rename the folder in the toolchain to arm in build/toolchain

mv ./arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi ./arm

Make sure the compilers are in <Project Folder>/build/toolchain/arm/bin/

Make sure the lib files are in <Project Folder>/build/toolchain/arm/lib/gcc/arm-none-eabi/12.2.1/

[5] Update variables in <Project Folder>/configuration

CPU=arm926ej-s //Set the cpu, arm926ej-s is the only tested one.

BOARD=qemu     //Only support qemu for now

PLATFORM=arm   //Only support arm for now

TOOLCHAIN=$(ROOT)/$(BUILD)/toolchain // Do not change

TOOLCHAIN_BIN=$(TOOLCHAIN)/$(PLATFORM)/bin // Do not change

TOOLCHAIN_VERSION=12.2.1 // Set the version of your toolchain

### Build ###

./make.sh to build


### Run In QEMU ###

sudo apt install qemu

qemu-system-arm -M versatilepb -nographic -kernel output

### Stop Running ###

Kill:
ps -A |grep qemu-system-arm |awk '{print $1}' |xargs -I {} kill {}
