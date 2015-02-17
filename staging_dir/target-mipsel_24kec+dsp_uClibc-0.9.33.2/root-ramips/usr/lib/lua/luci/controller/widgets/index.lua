module("luci.controller.widgets.index", package.seeall)
local fs = require "luci.fs"
function index()
	local c = entry({"widgets"}, call("action"), "widgets", 1)
	c.leaf = true
	c.dependent = false
	--c.sysauth = "root"
end
function action()
  local JSON = (loadfile "/usr/lib/lua/luci/JSON.lua")()
  JSON.strictTypes = true
  thispath = {"widget_registry"}
  local req = JSON:encode({ver="0001",request=JSON:encode({path=thispath,parameter={},method="GET"})})
  local rpc = io.popen("rsserial -j "..req:gsub("\\","\\\\"):gsub("\"","\\\""))
  local rsp = rpc:read("*a")
  rpc:close()
  local entries = JSON:decode(rsp)["result"]
  local codes ={}
  for i=1,#entries do
    local first = entries[i]
    local widgetRoot  = first["root"]
    local widgetIndex = first["index"]
    local widgetDir ="widget"..i
    local code_ = fs.readfile(widgetRoot.."/"..widgetIndex)
    local publicRoot = "/www/kappa/"..widgetDir
    os.execute("[ ! -h "..publicRoot.." ] && ( ln -s "..widgetRoot.." "..publicRoot.." )")
    codes[i] = code_:gsub("<@=%s*WIDGETROOT%s*@>","/kappa/"..widgetDir)
  end
  rtn = JSON:encode({status=0, data=codes})
  luci.http.prepare_content("application/json")
  luci.http.write(rtn:gsub("\n", ""))
end
