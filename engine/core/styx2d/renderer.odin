package styx2d

import "engine:common"
import "engine:platform"

import "engine:core/styxm"
import "engine:core/styxlua"

import SDL "vendor:SDL2"
import mu "vendor:microui"

// Figure out how to separate the renderer from the drawing in the platform layer. 
push_rect_fill :: proc(using window: ^platform.Window,
                  xy: styxm.Vec2_Coord, wh: styxm.Vec2_Coord,
                  col: styxm.Vec3c)
{
	SDL.SetRenderDrawColor(renderer, col.x, col.y, col.z, 0xFF)
	SDL.RenderFillRect(renderer, &SDL.Rect{x=xy.x, y=xy.y, w=wh.x, h=wh.y})
}

push_rect_frame :: proc(using window: ^platform.Window,
                 	    xy: styxm.Vec2_Coord, wh: styxm.Vec2_Coord,
                        col: styxm.Vec3c)
{
	SDL.SetRenderDrawColor(renderer, col.x, col.y, col.z, 0xFF)
	SDL.RenderDrawRect(renderer, &SDL.Rect{x=xy.x, y=xy.y, w=wh.x, h=wh.y})
}

push_gridrule :: proc(using window: ^platform.Window,
                      rule: common.Grid, col: styxm.Vec3c)
{
	wh := styxm.Vec2_Coord{
		i32(window.width / rule.grid_width), 
		i32(window.height / rule.grid_height),
	}

	for y in 0..<rule.grid_height {
		for x in 0..<rule.grid_width {
			if rule.grid[(y * rule.grid_width) + x] == 1 {
				push_rect_fill(window, styxm.Vec2_Coord{i32(x) * wh.x, i32(y) * wh.y}, wh, col)
			} else {
				push_rect_frame(window, styxm.Vec2_Coord{i32(x) * wh.x, i32(y) * wh.y}, wh, col)
			}
		}
	}
}

mu_render :: proc(using window: ^platform.Window) {
	render_texture :: proc(using window: ^platform.Window, dst: ^SDL.Rect, src: mu.Rect, color: mu.Color) {
		dst.w = src.w
		dst.h = src.h

		SDL.SetTextureAlphaMod(state.atlas_texture, color.a)
		SDL.SetTextureColorMod(state.atlas_texture, color.r, color.g, color.b)
		SDL.RenderCopy(renderer, state.atlas_texture, &SDL.Rect{src.x, src.y, src.w, src.h}, dst)
	}
	
	viewport_rect := &SDL.Rect{}
	SDL.GetRendererOutputSize(renderer, &viewport_rect.w, &viewport_rect.h)
	SDL.RenderSetViewport(renderer, viewport_rect)
	SDL.RenderSetClipRect(renderer, viewport_rect)

	command_backing: ^mu.Command
	for variant in mu.next_command_iterator(&mu_ctx, &command_backing) {
		switch cmd in variant {
		case ^mu.Command_Text:
			dst := SDL.Rect{cmd.pos.x, cmd.pos.y, 0, 0}
			for ch in cmd.str do if ch&0xc0 != 0x80 {
				r := min(int(ch), 127)
				src := mu.default_atlas[mu.DEFAULT_ATLAS_FONT + r]
				render_texture(window, &dst, src, cmd.color)
				dst.x += dst.w
			}
		case ^mu.Command_Rect:
			push_rect_fill(window, 
			               styxm.Vec2_Coord{cmd.rect.x, cmd.rect.y}, 
			               styxm.Vec2_Coord{cmd.rect.w, cmd.rect.h},
			               styxm.Vec3c{cmd.color.r, cmd.color.g, cmd.color.b})
		case ^mu.Command_Icon:
			src := mu.default_atlas[cmd.id]
			x := cmd.rect.x + (cmd.rect.w - src.w)/2
			y := cmd.rect.y + (cmd.rect.h - src.h)/2
			render_texture(window, &SDL.Rect{x, y, 0, 0}, src, cmd.color)
		case ^mu.Command_Clip:
			SDL.RenderSetClipRect(renderer, &SDL.Rect{cmd.rect.x, cmd.rect.y, cmd.rect.w, cmd.rect.h})
		case ^mu.Command_Jump: 
			unreachable()
		}
	}
}

