use core::prelude::*;
use core::num::Int;
use core::raw;
use core::intrinsics;
use core::fmt;

struct Chunk(u64);

impl Chunk {
    fn set_bit(&mut self, bit: u8) {
        assert!(bit < 64);
        self.0 = self.0 | (1 << bit);
    }
    fn clear_bit(&mut self, bit: u8) {
        assert!(bit < 64);
        self.0 = self.0 & !(1 << bit);
    }
    fn test_bit(&self, bit: u8) -> bool {
        assert!(bit < 64);
        0 != (self.0 & (1 << bit))
    }
    fn set(&mut self, num : u64) {
        self.0 = num;
    }
    fn get(&self) -> u64 {
        self.0
    }
}

impl fmt::LowerHex for Chunk {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        self.0.fmt(formatter)
    }
}

#[inline]
fn pa_to_pg(paddr: usize) -> usize {
    paddr >> 12
}

#[inline]
fn pg_to_pa(pgnum: usize) -> usize {
    pgnum << 12
}

#[inline]
fn sz_to_pg(size: usize) -> usize {
    (size+ 4095) >> 12
}

#[inline]
fn order_of(val: usize) -> usize {
    65 - ((val - 1).leading_zeros() as usize)
}

pub struct PageSlice {
    pub page: usize,
    pub order: usize,
}

impl PageSlice {
    pub fn pages(&self) -> usize {
        1 << self.order
    }
    pub fn ptr<T>(&self) -> *mut T {
        unsafe {
            intrinsics::transmute(pg_to_pa(self.page))
        }
    }
    pub fn bytes(&self) -> usize {
        pg_to_pa(self.order)
    }
}

pub struct Heap {
    order: usize,
    chunks: *mut Chunk,
}

impl Heap {
    pub fn new(mem_sz: usize, heap_rg: usize, heap_sz: usize) -> Heap {
        let pages = pa_to_pg(mem_sz + 4095);
        let order = order_of(pages);
        let mut heap = Heap {order: order, chunks: heap_rg as *mut Chunk};
        let tabsz = heap.size();

        println!("Pages: {:x}", pages);
        println!("Order: {}", order);
        println!("Tabsz: {}kB", tabsz/1024);
        println!("Heap region: @{:x} sz {:x}", heap_rg, heap_sz);
        if tabsz > heap_sz {
            panic!("Out of memory ({:x} > {:x})", tabsz, heap_sz);
        }

        heap.clear();
        heap
    }
    #[inline]
    pub fn size(&self) -> usize {
        (1 << (self.order - 3)) - 1
    }
    pub unsafe fn chunk_slice(&self) -> &'static mut[Chunk] {
        intrinsics::transmute(raw::Slice {data: self.chunks, len: self.size()})
    }
    pub fn clear(&mut self) {
        unsafe {
            for chunk in self.chunk_slice().iter_mut() {
                chunk.set(0);
            }
        }
        let heap_pg = pa_to_pg(self.chunks as usize);
        let heap_pgs = sz_to_pg(self.size());
        self.set_used_range(heap_pg, heap_pgs);
    }
    fn set_bit(&mut self, bit: usize) {
        unsafe { self.chunk_slice()[bit>>6].set_bit((bit & 63) as u8) };
    }
    fn clear_bit(&mut self, bit: usize) {
        unsafe { self.chunk_slice()[bit>>6].clear_bit((bit & 63) as u8) };
    }
    fn test_bit(&self, bit: usize) -> bool {
        unsafe {
            println!("memdbg: Testing chunk at {} (={:x}) for bit {}...", bit>>6, self.chunk_slice()[bit>>6], bit & 63);
            self.chunk_slice()[bit>>6].test_bit((bit & 63) as u8)
        }
    }
    // TODO: More efficient implementation
    fn set_bit_span(&mut self, bit: usize, span: usize) {
        for b in (bit..(bit+span)) {
            self.set_bit(b);
        }
    }
    fn clear_bit_span(&mut self, bit: usize, span: usize) {
        for b in (bit..(bit+span)) {
            self.clear_bit(b);
        }
    }
    #[inline]
    fn pg_order_to_bit(&self, pg: usize, order: usize) -> usize {
        (1 << (self.order - order - 1)) + (pg >> order)
    }
    fn set_used_order(&mut self, pg: usize, order: usize) {
        let chunks = unsafe { self.chunk_slice() };
        let _idx = self.pg_order_to_bit(pg, order);
        let mut idx = _idx;
        while idx > 1 {
            self.set_bit(idx);
            idx = idx / 2;
        }
        if order > 0 {
            idx = self.pg_order_to_bit(pg, order);
            let mut span = 1;
            let mut ord = order;
            while ord > 0 {
                span = span << 1;
                idx = idx << 1;
                ord = ord - 1;
                self.set_bit_span(idx, span);
            }
        }
    }
    fn clear_used_order(&mut self, pg: usize, order: usize) {
        let chunks = unsafe { self.chunk_slice() };
        let mut idx = self.pg_order_to_bit(pg, order);
        while idx > 1 {
            self.clear_bit(idx);
            idx = idx / 2;
            let buddy = idx ^ 1;
            if self.test_bit(buddy) { break; }
        }
        if order > 0 {
            idx = self.pg_order_to_bit(pg, order);
            let mut span = 1;
            let mut ord = order;
            while ord > 0 {
                span = span << 1;
                idx = idx << 1;
                ord = ord - 1;
                self.clear_bit_span(idx, span);
            }
        }
    }
    pub fn test_used(&self, pg: usize) -> bool {
        self.test_bit((1 << (self.order - 1)) + pg)
    }
    pub fn set_used(&mut self, pg: usize) {
        self.set_used_order(pg, 0);
    }
    pub fn clear_used(&mut self, pg: usize) {
        self.clear_used_order(pg, 0);
    }
    pub fn set_used_range(&mut self, pg: usize, num: usize) {
        self.set_used_order(pg, order_of(num));
    }
    pub fn clear_used_range(&mut self, pg: usize, num: usize) {
        self.clear_used_order(pg, order_of(num));
    }
    // TODO: Also more efficiency
    pub fn alloc_pages(&mut self, num: usize) -> Option<usize> {
        let order = order_of(num);
        let fbit = 1 << (self.order - order - 1);
        let lbit = (1 << (self.order - order)) - 1;
        println!("memdbg: Scanning the order {} bitmap (bits {}-{})", order, fbit, lbit);
        for bit in (fbit..lbit) {
            if !self.test_bit(bit) {
                println!("memdbg: {} bit is CLEAR (allocating...)", bit);
                let pg = (bit - fbit) << order;
                self.set_used_order(pg, order);
                return Some(pg);
            } else {
                println!("memdbg: {} bit is SET", bit);
            }
        }
        println!("memdbg: Scan unsuccessful (no clear bits found)");
        None
    }
    pub fn alloc_pmem(&mut self, bytes: usize) -> Option<usize> {
        self.alloc_pages(sz_to_pg(bytes)).map(pg_to_pa)
    }
    // XXX This is somewhat tacked-on.
    pub fn phyalloc(&mut self, bytes: usize) -> Option<PageSlice> {
        match self.alloc_pages(sz_to_pg(bytes)) {
            Some(page) => Some(PageSlice {page: page, order: sz_to_pg(bytes)}),
            None => None
        }
    }
    pub fn phyfree(&mut self, psl : PageSlice) {
        self.clear_used_range(psl.page, 1 << psl.order);
    }
}
