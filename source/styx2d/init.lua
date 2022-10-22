local styx2d = {}

--[[
#if 0
void styx_render_grid(styx_window *window, int *grid, u32 width, u32 height)
{
	u32 grid_width = window->width / width;
	u32 grid_height = window->height / height;
	
	SDL_SetRenderDrawColor(window->renderer, 0x00, 0x00, 0x00, 0xFF);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			b32 fill = (*((grid + y * width) + x + 1) == 1);

			SDL_Rect fill_rect = { x * grid_width, y * grid_height, grid_width, grid_height };
			if (fill) {
				SDL_RenderFillRect(window->renderer, &fill_rect);
			} else {
				SDL_RenderDrawRect(window->renderer, &fill_rect);
			}
		}
	}
}
#endif
--]]
function styx2d.render_grid(grid, width, height, grid_side)
	for y = 1, height do 
		for x = 1, width do 
			fill = (grid[y][x] == 1)
			styxren.render_rect({x = (x - 1) * grid_side, y = (y - 1) * grid_side},
								{w = grid_side, h = grid_side},
								{r = 0, g = 0, b = 0}, fill)
		end
	end
end

return styx2d
