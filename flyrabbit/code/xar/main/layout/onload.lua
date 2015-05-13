local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")
local gnLastReportRunTmUTC = 0
local g_tipNotifyIcon = nil
-----------------

function RegisterFunctionObject()
	local strFunhelpPath = __document.."\\..\\functionhelper.lua"
	XLLoadModule(strFunhelpPath)
	
	local tFunH = XLGetGlobal("Project.FunctionHelper")
	if type(tFunH) ~= "table" then
		return false
	else
		return true
	end
end


function RegisterRabbitFileObject()
	local strFunhelpPath = __document.."\\..\\rabbitfilehelper.lua"
	XLLoadModule(strFunhelpPath)
	local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")
	if type(tRabbitFileList) ~= "table" then
		return false
	end
	
	if not tRabbitFileList:Init() then
		return false
	end
	if not tRabbitFileList:LoadListFromFile() then
		-- return false
	end
	return true
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


function SendStartupReportGgl(bShowWnd)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
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


function SendStartReportLocal()
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
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
	
	local strUrl = ""
	FunctionObj.TipLog("[SendStartReportLocal]: " .. tostring(strUrl))
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
		objMainWnd:Show(5)
	end
	
	SendStartupReportGgl(true)
end


function CheckForceVersion(tForceVersion)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	if type(tForceVersion) ~= "table" then
		return false
	end

	local bRightVer = false
	
	local strCurVersion = FunctionObj.GetProjectVersion()
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
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
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
	
	local strCurVersion = FunctionObj.GetProjectVersion()
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
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	local tUserConfigInServer = tServerConfig["tUserConfigInServer"]
	if type(tUserConfigInServer) ~= "table" then
		return
	end
	
	-- FunctionObj.SaveConfigToFileByKey("tUserConfig")
end


function AnalyzeServerConfig(nDownServer, strServerPath)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	if nDownServer ~= 0 or not tipUtil:QueryFileExists(tostring(strServerPath)) then
		FunctionObj.TipLog("[AnalyzeServerConfig] Download server config failed , start tipmain ")
		return	
	end
	
	local tServerConfig = FunctionObj.LoadTableFromFile(strServerPath) or {}
	TryForceUpdate(tServerConfig)
	FixUserConfig(tServerConfig)
end


function SendRunTimeReport(nTimeSpanInSec, bExit)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper")
	local tStatInfo = {}
	tStatInfo.strEC = "runtime"
	tStatInfo.strEA = FunctionObj.GetInstallSrc() or ""
	
	local nRunTime = 0
	if bExit and gnLastReportRunTmUTC ~= 0 then
		nRunTime = math.abs(tipUtil:GetCurrentUTCTime() - gnLastReportRunTmUTC)
	else
		nRunTime = nTimeSpanInSec
	end
	tStatInfo.strEV = nRunTime
	
	FunctionObj.TipConvStatistic(tStatInfo)
end


function StartRunCountTimer()
	local FunctionObj = XLGetGlobal("Project.FunctionHelper")
	local nTimeSpanInSec = 10 * 60 
	local nTimeSpanInMs = nTimeSpanInSec * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		gnLastReportRunTmUTC = tipUtil:GetCurrentUTCTime()
		SendRunTimeReport(nTimeSpanInSec, false)
		XLSetGlobal("Project.LastReportRunTime", gnLastReportRunTmUTC) 
	end, nTimeSpanInMs)
	
	---飞兔服务器上报
	local nTimeSpanInMs = 2*60*1000
	timerManager:SetTimer(function(item, id)
		FunctionObj.SendLocalReport(10)
	end, nTimeSpanInMs)
end


function PopTipWnd(OnCreateFunc)
	local bSuccess = false
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate("TipMainWnd", "HostWndTemplate" )
	local frameHostWnd = nil
	if frameHostWndTemplate then
		frameHostWnd = frameHostWndTemplate:CreateInstance("MainTipWnd.MainFrame")
		if frameHostWnd then
			local objectTreeTemplate = nil
			objectTreeTemplate = templateMananger:GetTemplate("TipPanelTree", "ObjectTreeTemplate")
			if objectTreeTemplate then
				local uiObjectTree = objectTreeTemplate:CreateInstance("MainTipWnd.MainObjectTree")
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
	
	local FunctionObj = XLGetGlobal("Project.FunctionHelper")
	if not bSuccess then
		FunctionObj:FailExitTipWnd(4)
	end
	
	--初始化托盘
    if frameHostWnd then
	    FunctionObj.InitTrayTipWnd(frameHostWnd)
	end
end


function ProcessCommandLine()
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	local cmdString = tipUtil:GetCommandLine()
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

function ListenBrowserEvent()
	local objFactory = XLGetObject("APIListen.Factory")
	local apilisten
	if objFactory then
		apilisten = objFactory:CreateInstance()
		apilisten:AttachListener(function(event, ...)
			if event == "OnAddTask" then
				local url = select(1, ...)
				local ntype = select(2, ...)  
				local x = select(3, ...)
				local y = select(4, ...)
				local tFunHelper = XLGetGlobal("Project.FunctionHelper")
				local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
				local objDownLoadWnd = hostwndManager:GetHostWnd("TipNewTaskWnd.Instance")
				if objDownLoadWnd then
					local objtree = objDownLoadWnd:GetBindUIObjectTree()
					if objtree then
						local ctrl = objtree:GetUIObject("root.layout")
						if ctrl then
							ctrl:SetData({["url"]=url})
							objDownLoadWnd:BringWindowToTop(true)
						end
					end
				else
					tFunHelper.ShowModalDialog("TipNewTaskWnd",
						"TipNewTaskWnd.Instance", 
						"NewTaskTree", 
						"NewTaskTree.Instance",
						{["url"]=url}
					)
				end
			end
		end)
	end
	tipUtil:CreateMutex("xarmutex_{455EB122-3F18-4139-AE47-255F940CBCF0}")
end

function TipMain() 
	local FunctionObj = XLGetGlobal("Project.FunctionHelper")
	CreateMainTipWnd()
	ListenBrowserEvent()
	FunctionObj.CreatePopupTipWnd()
	ProcessCommandLine()
end


function PreTipMain() 
	gnLastReportRunTmUTC = tipUtil:GetCurrentUTCTime()
	XLSetGlobal("Project.LastReportRunTime", gnLastReportRunTmUTC) 
	
	if not RegisterFunctionObject() then
		tipUtil:Exit("Exit")
	end
	
	local FunctionObj = XLGetGlobal("Project.FunctionHelper")
	FunctionObj.ReadAllConfigInfo()
	if not RegisterRabbitFileObject() then
		tipUtil:Exit("Exit")
	end
	StartRunCountTimer()	
	FunctionObj.SendLocalReport(2)
	SendStartupReportGgl(false)
	TipMain()
	
	FunctionObj.DownLoadServerConfig(AnalyzeServerConfig)
end

PreTipMain()



