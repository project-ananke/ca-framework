local core = require('core')

local rule30 = {}

rule30.grid_width = 64
rule30.grid_height = 36

function rule30.init(styx_grid, grid_width, grid_height)
    local grid = core.zero_grid(grid_width, grid_height)

    grid[1][1 + grid_width / 2] = 1
    return grid
end

-- Runs in a loop. 
function rule30.run(styx_grid, grid_width, grid_height)
    local grid = styx_grid

    for y = 2, grid_height - 1 do
        if grid[y - 1][2] == 1 then break end
        for x = 2, grid_width - 1 do
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

    return grid
end

return rule30
