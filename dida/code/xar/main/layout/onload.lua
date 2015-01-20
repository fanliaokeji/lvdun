local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")
local gnLastReportRunTmUTC = 0

-----------------

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


function SendDiDaStartReport()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local strCID = FunctionObj.GetPeerID()
	local strMAC = ""
	
	if IsRealString(strCID) then
		local nIndex = 1
		for i=1, 6 do 
			local strTemp = string.sub(strCID, nIndex, nIndex+1)
			strMAC = strMAC..strTemp.."-"
			nIndex = nIndex+2
		end
	end
	local strMACFix = string.gsub(strMAC, "-$", "")
	local strChannelID = FunctionObj.GetInstallSrc()
	
	local strUrl = "http://stat.didarili.com:8083/?mac=" .. tostring(strMACFix) 
					.."&op=start&cid=" .. (strChannelID)
	FunctionObj.TipLog("[SendDiDaStartReport]: " .. tostring(strUrl))
	tipAsynUtil:AsynSendHttpStat(strUrl, function() end)
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
		if hWnd then
			tipUtil:SetForegroundWindow(hWnd)
		else
			objMainWnd:Show(5)
		end
		
	end
	
	SendStartupReport(true)
	InjectDLL()
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


function TryForceUpdate(tServerConfig)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
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
	local tForceUpdate = tNewVersionInfo["tForceUpdate"]
	if(type(tForceUpdate)) ~= "table" then
		return 
	end
	
	local strCurVersion = FunctionObj.GetDiDaVersion()
	local strNewVersion = tForceUpdate.strVersion		
	if not IsRealString(strCurVersion) or not IsRealString(strNewVersion)
		or not FunctionObj.CheckIsNewVersion(strNewVersion, strCurVersion) then
		return
	end
	
	local tVersionLimit = tForceUpdate["tVersion"]
	local bPassCheck = CheckForceVersion(tVersionLimit)
	FunctionObj.TipLog("[TryForceUpdate] CheckForceVersion bPassCheck:"..tostring(bPassCheck))
	if not bPassCheck then
		return 
	end
	
	FunctionObj.SetIsUpdating(true)
	
	FunctionObj.DownLoadNewVersion(tForceUpdate, function(strRealPath) 
		FunctionObj.SetIsUpdating(false)
		if not IsRealString(strRealPath) then
			return
		end
		
		FunctionObj.SaveCommonUpdateUTC()
		local strCmd = " /write /silent /run"
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



function AnalyzeServerConfig(nDownServer, strServerPath)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	if nDownServer ~= 0 or not tipUtil:QueryFileExists(tostring(strServerPath)) then
		FunctionObj.TipLog("[AnalyzeServerConfig] Download server config failed , start tipmain ")
		return	
	end
	
	local tServerConfig = FunctionObj.LoadTableFromFile(strServerPath) or {}
	TryForceUpdate(tServerConfig)
	FixUserConfig(tServerConfig)
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
end


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

function Add_Remove_FontResource(bAdd)
	local strRes = GetResourceDir()
	if strRes == nil then
		return false
	end
	local strFontDir = tipUtil:PathCombine(strRes, "font")
	if not tipUtil:QueryFileExists(strFontDir) then
		return false
	end
	local tabFont = tipUtil:FindFileList(strFontDir,"*.*")
	if type(tabFont) ~= "table" then
		return false
	end
	for i=1,#tabFont do
		local strFontPath = tipUtil:PathCombine(strFontDir, tabFont[i])
		if tipUtil:QueryFileExists(strFontPath) then
			if bAdd then
				tipUtil:AddFontResource(strFontPath)
			else
				tipUtil:RemoveFontResource(strFontPath)
			end
		end
	end
end


function InitFont()
	Add_Remove_FontResource(true)
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
	
	InitFont()
	StartRunCountTimer()
	
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
	FunctionObj.ReadAllConfigInfo()
	
	SendDiDaStartReport()
	SendStartupReport(false)
	TipMain()
	FunctionObj.DownLoadServerConfig(AnalyzeServerConfig)
end

PreTipMain()



