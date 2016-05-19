
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
"luacode\\helper_tree.lua",
"luacode\\helper_selector.lua",
"luacode\\helper_messagebox.lua",
"luacode\\helper_tray.lua",
"menu\\ImageRClickMenu.lua",
}
LoadLuaModule(File, __document)


local Helper = XLGetGlobal("Helper")

function OnLoadLuaFile()
	--一般是带任务拉起
	Helper:LOG("OnLoadLuaFileOnLoadLuaFileOnLoadLuaFile")
	Helper:CreateModelessWnd("MainWnd","MainWndTree")
	-- Helper:CreateModelessWnd("ImageWnd","ImageWndTree")
end

OnLoadLuaFile()
