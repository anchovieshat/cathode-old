#[repr(u8)]
enum Page {
    EMPTY = 0,
    USED = 1,
}

struct chunk(u64);

struct heap {
    size: u64,
    chunks: chunk,
}

impl heap {
    fn create(heap_pos: u64, heap_size: u64) {
        heap {
            size: heap_size,
            chunks: [0; heap_size/64], //TODO Fix chunks... Is unclear how to store...
        }
    }

    fn alloc(&mut self, size: u64) {
        if size == 0 {
            size = 1;
        }

        let log_size = 64 - unsafe { ctlz64(size - 1) }; //computes n in 2^n to determine smallest possible alloc space, nth level up from bottom

        let mut chunk_index = 0;
        let mut level = 0;

        loop {
            match (self.get(index), level == log_size) {
                (Page::EMPTY, true) => {
                }
            }
        }
    }

    fn get(bit: u64) -> Page {

    }

    fn bit_translate(bit_pos: u64) -> (u64, u64) {
        let chunk_num = (bit_pos & (!0x3f)); //  go to this chunk
        let chunk_pos = (bit_pos & (0x3f)); // go to this bit
        (chunk_num, chunk_pos)
    }
}
