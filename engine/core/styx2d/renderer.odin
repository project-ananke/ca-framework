package styx2d

import "engine:common"
import "engine:platform"

import "engine:core/styxm"
import "engine:core/styxlua"

import SDL "vendor:sdl2"
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

