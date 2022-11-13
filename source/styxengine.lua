local styxengine = {}

local styx2d = require('styx2d')
local rule69 = require('rule69')

function styxengine.run()
	while styxsys.window_running() do
		styxsys.process_messages()
		styxsys.window_clear(255, 255, 255)

		styx2d.render_grid(rule69.grid, rule69.grid_width, rule69.grid_height, 20)	

		styxsys.window_update()
	end
end

return styxengine
