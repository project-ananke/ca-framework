#include <SDL.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../base/styx_base.h"

#include "styx_window.h"
#include "styx_renderer.h"

int styxren_render_rect(lua_State *L)
{
    int x = 0, y = 0, w = 0, h = 0, r = 0, g = 0, b = 0;
    b32 fill = FALSE;

    {
        luaL_checktype(L, 1, LUA_TTABLE);
        lua_getfield(L, 1, "x");
        lua_getfield(L, 1, "y");
        x = luaL_checkinteger(L, -2);
        y = luaL_checkinteger(L, -1);
        lua_pop(L, 2);
    }
    
    {
        luaL_checktype(L, 2, LUA_TTABLE);
        lua_getfield(L, 2, "w");
        lua_getfield(L, 2, "h");
        w = luaL_checkinteger(L, -2);
        h = luaL_checkinteger(L, -1);
        lua_pop(L, 2);
    }
    
    {
        luaL_checktype(L, 3, LUA_TTABLE);
        lua_getfield(L, 3, "r");
        lua_getfield(L, 3, "g");
        lua_getfield(L, 3, "b");
        r = luaL_checkinteger(L, -3);
        g = luaL_checkinteger(L, -2);
        b = luaL_checkinteger(L, -1);
        lua_pop(L, 3);
    }
    
    luaL_checktype(L, 4, LUA_TBOOLEAN);
    fill = lua_toboolean(L, 4);

	SDL_Rect fill_rect = { x, y, w, h };
	
	SDL_SetRenderDrawColor(_global_window.renderer, r, g, b, 0xFF);
	
	if (fill) SDL_RenderFillRect(_global_window.renderer, &fill_rect);
	else SDL_RenderDrawRect(_global_window.renderer, &fill_rect);

    return 0;
}

static const luaL_Reg lib[] = {
    { "render_rect", styxren_render_rect }, 
    { NULL, NULL },
};

int luaopen_styxren(lua_State *L)
{
    luaL_newlib(L, lib);
    return 1;
}

