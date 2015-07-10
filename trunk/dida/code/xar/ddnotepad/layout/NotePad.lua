local tipUtil = XLGetObject("API.Util")
local Helper = XLGetGlobal("Helper")

--取出commandline，创建窗口。
-- 需要支持命令行格式:
	-- %installdir%\ddnotepad.exe c:\test.txt 
	-- %installdir%\ddnotepad.exe /path c:\test.txt 
	-- %installdir%\ddnotepad.exe /path c:\test.txt /sstartfrom desktop
function OnLoadLuaFile()
	local path = Helper:GetCommandStrValue("/path")
	local sstartfrom = Helper:GetCommandStrValue("/sstartfrom ")
	if nil == path or "" == path then
		local command = tipUtil:GetCommandLine()
		path = string.match(command, ".*%s+(%a:\\.*)%s*")
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