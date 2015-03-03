#ifndef _ELF_H_
#define _ELF_H_

#include "efi.h"

#define EI_NIDENT 16

typedef struct {
	u8  e_ident[EI_NIDENT];
	u16 e_type;
	u16 e_machine;
	u32 e_version;
	u64 e_entry;
	u64 e_phoff;
	u64 e_shoff;
	u32 e_flags;
	u16 e_ehsize;
	u16 e_phentsize;
	u16 e_phnum;
	u16 e_shentsize;
	u16 e_shnum;
	u16 e_shstrndx;
} Elf64_Ehdr;

typedef struct {
	u32 p_type;
	u32 p_flags;
	u64 p_offset;
	u64 p_vaddr;
	u64 p_paddr;
	u64 p_filesz;
	u64 p_memsz;
	u64 p_align;
} Elf64_Phdr;

typedef struct {
	u32 sh_name;
	u32 sh_type;
	u32 sh_flags;
	u64 sh_addr;
	u64 sh_offset;
	u32 sh_size;
	u32 sh_link;
	u32 sh_info;
	u64 sh_addralign;
	u64 sh_entsize;
} Elf64_Shdr;

enum SHT_Types {
	SHT_NULL = 0,
	SHT_PROGBITS = 1,
	SHT_SYMTAB = 2,
	SHT_STRTAB = 3,
	SHT_RELA = 4,
	SHT_HASH = 5,
	SHT_DYNAMIC = 6,
	SHT_NOTES = 7,
	SHT_NOBITS = 8,
	SHT_REL = 9,
	SHT_SHLIB = 10,
	SHT_DYNSYM = 11,
	SHT_INIT_ARRAY = 14,
	SHT_FINI_ARRAY = 15,
	SHT_PREINIT_ARRAY = 16,
	SHT_GROUP = 17,
	SHT_SYMTAB_SHNDX = 18
};

enum SHT_Flags {
	SHT_WRITE = 0x1,
	SHT_ALLOC = 0x2
};

enum PHT_Types {
	PT_NULL = 0,
	PT_LOAD = 1,
	PT_DYNAMIC = 2,
	PT_INTERP = 3,
	PT_NOTE = 4,
	PT_SHLIB = 5,
	PT_PHDR = 6,
	PT_TLS = 7
};

typedef struct {
	u64 r_offset;
	u64 r_info;
} Elf64_Rel;

#define ELF64_R_TYPE(x) ((u32)(x))

#define R_X86_64_RELATIVE 8

typedef struct {
	u64 r_offset;
	u64 r_info;
	i64 r_addend;
} Elf64_Rela;

#endif // _ELF_H_
