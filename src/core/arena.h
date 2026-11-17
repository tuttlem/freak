#ifndef __freak_arena_h_
#define __freak_arena_h_

/* A linear allocator for fixed-lifetime allocations. Reset reclaims all
   allocations at once; individual allocations are never freed. */
struct freak_arena {
    unsigned char *memory;
    unsigned long capacity;
    unsigned long used;
    unsigned long high_water;
};

int freak_arena_init(struct freak_arena *arena, unsigned long capacity);
void freak_arena_destroy(struct freak_arena *arena);
void freak_arena_reset(struct freak_arena *arena);
void *freak_arena_alloc(struct freak_arena *arena, unsigned long size);
void *freak_arena_alloc_aligned(struct freak_arena *arena, unsigned long size,
                                unsigned int alignment);

#endif
