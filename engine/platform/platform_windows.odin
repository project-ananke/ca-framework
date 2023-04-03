package platform

import "core:log"
import "core:strings"
import "core:mem"
import "core:os"

import win32 "core:sys/windows"

get_error_string :: proc(err: os.Errno, allocator := context.allocator) -> string
{
	buf: win32.LPWSTR
	err := win32.FormatMessageW(win32.FORMAT_MESSAGE_FROM_SYSTEM,
	                            nil, win32.DWORD(err), 0,
	                            buf, 0, nil)
	if err == 0 do return ""
	defer win32.LocalFree(buf)

	res, _ := win32.wstring_to_utf8(buf, -1, allocator)

	return res
}

list_dir :: proc(dir: string, allocator := context.allocator) -> []File_Entry
{
	handle, err := os.open(dir)
	if err != 0 {
		log.errorf("Failed to create a handle to the directory: %s.", get_error_string(err))
		return nil
	}
	defer os.close(handle)

	dir, err2 := os.read_dir(handle, 0, allocator)
	if err2 != 0 {
		log.errorf("Failed to read_dir: %s.", get_error_string(err))
		return nil
	}

	files := make([]File_Entry, len(dir), allocator)
	for file, i in dir {
		files[i].base_name = file.name
		files[i].abs_path = file.fullpath
	}

	return files
}

// I don't know. I guess if you use context.allocator instead of context.temp_allocator
free_file_entries :: proc(file_entries: []File_Entry)
{
	for file, _ in file_entries {
		delete(file.base_name)
		delete(file.abs_path)
	}
}

change_dir :: proc(path: string)
{
	os.change_directory(path)
}
