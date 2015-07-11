local tipUtil = XLGetObject("API.Util")
local Helper = XLGetGlobal("Helper")

--ȡ��commandline���������ڡ�
-- ��Ҫ֧�������и�ʽ:
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
	
	--�����ھ��
	if path and path ~= "" and tipUtil:QueryFileExists(path) then
		Helper:CreateModelessWnd("NotePadWnd", "NotePadWndTree", nil, {["filePath"] = path, ["bIndependentNotePad"] = true})
	else
		Helper:CreateModelessWnd("NotePadWnd", "NotePadWndTree", nil, {["bIndependentNotePad"] = true})
	end
end

OnLoadLuaFile()