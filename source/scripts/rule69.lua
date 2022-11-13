local core = require('core')

local rule69 = {}

rule69.grid_width = 64
rule69.grid_height = 36
rule69.grid = core.zero_grid(rule69.grid_width, rule69.grid_height)

rule69.grid[1][1 + rule69.grid_width / 2] = 1
for y = 2, rule69.grid_height do
    for x = 2, rule69.grid_width - 1 do
        if core.prev_row3(rule69.grid, x, y, 1, 1, 1) then
			rule69.grid[y][x] = 0
        elseif core.prev_row3(rule69.grid, x, y, 1, 1, 0) then
			rule69.grid[y][x] = 1
        elseif core.prev_row3(rule69.grid, x, y, 1, 0, 1) then
			rule69.grid[y][x] = 0
        elseif core.prev_row3(rule69.grid, x, y, 1, 0, 0) then
			rule69.grid[y][x] = 0
        elseif core.prev_row3(rule69.grid, x, y, 0, 1, 1) then
			rule69.grid[y][x] = 0
        elseif core.prev_row3(rule69.grid, x, y, 0, 1, 0) then
			rule69.grid[y][x] = 1
        elseif core.prev_row3(rule69.grid, x, y, 0, 0, 1) then
			rule69.grid[y][x] = 0
        elseif core.prev_row3(rule69.grid, x, y, 0, 0, 0) then
			rule69.grid[y][x] = 1
        end
    end
end

return rule69
