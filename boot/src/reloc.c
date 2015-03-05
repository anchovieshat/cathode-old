#include "efi.h"

#define DT_NULL 0
#define DT_RELA 7
#define DT_RELASZ 8
#define DT_RELAENT 9
#define DT_REL 17
#define DT_RELSZ 18
#define DT_RELENT 19

#define R_NONE 0
#define R_RELATIVE 8

#define ELF_R_TYPE(x) ((x) & 0xffffffffL)

typedef struct {
	i64 d_tag;
	u64 d_val;
} ElfDyn;

typedef struct {
	u64 r_offset;
	u64 r_info;
} ElfRel;

void _reloc(unsigned long image_base, ElfDyn *dyn, __attribute__((unused)) EfiHandle image_handle, __attribute__((unused)) EfiSystemTable *system_table) {
	unsigned long relsz, relent;
	unsigned long *newaddr;
	ElfRel *rel;
	ElfDyn *dynp;

	relsz = 0;
	relent = 0;
	for (dynp = dyn; dynp->d_tag != DT_NULL; dynp++) {
		switch (dynp->d_tag) {
			case DT_REL:
			case DT_RELA:
				rel = (ElfRel*) ((unsigned long) dynp->d_val + image_base);
				break;
			case DT_RELSZ:
			case DT_RELASZ:
				relsz = dynp->d_val;
				break;
			case DT_RELENT:
			case DT_RELAENT:
				relent = dynp->d_val;
				break;
			default:
				break;
		}
	}
	for (; relsz > 0; relsz -= relent) {
		switch (ELF_R_TYPE(rel->r_info)) {
			case R_NONE:
				break;
			case R_RELATIVE:
				newaddr = (unsigned long *)(image_base + rel->r_offset);
				*newaddr += image_base;
				break;
			default:
				break;
		}
		rel = (ElfRel*) ((void*)rel + relent);
	}
}
