local core = {}

function core.run()
	while styxsys.window_running() do
		styxsys.process_messages()

		styxsys.window_clear(255, 255, 255)
		styxsys.window_update()
	end
end

return core
