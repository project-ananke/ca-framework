package luasys

import "core:os"
import "core:runtime"
import "core:c"
import "core:log"

import SDL "vendor:sdl2"

import "ext:odin-lua/lua"
import "ext:odin-lua/luaL"

import "engine:common"
import "engine:platform"

// Memory leak in a system call somewhere here. 
list_dir :: proc "c" (L: ^lua.State) -> c.int
{
	context = common.styx_context()

	path := string(luaL.checkstring(L, 1))
	file_entries := platform.list_dir(path, context.temp_allocator)

	lua.newtable(L)
	for i := 0; i < len(file_entries); i += 1 {
		lua.pushstring(L, file_entries[i].base_name, context.temp_allocator)
		lua.rawseti(L, -2, i64(i + 1))
	}

	return 1
}

change_dir :: proc "c" (L: ^lua.State) -> c.int
{
	context = common.styx_context()
	path := string(luaL.checkstring(L, 1))
	platform.change_dir(path)

	return 0
}

get_exe_path :: proc "c" (L: ^lua.State) -> c.int
{
	context = common.styx_context()
	path := string(SDL.GetBasePath())

	lua.pushstring(L, path, context.temp_allocator)

	return 1
}

message_box :: proc "c" (L: ^lua.State) -> c.int
{
	context = common.styx_context()

	title := luaL.checkstring(L, 1)
	message := luaL.checkstring(L, 2)

	SDL.ShowSimpleMessageBox({}, title, message, nil)

	return 0
}

styxsys_lib := [?]luaL.Reg {
	{ "list_dir", list_dir },
	{ "chdir", change_dir },
	{ "get_exe_path", get_exe_path },
	{ "message_box", message_box },
	{ nil, nil },
}

luaopen :: proc "c" (L: ^lua.State) -> c.int
{
	context = common.styx_context()
	luaL.newlib(L, styxsys_lib[:])
	
	return 1
}