#include <SDL.h>

#include "../base/styx_base.h"
#include "styx_window.h"

styx_window _global_window;

void styx_message_box(char *title, char *message)
{
    SDL_ShowSimpleMessageBox(0, title, message, NULL);
}

void styxsys_create_window(char *title, u32 width, u32 height)
{
    _global_window.width = width;
    _global_window.height = height;

    _global_window.window = SDL_CreateWindow(title, 
                                     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     width, height,
                                     SDL_WINDOW_SHOWN);
    _global_window.renderer = SDL_CreateRenderer(_global_window.window, -1, SDL_RENDERER_ACCELERATED);

    _global_window.running = TRUE;
}

void styxsys_destroy_window()
{
    SDL_DestroyRenderer(_global_window.renderer);
    SDL_DestroyWindow(_global_window.window);
}

