package platform

import "core:os"
import "core:log"
import "core:strings"
import "core:mem"

File_Entry :: struct
{
	base_name: string,
	abs_path: string,
}

list_files :: proc(dir: string, allocator := context.allocator) -> []File_Entry
{
	files := make([dynamic]File_Entry, allocator)

	handle, err := os.open(dir, os.O_RDONLY)
	if err != 0 {
		log.errorf("Failed to create a handle to the directory: %s.", os.get_last_error_string())
		return nil
	}
	defer os.close(handle)

	dir := os._unix_fdopendir(handle)
	if dir == nil {
		log.errorf("Failed to open the directory: %s.", os.get_last_error_string())
		return nil
	}
	defer os._unix_closedir(dir)

	dirpath: string 
	dirpath, err = os.absolute_path_from_handle(handle)
	if err != 0 {
		return nil
	}
	defer delete(dirpath)

	for {
		direntry: os.Dirent
		result: ^os.Dirent

		err := os._unix_readdir_r(dir, &direntry, &result)
		if err != 0 || result == nil {
			break
		}
		
		file_entry: File_Entry
		file_entry.base_name = strings.clone_from_cstring(transmute(cstring)mem.Raw_Cstring{ data = &direntry.name[0] }, allocator)
		file_entry.abs_path = strings.join([]string{dirpath, file_entry.base_name}, "/", allocator)

		if (os.is_file_path(file_entry.abs_path)) {
			append(&files, file_entry)
		} else {
			delete(file_entry.base_name, allocator)
			delete(file_entry.abs_path, allocator)
		}
	}

	return files[:]
}

// I don't know. I guess if you use context.allocator instead of context.temp_allocator
free_file_entries :: proc(file_entries: []File_Entry)
{
	for file, _ in file_entries {
		delete(file.base_name)
		delete(file.abs_path)
	}
}