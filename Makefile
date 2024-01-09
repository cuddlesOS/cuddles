SHELL:=/bin/bash

override CFLAGS += \
	-nostdlib \
	-nostdinc \
	-fno-builtin \
	-fno-stack-protector \
	-mno-red-zone \
	-nostartfiles \
	-nodefaultlibs \
	-Wall \
	-Wextra

override LDFLAGS += --no-warn-rwx-segment

ifneq "$(DEBUG)" "0"
	override CFLAGS += -g -fno-omit-frame-pointer -DDEBUG
	override LDFLAGS += -g
	override NASMFLAGS_KERNEL += -g
endif

STAGE3_C = \
	stage3/init.o \
	stage3/main.o \
	stage3/gfx.o \
	stage3/halt.o \
	stage3/interrupts.o \
	stage3/pic.o \
	stage3/clock.o \
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
	stage3/version.o \
	stage3/rng.o \
	stage3/cheese3d.o \
	stage3/cheese_demo.o \
	stage3/debug.o

STAGE3 = $(STAGE3_C) \
	stage3/isr.o \
	stage3/yield.o \
	stage3/paging.o \
	stage3/watchdog.o

PAD_BOUNDARY = pad() { truncate -s $$(echo "($$(du -b $$1 | cut -f1)+$$2-1)/$$2*$$2" | bc) $$1; }; pad
DISAS = objdump -D -M intel -j .text stage3.elf

cuddles.img: stage1.bin stage23.bin fs.tar
	cat stage{1,23}.bin fs.tar > cuddles.img
	$(PAD_BOUNDARY) cuddles.img 1048576

stage1.bin: stage1/main.asm stage1/print.asm stage23.bin
	nasm $(NASMFLAGS_BOOT) -f bin stage1/main.asm -o stage1.bin \
		-dKSIZE=$$(du -cb stage23.bin | tail -n1 | cut -f1)

stage23.bin: stage2.bin stage3.elf
	cat stage2.bin stage3.elf > stage23.bin
	$(PAD_BOUNDARY) stage23.bin 512

stage2.bin: stage2/main.asm stage2/mmap.asm stage2/paging.asm stage2/vesa.asm stage1/print.asm
	nasm $(NASMFLAGS_BOOT) -f bin stage2/main.asm -o stage2.bin

stage3.elf: $(STAGE3) stage3.ld
	ld $(LDFLAGS) $(STAGE3) -T stage3.ld -o stage3.elf

fs/dbg/kernel.dis.asm: stage3.elf
	mkdir -p fs/dbg/
	$(DISAS) > fs/dbg/kernel.dis.asm

stage3/%.o: stage3/%.asm
	nasm $(NASMFLAGS_KERNEL) -f elf64 $< -o $@

stage3/interrupts.o: stage3/interrupts.c
	gcc $(CFLAGS) -MMD -mgeneral-regs-only -c $< -o $@
stage3/pic.o: stage3/pic.c
	gcc $(CFLAGS) -MMD -mgeneral-regs-only -c $< -o $@
stage3/init.o: stage3/init.c
	gcc $(CFLAGS) -MMD -mgeneral-regs-only -c $< -o $@

stage3/%.o: stage3/%.c
	gcc $(CFLAGS) -MMD -c $< -o $@

-include $(STAGE3_C:%.o=%.d)

GIT_VERSION := $(shell git describe --tags 2>/dev/null || git rev-parse --short HEAD)

.version-$(GIT_VERSION):
	rm -f .version-*
	touch $@

stage3/version.c: .version-$(GIT_VERSION)
	echo -e "#include \"def.h\"\nstr version = S(\"$(GIT_VERSION)\");" > $@

stage3/%.asm: stage3/%.lua
	lua  $< > $@

stage3/font.c: stage3/font_builtin.c

stage3/font_builtin.c: fs/fonts/ter-u16n.cuddlefont
	xxd -i $< > $@

fs.tar: $(shell find fs | sed 's/ /\\ /g') fs/dbg/kernel.dis.asm
	cd fs && tar --format=ustar -cf ../fs.tar *

.PHONY: run clean flash disas qemu bochs

bochs: cuddles.img
	rm -f cuddles.img.lock
	echo c | bochs -q

# to qemu slow: make QFLAGS="-icount shift=9,align=on,sleep=on" run
# try QFLAGS="-enable-kvm" for better performance
override QFLAGS += -drive format=raw,file=cuddles.img

qemu: cuddles.img
	qemu-system-x86_64 $(QFLAGS)

run: qemu

clean:
	rm -rf .version-* stage3/*.{o,d} *.bin *.img stage3/{isr.asm,version.c,font_builtin.c} fs.tar fs/dbg

flash: cuddles.img
	dd if=cuddles.img of=$(DEV)

disas: stage3.elf
	$(DISAS) --disassembler-color=on
