/* date = April 11th 2022 8:47 am */

#ifndef THREAD_CTX_H
#define THREAD_CTX_H

#if !defined(TCTX_ALLOCATOR_SIZE)
# if defined(STYX_PLATFORM)
#  define TCTX_ALLOCATOR_SIZE (megabytes(128))
# else 
#  define TCTX_ALLOCATOR_SIZE (gigabytes(2))
# endif
#endif 

#if !defined(TCTX_TEMP_ALLOCATOR_SIZE)
# if defined(STYX_PLATFORM)
#  define TCTX_TEMP_ALLOCATOR_SIZE (megabytes(256))
# else
#  define TCTX_TEMP_ALLOCATOR_SIZE (gigabytes(4))
# endif
#endif
// NOTE(sir->w): Should we add the platform calls to the styx_tctx?
typedef struct styx_tctx styx_tctx;
struct styx_tctx
{
    // NOTE(sir->w): Meant for lasting allocations.
    memory_arena *allocator;
    
    // NOTE(sir->w): Meant for shorter allocations (e.g., per cycle, per frame, etc.).
    // To be manually freed. 
    memory_arena *temp_allocator;
};

styx_function styx_tctx make_tctx(u64 allocator_size, u64 tmp_allocator_size);
styx_function void free_tctx(styx_tctx *tctx);

styx_function styx_tctx *get_tctx(void);
styx_function void set_tctx(styx_tctx *tctx);

// NOTE(sir->w): Defaults to the temp_allocator... but maybe we can have arena conflicts, 
// so we can choose precisely which allocator we don't want to use. 
// TODO(sir->w): This interface is really really dumb. Please fix it.
styx_function temp_arena get_scratch(void);
#define release_scratch(scratch) end_temp_arena(scratch)

#endif //BASE_CTX_H
