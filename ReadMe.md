sballoc — a custom memory allocator (from scratch, in C)

I got curious about what malloc() and free() are actually doing under the hood, so I decided to build my own version instead of just reading about it.

This is that attempt. It's a minimal but genuinely working heap allocator — no shortcuts, no wrapping the real malloc. It talks directly to the OS via sbrk() to grow the heap, keeps track of memory in a linked list of blocks, and hands out real usable pointers a program can actually write into.

What it does right now
sballoc(size) — like malloc. Finds a free block big enough, or asks the OS for more memory if nothing fits, and returns a usable pointer.
sbfree(ptr) — like free. Marks a block as free again so it can be reused later.
Reuses freed memory instead of always asking the OS for more — tested and confirmed in main(), where a freed block gets handed back out on the next allocation.
What it doesn't do yet (on purpose — coming next)
Coalescing — if two neighboring blocks are both freed, they should merge into one bigger free block instead of staying separate. Right now they don't, so the heap can fragment over time.
Splitting — if a free block is way bigger than what's requested, it should be split so the leftover space stays usable. Right now the whole block gets handed over as-is.

Both are actively in progress — this is v1, not a finished project.

Why I built this

I'm early in my CS degree and wanted to actually understand memory management instead of just knowing the vocabulary. This is part of a longer self-study path toward systems programming — compilers, OS internals, that kind of thing. This felt like the right next step after building a CHIP-8 emulator.

More features (and probably a few rewrites once I understand this better) coming soon.