SHELL:=/bin/bash

override CFLAGS += \
	-nostdlib \
	-nostdinc \
	-fno-builtin \
	-fno-stack-protector \
	-nostartfiles \
	-nodefaultlibs \
	-Wall \
	-Wextra \
	-MMD

STAGE3_C = \
	stage3/init.o \
	stage3/main.o \
	stage3/gfx.o \
	stage3/halt.o \
	stage3/interrupts.o \
	stage3/pic.o \
	stage3/memory.o \
	stage3/heap.o \
	stage3/font.o \
	stage3/font_classic.o \
	stage3/ata.o \
	stage3/string.o \
	stage3/pci.o \
	stage3/fs.o \
	stage3/ps2.o \
	stage3/thread.o \
	stage3/shell.o \
	stage3/version.o

STAGE3 = $(STAGE3_C) \
	stage3/isr.o \
	stage3/yield.o \
	stage3/paging.o

PAD_BOUNDARY = pad() { truncate -s $$(echo "($$(du -b $$1 | cut -f1)+$$2-1)/$$2*$$2" | bc) $$1; }; pad
DISAS = objdump -b binary -D -M intel -m i386:x86-64 stage3.bin --adjust-vma 0x9000

cuddles.img: stage1.bin stage2.bin stage3.bin fs.tar
	cat stage{1,2,3}.bin fs.tar > cuddles.img
	$(PAD_BOUNDARY) cuddles.img 1048576

stage1.bin: stage1/main.asm stage1/print.asm stage2.bin stage3.bin
	nasm -f bin stage1/main.asm -o stage1.bin \
		-dKSIZE=$$(du -cb stage{2,3}.bin | tail -n1 | cut -f1)

stage2.bin: stage2/main.asm stage2/mmap.asm stage2/paging.asm stage2/vesa.asm stage1/print.asm
	nasm -f bin stage2/main.asm -o stage2.bin
	truncate -s 4608 stage2.bin

stage3.bin fs/dbg/kernel.map &: $(STAGE3) stage3.ld
	mkdir -p fs/dbg/
	ld $(STAGE3) -T stage3.ld -Map=fs/dbg/kernel.map
	$(PAD_BOUNDARY) stage3.bin 512

fs/dbg/kernel.dis.asm: stage3.bin
	mkdir -p fs/dbg/
	$(DISAS) > fs/dbg/kernel.dis.asm

stage3/%.o: stage3/%.asm
	nasm -f elf64 $< -o $@

stage3/interrupts.o: stage3/interrupts.c
	gcc $(CFLAGS) -mgeneral-regs-only -c $< -o $@

stage3/%.o: stage3/%.c
	gcc $(CFLAGS) -c $< -o $@

-include $(STAGE3_C:%.o=%.d)

GIT_VERSION := $(shell git describe --tags 2>/dev/null || git rev-parse --short HEAD)

.version-$(GIT_VERSION):
	rm -f .version-*
	touch $@

stage3/version.c: .version-$(GIT_VERSION)
	echo -e "#include \"def.h\"\nstr version = S(\"$(GIT_VERSION)\");" > $@

stage3/isr.asm: stage3/isr.lua
	lua stage3/isr.lua > stage3/isr.asm

stage3/font.c: stage3/font_builtin.c

stage3/font_builtin.c: fs/fonts/ter-u16n.cuddlefont
	xxd -i $< > $@

fs.tar: $(shell find fs | sed 's/ /\\ /g') fs/dbg/kernel.map fs/dbg/kernel.dis.asm
	cd fs && tar --format=ustar -cf ../fs.tar *

.PHONY: run clean flash disas qemu bochs

bochs: cuddles.img
	rm -f cuddles.img.lock
	echo c | bochs -q

qemu: cuddles.img
	qemu-system-x86_64 -drive format=raw,file=cuddles.img

qemu_slow: cuddles.img
	qemu-system-x86_64 -icount shift=9,align=on,sleep=on -drive format=raw,file=cuddles.img

run: qemu

clean:
	rm -rf .version-* stage3/*.{o,d} *.bin *.img stage3/{isr.asm,version.c,font_builtin.c} fs.tar fs/dbg

flash: cuddles.img
	dd if=cuddles.img of=$(DEV)

disas: stage3.bin
	$(DISAS) --disassembler-color=on
