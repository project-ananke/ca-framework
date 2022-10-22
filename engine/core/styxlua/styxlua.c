#include <stdio.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../../base/styx_base.h"
#include "../../sys/styxsys.h"
#include "../../sys/styx_renderer.h"

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

static const luaL_Reg libs[] = {
  { "styxsys", luaopen_styxsys },
  { "styxren", luaopen_styxren },
  { NULL, NULL }
};

void styxlua_load_libs(lua_State *L) 
{
  for (int i = 0; libs[i].name; i++) {
    luaL_requiref(L, libs[i].name, libs[i].func, 1);
  }
}
