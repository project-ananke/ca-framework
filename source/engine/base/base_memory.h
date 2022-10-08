/* date = April 8th 2022 8:22 pm */

#ifndef MEMORY_H
#define MEMORY_H

#ifndef DEF_ALIGN
#define DEF_ALIGN (2 * sizeof(void *))
#endif

// NOTE(sir->w): Manual memory management that's actually mentally sane,
// unlike ahmm... malloc, calloc ahmm...

// Add logging capabilities to the arenas for debugging purposes, so we
// can detect memory leaks and its kin.
typedef struct memory_arena memory_arena;
struct memory_arena
{
    u8 *mem;
    u64 size;
    
    u64 prev_offset;
    u64 curr_offset;
};

typedef struct temp_arena temp_arena;
struct temp_arena
{
    memory_arena *parent_arena;
    u64 prev_offset;
    u64 curr_offset;
};


// TODO(sir->w): Implement a pool allocator for reasons I have yet to know of.
inline b32 is_power_of_two(uintptr_t x);
styx_function uintptr_t align_forth(uintptr_t ptr, u32 align);

// NOTE(sir->w): Performs a byte-by-byte copy of data.
styx_function void memory_copy(void *dest, void *src, u64 size);

styx_function memory_arena *init_arena(u64 size);
styx_function void free_arena(memory_arena *arena);
styx_function void arena_reset(memory_arena *arena);

styx_function void *arena_push_align(memory_arena *arena, u64 size, u32 align);
#define arena_push(arena, size) arena_push_align(arena, size, DEF_ALIGN)
#define arena_push_array(arena, size, count) arena_push(arena, size * count)
#define arena_push_struct(arena, structure) (structure *)arena_push(arena, sizeof(structure))

styx_function void *arena_resize_align(memory_arena *arena, void *old_memory, u64 old_size, u64 new_size, u32 align);
#define arena_resize(arena, old_mem, old_size, new_size) \
arena_resize_align(arena, old_mem, old_size, new_size, DEF_ALIGN)

//- NOTE(sir->w): Temp arena utilities.
styx_function temp_arena begin_temp_arena(memory_arena *arena);
styx_function void end_temp_arena(temp_arena *temp_arena);

#define temp_arena_push_align(temp, size, align) \
arena_push_align(temp->parent_arena, size, align)
#define temp_arena_push(temp, size) arena_push(temp->parent_arena, size)
#define temp_arena_resize_align(temp, old_mem, old_size, new_size, align) \
arena_resize_align(temp->parent_arena, old_mem, old_size, new_size, align)
#define temp_arena_resize(temp, old_mem, old_size, new_size) \
arena_resize(temp->parent_arena, old_mem, old_size, new_size)

#endif //MEMORY_H
