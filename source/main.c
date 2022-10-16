#include "engine/styx_engine.h"

#define GRID_WIDTH 64
#define GRID_HEIGHT 36

int grid[GRID_HEIGHT][GRID_WIDTH] = {0};

int main(int argc, char *argv[])
{
	init_styx_engine();
		
	styx_settings settings = styx_load_settings("source/settings.lua");
	styx_window window = styx_create_window("Project Ananke", settings.width, settings.height);

	{
		lua_pushinteger(L, GRID_WIDTH);
		lua_setglobal(L, "grid_width");
		lua_pushinteger(L, GRID_HEIGHT);
		lua_setglobal(L, "grid_height");

		lua_newtable(L);
		for (int y = 0; y < GRID_HEIGHT; ++y) {
			lua_newtable(L);
			for (int x = 0; x < GRID_WIDTH; ++x) {
				lua_pushinteger(L, 0);
				lua_rawseti(L, -2, x + 1);
			}
			lua_rawseti(L, -2, y + 1);
		}
		lua_setglobal(L, "grid");
	}
	
	if (luaL_dofile(L, "scripts/rule90.lua") != LUA_OK) {
		fprintln(stderr, "%s", lua_tostring(L, -1));
		lua_pop(L, 1);
		return -1;
	}
	{
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
