ENTRY(_start)

SECTIONS {
    /* QEMU loads the kernel in Flash here. I strongly suggest you look at the
     * memory map provided in the CoreTile TRM (see below).
     */
    . = 0x80010000;
    
    /* Make sure our entry stub goes first */
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
