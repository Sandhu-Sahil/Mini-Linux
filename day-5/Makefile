OBJECTS = loader.o kmain.o \
			frameBuffer/fb.o \
			IO_in-out/io.o \
			deviceTracker/device.o \
			serialPort/serial.o \
			gdt/gdt.o \
			gdt/gdt.asm.o
CC = gcc
CFLAGS = -g -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
			-nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
LDFLAGS = -g -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

all: kernel.elf

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
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

run: os.iso
	bochs -f bochsrc.txt -q

%.o: %.c
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# also remove from subdirectories
clean:
	rm -rf *.o kernel.elf os.iso frameBuffer/*.o IO_in-out/*.o deviceTracker/*.o serialPort/*.o gdt/*.o

debug: kernel.elf
	gdb -ex "target remote :1234" -ex "break fb_scroll" -ex "continue" -ex "layout asm" -ex "layout src" kernel.elf
