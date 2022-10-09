#include <string.h>

#include "base_memory.h"

styx_inline b32
is_power_of_two(uintptr_t x) 
{
    return (x & (x - 1)) == 0;
}

styx_function uintptr_t
align_forth(uintptr_t ptr, u32 align)
{
    assert_msg(is_power_of_two(align), "Alignment value is not a power of two.");
    uintptr_t p = ptr;
    uintptr_t a = (uintptr_t)align;
    
    // NOTE(sir->w): Basically p % a. No clue how this works.
    uintptr_t modulo = p & (a - 1);
    if (modulo != 0) {
        p += a - modulo;
    }
    
    return p;
}

styx_function void 
memory_copy(void *dest, void *src, u64 size)
{
    char *dest_ptr = (char *)dest;
    char *src_ptr = (char *)src;
    
    for (u64 i = 0; i < size; ++i) {
        *dest_ptr++ = *src_ptr++;
    }
}

// TODO(sir->w): Eventually have this function reserve memory from the StyxOS, instead of having the user supply the backing memory buffer.
styx_function memory_arena *
init_arena(u64 size)
{
    assert_msg(size, "Size cannot be 0.");
    
    memory_arena *arena = (memory_arena *)styx_reserve_mem(sizeof(*arena));
    arena->mem = styx_reserve_mem(size);
    arena->size = size;
    
    return arena;
}

styx_function void
free_arena(memory_arena *arena)
{
    styx_free_mem(arena->mem, arena->size);
    *arena = (memory_arena){0};
}

styx_function void 
arena_reset(memory_arena *arena)
{
    if (arena == NULL) return;
    
    arena->prev_offset = 0;
    arena->curr_offset = 0;
}

styx_function void *
arena_push_align(memory_arena *arena, u64 size, u32 align)
{
    uintptr_t curr_ptr = (uintptr_t)arena->mem + (uintptr_t)arena->curr_offset;
    uintptr_t offset = align_forth(curr_ptr, align);
    offset -= (uintptr_t)arena->mem;
    
    assert_msg(offset + size <= arena->size, "Out of arena space.");
    void *ptr = &arena->mem[offset];
    arena->prev_offset = offset;
    arena->curr_offset = offset + size;
    
    memset(ptr, 0, size);
    return ptr;
}

styx_function void *
arena_resize_align(memory_arena *arena, 
                   void *old_memory, u64 old_size,
                   u64 new_size, u32 align)
{
    u8 *old_mem = (u8 *)old_memory;
    if (old_mem == 0 || old_size == 0) {
        return arena_push_align(arena, new_size, align);
    } else if (old_mem >= arena->mem && old_mem < arena->mem + arena->size) {
        if (arena->mem + arena->prev_offset == old_mem) {
            arena->curr_offset = arena->prev_offset + new_size;
            if (new_size > old_size) {
                memset(&arena->mem[arena->curr_offset], 0, new_size - old_size);
            }
            return old_mem;
        } else {
            void *new_mem = arena_push_align(arena, new_size, align);
            memmove(new_mem, old_mem, new_size > old_size ? old_size : new_size);
            return new_mem;
        }
    }
    
    assert_msg(FALSE, "Old memory location out of arena memory bounds.");
    return 0;
}

//- NOTE(sir->w): Temp arena function definitions.
styx_function temp_arena 
begin_temp_arena(memory_arena *arena)
{
    temp_arena temp = {
        .parent_arena = arena,
        
        .prev_offset = arena->prev_offset,
        .curr_offset = arena->curr_offset,
    };
    
    return temp;
}

// NOTE(sir->w): This is assuming no allocations is made on the main arena,
// which is a pretty far-fetched supposition. 
styx_function void 
end_temp_arena(temp_arena *temp_arena)
{
    temp_arena->parent_arena->prev_offset = temp_arena->prev_offset;
    temp_arena->parent_arena->curr_offset = temp_arena->curr_offset;
}
