package styx2d

import "engine:common"
import "engine:core/styxm"
import "engine:core/styx2d"

MAX_QUAD_COUNT :: 10000
MAX_VERTEX_COUNT :: MAX_QUAD_COUNT * 4
MAX_INDEX_COUNT :: MAX_QUAD_COUNT * 6

// NOTE(sir->w7): This is the classic sir->w7 implementation of the FIFO stack-based
// rendering pipeline, and it surely isn't the most effective solution to this. Maybe
// look for better ways to implement this system.
Vertex :: struct
{
	pos: styxm.Vec3f,
	col: styxm.Vec4f,
}

// TODO(sir->w7): Work on a resizable command buffer.
Renderer :: struct
{
	data: [MAX_VERTEX_COUNT]Vertex,

	w, h: i32,
	write_at: u32,
}

// What will I use this function for?
init_renderer :: proc(width, height: i32) -> (renderer: Renderer)
{
	renderer.w = width
	renderer.h = height

	return
}

free_renderer :: proc(using renderer: ^Renderer)
{
	return
}

renderer_clear :: proc(using renderer: ^Renderer)
{
	write_at = 0
}

push_vertex :: proc(using renderer: ^Renderer,
                    pos: styxm.Vec3f, col: styxm.Vec4c)
{
	assert(write_at < MAX_VERTEX_COUNT)
	data[write_at] = Vertex{ pos, styxm.v4c_to_v4f(col) }

	write_at += 1
}

push_quad :: proc(using renderer: ^Renderer, 
                  coord: styxm.Vec3f, dim: styxm.Vec2, col: styxm.Vec4c)
{

	push_vertex(renderer, styxm.Vec3f{coord.x, coord.y, coord.z}, col)
	push_vertex(renderer, 
	            styxm.Vec3f{coord.x + f32(dim.x), coord.y, coord.z}, col)
	push_vertex(renderer, 
	            styxm.Vec3f{coord.x + f32(dim.x), coord.y + f32(dim.y), coord.z}, col)
	push_vertex(renderer, 
	            styxm.Vec3f{coord.x, coord.y + f32(dim.y), coord.z}, col)
}

push_clear :: #force_inline proc(using renderer: ^Renderer, col: styxm.Vec4c)
{
	push_quad(renderer, styxm.Vec3f{0, 0, 500}, styxm.Vec2{w, h}, col)
}

push_grid :: proc(using renderer: ^Renderer, grid: ^common.Grid)
{
	square_width := w / i32(grid.grid_width)
	square_height := h / i32(grid.grid_height)

	for y: u32 = 0; y < grid.grid_height; y += 1 {
        for x: u32 = 0; x < grid.grid_width; x += 1 {
        	if grid.grid[y * grid.grid_width + x] == 1 {
        		styx2d.push_quad(renderer,
        		                 styxm.Vec3f{f32(square_width*i32(x)), f32(square_height*i32(y)), 500},
        		                 styxm.Vec2{i32(square_width), i32(square_height)},
        		                 styxm.Vec4c{0, 0, 0, 255})
        	}
        }
    }

}