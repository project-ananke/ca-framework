#include "base_str.h"

styx_function str8 
new_str8(u8 *cstr, u64 len)
{
    str8 str = {
        .str = cstr,
        .len = len,
    };
    
    return str;
}

styx_function str8 
push_str8(u8 *cstr, u64 len)
{
    styx_tctx *tctx = get_tctx();
    
    str8 str = {0};
    str.len = len;
    str.str = (u8 *)arena_push(tctx->allocator, len + 1);
    
    memory_copy(str.str, cstr, len + 1);
    
    return str;
}

styx_function str8 
push_str8_copy(memory_arena *allocator, str8 str)
{
    str8 new_str = {0};
    new_str.len = str.len;
    new_str.str = (u8 *)arena_push(allocator, new_str.len);
    memory_copy(new_str.str, str.str, new_str.len);
    
    return new_str;
}

styx_function str8 
push_str8_concat(memory_arena *allocator, str8 init, str8 add)
{
    str8 new_str = {0};
    
    new_str.len = init.len + add.len; 
    new_str.str = (u8 *)arena_push_array(allocator, sizeof(u8), new_str.len);
    memory_copy(new_str.str, init.str, init.len);
    memory_copy(new_str.str + init.len, add.str, add.len);
    new_str.str[new_str.len] = '\0';
    
    return new_str;
}