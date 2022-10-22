local core = require('core')

local rule30 = {}

rule30.grid_width = 64
rule30.grid_height = 36
rule30.grid = core.zero_grid(rule30.grid_width, rule30.grid_height)

rule30.grid[1][1 + rule30.grid_width / 2] = 1
for y = 2, rule30.grid_height do
    if rule30.grid[y - 1][2] == 1 then break end
    for x = 2, rule30.grid_width do
        if core.prev_row3(rule30.grid, x, y, 1, 1, 1) then
			rule30.grid[y][x] = 0
        elseif core.prev_row3(rule30.grid, x, y, 1, 1, 0) then
			rule30.grid[y][x] = 1
        elseif core.prev_row3(rule30.grid, x, y, 1, 0, 1) then
			rule30.grid[y][x] = 0
        elseif core.prev_row3(rule30.grid, x, y, 1, 0, 0) then
			rule30.grid[y][x] = 1
        elseif core.prev_row3(rule30.grid, x, y, 0, 1, 1) then
			rule30.grid[y][x] = 1
        elseif core.prev_row3(rule30.grid, x, y, 0, 1, 0) then
			rule30.grid[y][x] = 0
        elseif core.prev_row3(rule30.grid, x, y, 0, 0, 1) then
			rule30.grid[y][x] = 1
        elseif core.prev_row3(rule30.grid, x, y, 0, 0, 0) then
			rule30.grid[y][x] = 0
        end
    end
end

return rule30
