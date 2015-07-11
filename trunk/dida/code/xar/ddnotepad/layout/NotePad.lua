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
	
	--父窗口句柄
	if path and path ~= "" and tipUtil:QueryFileExists(path) then
		Helper:CreateModelessWnd("NotePadWnd", "NotePadWndTree", nil, {["filePath"] = path, ["bIndependentNotePad"] = true})
	else
		Helper:CreateModelessWnd("NotePadWnd", "NotePadWndTree", nil, {["bIndependentNotePad"] = true})
	end
end

OnLoadLuaFile()