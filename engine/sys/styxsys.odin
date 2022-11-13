package styxsys

import "core:os"
import "core:runtime"
import "core:c"

import "ext:odin-lua/lua"
import "ext:odin-lua/luaL"

// TODO(sir->w7): Make sure that this is a reasonable number. 
MAX_FILE_DIR :: 256

list_dir :: proc "c" (L: ^lua.State) -> c.int
{
	context = runtime.default_context()

	path := cast(string)luaL.checklstring(L, 1, nil)
	handle, err := os.open(path)

	if err != 0 {
		luaL.pushnil(L)
		lua.pushstring(L, os.get_last_error_string())
		return 2
	}
	defer os.close(handle)

// check again
	file_info, err2 := os.read_dir(handle, 0)
	if err2 != 0 {
		luaL.pushnil(L)
		lua.pushstring(L, os.get_last_error_string())
		return 2
	}

	for file, idx in file_info {
		lua.pushnumber(L, f32(idx + 1))
		lua.pushstring(L, file.name)
		lua.settable(L, -3)
	}

	return 1
}

styxsys_lib := [?]luaL.Reg {
	luaL.Reg{ "list_dir", list_dir },
}

luaopen_styxsys :: proc(L: ^lua.State) -> int
{
	luaL.newlib(L, styxsys_lib)
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