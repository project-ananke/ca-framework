local styxengine = {}

local styx2d = require('styx2d')
local rule90 = require('rule90')

function styxengine.run()
	while styxsys.window_running() do
		styxsys.process_messages()
		styxsys.window_clear(255, 255, 255)

		styx2d.render_grid(rule90.grid, rule90.grid_width, rule90.grid_height, 20)	

		styxsys.window_update()
	end
end

return styxengine
