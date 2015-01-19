local tipUtil = XLGetObject("GS.Util")
local tipAsynUtil = XLGetObject("GS.AsynUtil")
local gnLastReportRunTmUTC = 0

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
	
	local strUrl = "http://stat.aizhuomian.com:8084/?mac=" .. tostring(strMACFix) 
					.."&op=start&cid=" .. (strChannelID)
	
	FunctionObj.TipLog("[SendStartReportLocal]: " .. tostring(strUrl))
	tipAsynUtil:AsynSendHttpStat(strUrl, function() end)
end


function ShowMainTipWnd(objMainWnd)
	local bHideMainPage = true
	local cmdString = tipUtil:GetCommandLine()
	
	local bRet = string.find(tostring(cmdString), "/forceshow")
	if bRet then
		bHideMainPage = false
	else
		local bRet = string.find(tostring(cmdString), "/embedding")
		if bRet then
			bHideMainPage = true
		end
	end
	
	if bHideMainPage then
		objMainWnd:Show(0)
	else
		objMainWnd:Show(5)
	end
	
	objMainWnd:SetTitle("广告清道夫")
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


function SendFileDataToFilterThread()
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	local bSucc = SendRuleListtToFilterThread()
	if not bSucc then
		FunctionObj.MessageBox(tostring("文件被损坏，请重新安装"))
		return false
	end	

	local bSucc = SendWhiteListToFilterThread()
	if not bSucc then
		return false
	end
	
	return true
end


function GenDecFilePath(strEncFilePath)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	local strKey = "adS5m9PE8avRxmbs"
	local strDecString = tipUtil:DecryptFileAES(strEncFilePath, strKey)
	if type(strDecString) ~= "string" then
		FunctionObj.TipLog("[GenDecFilePath] DecryptFileAES failed : "..tostring(strEncFilePath))
		return ""
	end
	
	local strTmpDir = tipUtil:GetSystemTempPath()
	if not tipUtil:QueryFileExists(strTmpDir) then
		TipLog("[GenDecFilePath] GetSystemTempPath failed strTmpDir: "..tostring(strTmpDir))
		return ""
	end
	
	local strCfgName = tipUtil:GetTmpFileName() or "data.dat"
	local strCfgPath = tipUtil:PathCombine(strTmpDir, strCfgName)
	tipUtil:WriteStringToFile(strCfgPath, strDecString)
	return strCfgPath
end


function GetRulePath(strServerFile, strLocalFile)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 

	local strServerRulePath = FunctionObj.GetCfgPathWithName(strServerFile)
	if IsRealString(strServerRulePath) and tipUtil:QueryFileExists(strServerRulePath) then
		return strServerRulePath
	end

	local strLocalRulePath = FunctionObj.GetCfgPathWithName(strLocalFile)
	if IsRealString(strLocalRulePath) and tipUtil:QueryFileExists(strLocalRulePath) then
		return strLocalRulePath
	end
	
	return ""
end


function SendRuleListtToFilterThread()
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 

	local strWebRulePath = GetRulePath("ServerADCWeb.dat", "ADCWeb.dat")
	local strVideoRulePath = GetRulePath("ServerADCVideo.dat", "ADCVideo.dat")
	if not IsRealString(strWebRulePath) or not tipUtil:QueryFileExists(strWebRulePath) 
	   or not IsRealString(strVideoRulePath) or not tipUtil:QueryFileExists(strVideoRulePath) then
		return false
	end

	local strDecWebRulePath = GenDecFilePath(strWebRulePath)
	if not IsRealString(strDecWebRulePath) then
		return false	
	end
	
	local bSucc = tipUtil:LoadWebRules(strDecWebRulePath)
	if not bSucc then
		FunctionObj.TipLog("[SendRuleListtToFilterThread] LoadWebRules failed")
		return false
	end
	tipUtil:DeletePathFile(strDecWebRulePath)
	
	local strDecVideoRulePath = GenDecFilePath(strVideoRulePath)
	if not IsRealString(strDecVideoRulePath) then
		return false	
	end
	local bSucc = tipUtil:LoadVideoRules(strDecVideoRulePath)
	if not bSucc then
		FunctionObj.TipLog("[SendRuleListtToFilterThread] LoadVideoRules failed")
		return false
	end
	tipUtil:DeletePathFile(strDecVideoRulePath)
	
	return true
end


function SendWhiteListToFilterThread()
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 

	local strWhiteListPath = GetRulePath("ServerADCWhite.dat", "ADCWhite.dat")
	if not IsRealString(strWhiteListPath) or not tipUtil:QueryFileExists(strWhiteListPath) then
		return false
	end		
	
	local tFileInfo = FunctionObj.LoadTableFromFile(strWhiteListPath) or {}
	local tWhiteList = tFileInfo["tWhiteList"] 
	if type(tWhiteList) ~= "table" then
		return false
	end
			
	for key, tWhiteElem in pairs(tWhiteList) do
		local strWhiteDomain = key
		local bStateOpen = tWhiteElem["bState"]
		if IsRealString(strWhiteDomain) and bStateOpen then
			AddWhiteDomain(strWhiteDomain)
		end
	end
	
	return true
end


function AddWhiteDomain(strDomain)
	local strDomainWithFix = FormatDomain(strDomain)

	if not IsRealString(strDomainWithFix) then
		return
	end

	tipUtil:AddWhiteHost(strDomainWithFix)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	FunctionObj.TipLog("[AddWhiteDomain] strDomainWithFix: "..tostring(strDomainWithFix))
end


function FormatDomain(strDomain)
	if not IsRealString(strDomain) then
		return ""
	end

	local strDomainWithFix = string.gsub(strDomain, "^www%.", "")
	return strDomainWithFix
end


function DownLoadServerRule(tDownRuleList)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	local nDownFlag = #tDownRuleList
	if nDownFlag == 0 then
		FunctionObj.TipLog("[DownLoadServerRule] no rule to down, start tipmain ")
		TipMain()
		return
	end	
	
	for key, tDownInfo in pairs(tDownRuleList) do 
		local strURL = tDownInfo["strURL"]
		local strPath = tDownInfo["strPath"]
	
		FunctionObj.NewAsynGetHttpFile(strURL, strPath, false
		, function(bRet, strVideoPath)
			FunctionObj.TipLog("[DownLoadServerRule] bRet:"..tostring(bRet)
					.." strVideoPath:"..tostring(strVideoPath))
					
			nDownFlag = nDownFlag - 1 
			if nDownFlag < 1 then
				FunctionObj.TipLog("[DownLoadServerRule] download finish, start tipmain ")
				TipMain()
			end

		end, 5*1000)
	end
end



function CheckServerRuleFile(tServerConfig)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	local tServerData = FetchValueByPath(tServerConfig, {"tServerData"}) or {}
	
	local strServerVideoURL = FetchValueByPath(tServerData, {"tServerDataV", "strURL"})
	local strServerVideoMD5 = FetchValueByPath(tServerData, {"tServerDataV", "strMD5"})
	local strServerWebURL = FetchValueByPath(tServerData, {"tServerDataW", "strURL"})
	local strServerWebMD5 = FetchValueByPath(tServerData, {"tServerDataW", "strMD5"})
	local strServerWhiteURL = FetchValueByPath(tServerData, {"tServerWhite", "strURL"})
	local strServerWhiteMD5 = FetchValueByPath(tServerData, {"tServerWhite", "strMD5"})
		
	local strVideoSavePath = FunctionObj.GetCfgPathWithName("ServerADCVideo.dat")
	local strWebSavePath = FunctionObj.GetCfgPathWithName("ServerADCWeb.dat")
	local strWhiteListPath = FunctionObj.GetCfgPathWithName("ServerADCWhite.dat")
	
	if not IsRealString(strVideoSavePath) or not tipUtil:QueryFileExists(strVideoSavePath) then
		strVideoSavePath = FunctionObj.GetCfgPathWithName("ADCVideo.dat")
	end
	if not IsRealString(strWebSavePath) or not tipUtil:QueryFileExists(strWebSavePath) then
		strWebSavePath = FunctionObj.GetCfgPathWithName("ADCWeb.dat")
	end
	if not IsRealString(strWhiteListPath) or not tipUtil:QueryFileExists(strWhiteListPath) then
		strWhiteListPath = FunctionObj.GetCfgPathWithName("ADCWhite.dat")
	end
	
	local strDataVMD5 = tipUtil:GetMD5Value(strVideoSavePath)
	local strDataWMD5 = tipUtil:GetMD5Value(strWebSavePath)
	local strWhiteMD5 = tipUtil:GetMD5Value(strWhiteListPath)
	local tDownRuleList = {}
	
	if IsRealString(strServerVideoURL) and tostring(strDataVMD5) ~= tostring(strServerVideoMD5) then
		local nIndex = #tDownRuleList+1
		tDownRuleList[nIndex] = {}
		tDownRuleList[nIndex]["strURL"] = strServerVideoURL
		tDownRuleList[nIndex]["strPath"] = FunctionObj.GetCfgPathWithName("ServerADCVideo.dat")
	end
	
	if IsRealString(strServerWebURL) and tostring(strDataWMD5) ~= tostring(strServerWebMD5) then
		local nIndex = #tDownRuleList+1
		tDownRuleList[nIndex] = {}
		tDownRuleList[nIndex]["strURL"] = strServerWebURL
		tDownRuleList[nIndex]["strPath"] = FunctionObj.GetCfgPathWithName("ServerADCWeb.dat")
	end
	
	if IsRealString(strServerWhiteURL) and tostring(strWhiteMD5) ~= tostring(strServerWhiteMD5) then
		local nIndex = #tDownRuleList+1
		tDownRuleList[nIndex] = {}
		tDownRuleList[nIndex]["strURL"] = strServerWhiteURL
		tDownRuleList[nIndex]["strPath"] = FunctionObj.GetCfgPathWithName("ServerADCWhite.dat")
	end
	
	DownLoadServerRule(tDownRuleList)
end


function AnalyzeServerConfig(nDownServer, strServerPath)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	if nDownServer ~= 0 or not tipUtil:QueryFileExists(tostring(strServerPath)) then
		FunctionObj.TipLog("[AnalyzeServerConfig] Download server config failed , start tipmain ")
		TipMain()
		return	
	end
	
	local tServerConfig = FunctionObj.LoadTableFromFile(strServerPath) or {}
	TryForceUpdate(tServerConfig)
	FixStartConfig(tServerConfig)
	FixUserConfig(tServerConfig)
	CheckServerRuleFile(tServerConfig)
end


function FixStartConfig(tServerConfig)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 

	local tStartConfig = tServerConfig["tStartConfig"]
	if type(tStartConfig) ~= "table" then
		return
	end
	
	local strStartCfgPath = FunctionObj.GetCfgPathWithName("startcfg.ini")
	if not IsRealString(strStartCfgPath) then
		return
	end

	local noremindspanday = tonumber(tStartConfig["noremindspanday"])
	local intervaltime = tonumber(tStartConfig["intervaltime"])
	local maxcntperday = tonumber(tStartConfig["maxcntperday"])

	if not IsNilString(noremindspanday) then
		tipUtil:WriteINI("pusher", "noremindspanday", noremindspanday, strStartCfgPath)
	end
	if not IsNilString(intervaltime) then
		tipUtil:WriteINI("pusher", "intervaltime", intervaltime, strStartCfgPath)
	end
	if not IsNilString(maxcntperday) then
		tipUtil:WriteINI("pusher", "maxcntperday", maxcntperday, strStartCfgPath)
	end
end



function StartRunCountTimer()
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 

	local nTimeSpanInSec = 10 * 60 
	local nTimeSpanInMs = nTimeSpanInSec * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		gnLastReportRunTmUTC = tipUtil:GetCurrentUTCTime()
		FunctionObj.SendRunTimeReport(nTimeSpanInSec, false)
		XLSetGlobal("Project.LastReportRunTime", gnLastReportRunTmUTC) 
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
		FunctionObj.FailExitTipWnd(4)
	end
	
	---初始化托盘
    if frameHostWnd then
	    FunctionObj.InitTrayTipWnd(frameHostWnd)
	end
end


function ProcessCommandLine()
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	local cmdString = tipUtil:GetCommandLine()
	
	if string.find(tostring(cmdString), "/showbubble") then
		FunctionObj.ShowPopupWndByName("TipBubbleWnd.Instance", true)
	end
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


function InitADCFilter()
	local FunctionObj = XLGetGlobal("Project.FunctionHelper")
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	local bFilterOpen = tUserConfig["bFilterOpen"]

	return FunctionObj.SwitchADCFilter(bFilterOpen)
end


function TipMain() 
	local FunctionObj = XLGetGlobal("Project.FunctionHelper")
	local bSucc = SendFileDataToFilterThread()
	if not bSucc then
		FunctionObj:FailExitTipWnd(2)
		return
	end
	
	local bSucc = InitADCFilter()  --里面退出
	if not bSucc then
		return
	end 
	
	CreateMainTipWnd()
	FunctionObj.CreatePopupTipWnd()
	ProcessCommandLine()
end


function PreTipMain() 
	gnLastReportRunTmUTC = tipUtil:GetCurrentUTCTime()
	XLSetGlobal("Project.LastReportRunTime", gnLastReportRunTmUTC) 
	
	if not RegisterFunctionObject() then
		tipUtil:Exit("Exit")
	end
	
	StartRunCountTimer()
	local FunctionObj = XLGetGlobal("Project.FunctionHelper")
	FunctionObj.ReadAllConfigInfo()
	SendStartReportLocal()
	SendStartupReportGgl(false)
	
	-- TipMain()
	
	FunctionObj.DownLoadServerConfig(AnalyzeServerConfig)
end

PreTipMain()

