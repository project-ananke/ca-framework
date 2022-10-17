#include <stdio.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../../base/styx_base.h"

#include "styxlua.h"

lua_State *L;

int styx_get_globalint(char *var) 
{
	int isnum, result;
	lua_getglobal(L, var);
	result = (int)lua_tointegerx(L, -1, &isnum);
	if (!isnum) {
		//TODO: logging
	}

	lua_pop(L, 1);
	
	return result;
}

struct styx_settings
styx_load_settings(char *filename)
{
	struct styx_settings settings = {0};
	
	if (luaL_dofile(L, filename) != LUA_OK) {
		// Implement logging system. 
	}

	settings.width = styx_get_globalint("width");
	settings.height = styx_get_globalint("height");

	return settings;
}
