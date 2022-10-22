styxsys.chdir("bin")
dir_entries = styxsys.list_dir(".")
for i = 1, #dir_entries do
    print(dir_entries[i])
end

local function prev_row3(x, y, a, b, c)  
    return grid[y - 1][x - 1] == a and 
           grid[y - 1][x]     == b and 
           grid[y - 1][x + 1] == c
end

local function write_grid(x, y, a)
    grid[y][x] = a
end

grid[1][1 + grid_width / 2] = 1
for y = 2, grid_height do
    if grid[y - 1][2] == 1 then break end
    for x = 2, grid_width do
        if prev_row3(x, y, 1, 1, 1) then
            write_grid(x, y, 0)
        elseif prev_row3(x, y, 1, 1, 0) then
            write_grid(x, y, 1)
        elseif prev_row3(x, y, 1, 0, 1) then
            write_grid(x, y, 0)
        elseif prev_row3(x, y, 1, 0, 0) then
            write_grid(x, y, 1)
        elseif prev_row3(x, y, 0, 1, 1) then
            write_grid(x, y, 1)
        elseif prev_row3(x, y, 0, 1, 0) then
            write_grid(x, y, 0)
        elseif prev_row3(x, y, 0, 0, 1) then
            write_grid(x, y, 1)
        elseif prev_row3(x, y, 0, 0, 0) then
            write_grid(x, y, 0)
        end
    end
end
