#include "base_tctx.h"

local_perthread styx_tctx *_thrl_tctx;

// TODO(sir->w): Allow the developer to set the size of the thread context.
styx_function styx_tctx 
make_tctx(u64 allocator_size, u64 tmp_allocator_size)
{
    styx_tctx tctx = {
        .allocator = init_arena(allocator_size),
        .temp_allocator = init_arena(tmp_allocator_size),
    };
    
    return tctx;
}

styx_function void
free_tctx(styx_tctx *tctx)
{
    free_arena(tctx->allocator);
    free_arena(tctx->temp_allocator);
    
    *tctx = (styx_tctx){0};
}

styx_function styx_tctx *
get_tctx(void)
{
    return _thrl_tctx;
}

styx_function void
set_tctx(styx_tctx *tctx)
{
    _thrl_tctx = tctx;
}

styx_function temp_arena
get_scratch(void)
{
    styx_tctx *tctx = get_tctx();
    temp_arena scratch = begin_temp_arena(tctx->temp_allocator);
    
    return scratch;
}