#include "engine/styx_engine.h"

#define GRID_WIDTH 64
#define GRID_HEIGHT 36

int grid[GRID_HEIGHT][GRID_WIDTH] = {0};

void rule30() 
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

int main(int argc, char *argv[])
{
	init_styx_engine();
		
	styx_settings settings = styx_load_settings("source/settings.lua");
	styx_window window = styx_create_window("Project Ananke", settings.width, settings.height);

	if (luaL_dofile(L, "scripts/rule30.lua") != LUA_OK) {
		fprintln(stderr, "%s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	lua_getglobal(L, "grid");
	for (int y = 0; y < GRID_HEIGHT; ++y) {
		lua_rawgeti(L, -1, y + 1);
		for (int x = 0; x < GRID_WIDTH; ++x) {
			lua_rawgeti(L, -1, x + 1);
			grid[y][x] = lua_tointeger(L, -1);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}

	while (window.running) {
		styx_process_messages(&window);
		styx_window_clear(&window, vec3c_lit(0xFF, 0xFF, 0xFF));

		styx_render_grid(&window, (int *)grid, GRID_WIDTH, GRID_HEIGHT);
		
		styx_window_update(&window);
	}
	
	quit_styx_engine();
	
	return 0;
}
