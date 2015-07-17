local tipUtil = XLGetObject("API.Util")
local Helper = XLGetGlobal("Helper")

--ȡ��commandline���������ڡ�
-- ��Ҫ֧�������и�ʽ:
	-- %installdir%\ddnotepad.exe c:\test.txt 
	-- %installdir%\ddnotepad.exe /path c:\test.txt 
	-- %installdir%\ddnotepad.exe /path c:\test.txt /sstartfrom desktop
-- ;��txt����
function SetTxtAssociation()
	local didaDir = Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\InstDir")
	local ddnotepadDir = didaDir.."\\program\\ddnotepad.exe"
	if not tipUtil:QueryFileExists(ddnotepadDir) then
		Helper:Assert(false, "ddnotepadDir is not exisit")
		return
	end
	local sysVersion =  Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\CurrentVersion")
	local regPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.txt"
	if tonumber(sysVersion) > 6.0 then
		regPath = regPath.."\\UserChoice"
	end
	local regProgidPath = regPath.."\\Progid"
	local curProgid = Helper:QueryRegValue(regProgidPath)
	Helper:LOG("SetTxtAssociation curProgid: ", curProgid)
	if tonumber(sysVersion) > 6.0 then
		--6.0���ϰ汾ɾ��ע���Progid��
		Helper:DeleteRegKey(regProgidPath)
		Helper:LOG("SetTxtAssociation DeleteRegKey: ", regProgidPath)
	end
	
	local defaultIcon = ""
	if curProgid and "" ~= curProgid and "ddtxtfile" ~= curProgid then
		--���浱ǰProgid
		Helper:SetRegValue(regPath.."\\ddnotepad_backup", curProgid)
		defaultIcon = Helper:QueryRegValueEx("HKEY_CLASSES_ROOT\\"..curProgid.."\\DefaultIcon" ,"")
		Helper:LOG("SetTxtAssociation defaultIcon: ", defaultIcon)
	end
	if not defaultIcon or "" == defaultIcon then
		defaultIcon = Helper:QueryRegValueEx("HKEY_CLASSES_ROOT\\txtfile\\DefaultIcon", "")
	end
	if not defaultIcon or "" == defaultIcon then
		defaultIcon = "%SystemRoot%\\system32\\NOTEPAD.EXE"
	end
	Helper:LOG("SetTxtAssociation defaultIcon: ", defaultIcon)
	
	Helper:SetRegValueEx("HKEY_CLASSES_ROOT\\ddtxtfile\\DefaultIcon", "", defaultIcon)
	
	local tempStr = "\""..ddnotepadDir.."\"".." \"%1\" ".."/sstartfrom association"
	Helper:SetRegValueEx("HKEY_CLASSES_ROOT\\ddtxtfile\\shell\\open\\command", "", tempStr)
	Helper:SetRegValue(regProgidPath, "ddtxtfile")
	
	--��ӵ��򿪷�ʽ�б�
	Helper:LOG("SetTxtAssociation tempStr: ", tempStr)
	Helper:SetRegValue("HKEY_CLASSES_ROOT\\Applications\\ddnotepad.exe\\shell\\open\\command", tempStr)
	Helper:SetRegValue("HKEY_CLASSES_ROOT\\Applications\\ddnotepad.exe\\shell\\edit\\command", tempStr)
	
end

function ReSetTxtAssociation()
	local didaDir = Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\InstDir")
	local ddnotepadDir = didaDir.."\\program\\ddnotepad.exe"
	if not tipUtil:QueryFileExists(ddnotepadDir) then
		return
	end
	local sysVersion =  Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\CurrentVersion")
	local regPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.txt"
	if tonumber(sysVersion) > 6.0 then
		regPath = regPath.."\\UserChoice"
	end
	local regProgidPath = regPath.."\\Progid"
	local regBackupPath = regPath.."\\ddnotepad_backup"
	local curProgid = Helper:QueryRegValue(regProgidPath)
	local regBackup = Helper:QueryRegValue(regBackupPath)
	
	if "ddtxtfile" == curProgid then
		Helper:DeleteRegKey("HKEY_CLASSES_ROOT\\ddtxtfile")
		Helper:DeleteRegKey("HKEY_CURRENT_USER\\"..regPath)
	end
	
	if "" ~= regBackup then
		--�ָ���ǰ�Ĺ���
		Helper:SetRegValue(regPath.."\\Progid", regBackup)
	end
	Helper:DeleteRegKey(regBackupPath)
	--ɾ���򿪷�ʽ�б�
	Helper:DeleteRegKey("HKEY_CLASSES_ROOT\\Applications\\ddnotepad.exe")
end

function OnLoadLuaFile()
	local ret, path = Helper:GetCommandStrValue("/path")
	local startFromRet, sstartfrom = Helper:GetCommandStrValue("/sstartfrom")
	local assRet, association = Helper:GetCommandStrValue("/association")
	if startFromRet and assRet and "explorer" == sstartfrom then
		if "1" == association then
			SetTxtAssociation()
			return
		-- elseif "0" == association then
			-- ReSetTxtAssociation()
			-- return
		end
	end
	
	Helper:LOG("command file path: ret��", ret, " path: ", path)
	if not ret then
		local command = tipUtil:GetCommandLine()
		path = string.match(command, ".*(%a:\\.*)\"%s*")
		-- XLMessageBox(path)
		if not path then
			path = string.match(command, ".*(\\\\.*)\"%s*")
		end
		
		Helper:LOG("command file path: ", path)
	end
	
	local modelessWnd = nil
	--�����ھ���ÿ�
	if path and path ~= "" and tipUtil:QueryFileExists(path) then
		modelessWnd = Helper:CreateModelessWnd("NotePadWnd", "NotePadWndTree", nil, {["filePath"] = path, ["bIndependentNotePad"] = true})
	else
		modelessWnd = Helper:CreateModelessWnd("NotePadWnd", "NotePadWndTree", nil, {["bIndependentNotePad"] = true})
	end
	if startFromRet and "association" == sstartfrom then
		--����δ�����
		local ddPath =  Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\Path")
		local para = "/sstartfrom ddnotepad /embedding"
		if "string" == type(ddPath) and tipUtil:QueryFileExists(ddPath) then
			local ret = tipUtil:ShellExecute(modelessWnd:GetWndHandle(), "open", ddPath, para, "", "")--0: SW_HIDE
		end
	end
end

OnLoadLuaFile()