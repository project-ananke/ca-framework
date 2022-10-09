#include "macos_platform.h"

styx_global StyxOS _os;

styx_function void
mac_msg_box(char *title, char *msg)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title, msg, NULL);
}

styx_function void *
mac_alloc_mem(u64 size)
{
    return mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
}

styx_function void
mac_free_mem(void *mem, u64 size)
{
    munmap(mem, size);
}

styx_function styx_window *
styxplat_create_window(str8 title, u32 width, u32 height)
{
    styx_tctx *tctx = get_tctx();
    styx_window *window = arena_push_struct(tctx->allocator, styx_window);

    window->running = TRUE;
    window->handle = SDL_CreateWindow((const char *)title.str, 
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                      width, height, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    window->title = push_str8(title.str, title.len);

    return window;
}

styx_function void
styxplat_destroy_window(styx_window *window)
{
    SDL_DestroyWindow(window->handle);
}

styx_function void
styxplat_process_messages(styx_window *window)
{
    SDL_Event e = {0};
    while (SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT) {
            window->running = FALSE;
        }
    }
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        fprintln(stderr, "Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    _os.reserve_mem = mac_alloc_mem;
    _os.free_mem = mac_free_mem;
    _os.msg_box = mac_msg_box;

    styx_os = &_os;

    styx_tctx tctx = make_tctx(TCTX_ALLOCATOR_SIZE, TCTX_TEMP_ALLOCATOR_SIZE);
    set_tctx(&tctx);
    int return_code = styx_main();
    free_tctx(&tctx);	

    SDL_Quit();

    return return_code;
}
