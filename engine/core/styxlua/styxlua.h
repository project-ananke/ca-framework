#ifndef STYXLUA_H
#define STYXLUA_H

extern lua_State *L;

int styx_get_globalint(char *var);

void styxlua_load_libs(lua_State *L);

#endif
