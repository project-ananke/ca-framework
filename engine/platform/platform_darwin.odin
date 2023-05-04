package platform

import "core:log"
import "core:strings"
import "core:mem"
import "core:os"
import "core:time"
import "core:fmt"

import SDL "vendor:sdl2"

import NS "vendor:darwin/Foundation"
import MTL "vendor:darwin/Metal"
import CA "vendor:darwin/QuartzCore"

import "engine:core/styxm"
import "engine:core/styx2d"

Window :: struct
{
	title: string,
	width, height: u32,
	running: bool, 

	handle: ^SDL.Window,
	renderer: ^SDL.Renderer,

	keys: [^]u8,

	_native_renderer: MetalRenderer,
}

init :: proc()
{
	SDL.SetHint(SDL.HINT_RENDER_DRIVER, "metal")
	SDL.setenv("METAL_DEVICE_WRAPPER_TYPE", "1", 0)

    SDL.Init({.VIDEO})
}

quit :: proc()
{
	SDL.Quit()
}

init_window :: proc(width: u32, height: u32, title: string) -> (window: ^Window)
{
	window = new(Window)
	
	window.width = width;
	window.height = height;
	window.title = title;
	
	c_title := strings.clone_to_cstring(title, context.temp_allocator)
	window.handle = SDL.CreateWindow(c_title,
	                                 SDL.WINDOWPOS_CENTERED, SDL.WINDOWPOS_CENTERED,
	                                 i32(width), i32(height), { .RESIZABLE, .HIDDEN })

	window.renderer = SDL.CreateRenderer(window.handle, -1,
	                                     { .ACCELERATED, .PRESENTVSYNC })

	err: ^NS.Error
	window._native_renderer, err = init_renderer(window.handle, width, height, styx2d.MAX_VERTEX_COUNT)

	size: i32 = 256
	window.keys = SDL.GetKeyboardState(&size)

	// TODO(sir->w7): Error handling.
	if err != nil {
		fmt.eprintln(err->localizedDescription()->odinString())
	}

	window.running = true
	SDL.ShowWindow(window.handle)

	return
}

free_window :: proc(using window: ^Window)
{
	SDL.DestroyRenderer(renderer)
	SDL.DestroyWindow(handle)
	
	free(window)
}

window_process :: proc(using window: ^Window)
{
	for e: SDL.Event; SDL.PollEvent(&e); {
		#partial switch e.type {
		case .QUIT:
			running = false
		case .KEYDOWN, .KEYUP:
			if e.type == .KEYUP && e.key.keysym.sym == .ESCAPE {
				SDL.PushEvent(&SDL.Event{type = .QUIT})
			}
		}
	}
}

window_cap_fps :: proc(cap: u32)
{
	@static time_last: time.Tick
	if time_last._nsec == 0 {
		time_last = time.tick_now()
		return
	}

	d := time.tick_since(time_last)
	d_ms := time.duration_milliseconds(d)

	ms_cap := 1000.0 / f64(cap)
	if d_ms < ms_cap {
		// One millisecond is 1/1000 of a second.
		// One nanosecond is 1/1000000000 of a second. 
		time.sleep(time.Duration((ms_cap - d_ms) * 1000000.0))
	}

	time_last = time.tick_now()
}

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
