#include <SDL.h>

#include "../base/styx_base.h"
#include "styx_window.h"

void styx_message_box(char *title, char *message)
{
    SDL_ShowSimpleMessageBox(0, title, message, NULL);
}

styx_window styx_create_window(char *title, u32 width, u32 height)
{
    styx_window window = {0};

    window.width = width;
    window.height = height;

    window.window = SDL_CreateWindow(title, 
                                     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     width, height,
                                     SDL_WINDOW_SHOWN);
    window.renderer = SDL_CreateRenderer(window.window, -1, SDL_RENDERER_ACCELERATED);

    window.running = TRUE;
    
    return window;
}

void styx_destroy_window(styx_window *window)
{
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
}

void styx_process_messages(styx_window *window)
{
	SDL_Event e = {0};
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            window->running = FALSE;
            break;
        }
    }
}

void styx_window_clear(styx_window *window, vec3c col)
{
    SDL_SetRenderDrawColor(window->renderer, col.r, col.g, col.b, 0xFF);
    SDL_RenderClear(window->renderer);
}

void styx_window_update(styx_window *window)
{
    SDL_RenderPresent(window->renderer);
}
