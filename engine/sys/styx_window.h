#ifndef WINDOW_H
#define WINDOW_H

typedef struct styx_window styx_window;
struct styx_window
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    u32 width, height;
    b32 running;
};

extern styx_window _global_window;

// Do we need a reimplementation of this for styxsys?
void styx_message_box(char *title, char *message);

void styxsys_create_window(char *title, u32 width, u32 height);
void styxsys_destroy_window();

#endif
