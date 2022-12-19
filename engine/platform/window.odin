package platform

import "core:strings"

import SDL "vendor:SDL2"

import "engine:core/styxm"

Window :: struct
{
	title: string,
	width, height: u32,
	running: bool, 

	handle: ^SDL.Window,
	renderer: ^SDL.Renderer,
}

init_window :: proc(width: u32, height: u32, title: string) -> (window: Window)
{
	window.width = width;
	window.height = height;
	window.title = title;
	
	c_title := strings.clone_to_cstring(title, context.temp_allocator)
	window.handle = SDL.CreateWindow(c_title,
	                                 SDL.WINDOWPOS_CENTERED, SDL.WINDOWPOS_CENTERED,
	                                 i32(width), i32(height), nil)

	window.renderer = SDL.CreateRenderer(window.handle, -1,
	                                     { .ACCELERATED })

	window.running = true

	return
}

free_window :: proc(using window: ^Window)
{
	SDL.DestroyRenderer(renderer)
	SDL.DestroyWindow(handle)
}

window_process :: proc(using window: ^Window)
{
	for e: SDL.Event; SDL.PollEvent(&e); {
		#partial switch e.type {
		case .QUIT:
			running = false
		case .KEYDOWN:
			if e.key.keysym.sym == .ESCAPE {
				running = false
			}
		}
	}
}

window_clear :: proc(using window: ^Window, col: styxm.Vec3c)
{
	SDL.SetRenderDrawColor(renderer, col.x, col.y, col.z, 0xFF)
	SDL.RenderClear(renderer)
}

window_update :: proc(using window: ^Window)
{
	SDL.RenderPresent(renderer)
}