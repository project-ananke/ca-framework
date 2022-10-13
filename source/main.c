#include <stdbool.h>

#include <SDL.h>

SDL_Window *window;
bool running = true;

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
	
	window = SDL_CreateWindow("Project Ananke", 
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  1280, 720,
							  SDL_WINDOW_ALLOW_HIGHDPI);

	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				running = false;
				break;
			}
		}
	}
	
	SDL_DestroyWindow(window);
}
