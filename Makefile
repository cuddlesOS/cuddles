SHELL:=/bin/bash

CFLAGS:= \
	-nostdlib \
	-nostdinc \
	-fno-builtin \
	-fno-stack-protector \
	-nostartfiles \
	-nodefaultlibs \
	-Wall \
	-Wextra \
	-Werror

STAGE3 = \
	stage3/main.o \
	stage3/gfx.o \
	stage3/halt.o \
	stage3/framebuffer.o \
	stage3/memory.o \
	stage3/paging.o \
	stage3/heap.o \
	stage3/font.o \
	stage3/letters.o

lizzyx.img: stage1.out stage2.out stage3.out
	cat stage{1,2,3}.out > lizzyx.img

stage1.out: stage1/main.asm stage1/print.asm stage2.out stage3.out
	nasm -f bin stage1/main.asm -o stage1.out \
		-dKSIZE=$$(du -cb stage{2,3}.out | tail -n1 | cut -f1)

stage2.out: stage2/main.asm stage2/mmap.asm stage2/paging.asm stage2/vesa.asm stage1/print.asm
	nasm -f bin stage2/main.asm -o stage2.out
	dd if=/dev/zero bs=1 count=$$(echo 4608-$$(du -b stage2.out | cut -f1) | bc) >> stage2.out

stage3.out: $(STAGE3) stage3.ld
	ld $(STAGE3) -T stage3.ld -Map=stage3.map

stage3/%.o: stage3/%.asm
	nasm -f elf64 $< -o $@

stage3/%.o: stage3/%.c
	cc $(CFLAGS) -c $< -o $@

.PHONY: run clean flash disas map

run: lizzyx.img
	echo c | bochs -q

clean:
	rm -rf stage3/*.o *.out *.img *.map

flash: lizzyx.img
	dd if=lizzyx.img of=$(DEV)

disas: stage3.out
	objdump -b binary -D -M intel -m i386:x86-64 stage3.out --adjust-vma 0x9000

map: stage3.out
	cat stage3.map
