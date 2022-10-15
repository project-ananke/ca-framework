grid_width = 64
grid_height = 36

grid = {}

print('rule 30')

print('grid_width: ' .. grid_width)
print('grid_height: ' .. grid_height)

for y = 1, grid_height do
    grid[y] = {}
    for x = 1, grid_width do
        grid[y][x] = 0
    end
end

function prev_row3(x, y, a, b, c)  
    return grid[y - 1][x - 1] == a and 
           grid[y - 1][x]     == b and 
           grid[y - 1][x + 1] == c
end

grid[1][grid_width / 2] = 1
for y = 2, grid_height do
    if grid[y - 1][2] == 1 then break end
    for x = 2, grid_width - 1 do
        if prev_row3(x, y, 1, 1, 1) then
           grid[y][x] = 0
        elseif prev_row3(x, y, 1, 1, 0) then
            grid[y][x] = 0
        elseif prev_row3(x, y, 1, 0, 1) then
            grid[y][x] = 0
        elseif prev_row3(x, y, 1, 0, 0) then
            grid[y][x] = 1
        elseif prev_row3(x, y, 0, 1, 1) then
            grid[y][x] = 1
        elseif prev_row3(x, y, 0, 1, 0) then
            grid[y][x] = 1
        elseif prev_row3(x, y, 0, 0, 1) then
            grid[y][x] = 1
        elseif prev_row3(x, y, 0, 0, 0) then
            grid[y][x] = 0
        end
    end
end
