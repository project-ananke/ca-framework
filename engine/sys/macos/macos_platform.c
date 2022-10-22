// Cool Mac exclusive crap.
#include <mach-o/dyld.h>

// POSIX API
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

#include <SDL.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../../base/styx_base.h"
#include "../styxsys.h"
#include "../styx_window.h"

int styxsys_list_dir(lua_State *L)
{
    char *path = luaL_checkstring(L, 1);

    DIR *dir = opendir(path);
    if (dir == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    lua_newtable(L);
    struct dirent *entry = NULL;
    for (int i = 1; (entry = readdir(dir)) != NULL;) {
        lua_pushnumber(L, i++);
        lua_pushstring(L, entry->d_name);
        lua_settable(L, -3);
    }

    closedir(dir);
    return 1;
}

int styxsys_chdir(lua_State *L)
{
    char *path = luaL_checkstring(L, 1);
    int err = chdir(path);
    if (err) luaL_error(L, "chdir() failed");

    return 0;
}

#define PATH_BUF_SIZE 1024
int styxsys_get_exe_path(lua_State *L)
{
    u32 bufsize = PATH_BUF_SIZE;
    char buf[PATH_BUF_SIZE];

    _NSGetExecutablePath(buf, &bufsize);
    lua_pushstring(L, buf);

    return 1;
}

int styxsys_message_box(lua_State *L)
{
    char *title = luaL_checkstring(L, 1);
    char *message = luaL_checkstring(L, 2);
    SDL_ShowSimpleMessageBox(0, title, message, NULL);

    return 0;
}

int styxsys_process_messages(lua_State *L)
{
    SDL_Event e = {0};
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            _global_window.running = FALSE;
            break;
        }
    }

    return 0;
}

int styxsys_window_clear(lua_State *L)
{
    int r = luaL_checkinteger(L, 1);
    int g = luaL_checkinteger(L, 2);
    int b = luaL_checkinteger(L, 3);

    SDL_SetRenderDrawColor(_global_window.renderer, r, g, b, 0xFF);
    SDL_RenderClear(_global_window.renderer);

    return 0;
}

int styxsys_window_update(lua_State *L)
{
    SDL_RenderPresent(_global_window.renderer);
    return 0;
}

int styxsys_window_running(lua_State *L)
{
    lua_pushboolean(L, _global_window.running);
    return 1;
}


static const luaL_Reg lib[] = {
    { "list_dir", styxsys_list_dir },
    { "chdir", styxsys_chdir },
    { "get_exe_path", styxsys_get_exe_path },
    { "message_box", styxsys_message_box },
    { "process_messages", styxsys_process_messages },
    { "window_clear", styxsys_window_clear },
    { "window_update", styxsys_window_update },
    { "window_running", styxsys_window_running }, 
    { NULL, NULL },
};

int luaopen_styxsys(lua_State *L)
{
    luaL_newlib(L, lib);
    return 1;
}
