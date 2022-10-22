exe_path = styxsys.get_exe_path()
exe_dir = exe_path:match("^(.+)[/\\\\].*$")

styxsys.chdir(exe_dir)
package.path = exe_dir .. "/../source/?/init.lua;" .. package.path

local core = require("core")
core.run()
