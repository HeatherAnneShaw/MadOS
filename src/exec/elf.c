////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <exec.h>
#include <memory.h>
#include <elf.h>

/*
typedef struct
{
  unsigned char	e_ident[EI_NIDENT];	 Magic number and other info 
  Elf32_Half	e_type;              Object file type 
  Elf32_Half	e_machine;           Architecture 
  Elf32_Word	e_version;		     Object file version 
  Elf32_Addr	e_entry;		     Entry point virtual address 
  Elf32_Off	e_phoff;		         Program header table file offset 
  Elf32_Off	e_shoff;		         Section header table file offset 
  Elf32_Word	e_flags;		     Processor-specific flags 
  Elf32_Half	e_ehsize;		     ELF header size in bytes 
  Elf32_Half	e_phentsize;		 Program header table entry size 
  Elf32_Half	e_phnum;             Program header table entry count 
  Elf32_Half	e_shentsize;		 Section header table entry size 
  Elf32_Half	e_shnum;		     Section header table entry count 
  Elf32_Half	e_shstrndx;		     Section header string table index
} Elf32_Ehdr;

typedef struct
{
  Elf32_Word	p_type;			 Segment type
  Elf32_Off	p_offset;		     Segment file offset 
  Elf32_Addr	p_vaddr;		 Segment virtual address 
  Elf32_Addr	p_paddr;		 Segment physical address 
  Elf32_Word	p_filesz;		 Segment size in file 
  Elf32_Word	p_memsz;		 Segment size in memory 
  Elf32_Word	p_flags;		 Segment flags 
  Elf32_Word	p_align;		 Segment alignment 
} Elf32_Phdr;
*/


bool is_elf(uint32_t* file)
{
    return file[0] == 0x464c457f;  // "\x7fELF" in little endian
}

void load_module(char* name, void* p)
{
    Elf32_Ehdr* Eheader = p;
    Elf32_Phdr* Pheader = p+Eheader->e_phoff;
    // use exec to load the code after offset and then jump to the entry point
    if(((char*) p)[4] == ET_REL)
        exec_loadmodule(name, p + Pheader->p_offset, Pheader->p_vaddr, Eheader->e_entry - Pheader->p_offset, Pheader->p_memsz);
    else
        puts("This is not a relocatable file");
}

static char name[] = "ELF";
void __attribute__((constructor)) init_elf()
{
    // set up executable format descriptor
    puts("Registered ELF executable format.");
    exec_entry_t* elf = malloc(sizeof(exec_entry_t));
    elf->name = name;
    elf->is_type = (void*) is_elf;
    elf->load_module = (void*) load_module;

    // set up memory region for format specifier
    mem_entry_t* p = (void*) elf - sizeof(mem_entry_t);
    p->type = STR;
    register_exec(elf);
}


