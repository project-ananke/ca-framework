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

return core
