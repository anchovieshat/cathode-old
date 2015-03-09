#ifndef _PT_H_
#define _PT_H_
#include "efi.h"

#define PAGESHIFT 12
#define PAGESZ (1l << PAGESHIFT)
#define SZOF_PTE sizeof(usize)
#define PTENTS (PAGESZ / SZOF_PTE)

#define VADDR_L1(l1) ((l1) * PAGESZ)
#define VADDR_L2(l2, l1) (VADDR_L1(l2) * PAGESZ + VADDR_L1(l1))
#define VADDR_L3(l3, l2, l1) (VADDR_L2(l3, 0) * PAGESZ + VADDR_L2(l2, l1))
#define VADDR_L4(l4, l3, l2, l1) (VADDR_L3(l4, 0, 0) * PAGESZ + VADDR_L3(l3, l2, l1))

#define PGNUM_L1(va) (((va) >> PAGESHIFT) & (PAGESZ - 1))
#define PGNUM_L2(va) (((va) >> (2 * PAGESHIFT)) & (PAGESZ - 1))
#define PGNUM_L3(va) (((va) >> (3 * PAGESHIFT)) & (PAGESZ - 1))
#define PGNUM_L4(va) (((va) >> (4 * PAGESHIFT)) & (PAGESZ - 1))

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
