#include "pt.h"
#include "io.h"

void pt_empty(usize *pt) {
	usize *p = (usize *)(((usize) pt) & ~(PAGESZ - 1));
	printf("PT: Zeroing page %lu0x (aligned %lu0x) %ld times\n", (usize) pt, (usize) p, PTENTS);
	for(u32 i=0; i<PTENTS; i++)
		p[i] = 0;
}

void pt_man_init(struct pt_man *man, void *(*alloc)(usize)) {
	man->page_alloc = alloc;
	__asm__ volatile ("mov %%cr3, %0" : "=r"(man->l4) : :);
}

void _pt_man_map_inner(struct pt_man *man, void *phy, void *vir, usize *pt, u8 lev) {
	u32 idx;
	switch(lev) {
		case 4: idx = PGNUM_L4((usize) vir); break;
		case 3: idx = PGNUM_L3((usize) vir); break;
		case 2: idx = PGNUM_L2((usize) vir); break;
		case 1: idx = PGNUM_L1((usize) vir); break;
	}
	//printf("PT: CALLIN' DOWN THE MOUNTAIN %d:%d %lu0x->%lu0x\n", lev, idx, (usize) vir, (usize) phy);
	if(lev == 1) {
		// Presumably, this page exists.
		pt[idx] = (((usize) phy) & ~(PAGESZ - 1)) | PF_DEFAULT;
		printf("PT: %lu0x mapped to %lu0x\n", (usize) phy, (usize) vir);
	} else {
		//printf("PT: Page ptr at %d:%d (page @%lu0x) = %lu0x\n", lev, idx, (usize) pt, pt[idx]);
		if(pt[idx] == 0) {
			usize *page = man->page_alloc(1);
			//printf("PT: Alloc'ing PT at level %d at %lu0x\n", lev, (usize) page);
			pt_empty(page);
			pt[idx] = (((usize) page) & ~(PAGESZ - 1)) | PF_DEFAULT;
		}
		_pt_man_map_inner(man, phy, vir, (usize *)(pt[idx] & ~(PAGESZ - 1)), lev-1);
	}
}

void pt_man_map(struct pt_man *man, void *phy, void *vir, usize bytes) {
	for(usize v = (usize) vir; v < ((u64)vir) + bytes; v += PAGESZ, phy += PAGESZ)
		_pt_man_map_inner(man, phy, (void *) v, man->l4, 4);
}

void _pt_man_print_inner(usize *pt, usize *vaddr, u8 lev) {
	usize *vir;
	// printf("PT: Printing lev %d @%lu0x (vaddr-shift %lu0x)\n", (u32) lev, (usize) pt, (usize) vaddr);
	for(u32 i=0; i<PTENTS; i++) {
		if(i%32 == 0 || i>(PTENTS-5)) {
			// printf("PT: Print %d:%d\n", (u32) lev, i);
		}
		if(pt[i] & 1) {
			vir = (usize *)((((usize) vaddr) << 9) + i);
			if(lev == 4) {
				vir = (usize *)(((usize) vir)&(PTENTS>>1) ? ((usize) vir) | ~(PTENTS - 1) : ((usize) vir));
			}
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
