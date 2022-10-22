#ifndef STYXSYS_H
#define STYXSYS_H

int styxsys_list_dir(lua_State *L);
int styxsys_chdir(lua_State *L);
int styxsys_get_exe_path(lua_State *L);

int styxsys_message_box(lua_State *L);

int styxsys_process_messages(lua_State *L);
int styxsys_window_clear(lua_State *L);
int styxsys_window_update(lua_State *L);
int styxsys_window_running(lua_State *L);

int luaopen_styxsys(lua_State *L);

#endif
