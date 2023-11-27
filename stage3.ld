OUTPUT(stage3.out)
OUTPUT_FORMAT(binary)

SECTIONS
{
	. = 0x9000;
	.text : { *(.text) }
	.data : { *(.bss .data .rodata) }
}
