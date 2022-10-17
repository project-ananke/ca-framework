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

void styx_message_box(char *title, char *message);

styx_window styx_create_window(char *title, u32 width, u32 height);
void styx_destroy_window(styx_window *window);

void styx_process_messages(styx_window *window);
void styx_window_clear(styx_window *window, vec3c col);
void styx_window_update(styx_window *window);

#endif
