#ifndef MACOS_PLATFORM_H
#define MACOS_PLATFORM_H

typedef struct styx_window styx_window;
struct styx_window
{
	SDL_Window *handle;

    u32 width, height;
    str8 title;
    b32 running;
};

styx_function int styx_main(void);

styx_function styx_window *styxplat_create_window(str8 title, u32 width, u32 height);
styx_function void styxplat_destroy_window(styx_window *window);

#endif
