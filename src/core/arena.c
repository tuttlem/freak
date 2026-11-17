#include <malloc.h>

#include "arena.h"

int freak_arena_init(struct freak_arena *arena, unsigned long capacity)
{
    if (arena == 0 || capacity == 0)
        return 0;
    arena->memory = (unsigned char *)malloc(capacity);
    if (arena->memory == 0) {
        arena->capacity = 0;
        arena->used = 0;
        arena->high_water = 0;
        return 0;
    }
    arena->capacity = capacity;
    arena->used = 0;
    arena->high_water = 0;
    return 1;
}

void freak_arena_destroy(struct freak_arena *arena)
{
    if (arena == 0)
        return;
    free(arena->memory);
    arena->memory = 0;
    arena->capacity = 0;
    arena->used = 0;
    arena->high_water = 0;
}

void freak_arena_reset(struct freak_arena *arena)
{
    if (arena)
        arena->used = 0;
}

void *freak_arena_alloc(struct freak_arena *arena, unsigned long size)
{
    return freak_arena_alloc_aligned(arena, size, 4);
}

void *freak_arena_alloc_aligned(struct freak_arena *arena, unsigned long size,
                                unsigned int alignment)
{
    unsigned long padding;
    unsigned long offset;

    if (arena == 0 || arena->memory == 0 || size == 0)
        return 0;
    if (alignment == 0)
        alignment = 1;
    padding = (alignment - (arena->used % alignment)) % alignment;
    if (padding > arena->capacity - arena->used)
        return 0;
    offset = arena->used + padding;
    if (size > arena->capacity - offset)
        return 0;
    arena->used = offset + size;
    if (arena->used > arena->high_water)
        arena->high_water = arena->used;
    return arena->memory + offset;
}
