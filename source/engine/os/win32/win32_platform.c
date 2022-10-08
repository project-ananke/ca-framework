#include "win32_platform.h"

// Minimize globals in final engine.
styx_global HINSTANCE _hinstance;
styx_global WNDCLASSEXA _wndclass;
styx_global StyxOS _os;

styx_global WASAPIAudio *audio;

styx_function void 
win32_msg_box(char *title, char *msg)
{
    MessageBoxA(0, msg, title, MB_OK);
}

styx_function void *
win32_alloc_mem(u64 size)
{
    return VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

styx_function void
win32_free_mem(void *mem)
{
    VirtualFree(mem, 0, MEM_RELEASE);
}

styx_function void
win32_debug_print(char *str)
{
    OutputDebugStringA(str);
}

styx_function void *
win32_read_file(memory_arena *arena, str8 path)
{
    void *data = NULL;
    HANDLE file_handle = CreateFileA((LPCSTR)path.str, 
                                     GENERIC_READ, FILE_SHARE_READ,
                                     NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE) return data;
    // Implement logging for styx.
    
    LARGE_INTEGER file_size_large = {0};
    u64 size = 0;
    u32 size_read = 0;
    
    GetFileSizeEx(file_handle, &file_size_large);
    size = file_size_large.QuadPart;
    
    data = arena_push(arena, size);
    ReadFile(file_handle, data, (DWORD)size, (LPDWORD)&size_read, NULL);
    
    CloseHandle(file_handle);
    return data;
}

styx_function LRESULT
win32_window_proc(HWND hwnd,
                  UINT msg,
                  WPARAM wparam,
                  LPARAM lparam)
{
    LRESULT result = {0};
    styx_window *window = (styx_window *)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    
    if (window) {
        switch (msg) {
            case WM_QUIT:
            case WM_DESTROY:
            case WM_CLOSE: {
                window->running = FALSE;
            } break;
            case WM_SIZE: {
                styxplat_update_window_size(window);
                if (window->width != 0 && window->height != 0 &&
                    window->renderer != NULL) {
                    win32_window_resize(window->d3d_renderer);
                }
            } break;
            case WM_SETCURSOR: {
                // NOTE(sir->w): Let's hide the cursor.
                if (LOWORD(lparam) == HTCLIENT) {
                    SetCursor(0);
                } else {
                    result = DefWindowProcA(hwnd, msg, wparam, lparam);
                }
            } break;
            default: {
                result = DefWindowProcA(hwnd, msg, wparam, lparam);
            } break;
        }
    } else {
        result = DefWindowProcA(hwnd, msg, wparam, lparam);
    }
    
    return result;
}

styx_function u64
styxplat_file_last_write(str8 path)
{
    WIN32_FILE_ATTRIBUTE_DATA file_data = {0};
    ULARGE_INTEGER large_int = {0};
    
    GetFileAttributesExA((LPCSTR)path.str, GetFileExInfoStandard, &file_data);
    large_int.LowPart = file_data.ftLastWriteTime.dwLowDateTime;
    large_int.HighPart = file_data.ftLastWriteTime.dwHighDateTime;
    
    return (u64)large_int.QuadPart;
}

// TODO(sir->w): Throw rendering on a different thread, 
// so rendering doesn't freeze whenever messages are handled.
styx_function styx_window *
styxplat_create_window(str8 title, u32 width, u32 height)
{
    styx_tctx *tctx = get_tctx();
    styx_window *window = arena_push_struct(tctx->allocator, styx_window);
    
    window->running = TRUE;
    window->handle = CreateWindowA("Styx Window Class",
                                   (LPCTSTR)title.str,
                                   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                   CW_USEDEFAULT, CW_USEDEFAULT,
                                   width, height,
                                   0, 0, 0, 0);
    window->title = push_str8(title.str, title.len);
    window->d3d_renderer = win32_init_renderer(window->handle);
    window->renderer = styx_init_render_queue();
    
    assert_msg(window->handle, "Failed to create window.");
    styxplat_update_window_size(window);
    SetWindowLongPtrA(window->handle, GWLP_USERDATA, (LONG_PTR)window);
    
    return window;
}

styx_function void 
styxplat_destroy_window(styx_window *window)
{
    win32_free_renderer(window->d3d_renderer);
    DestroyWindow(window->handle);
    *window = (styx_window){0};
}

styx_function void
styxplat_process_messages(styx_window *window)
{
    MSG msg = {0};
    while (PeekMessageA(&msg, window->handle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

styx_function void
styxplat_update_window_size(styx_window *window)
{
    if (window == 0) return;
    
    RECT rect = {0};
    GetClientRect(window->handle, &rect);
    window->width = rect.right;
    window->height = rect.bottom;
}

inline void 
styxplat_update_window(styx_window *window, b32 vsync)
{
    win32_update_window(window->renderer, window->d3d_renderer, window->width, window->height, vsync);
}
styx_function styx_module *
styxplat_load_module(str8 mod_path, str8 load, str8 unload, str8 loop,
                     styx_window *window)
{
    if (mod_path.len == 0) return NULL;
    
    styx_tctx *tctx = get_tctx();
    styx_module *module = arena_push_struct(tctx->allocator, styx_module);
    
    module->mod_path = push_str8_copy(tctx->allocator, mod_path);
    module->load_name = push_str8_copy(tctx->allocator, load);
    module->unload_name = push_str8_copy(tctx->allocator, unload);
    module->loop_name = push_str8_copy(tctx->allocator, loop);
    
    temp_arena scratch = begin_temp_arena(tctx->temp_allocator);
    // TODO(sir->w): Write a trimming function.
    // This removes the file extension for the base filename,
    // but it is clearly very buggy.
    str8 trimmed_mod_path = {.str = module->mod_path.str, .len = module->mod_path.len - 4};
    str8 mod_pdb = push_str8_concat(tctx->temp_allocator, trimmed_mod_path, str8_lit(".pdb"));
    str8 tmp_mod_path = push_str8_concat(tctx->temp_allocator, 
                                         trimmed_mod_path, str8_lit("-tmp.dll"));
    str8 tmp_mod_pdb = push_str8_concat(tctx->temp_allocator,
                                        trimmed_mod_path, str8_lit("-tmp.pdb"));
    
    // NOTE(sir->w): Seven tries to load the library. If all fails, then we quit.
    for (u8 i = 0; i < STYX_HOLY_NUMBER; ++i) {
        CopyFileA((LPCSTR)module->mod_path.str, (LPCSTR)tmp_mod_path.str, FALSE);
        CopyFileA((LPCSTR)mod_pdb.str, (LPCSTR)tmp_mod_pdb.str, FALSE);
        module->mod = LoadLibraryA((LPCSTR)tmp_mod_path.str);
        if (module->mod) break;
        
        // Wait 25 milliseconds before retrying.
        Sleep(STYX_HOLY_NUMBER * 5);
    }
    
    end_temp_arena(&scratch);
    
    module->last_write = styxplat_file_last_write(mod_path);
    assert_soft_msg(module->mod, "Failed to load Styx module.");
    
    if (module->mod) {
        module->styx_load = (StyxLoad *)GetProcAddress(module->mod, (LPCSTR)load.str);
        module->styx_unload = (StyxLoad *)GetProcAddress(module->mod, (LPCSTR)unload.str);
        module->styx_update = (StyxLoad *)GetProcAddress(module->mod, (LPCSTR)loop.str);
        
        if (!module->styx_load) module->styx_load = styx_load_stub;
        if (!module->styx_unload) module->styx_unload = styx_unload_stub;
        if (!module->styx_update) module->styx_update = styx_loop_stub;
    } else {
        module->styx_load = styx_load_stub;
        module->styx_unload = styx_unload_stub;
        module->styx_update = styx_loop_stub;
    }
    
    module->styx_load(styx_os, window->renderer);
    
    return module;
}

styx_function void 
styxplat_unload_module(styx_module *module, styx_window *window)
{
    module->styx_unload(styx_os, window->renderer);
    
    FreeLibrary(module->mod);
    
    styx_tctx *tctx = get_tctx();
    temp_arena scratch = begin_temp_arena(tctx->temp_allocator);
    str8 trimmed_mod_path = {.str = module->mod_path.str, .len = module->mod_path.len - 4};
    str8 tmp_mod_path = push_str8_concat(tctx->temp_allocator, 
                                         trimmed_mod_path, str8_lit("-tmp.dll"));
    str8 tmp_mod_pdb = push_str8_concat(tctx->temp_allocator,
                                        trimmed_mod_path, str8_lit("-tmp.pdb"));
    
    for (u8 i = 0; i < STYX_HOLY_NUMBER; ++i) {
        DeleteFileA((LPCSTR)tmp_mod_path.str);
        if (!PathFileExistsA((LPCSTR)tmp_mod_path.str)) {
            break;
        }
    }
    for (u8 i = 0; i < STYX_HOLY_NUMBER; ++i) {
        DeleteFileA((LPCSTR)tmp_mod_pdb.str);
        if (!PathFileExistsA((LPCSTR)tmp_mod_pdb.str)) {
            break;
        }
    }
    
    end_temp_arena(&scratch);
    
    *module = (styx_module){0};
}

inline void 
styxplat_module_update(styx_module *module, styx_window *window)
{
    module->styx_update(styx_os, window->renderer);
}

styx_function void
styxplat_reload_module(styx_module *module, styx_window *window)
{
    styx_tctx *tctx = get_tctx();
    module->styx_unload(styx_os, window->renderer);
    FreeLibrary(module->mod);
    
    module->mod = (HMODULE){0};
    module->styx_load = NULL;
    module->styx_unload = NULL;
    module->styx_update = NULL;
    
    temp_arena scratch = begin_temp_arena(tctx->temp_allocator);
    // TODO(sir->w): Write a trimming function.
    // This removes the file extension for the base filename,
    // but it is clearly very buggy.
    str8 trimmed_mod_path = {.str = module->mod_path.str, .len = module->mod_path.len - 4};
    str8 mod_pdb = push_str8_concat(tctx->temp_allocator, trimmed_mod_path, str8_lit(".pdb"));
    str8 tmp_mod_path = push_str8_concat(tctx->temp_allocator, 
                                         trimmed_mod_path, str8_lit("-tmp.dll"));
    str8 tmp_mod_pdb = push_str8_concat(tctx->temp_allocator,
                                        trimmed_mod_path, str8_lit("-tmp.pdb"));
    
    for (u8 i = 0; i < STYX_HOLY_NUMBER; ++i) {
        DeleteFileA((LPCSTR)tmp_mod_path.str);
        if (!PathFileExistsA((LPCSTR)tmp_mod_path.str)) {
            break;
        }
        
        Sleep(STYX_HOLY_NUMBER * 5);
    }
    for (u8 i = 0; i < STYX_HOLY_NUMBER; ++i) {
        DeleteFileA((LPCSTR)tmp_mod_pdb.str);
        if (!PathFileExistsA((LPCSTR)tmp_mod_pdb.str)) {
            break;
        }
        
        Sleep(STYX_HOLY_NUMBER * 5);
    }
    
    // NOTE(sir->w): Seven tries to load the library. If all fails, then we quit.
    for (u8 i = 0; i < STYX_HOLY_NUMBER; ++i) {
        CopyFileA((LPCSTR)module->mod_path.str, (LPCSTR)tmp_mod_path.str, FALSE);
        CopyFileA((LPCSTR)mod_pdb.str, (LPCSTR)tmp_mod_pdb.str, FALSE);
        module->mod = LoadLibraryA((LPCSTR)tmp_mod_path.str);
        if (module->mod) break;
        
        // Wait 25 milliseconds before retrying.
        Sleep(STYX_HOLY_NUMBER * 5);
    }
    
    end_temp_arena(&scratch);
    
    module->last_write = styxplat_file_last_write(module->mod_path);
    assert_soft_msg(module->mod, "Failed to load Styx module.");
    
    if (module->mod) {
        module->styx_load =
        (StyxLoad *)GetProcAddress(module->mod, (LPCSTR)module->load_name.str);
        module->styx_unload =
        (StyxLoad *)GetProcAddress(module->mod, (LPCSTR)module->unload_name.str);
        module->styx_update =
        (StyxLoad *)GetProcAddress(module->mod, (LPCSTR)module->loop_name.str);
        
        if (!module->styx_load) module->styx_load = styx_load_stub;
        if (!module->styx_unload) module->styx_unload = styx_unload_stub;
        if (!module->styx_update) module->styx_update = styx_loop_stub;
    } else {
        module->styx_load = styx_load_stub;
        module->styx_unload = styx_unload_stub;
        module->styx_update = styx_loop_stub;
    }
    module->styx_load(styx_os, window->renderer);
}

styx_function b32 
styxplat_module_should_reload(styx_module *module)
{
    u64 new_write_time = styxplat_file_last_write(module->mod_path);
    if (new_write_time != module->last_write) {
        module->last_write = new_write_time;
        return TRUE;
    } else {
        return FALSE;
    }
}

styx_function void
win32_init() 
{
    _hinstance = GetModuleHandleA(0);
    _wndclass = (WNDCLASSEXA){
        .cbSize = sizeof(_wndclass),
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = win32_window_proc,
        .hInstance = _hinstance,
        .lpszClassName = "Styx Window Class",
    };
    
    // Set all calls before calling any base functions, or we die.
    _os.reserve_mem = win32_alloc_mem;
    _os.free_mem = win32_free_mem;
    _os.msg_box = win32_msg_box;
    _os.debug_print = win32_debug_print;
    _os.read_entire_file = win32_read_file;
    
    styx_os = &_os;
    
    // NOTE(sir->w): Overusing assertions?
    assert_soft_msg(RegisterClassExA(&_wndclass), "Failed to register window class.");
}

// NOTE(sir->w): This is needed to escape having to call
// some start function in the main.c file to reduce user friction.
int WinMain(HINSTANCE instance, HINSTANCE prev_instance, 
            LPSTR cmd, int show_cmd)
{
    win32_init();
    
    styx_tctx tctx = make_tctx(TCTX_ALLOCATOR_SIZE, TCTX_TEMP_ALLOCATOR_SIZE);
    set_tctx(&tctx);
    
    audio = win32_init_wasapi();
    
    int return_code = styx_main();
    
    win32_free_wasapi(audio);
    free_tctx(&tctx);
    
    return return_code;
}
