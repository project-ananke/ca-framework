#include <SDL.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "base/styx_base.h"
#include "sys/styx_window.h"
#include "sys/styx_renderer.h"

#include "core/styxlua/styxlua.h"

int main(int argc, char *argv[])
{
    L = luaL_newstate();
    luaL_openlibs(L);
    styxlua_load_libs(L);

    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);

    styxsys_create_window("Project Ananke", 1280, 720);

    if (luaL_dofile(L, "source/styxengine.lua") != LUA_OK) {
        fprintln(stderr, "%s", lua_tostring(L, -1));
        lua_pop(L, 1);
        return -1;
    }

    styxsys_destroy_window();

    lua_close(L);

    return 0;
}
