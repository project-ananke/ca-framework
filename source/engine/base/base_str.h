/* date = April 14th 2022 8:30 pm */

#ifndef STR_H
#define STR_H

typedef struct str8 str8;
struct str8
{
    u8 *str;
    u64 len;
};
// NOTE(sir->w): Stack allocation.
styx_function str8 new_str8(u8 *cstr, u64 len);
#define str8_lit(cstr) new_str8((u8 *)cstr, sizeof(cstr) - 1)

// NOTE(sir->w): Heap allocation. Perma-alloc.
// TODO(sir->w): Should this return a ptr instead so we can tell difference?
styx_function str8 push_str8(u8 *cstr, u64 len);
styx_function str8 push_str8_copy(memory_arena *allocator, str8 str);
styx_function str8 push_str8_concat(memory_arena *allocator, str8 init, str8 add);

#endif //STR_H
