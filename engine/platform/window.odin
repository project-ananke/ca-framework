package platform

import "core:strings"
import "core:fmt"

import SDL "vendor:SDL2"
import mu "vendor:microui"

import "engine:core/styxm"

State :: struct
{
	mu_ctx: mu.Context,
	atlas_texture: ^SDL.Texture,
}

Window :: struct
{
	title: string,
	width, height: u32,
	running: bool, 

	handle: ^SDL.Window,
	renderer: ^SDL.Renderer,

	using state: State, 
}

@(private)
init_microui :: proc(using window: ^Window)
{
	state.atlas_texture = SDL.CreateTexture(renderer, u32(SDL.PixelFormatEnum.RGBA32), 
	                                        .TARGET, mu.DEFAULT_ATLAS_WIDTH, mu.DEFAULT_ATLAS_HEIGHT)
	assert(state.atlas_texture != nil)
	if err := SDL.SetTextureBlendMode(state.atlas_texture, .BLEND); err != 0 {
		fmt.eprintln("SDL.SetTextureBlendMode:", err)
		return
	}
	
	pixels := make([][4]u8, mu.DEFAULT_ATLAS_WIDTH*mu.DEFAULT_ATLAS_HEIGHT)
	defer delete(pixels)

	for alpha, i in mu.default_atlas_alpha {
		pixels[i].rgb = 0xff
		pixels[i].a   = alpha
	}
	
	if err := SDL.UpdateTexture(state.atlas_texture, nil, raw_data(pixels), 4*mu.DEFAULT_ATLAS_WIDTH); err != 0 {
		fmt.eprintln("SDL.UpdateTexture:", err)
		return
	}

	ctx := &state.mu_ctx
	mu.init(ctx)
	
	ctx.text_width = mu.default_atlas_text_width
	ctx.text_height = mu.default_atlas_text_height
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
	                                     { .ACCELERATED, .PRESENTVSYNC })

	init_microui(&window)

	window.running = true

	return
}

free_window :: proc(using window: ^Window)
{
	SDL.DestroyTexture(atlas_texture)
	SDL.DestroyRenderer(renderer)
	SDL.DestroyWindow(handle)
}

window_process :: proc(using window: ^Window)
{
	for e: SDL.Event; SDL.PollEvent(&e); {
		#partial switch e.type {
		case .QUIT:
			running = false
		case .MOUSEMOTION:
			mu.input_mouse_move(&state.mu_ctx, e.motion.x, e.motion.y)
		case .MOUSEWHEEL:
			mu.input_scroll(&state.mu_ctx, e.wheel.x * 30, e.wheel.y * -30)
		case .TEXTINPUT:
			mu.input_text(&state.mu_ctx, string(cstring(&e.text.text[0])))
			
		case .MOUSEBUTTONDOWN, .MOUSEBUTTONUP:
			fn := mu.input_mouse_down if e.type == .MOUSEBUTTONDOWN else mu.input_mouse_up
			switch e.button.button {
			case SDL.BUTTON_LEFT:   fn(&state.mu_ctx, e.button.x, e.button.y, .LEFT)
			case SDL.BUTTON_MIDDLE: fn(&state.mu_ctx, e.button.x, e.button.y, .MIDDLE)
			case SDL.BUTTON_RIGHT:  fn(&state.mu_ctx, e.button.x, e.button.y, .RIGHT)
			}
			
		case .KEYDOWN, .KEYUP:
			if e.type == .KEYUP && e.key.keysym.sym == .ESCAPE {
				SDL.PushEvent(&SDL.Event{type = .QUIT})
			}
			
			fn := mu.input_key_down if e.type == .KEYDOWN else mu.input_key_up

			#partial switch e.key.keysym.sym {
			case .LSHIFT:    fn(&state.mu_ctx, .SHIFT)
			case .RSHIFT:    fn(&state.mu_ctx, .SHIFT)
			case .LCTRL:     fn(&state.mu_ctx, .CTRL)
			case .RCTRL:     fn(&state.mu_ctx, .CTRL)
			case .LALT:      fn(&state.mu_ctx, .ALT)
			case .RALT:      fn(&state.mu_ctx, .ALT)
			case .RETURN:    fn(&state.mu_ctx, .RETURN)
			case .KP_ENTER:  fn(&state.mu_ctx, .RETURN)
			case .BACKSPACE: fn(&state.mu_ctx, .BACKSPACE)
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