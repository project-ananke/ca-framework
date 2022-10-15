#include <SDL.h>

#include "../base/styx_base.h"

#include "styx_window.h"
#include "styx_renderer.h"

void styx_render_grid(styx_window *window, int *grid, u32 width, u32 height)
{
	u32 grid_width = window->width / width;
	u32 grid_height = window->height / height;
	
	SDL_SetRenderDrawColor(window->renderer, 0x00, 0x00, 0x00, 0xFF);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			b32 fill = (*((grid + y * width) + x + 1) == 1);

			SDL_Rect fill_rect = { x * grid_width, y * grid_height, grid_width, grid_height };
			if (fill) {
				SDL_RenderFillRect(window->renderer, &fill_rect);
			} else {
				SDL_RenderDrawRect(window->renderer, &fill_rect);
			}
		}
	}
}
