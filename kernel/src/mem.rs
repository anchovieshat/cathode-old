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

        let chunk_pos = (size & (!0x3f));
        let level = 64 - ctlz64(size - 1)
        let pos = 0 as u64;
        let pos_state = ((chunk >> pos) & 0x1);

        if (level == 0) {
            while (state == 1) {
                pos++;
                state = ((chunk>>pos) & 0x1);
            }
            setbit(chunk, pos, true);
        }
        else {
            setbit(chunk, (pos/2), true);
            setbit(chunk, pos, true);
            setchildren(chunk, pos, level, true);
        }
    }

    fn setbit(chunk: u64, pos: u64, state: u8) {
        if (state == 0) {
            (chunk & (&(0x1) << pos))
        } else {
            (chunk | (0x1 << pos))
        }
    }

    fn setchildren(chunk: u64, pos: u64, level: u64, state: u8) {
        if (level > -1) {
            setchildren(chunk, (2*pos)+1, level-1, state); //right
            setchildren(chunk, (2*pos), level-1, state); //left
        }
        setbit(chunk, pos, state);
    }
}
