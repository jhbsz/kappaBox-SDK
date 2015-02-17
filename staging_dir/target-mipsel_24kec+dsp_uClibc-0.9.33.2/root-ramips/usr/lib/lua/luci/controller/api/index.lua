module("luci.controller.api.index", package.seeall)

function index()
	local c = entry({"api"}, call("action"), "api", 1)
	c.leaf = true
	c.dependent = false
	--c.sysauth = "root"
end
 
function action()
	local JSON = (loadfile "/usr/lib/lua/luci/JSON.lua")()
	JSON.strictTypes = true
	local mode = luci.dispatcher.context.requestpath
	local req = JSON:encode({ver="0001",request=JSON:encode({path=luci.dispatcher.context.args,parameter=luci.http.formvalue(),method=luci.http.getenv("REQUEST_METHOD")})})
	local rpc = io.popen("rsserial -j "..req:gsub("\\","\\\\"):gsub("\"","\\\""))
	local rsp = rpc:read("*a")
	rpc:close()
	luci.http.prepare_content("application/json")
	luci.http.write(rsp)
end
