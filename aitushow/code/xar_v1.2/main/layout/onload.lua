local tipUtil = XLGetObject("API.Util")

function LoadLuaModule(tFile, curDocPath)
--tFile可以传lua文件绝对路径、相对路径
	if "table" == type(tFile) then
		for index, value in ipairs(tFile) do
			if "string" == type(value) and value ~= "" then
				local dstPath = curDocPath.."\\..\\"..value
				if XLModuleExists(dstPath) then
					XLUnloadModule(dstPath)
					XLLoadModule(dstPath)
				else
					XLLoadModule(dstPath)
				end
				
			end
		end
	elseif "string" == type(tFile) and tFile ~= ""then
		if curDocPath then
			tFile = curDocPath.."\\..\\"..tFile
		end
		if XLModuleExists(tFile) then
			XLUnloadModule(tFile)
			XLLoadModule(tFile)
		else
			XLLoadModule(tFile)
		end
	end
end

local File = {
"luacode\\objectbase.lua",
"luacode\\helper.lua",
"luacode\\helper_token.lua",
"luacode\\helper_data.lua",
"luacode\\UserConfig.lua",
"luacode\\helper_tree.lua",
"luacode\\helper_selector.lua",
"luacode\\helper_messagebox.lua",
"luacode\\helper_setting.lua",
"luacode\\helper_tray.lua",
"luacode\\helper_listener.lua",
"luacode\\helper_APIproxy.lua",
"luacode\\ImagePool.lua",
"menu\\ImageRClickMenu.lua",
"menu\\SortMenu.lua",
}
LoadLuaModule(File, __document)


local Helper = XLGetGlobal("Helper")

function OnLoadLuaFile()
	--一般是带任务拉起
	Helper:LOG("OnLoadLuaFileOnLoadLuaFileOnLoadLuaFile")
	local cmdString = tostring(tipUtil:GetCommandLine())
	LOG("OnLoadLuaFile cmdString: ", cmdString)
	local HostWnd
	--打开本地文件
	if string.find(string.lower(cmdString), "/sstartfrom%s+localfile") then
		HostWnd = Helper:CreateModelessWnd("Kuaikan.MainWnd","Kuaikan.MainObjTree")
		local filepath = string.match(cmdString, "\"([^\"]+)\"[^\"]*$")
		local imgctrl = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
		if filepath and tipUtil:QueryFileExists(filepath) and imgctrl then
			imgctrl:LoadImageFile(filepath, nil, nil, function() imgctrl:UpdateFileList() end)
		end
	--打开主界面
	else
		HostWnd = Helper:CreateModelessWnd("MainWnd","MainWndTree")
	end
	if HostWnd and string.find(string.lower(tostring(cmdString)), "embedding") then
		HostWnd:Show(0)
	else
		HostWnd:Show(5)
	end
	--初始化托盘
	if HostWnd then
		Helper.Tray.Init(HostWnd)
	end
end

OnLoadLuaFile()
