ENTRY(_start)

SECTIONS
{
    . = 1M;
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data. */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
        __init_array_start = .;
        *(.init_array)
        __init_array_end = .;
        
        __fini_array_start = .;
        *(.fini_array)
        __fini_array_end = .;
    }

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
    STACK_BASE = .;
    . += 0x10000;
    STACK_TOP = .;
    
    KERNEL_END = .;
}
