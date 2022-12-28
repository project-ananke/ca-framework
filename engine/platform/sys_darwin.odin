package platform

import "core:log"
import "core:strings"
import "core:mem"
import "core:os"

list_dir :: proc(dir: string, allocator := context.allocator) -> []File_Entry
{
	handle, err := os.open(dir)
	if err != 0 {
		log.errorf("Failed to create a handle to the directory: %s.", os.get_last_error_string())
		return nil
	}
	defer os.close(handle)

	dir, err2 := os.read_dir(handle, 0, allocator)
	if err2 != 0 {
		log.errorf("Failed to read_dir: %s.", os.get_last_error_string())
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

change_dir :: proc(_path: string, allocator := context.temp_allocator)
{
	path := strings.clone_to_cstring(_path, allocator)
	os._unix_chdir(path)
}