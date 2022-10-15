#include <SDL.h>

#include "styx_engine.h"

void init_styx_engine()
{
    styx_init_lua();
    SDL_Init(SDL_INIT_VIDEO);
}

void quit_styx_engine()
{
    SDL_Quit();
    styx_quit_lua();
}   
