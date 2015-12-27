
_start()
{
    char* str = "Hello world, I'm a kernel module!!!\n";
    asm(".intel_syntax noprefix\n"
    "mov eax, 4\n"
    "mov ebx, 1\n"
    "lea ecx, [%0]\n"
    "mov edx, 36\n"
    "int 31\n"
    ::"r"(str));
}
