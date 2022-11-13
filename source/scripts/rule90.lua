local core = require('core')

local rule90 = {}

rule90.grid_width = 64
rule90.grid_height = 36
rule90.grid = core.zero_grid(rule90.grid_width, rule90.grid_height)

rule90.grid[1][1 + rule90.grid_width / 2] = 1
for y = 2, rule90.grid_height do
    if rule90.grid[y - 1][2] == 1 then break end
    for x = 2, rule90.grid_width do
        if core.prev_row3(rule90.grid, x, y, 1, 1, 1) then
			rule90.grid[y][x] = 0
        elseif core.prev_row3(rule90.grid, x, y, 1, 1, 0) then
			rule90.grid[y][x] = 1
        elseif core.prev_row3(rule90.grid, x, y, 1, 0, 1) then
			rule90.grid[y][x] = 0
        elseif core.prev_row3(rule90.grid, x, y, 1, 0, 0) then
			rule90.grid[y][x] = 1
        elseif core.prev_row3(rule90.grid, x, y, 0, 1, 1) then
			rule90.grid[y][x] = 1
        elseif core.prev_row3(rule90.grid, x, y, 0, 1, 0) then
			rule90.grid[y][x] = 0
        elseif core.prev_row3(rule90.grid, x, y, 0, 0, 1) then
			rule90.grid[y][x] = 1
        elseif core.prev_row3(rule90.grid, x, y, 0, 0, 0) then
			rule90.grid[y][x] = 0
        end
    end
end

return rule90
