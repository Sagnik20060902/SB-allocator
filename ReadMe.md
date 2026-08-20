# sballoc - a custom memory allocator (from scratch, in C)

I got curious about what malloc() and free() are actually doing under the hood, so I decided to build my own version instead of just reading about it.

This is that attempt. It's a minimal but genuinely working heap allocator — no shortcuts, no wrapping the real malloc. It talks directly to the OS via sbrk() to grow the heap, keeps track of memory in a doubly-linked list of blocks, and hands out real usable pointers a program can actually write into.

## Core Functions

* **`sballoc(size)`**: like malloc. Finds a free block big enough, splits it if it's larger than needed, or asks the OS for more memory if nothing fits, and returns a usable pointer.
* **`sbfree(ptr)`**: like free. Marks a block as free and immediately coalesces it with any adjacent free neighbors to prevent fragmentation.
* **`split(block, size)`**: when a free block is significantly larger than requested, splits it into an allocated block and a free remainder, so the leftover space stays usable for future allocations.
* **`collease(block)`**: after freeing, merges adjacent free blocks in both directions (previous and next) into one larger block. Verified with a three-block coalesce test.


## Why I built this 

I'm early in my UG degree and wanted to actually understand memory management instead of just knowing the vocabulary. This is part of a longer self-study path toward systems programming — compilers, OS internals, that kind of thing. This felt like the right next step after building a CHIP-8 emulator.

