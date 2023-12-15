OUTPUT(stage3.bin)
OUTPUT_FORMAT(binary)

SECTIONS
{
	. = 0x9000;
	.text : { *(.text) }
	.data : { *(.bss .data .rodata) }
}
