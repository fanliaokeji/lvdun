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
"luacode\\ServerConfig.lua",
"luacode\\helper_tree.lua",
"luacode\\helper_selector.lua",
"luacode\\helper_messagebox.lua",
"luacode\\helper_setting.lua",
"luacode\\helper_tray.lua",
"luacode\\helper_stat.lua",
"luacode\\helper_listener.lua",
"luacode\\helper_APIproxy.lua",
"luacode\\CheckAssociate.lua",
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
	local filepath = string.match(cmdString, "\"([^\"]+)\"[^\"]*$")
	if not filepath or not tipUtil:QueryFileExists(filepath) then
		filepath = cmdString
	end
	if filepath and tipUtil:QueryFileExists(filepath) then
		HostWnd = Helper:CreateModelessWnd("Kuaikan.MainWnd","Kuaikan.MainObjTree")
		local imgctrl = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
		if imgctrl then
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
	--上报
	StatUtil.SendStartupStat()
end

OnLoadLuaFile()

function OnConfigLoaded(_, event, tConfig)
	-- XLMessageBox("event: "..tostring(event))
	-- ServerConfig:DownloadExtraCode()
	-- ServerConfig:TryManualUpdate()
	-- ServerConfig.IsUpdating = nil
	-- ServerConfig:TryForceUpdate()
end

function OnExtraCodeReady(_, event, savePath)
	-- XLMessageBox("OnExtraCodeReady")
	--执行lua代码
end

function OnManualUpdateReady(_, event, savePath)
	-- XLMessageBox("OnManualUpdateReady")
	-- 命令行拉起exe
end
function OnForceUpdateReady(_, event, savePath)
	-- XLMessageBox("OnForceUpdateReady")
	-- 命令行拉起exe
end

ServerConfig:AddListener("OnConfigLoaded", OnConfigLoaded)
ServerConfig:AddListener("OnExtraCodeReady", OnExtraCodeReady)
ServerConfig:AddListener("OnManualUpdateReady", OnManualUpdateReady)
ServerConfig:AddListener("OnForceUpdateReady", OnForceUpdateReady)