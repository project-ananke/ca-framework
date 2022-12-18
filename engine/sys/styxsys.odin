package styxsys

import "core:os"
import "core:runtime"
import "core:c"
import "core:log"

import "engine:core"

import "ext:odin-lua/lua"
import "ext:odin-lua/luaL"

import SDL "vendor:sdl2"

// Memory leak in a system call somewhere here. 
list_dir :: proc "c" (L: ^lua.State) -> c.int
{
	context = core.styx_context()

	path := string(luaL.checkstring(L, 1))
	handle, err := os.open(path, int(os.File_Mode_Dir))

	if err != 0 {
		lua.pushnil(L)
		lua.pushstring(L, os.get_last_error_string(), context.temp_allocator)
		return 2
	}
	defer os.close(handle)

	// check again
	file_info, err2 := os.read_dir(handle, 0, context.temp_allocator)
	if err2 != 0 {
		lua.pushnil(L)
		lua.pushstring(L, os.get_last_error_string(), context.temp_allocator)

		return 2
	}

	lua.newtable(L)
	for i := 0; i < len(file_info); i += 1 {
		lua.pushstring(L, file_info[i].name, context.temp_allocator)
		lua.rawseti(L, -2, i64(i + 1))
	}

	return 1
}

chdir :: proc "c" (L: ^lua.State) -> c.int
{
	context = core.styx_context()
	path := luaL.checklstring(L, 1, nil)

when ODIN_OS == .Darwin {
	err := os._unix_chdir(path)
}

	return err
}

get_exe_path :: proc "c" (L: ^lua.State) -> c.int
{
	context = core.styx_context()
	path := string(SDL.GetBasePath())

	lua.pushstring(L, path, context.temp_allocator)

	return 1
}

message_box :: proc "c" (L: ^lua.State) -> c.int
{
	context = core.styx_context()

	title := luaL.checkstring(L, 1)
	message := luaL.checkstring(L, 2)

	SDL.ShowSimpleMessageBox({}, title, message, nil)

	return 0
}

styxsys_lib := [?]luaL.Reg {
	{ "list_dir", list_dir },
	{ "chdir", chdir },
	{ "get_exe_path", get_exe_path },
	{ "message_box", message_box },
	{ nil, nil },
}

luaopen :: proc "c" (L: ^lua.State) -> c.int
{
	context = core.styx_context()
	luaL.newlib(L, styxsys_lib[:])
	
	return 1
}

/*int styxsys_list_dir(lua_State *L);
int styxsys_chdir(lua_State *L);
int styxsys_get_exe_path(lua_State *L);

int styxsys_message_box(lua_State *L);

int styxsys_process_messages(lua_State *L);
int styxsys_window_clear(lua_State *L);
int styxsys_window_update(lua_State *L);
int styxsys_window_running(lua_State *L);*/