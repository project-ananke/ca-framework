grid_width = 64
grid_height = 36

print('rule 30')

print(grid_width)
print(grid_height)
--[[
grid = {}

grid[0][GRID_WIDTH / 2] = 1;
for y = 1, GRID_HEIGHT do

end

for (int y = 1; y < GRID_HEIGHT; ++y) {
    if (grid[y - 1][1] == 1) break;
    for (int x = 1; x < GRID_WIDTH - 1; ++x) {
        if (grid[y - 1][x - 1] == 1 &&
            grid[y - 1][x]     == 1 &&
            grid[y - 1][x + 1] == 1) {
            grid[y][x] = 0;
        } else if (grid[y - 1][x - 1] == 1 &&
                   grid[y - 1][x]     == 1 &&
                   grid[y - 1][x + 1] == 0) {
            grid[y][x] = 0;
        } else if (grid[y - 1][x - 1] == 1 &&
                   grid[y - 1][x]     == 0 &&
                   grid[y - 1][x + 1] == 1) {
            grid[y][x] = 0;
        } else if (grid[y - 1][x - 1] == 1 &&
                   grid[y - 1][x]     == 0 &&
                   grid[y - 1][x + 1] == 0) {
            grid[y][x] = 1;
        } else if (grid[y - 1][x - 1] == 0 &&
                   grid[y - 1][x]     == 1 &&
                   grid[y - 1][x + 1] == 1) {
            grid[y][x] = 1;
        } else if (grid[y - 1][x - 1] == 0 &&
                   grid[y - 1][x]     == 1 &&
                   grid[y - 1][x + 1] == 0) {
            grid[y][x] = 1;
        } else if (grid[y - 1][x - 1] == 0 &&
                   grid[y - 1][x]     == 0 &&
                   grid[y - 1][x + 1] == 1) {
            grid[y][x] = 1;
        } else if (grid[y - 1][x - 1] == 0 &&
                   grid[y - 1][x]     == 0 &&
                   grid[y - 1][x + 1] == 0) {
            grid[y][x] = 0;
        }  
    }
}
--]]

