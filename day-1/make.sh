#!/bin/bash

# check if installed (sudo apt-get install build-essential nasm genisoimage bochs bochs-sdl)
if [ ! -f /usr/bin/nasm ]; then
    echo "nasm not installed"
    sudo apt-get install nasm
fi

if [ ! -f /usr/bin/genisoimage ]; then
    echo "genisoimage not installed"
    sudo apt-get install genisoimage
fi

if [ ! -f /usr/bin/bochs ]; then
    echo "bochs not installed"
    sudo apt-get install bochs
fi

if [ ! -f /usr/bin/bochs-sdl ]; then
    echo "bochs-sdl not installed"
    sudo apt-get install bochs-sdl
fi

if [ ! -f /usr/bin/build-essential ]; then
    echo "build-essential not installed"
    sudo apt-get install build-essential
fi

# compile
nasm -f elf32 loader.s -o loader.o
ld -T link.ld -melf_i386 loader.o -o kernel.elf
cp kernel.elf iso/boot/

# create iso
genisoimage -R                              \
            -b boot/grub/stage2_eltorito    \
            -no-emul-boot                   \
            -boot-load-size 4               \
            -A os                           \
            -input-charset utf8             \
            -quiet                          \
            -boot-info-table                \
            -o os.iso                       \
            iso

# run
bochs -f bochsrc.txt -q

# logs
# cat bochslog.txt