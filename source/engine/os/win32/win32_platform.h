/* date = April 8th 2022 10:33 am */

#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H

typedef struct styx_window styx_window;
struct styx_window
{
    HWND handle;
    StyxRenderer *d3d_renderer;
    StyxRenderQueue *renderer;
    
    u32 width, height;
    str8 title;
    b32 running;
};

typedef struct styx_module styx_module;
struct styx_module
{
    HMODULE mod;
    u64 last_write;
    
    StyxLoad   *styx_load;
    StyxUnload *styx_unload;
    StyxUpdate *styx_update;
    
    str8 mod_path;
    str8 load_name;
    str8 unload_name;
    str8 loop_name;
};

// TODO(sir->w): Different entry points that setup all the StyxOS utilities needed by the engine for each StyxOS platform.
styx_function int styx_main(void);

// TODO(sir->w): Custom string implementation without cyclic importation.
// TODO(sir->w): Change calls to styxplat_ calls.
styx_function void win32_msg_box(char *title, char *msg);
styx_function void *win32_alloc_mem(u64 size);
styx_function void win32_debug_print(char *str);
styx_function void *win32_read_file(memory_arena *arena, str8 path);


styx_function LRESULT win32_window_proc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);
styx_function void win32_init(void);

///////////////////////////////////////////////////////////////////////////////
// The platform functions for developers to call.
styx_function u64 styxplat_file_last_write(str8 path);

styx_function styx_window *styxplat_create_window(str8 title, u32 width, u32 height);
styx_function void styxplat_destroy_window(styx_window *window);
styx_function void styxplat_process_messages(styx_window *window);
styx_function void styxplat_update_window_size(styx_window *window);
inline void styxplat_update_window(styx_window *window, b32 vsync);

// NOTE(sir->w): Really misnomers because they actually initialize and free the modules.
// More like constructors and destructors. 
styx_function styx_module *styxplat_load_module(str8 mod_path, str8 load, str8 unload, str8 loop, styx_window *window);
styx_function void styxplat_unload_module(styx_module *module, styx_window *window);
inline void styxplat_module_update(styx_module *module, styx_window *window);

// TODO(sir->w): Implement copying the DLL into a temporary DLL for hot reloading to work.
styx_function void styxplat_reload_module(styx_module *module, styx_window *window);
styx_function b32 styxplat_module_should_reload(styx_module *module);

///////////////////////////////////////////////////////////////////////////////

#endif //PLATFORM_H
