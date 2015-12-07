ENTRY(_start)

SECTIONS {
    . = 0x80010000;
    .stub :
    {
        boot.o (.text)
    }
    .text :
    {
        *(.text)
    }
    .rodata :
    {
        *(.rodata)
        __init_array_start = .;
        *(.init_array)
        __init_array_end = .;
        
        __fini_array_start = .;
        *(.fini_array)
        __fini_array_end = .;
    }
    .data :
    {
        *(.data)
    }
    .bss :
    {
        *(.bss COMMON)
    }
    .rel.plt :
    {
        *(.rel.plt)
    }
    STACK_BASE = .;
    . += 0x10000;
    STACK_TOP = .;
}
