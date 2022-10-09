/* date = April 18th 2022 1:45 pm */

#ifndef STYX_MODULE_H
#define STYX_MODULE_H

// Implemented in the platform layer. Actually, we can do OS interface function calls.
#if STYX_PLATFORM
// Platform only for fun and games. No need to actually export the functions.
#define MODULE_LOAD(name) void name(StyxOS *plat_os, StyxRenderQueue *renderer)
#define MODULE_UNLOAD(name) void name(StyxOS *plat_os, StyxRenderQueue *renderer)
#define MODULE_LOOP(name) void name(StyxOS *plat_os, StyxRenderQueue *renderer)

typedef MODULE_LOAD(StyxLoad);
typedef MODULE_UNLOAD(StyxUnload);
typedef MODULE_LOOP(StyxUpdate);

MODULE_LOAD(styx_load_stub) {}
MODULE_UNLOAD(styx_unload_stub) {}
MODULE_LOOP(styx_loop_stub) {}

extern void styx_load(StyxOS *plat_os, StyxRenderQueue *renderer);
extern void styx_unload(StyxOS *plat_os, StyxRenderQueue *renderer);
extern void styx_update(StyxOS *plat_os, StyxRenderQueue *renderer);

#else

#define MODULE_LOAD(name) styx_export void name(StyxOS *plat_os, StyxRenderQueue *renderer)
#define MODULE_UNLOAD(name) styx_export void name(StyxOS *plat_os, StyxRenderQueue *renderer)
#define MODULE_LOOP(name) styx_export void name(StyxOS *plat_os, StyxRenderQueue *renderer)

styx_inline void 
_styx_module_init(StyxOS *os, u64 alloc_size, u64 tmp_alloc_size)
{
    styx_os = os;
    
    styx_tctx tctx = make_tctx(alloc_size, tmp_alloc_size);
    set_tctx(&tctx);
}

#define styx_module_init(alloc_size, tmp_alloc_size) \
_styx_module_init(plat_os, alloc_size, tmp_alloc_size)

#endif

#endif //STYX_MODULE_H
