local core = require('core')

local rule30 = {}

rule30.grid_width = 64
rule30.grid_height = 36

function rule30.init(grid, grid_width, grid_height) 
    grid[1][1 + grid_width / 2] = 1
end

-- Runs in a loop. 
function rule30.run(grid, grid_width, grid_height)

end

for y = 2, orig_height do
    if grid[y - 1][2] == 1 then break end
    for x = 2, orig_width do
        if core.prev_row3(grid, x, y, 1, 1, 1) then
            grid[y][x] = 0
        elseif core.prev_row3(grid, x, y, 1, 1, 0) then
            grid[y][x] = 0
        elseif core.prev_row3(grid, x, y, 1, 0, 1) then
            grid[y][x] = 0
        elseif core.prev_row3(grid, x, y, 1, 0, 0) then
            grid[y][x] = 1
        elseif core.prev_row3(grid, x, y, 0, 1, 1) then
            grid[y][x] = 1
        elseif core.prev_row3(grid, x, y, 0, 1, 0) then
            grid[y][x] = 1
        elseif core.prev_row3(grid, x, y, 0, 0, 1) then
            grid[y][x] = 1
        elseif core.prev_row3(grid, x, y, 0, 0, 0) then
            grid[y][x] = 0
        end
    end
end

rule30.grid = core.crop_center_grid(grid, orig_width, orig_height, rule30.grid_width)

return rule30