#include <SDL.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "base/styx_base.h"
#include "sys/styx_window.h"
#include "sys/styx_renderer.h"

#include "core/styxlua/styxlua.h"

const char *base_script = 
    "xpcall(function()\n"
    "   exe_path = styxsys.get_exe_path()\n"
    "   exe_dir = exe_path:match(\"^(.+)[/\\\\].*$\")\n"
    "   styxsys.chdir(exe_dir)\n"
    "   package.path = exe_dir .. \"/../source/?/init.lua;\" .. package.path\n"
    "   package.path = exe_dir .. \"/../source/?.lua;\" .. package.path\n"
    "   package.path = exe_dir .. \"/../source/scripts/?.lua;\" .. package.path\n"
    "   local styxengine = require('styxengine')\n"
    "   styxengine.run()\n"
    "end, function(err)\n"
    "  print('Error: ' .. tostring(err))\n"
    "end)\n";

int main(int argc, char *argv[])
{
    L = luaL_newstate();
    luaL_openlibs(L);
    styxlua_load_libs(L);

    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);

    styxsys_create_window("Project Ananke", 1280, 720);

    if (luaL_dostring(L, base_script) != LUA_OK) {
        fprintln(stderr, "Base script error: %s", lua_tostring(L, -1));
        lua_pop(L, -1);
        return -1;
    }

    styxsys_destroy_window();

    lua_close(L);

    return 0;
}
