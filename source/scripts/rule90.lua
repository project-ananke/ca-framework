local core = require('core')

local rule90 = {}

local orig_width = 100
local orig_height = 36
local grid = core.zero_grid(orig_width, orig_height)

grid[1][1 + orig_width / 2] = 1
for y = 2, orig_height do
    if grid[y - 1][2] == 1 then break end
    for x = 2, orig_width do
        if core.prev_row3(grid, x, y, 1, 1, 1) then
            grid[y][x] = 0
        elseif core.prev_row3(grid, x, y, 1, 1, 0) then
            grid[y][x] = 1
        elseif core.prev_row3(grid, x, y, 1, 0, 1) then
            grid[y][x] = 0
        elseif core.prev_row3(grid, x, y, 1, 0, 0) then
            grid[y][x] = 1
        elseif core.prev_row3(grid, x, y, 0, 1, 1) then
            grid[y][x] = 1
        elseif core.prev_row3(grid, x, y, 0, 1, 0) then
            grid[y][x] = 0
        elseif core.prev_row3(grid, x, y, 0, 0, 1) then
            grid[y][x] = 1
        elseif core.prev_row3(grid, x, y, 0, 0, 0) then
            grid[y][x] = 0
        end
    end
end

rule90.grid_width = 64
rule90.grid_height = orig_height

rule90.grid = core.crop_center_grid(grid, orig_width, orig_height, rule90.grid_width)

return rule90
