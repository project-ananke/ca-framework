local core = {}

function core.zero_grid(width, height)
	local grid = {}
	for y = 1, height do 
		grid[y] = {}
		for x = 1, width do
			grid[y][x] = 0
		end
	end

	return grid
end

function core.prev_row3(grid, x, y, a, b, c)  
    return grid[y - 1][x - 1] == a and 
           grid[y - 1][x]     == b and 
           grid[y - 1][x + 1] == c
end

function core.crop_center_grid(uncropped, width, height, to_width)
	local grid = core.zero_grid(to_width, height)

	local x_start = (width - to_width) / 2
	local x_end = width - x_start

	for y = 1, height do
		for x = x_start, x_end do
			grid[y][x - x_start + 1] = uncropped[y][x]
		end
	end

	return grid
end

return core
