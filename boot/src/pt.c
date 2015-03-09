#include "pt.h"
#include "io.h"

void pt_empty(usize *pt) {
	usize *p = (usize *)(((usize) pt) & ~(PAGESZ - 1));
	for(u32 i=0; i<PTENTS; i++)
		p[i] = 0;
}

void pt_man_init(struct pt_man *man, void *(*alloc)(usize)) {
	man->page_alloc = alloc;
	usize *pg = alloc(1);
	pt_empty(pg);
	man->l4 = pg;
}

void _pt_man_map_inner(struct pt_man *man, void *phy, void *vir, usize *pt, u8 lev) {
	u32 idx;
	switch(lev) {
		case 4: idx = PGNUM_L4((usize) vir); break;
		case 3: idx = PGNUM_L3((usize) vir); break;
		case 2: idx = PGNUM_L2((usize) vir); break;
		case 1: idx = PGNUM_L1((usize) vir); break;
	}
	if(lev == 1) {
		// Presumably, this page exists.
		pt[idx] = (((usize) phy) & ~(PAGESZ - 1)) | PF_DEFAULT;
	} else {
		if(pt[idx] == 0) {
			usize *page = man->page_alloc(1);
			pt[idx] = (((usize) page) & ~(PAGESZ - 1)) | PF_DEFAULT;
		}
		_pt_man_map_inner(man, phy, vir, (usize *)(pt[idx] & ~(PAGESZ - 1)), lev-1);
	}
}

void pt_man_map(struct pt_man *man, void *phy, void *vir, usize bytes) {
	for(usize v = (usize) vir; v < bytes; v += PAGESZ, phy += PAGESZ)
		_pt_man_map_inner(man, phy, (void *) v, man->l4, 4);
}

void _pt_man_print_inner(usize *pt, usize *vaddr, u8 lev) {
	usize *vir;
	for(u32 i=0; i<PTENTS; i++) {
		if(pt[i] != 0) {
			vir = (usize *)((((usize) vaddr) << PAGESHIFT) + i * SZOF_PTE);
			if(lev == 1) {
				printf("vir %lu0x -> %lu0x phy\n", ((usize) vir) << PAGESHIFT, (usize)(pt[i] & ~(PAGESZ - 1)));
			} else {
				_pt_man_print_inner((usize *)(pt[i] & ~(PAGESZ - 1)), vir, lev - 1);
			}
		}
	}
}

void pt_man_print(struct pt_man *man) {
	_pt_man_print_inner(man->l4, 0, 4);
}
