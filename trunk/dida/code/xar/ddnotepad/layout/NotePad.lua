local tipUtil = XLGetObject("API.Util")
local Helper = XLGetGlobal("Helper")

--取出commandline，创建窗口。
-- 需要支持命令行格式:
	-- %installdir%\ddnotepad.exe c:\test.txt 
	-- %installdir%\ddnotepad.exe /path c:\test.txt 
	-- %installdir%\ddnotepad.exe /path c:\test.txt /sstartfrom desktop
function OnLoadLuaFile()
	local ret, path = Helper:GetCommandStrValue("/path")
	local startFromRet, sstartfrom = Helper:GetCommandStrValue("/sstartfrom ")
	if not ret then
		local command = tipUtil:GetCommandLine()
		path = string.match(command, ".*(%a:\\.*)%s*")
		-- XLMessageBox(path)
		Helper:LOG("command file path: ", path)
	end
	
	local modelessWnd = nil
	--父窗口句柄
	if path and path ~= "" and tipUtil:QueryFileExists(path) then
		modelessWnd = Helper:CreateModelessWnd("NotePadWnd", "NotePadWndTree", nil, {["filePath"] = path, ["bIndependentNotePad"] = true})
	else
		modelessWnd = Helper:CreateModelessWnd("NotePadWnd", "NotePadWndTree", nil, {["bIndependentNotePad"] = true})
	end
	if startFromRet and "association" == sstartfrom then
		--拉起滴答到托盘
		local ddPath =  Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\Path")
		local para = "/sstartfrom ddnotepad /embedding"
		if "string" == type(ddPath) and tipUtil:QueryFileExists(ddPath) then
			local ret = tipUtil:ShellExecute(modelessWnd:GetWndHandle(), "open", ddPath, para, "", "")--0: SW_HIDE
		end
	end
end

OnLoadLuaFile()