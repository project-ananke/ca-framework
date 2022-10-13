#include <stdbool.h>

#include <SDL.h>

SDL_Window *window;
SDL_Renderer *renderer;

bool running = true;

#define GRID_WIDTH 64
#define GRID_HEIGHT 36
int grid[GRID_HEIGHT][GRID_WIDTH + 2];

void render_grid()
{
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	for (int y = 0; y < GRID_HEIGHT; ++y) {
		for (int x = 0; x < GRID_WIDTH; ++x) {
			bool fill = (grid[y][x + 1] == 1);
			
			SDL_Rect fill_rect = { x * 20, y * 20, 20, 20 };
			if (fill) {
				SDL_RenderFillRect(renderer, &fill_rect);
			} else {
				SDL_RenderDrawRect(renderer, &fill_rect);
			}
		}
	}
}

void rule_30() 
{
	grid[0][GRID_WIDTH / 2] = 1;

	for (int y = 1; y < GRID_HEIGHT; ++y) {
		for (int x = 1; x < GRID_WIDTH - 1; ++x) {
			if (grid[y - 1][x - 1] == 1 &&
				grid[y - 1][x]     == 1 &&
				grid[y - 1][x + 1] == 1) {
				grid[y][x] = 0;
			} else if (grid[y - 1][x - 1] == 1 &&
				       grid[y - 1][x]     == 1 &&
				       grid[y - 1][x + 1] == 0) {
				grid[y][x] = 0;
			} else if (grid[y - 1][x - 1] == 1 &&
				       grid[y - 1][x]     == 0 &&
				       grid[y - 1][x + 1] == 1) {
				grid[y][x] = 0;
			} else if (grid[y - 1][x - 1] == 1 &&
				       grid[y - 1][x]     == 0 &&
				       grid[y - 1][x + 1] == 0) {
				grid[y][x] = 1;
			} else if (grid[y - 1][x - 1] == 0 &&
				       grid[y - 1][x]     == 1 &&
				       grid[y - 1][x + 1] == 1) {
				grid[y][x] = 1;
			} else if (grid[y - 1][x - 1] == 0 &&
				       grid[y - 1][x]     == 1 &&
				       grid[y - 1][x + 1] == 0) {
				grid[y][x] = 1;
			} else if (grid[y - 1][x - 1] == 0 &&
				       grid[y - 1][x]     == 0 &&
				       grid[y - 1][x + 1] == 1) {
				grid[y][x] = 1;
			} else if (grid[y - 1][x - 1] == 0 &&
				       grid[y - 1][x]     == 0 &&
				       grid[y - 1][x + 1] == 0) {
				grid[y][x] = 0;
			}  
		}
	}
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
	
	window = SDL_CreateWindow("Project Ananke", 
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  1280, 720,
							  SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	rule_30();
	
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				running = false;
				break;
			}
		}
		
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		render_grid();
		
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}
