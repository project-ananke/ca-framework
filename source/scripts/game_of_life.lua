local core = require('core')

local game_of_life = {}

game_of_life.grid_width = 64
game_of_life.grid_height = 36
game_of_life.grid = styx_grid

-- Runs once when the script is loaded.
function game_of_life.init(grid, grid_width, grid_height) 
    game_of_life.grid = core.zero_grid()
    -- Seed the data.
end

-- Runs in a loop. 
function game_of_life.run(grid, grid_width, grid_height)

end

return game_of_life
