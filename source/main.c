#include <stdbool.h>
#include <string.h>

#include <SDL.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

SDL_Window *window;
SDL_Renderer *renderer;

bool running = true;

#define GRID_WIDTH 64
#define GRID_HEIGHT 36
int grid[GRID_HEIGHT][GRID_WIDTH + 2];
int grid_1d[2][GRID_WIDTH + 3];

void render_grid()
{
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	for (int y = 0; y < GRID_HEIGHT; ++y) {
		for (int x = 0; x < GRID_WIDTH; ++x) {
			bool fill = (grid[y][x + 1] == 1);
			
			SDL_Rect fill_rect = { x * (WINDOW_WIDTH / GRID_WIDTH), y * (WINDOW_HEIGHT / GRID_HEIGHT), 
								   WINDOW_WIDTH / GRID_WIDTH, WINDOW_HEIGHT / GRID_HEIGHT };
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
		if (grid[y - 1][1] == 1) break;
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

void render_grid_1d()
{
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	for (int x = 0; x < GRID_WIDTH; ++x) {
		bool fill = (grid_1d[1][x] == 1);
		
		//SDL_Rect fill_rect = { x * (WINDOW_WIDTH / GRID_WIDTH), (WINDOW_HEIGHT / 2) - ((WINDOW_WIDTH / GRID_WIDTH) / 2), 
		//					   WINDOW_WIDTH / GRID_WIDTH, WINDOW_HEIGHT / GRID_HEIGHT };
		SDL_Rect fill_rect = { x * 20, 300, 20, 20 };
		//SDL_Log("%d", (WINDOW_HEIGHT / 2) - (GRID_HEIGHT / 2));
		if (fill) {
			SDL_RenderFillRect(renderer, &fill_rect);
		} else {
			SDL_RenderDrawRect(renderer, &fill_rect);
		}
	}
}

void animate_rule30() 
{
	static bool init = true;
	if (init) {
		grid_1d[0][GRID_WIDTH / 2] = 1;
		init = false;
		return;
	}

	for (int x = 1; x < GRID_WIDTH; ++x) {
		if (grid_1d[0][x - 1] == 1 &&
			grid_1d[0][x]     == 1 &&
			grid_1d[0][x + 1] == 1) {
			grid_1d[1][x] = 0;
		} else if (grid_1d[0][x - 1] == 1 &&
				   grid_1d[0][x]     == 1 &&
				   grid_1d[0][x + 1] == 0) {
			grid_1d[1][x] = 0;
		} else if (grid_1d[0][x - 1] == 1 &&
				   grid_1d[0][x]     == 0 &&
				   grid_1d[0][x + 1] == 1) {
			grid_1d[1][x] = 0;
		} else if (grid_1d[0][x - 1] == 1 &&
				   grid_1d[0][x]     == 0 &&
				   grid_1d[0][x + 1] == 0) {
			grid_1d[1][x] = 1;
		} else if (grid_1d[0][x - 1] == 0 &&
				   grid_1d[0][x]     == 1 &&
				   grid_1d[0][x + 1] == 1) {
			grid_1d[1][x] = 1;
		} else if (grid_1d[0][x - 1] == 0 &&
				   grid_1d[0][x]     == 1 &&
				   grid_1d[0][x + 1] == 0) {
			grid_1d[1][x] = 1;
		} else if (grid_1d[0][x - 1] == 0 &&
				   grid_1d[0][x]     == 0 &&
				   grid_1d[0][x + 1] == 1) {
			grid_1d[1][x] = 1;
		} else if (grid_1d[0][x - 1] == 0 &&
				   grid_1d[0][x]     == 0 &&
				   grid_1d[0][x + 1] == 0) {
			grid_1d[1][x] = 0;
		}  
	}

	for (int i = 0; i < GRID_WIDTH; ++i) {
		grid_1d[0][i] = grid_1d[1][i];
	}
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
	
	window = SDL_CreateWindow("Project Ananke", 
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  WINDOW_WIDTH, WINDOW_HEIGHT,
							  SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//rule_30();
	
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

		animate_rule30();
		render_grid_1d();
		//render_grid();

		SDL_RenderPresent(renderer);

		SDL_Delay(150);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}
