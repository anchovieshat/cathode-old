#ifndef _PT_H_
#define _PT_H_
#include "efi.h"

#define PAGESHIFT 12
#define PAGESZ (1l << PAGESHIFT)
#define SZOF_PTE sizeof(usize)
#define PTENTS (PAGESZ / SZOF_PTE)

#define PGNUM_L4(va) ((((usize) va) >> 39) & (PTENTS - 1))
#define PGNUM_L3(va) ((((usize) va) >> 30) & (PTENTS - 1))
#define PGNUM_L2(va) ((((usize) va) >> 21) & (PTENTS - 1))
#define PGNUM_L1(va) ((((usize) va) >> 12) & (PTENTS - 1))

/* PRESENT | WRITABLE | GLOBAL  (! USER) */
#define PF_DEFAULT 259

struct pt_man {
	void *(*page_alloc)(usize);
	usize *l4;
};

void pt_empty(usize *);
void pt_man_init(struct pt_man *, void *(*)(usize));
void pt_man_map(struct pt_man *, void *, void *, usize);
void pt_man_print(struct pt_man *);

#endif
