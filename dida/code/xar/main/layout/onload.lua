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




function ShowMainTipWnd(objMainWnd)
	objMainWnd:Show(5)
	SendStartupReport(true)
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
		TipMain()
		return	
	end
	
	local tServerConfig = FunctionObj.LoadTableFromFile(strServerPath) or {}
	TryForceUpdate(tServerConfig)
	FixUserConfig(tServerConfig)
	
	TipMain()
end


function StartRunCountTimer()
	local nTimeSpanInSec = 10 * 60 
	local nTimeSpanInMs = nTimeSpanInSec * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		gnLastReportRunTmUTC = tipUtil:GetCurrentUTCTime()
		SendRunTimeReport(nTimeSpanInSec, false)
		XLSetGlobal("DiDa.LastReportRunTime", gnLastReportRunTmUTC) 
	end, nTimeSpanInMs)
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
	local bRet, strURL = FunctionObj.GetCommandStrValue("/openlink")

	--to do
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
	CreateMainTipWnd()
	ProcessCommandLine()
end


function PreTipMain() 
	gnLastReportRunTmUTC = tipUtil:GetCurrentUTCTime()
	XLSetGlobal("DiDa.LastReportRunTime", gnLastReportRunTmUTC) 
	
	if not RegisterFunctionObject() then
		tipUtil:Exit("Exit")
	end
	StartRunCountTimer()
	
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
	FunctionObj.ReadAllConfigInfo()
	
	SendStartupReport(false)
	
	FunctionObj.DownLoadServerConfig(AnalyzeServerConfig)
end

PreTipMain()



