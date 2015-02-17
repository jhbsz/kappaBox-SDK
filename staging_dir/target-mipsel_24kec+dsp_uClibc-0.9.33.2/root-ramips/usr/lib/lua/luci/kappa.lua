local io = require "io"
local fs = require "luci.fs"
function hello(request)
	fs.writefile("/tmp/howl", request)
end
	if true then 
		local JSON = (loadfile "/usr/lib/lua/luci/JSON.lua")()
		JSON.strictTypes = true
		local mode = luci.dispatcher.context.requestpath
		fs.writefile("/tmp/inskp", JSON:encode(mode)..JSON:encode(luci.dispatcher.context.args))
	end 

