/* The bootloader will jump to the start of our code */
ENTRY(_start)

SECTIONS {
    /* We want to run our kernel in the higher half, starting at 0xFFFF_FFFF_8000_0000 */
    . = 0xFFFF_FFFF_8000_0000;

    .text : {
        *(.text .text.*)
    }

    .rodata : {
        *(.rodata .rodata.*)
    }

    .data : {
        *(.data .data.*)
    }

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    }
}
