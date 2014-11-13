local gStartCfg = {}

local g_bShowWndByTray = false
local gStatCount = 0
local gnLastReportRunTmUTC = 0
local gTimeoutTimerId = nil

local gbLoadCfgSucc = false
local g_tipNotifyIcon = nil
local tipUtil = XLGetObject("GS.Util")
local tipAsynUtil = XLGetObject("GS.AsynUtil")

local g_tPopupWndList = {
	[1] = {"TipFilterRemindWnd", "TipFilterRemindTree"},
	[2] = {"TipExitRemindWnd", "TipExitRemindTree"},
	[3] = {"TipAboutWnd", "TipAboutTree"},
	[4] = {"TipUpdateWnd", "TipUpdateTree"},
	[5] = {"TipBubbleWnd", "TipBubbleTree"},
}

local g_tConfigFileStruct = {
	["tUserConfig"] = {
		["strFileName"] = "UserConfig.dat",
		["tContent"] = {}, 
		["fnMergeOldFile"] = function(infoTable, strFileName) return MergeOldUserCfg(infoTable, strFileName) end,
	},
	["tFilterConfig"] = {
		["strFileName"] = "FilterConfig.dat",
		["tContent"] = {},
		["fnMergeOldFile"] = function(infoTable, strFileName) return MergeOldFilterCfg(infoTable, strFileName) end,
	},
	["tVideoList"] = {
		["strFileName"] = "VideoList.dat",
		["tContent"] = {},
		["fnMergeOldFile"] = function(infoTable, strFileName) return MergeOldVideoList(infoTable, strFileName) end,
	},
}


-----------------

function RegisterFunctionObject(self)
	local function FailExitTipWnd(self, iExitCode)
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		local tStatInfo = {}
			
		tStatInfo.strEC = "failexit"
		tStatInfo.strEA = GetInstallSrc() or ""
		tStatInfo.strEL = tostring(iExitCode)
		tStatInfo.Exit = true
			
		FunctionObj.TipConvStatistic(tStatInfo)
	end
	
	local function TipConvStatistic(tStat)
		local rdRandom = tipUtil:GetCurrentUTCTime()
		local tStatInfo = tStat or {}
		local strDefaultNil = "gs_null"
		
		local strCID = GetPeerID()
		local strEC = tStatInfo.strEC 
		local strEA = tStatInfo.strEA 
		local strEL = tStatInfo.strEL
		local strEV = tStatInfo.strEV
		
		if IsNilString(strEC) then
			strEC = strDefaultNil
		end
		
		if IsNilString(strEA) then
			strEA = strDefaultNil
		end
		
		if IsNilString(strEL) then
			strEL = strDefaultNil
		end
		
		if tonumber(strEV) == nil then
			strEV = 1
		end

		local strUrl = "http://www.google-analytics.com/collect?v=1&tid=UA-55122790-1&cid="..tostring(strCID)
						.."&t=event&ec="..tostring(strEC).."&ea="..tostring(strEA)
						.."&el="..tostring(strEL).."&ev="..tostring(strEV)
		TipLog("TipConvStatistic: " .. tostring(strUrl))
		
		gStatCount = gStatCount + 1
		if not gForceExit and tStat.Exit then
			gForceExit = true
		end
		tipAsynUtil:AsynSendHttpStat(strUrl, function()
			gStatCount = gStatCount - 1
			if gStatCount == 0 and gForceExit then
				ExitTipWnd()
			end
		end)
		
		local iStatCount = gStatCount
		if gForceExit and iStatCount > 0 and gTimeoutTimerId == nil then	--开启定时退出定时器
			local timeMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
			gTimeoutTimerId = timeMgr:SetTimer(function(Itm, id)
				Itm:KillTimer(id)
				ExitTipWnd()
			end, 15000 * iStatCount)
		end
	end
	
	local obj = {}
	obj.FailExitTipWnd = FailExitTipWnd
	obj.TipConvStatistic = TipConvStatistic
	obj.ReportAndExit = ReportAndExit
	obj.ShowPopupWndByName = ShowPopupWndByName
	obj.GetCfgPathWithName = GetCfgPathWithName
	obj.LoadTableFromFile = LoadTableFromFile
	obj.ShowExitRemindWnd = ShowExitRemindWnd
	obj.RegQueryValue = RegQueryValue
	obj.RegSetValue = RegSetValue
	obj.GetGSVersion = GetGSVersion
	obj.CheckTimeIsAnotherDay = CheckTimeIsAnotherDay
	obj.GetVideoDomainState = GetVideoDomainState
	obj.EnableWhiteDomain = EnableWhiteDomain
	obj.EnableVideoDomain = EnableVideoDomain
	obj.PopupBubbleOneDay = PopupBubbleOneDay
	obj.NewAsynGetHttpFile = NewAsynGetHttpFile
	obj.DownLoadFileWithCheck = DownLoadFileWithCheck
	obj.DownLoadServerConfig = DownLoadServerConfig
	obj.CheckIsNewVersion = CheckIsNewVersion
	obj.IsVideoDomain = IsVideoDomain
	obj.GetFileSaveNameFromUrl = GetFileSaveNameFromUrl
	obj.SetNotifyIconState = SetNotifyIconState
	obj.SetWndForeGround = SetWndForeGround
	obj.SwitchGSFilter = SwitchGSFilter
	obj.PopupNotifyIconTip = PopupNotifyIconTip
	obj.CheckCommonUpdateTime = CheckCommonUpdateTime
	obj.CheckAutoUpdateTime = CheckAutoUpdateTime
	obj.SaveCommonUpdateUTC = SaveCommonUpdateUTC
	obj.SaveAutoUpdateUTC = SaveAutoUpdateUTC
	obj.CheckMD5 = CheckMD5
	obj.GetSpecifyFilterTableFromMem = GetSpecifyFilterTableFromMem
	obj.SaveSpecifyFilterTableToMem = SaveSpecifyFilterTableToMem
	obj.SaveConfigToFileByKey = SaveConfigToFileByKey
	obj.ReadConfigFromMemByKey = ReadConfigFromMemByKey

	XLSetGlobal("GreenWallTip.FunctionHelper", obj)
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

function MessageBox(str)
	if not IsRealString(str) then
		return
	end
	
	tipUtil:MsgBox(str, "错误", 0x10)
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@greenwall_Template MainTipLog: " .. tostring(strLog))
	end
end

function IsUserFullScreen()
	local bRet = false
	if type(tipUtil.IsNowFullScreen) == "function" then
		bRet = tipUtil:IsNowFullScreen()
	end
	return bRet
end


function CheckTimeIsAnotherDay(LastTime)
	local bRet = false
	local LYear, LMonth, LDay, LHour, LMinute, LSecond = tipUtil:FormatCrtTime(LastTime)
	local curTime = tipUtil:GetCurrentUTCTime()
	local CYear, CMonth, CDay, CHour, CMinute, CSecond = tipUtil:FormatCrtTime(curTime)
	if LYear ~= CYear or LMonth ~= CMonth or LDay ~= CDay then
		bRet = true
	end
	return bRet
end


function QueryAllUsersDir()	--获取AllUser路径
	local bRet = false
	local strPublicEnv = "%PUBLIC%"
	local strRet = tipUtil:ExpandEnvironmentStrings(strPublicEnv)
	if strRet == nil or strRet == "" or strRet == strPublicEnv then
		local nCSIDL_COMMON_APPDATA = 35 --CSIDL_COMMON_APPDATA(0x0023)
		strRet = tipUtil:GetSpecialFolderPathEx(nCSIDL_COMMON_APPDATA)
	end
	if not IsNilString(strRet) and tipUtil:QueryFileExists(strRet) then
		bRet = true
	end
	return bRet, strRet
end


function GetFileSaveNameFromUrl(url)
	local _, _, strFileName = string.find(tostring(url), ".*/(.*)$")
	local npos = string.find(strFileName, "?", 1, true)
	if npos ~= nil then
		strFileName = string.sub(strFileName, 1, npos-1)
	end
	return strFileName
end


function CheckIsNewVersion(strNewVer, strCurVer)
	if not IsRealString(strNewVer) or not IsRealString(strCurVer) then
		return false
	end

	local a,b,c,d = string.match(strNewVer, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
	local A,B,C,D = string.match(strCurVer, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
	return a>A or (a==A and (b>B or (b==B and (c>C or (c==C and d>D)))))
end


function GetPeerID()
	local strPeerID = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\GreenShield\\PeerId")
	if IsRealString(strPeerID) then
		return strPeerID
	end

	local strRandPeerID = tipUtil:GetPeerId()
	if not IsRealString(strRandPeerID) then
		return ""
	end
	
	RegSetValue("HKEY_LOCAL_MACHINE\\Software\\GreenShield\\PeerId", strRandPeerID)
	return strRandPeerID
end

--渠道
function GetInstallSrc()
	local strInstallSrc = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\GreenShield\\InstallSource")
	if not IsNilString(strInstallSrc) then
		return tostring(strInstallSrc)
	end
	
	return ""
end


function NewAsynGetHttpFile(strUrl, strSavePath, bDelete, funCallback, nTimeoutInMS)
	local bHasAlreadyCallback = false
	local timerID = nil
	
	tipAsynUtil:AsynGetHttpFile(strUrl, strSavePath, bDelete, 
		function (nRet, strTargetFilePath, strHeaders)
			if timerID ~= nil then
				tipAsynUtil:KillTimer(timerID)
			end
			if not bHasAlreadyCallback then
				bHasAlreadyCallback = true
				funCallback(nRet, strTargetFilePath, strHeaders)
			end
		end)
	
	timerID = tipAsynUtil:SetTimer(nTimeoutInMS or 2 * 60 * 1000,
		function (nTimerId)
			tipAsynUtil:KillTimer(nTimerId)
			timerID = nil
			if not bHasAlreadyCallback then
				bHasAlreadyCallback = true
				funCallback(-2)
			end
		end)
end


function CheckMD5(strFilePath, strExpectedMD5) 
	local bPassCheck = false
	
	if not IsNilString(strFilePath) then
		local strMD5 = tipUtil:GetMD5Value(strFilePath)
		TipLog("[CheckMD5] strFilePath = " .. tostring(strFilePath) .. ", strMD5 = " .. tostring(strMD5))
		if not IsRealString(strExpectedMD5) 
			or (not IsNilString(strMD5) and not IsNilString(strExpectedMD5) and string.lower(strMD5) == string.lower(strExpectedMD5))
			then
			bPassCheck = true
		end
	end
	
	TipLog("[CheckMD5] strFilePath = " .. tostring(strFilePath) .. ", strExpectedMD5 = " .. tostring(strExpectedMD5) .. ". bPassCheck = " .. tostring(bPassCheck))
	return bPassCheck
end


function DownLoadFileWithCheck(strURL, strSavePath, strCheckMD5, fnCallBack)
	if type(fnCallBack) ~= "function"  then
		return
	end

	if IsRealString(strCheckMD5) and CheckMD5(strSavePath, strCheckMD5) then
		TipLog("[DownLoadFileWithCheck]File Already existed")
		fnCallBack(1)
		return
	end
	
	NewAsynGetHttpFile(strURL, strSavePath, false, function(bRet, strDownLoadPath)
		TipLog("[DownLoadFileWithCheck] NewAsynGetHttpFile:bret = " .. tostring(bRet) 
				.. ", strURL = " .. tostring(strURL) .. ", strDownLoadPath = " .. tostring(strDownLoadPath))
		if 0 == bRet then
			strSavePath = strDownLoadPath
            if CheckMD5(strSavePath, strCheckMD5) then
				fnCallBack(bRet, strSavePath)
			else
				TipLog("[DownLoadFileWithCheck]Did Not Pass MD5 Check")
				fnCallBack(-2)
			end	
		else
			TipLog("[DownLoadFileWithCheck] DownLoad failed")
			fnCallBack(-3)
		end
	end)
end


function DownLoadServerConfig(fnCallBack, nTimeInMs)
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	
	local strConfigURL = tUserConfig["strServerConfigURL"]
	if not IsRealString(strConfigURL) then
		fnCallBack(-1)
		return
	end
	
	local strSavePath = GetCfgPathWithName("ServerConfig.dat")
	if not IsRealString(strSavePath) then
		fnCallBack(-1)
		return
	end
	
	local nTime = tonumber(nTimeInMs) or 5*1000
		
	NewAsynGetHttpFile(strConfigURL, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[DownLoadServerConfig] bRet:"..tostring(bRet)
				.." strRealPath:"..tostring(strRealPath))
				
		if 0 == bRet then
			fnCallBack(0, strSavePath)
		else
			fnCallBack(bRet)
		end		
	end, nTime)
end


function SwitchGSFilter(bOpen)
	local bSucc = tipUtil:GSFilter(bOpen)
	if not bSucc then
		MessageBox(tostring("文件被损坏，请重新安装"))
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		TipLog("[SwitchGSFilter] GSFilter failed ")
		FunctionObj:FailExitTipWnd(3)
	end	
	
	return bSucc
end


function InitGSFilter()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local bFilterOpen = tUserConfig["bFilterOpen"]

	return SwitchGSFilter(bFilterOpen)
end


function SaveAllConfig()
	if gbLoadCfgSucc then
		for strKey, tContent in pairs(g_tConfigFileStruct) do
			SaveConfigToFileByKey(strKey)
		end
	end
end


function ExitTipWnd(statInfo)
	SaveAllConfig()	
	RestoreOSConfig()	
		
	TipLog("************ Exit ************")
	tipUtil:Exit("Exit")
end


function DestroyMainWnd()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local strHostWndName = "GreenWallTipWnd.MainFrame"
	local objHostWnd = hostwndManager:GetHostWnd(strHostWndName)
	if objHostWnd then
		hostwndManager:RemoveHostWnd(strHostWndName)
	end
end


function DestroyPopupWnd()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")

	for key, tItem in pairs(g_tPopupWndList) do
		local strPopupWndName = tItem[1]
		local strPopupInst = strPopupWndName..".Instance"
		
		local objPopupWnd = hostwndManager:GetHostWnd(strPopupInst)
		if objPopupWnd then
			hostwndManager:RemoveHostWnd(strPopupInst)
		end
	end
end


function GetGSVersion()
	local strGreenShieldPath = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\GreenShield\\path")
	if not IsRealString(strGreenShieldPath) or not tipUtil:QueryFileExists(strGreenShieldPath) then
		return ""
	end

	return tipUtil:GetFileVersionString(strGreenShieldPath)
end


function GetGSMinorVer()
	local strVersion = GetGSVersion()
	if not IsRealString(strVersion) then
		return ""
	end
	
	local _, _, strMinorVer = string.find(strVersion, "%d+%.%d+%.%d+%.(%d+)")
	return strMinorVer
end


function RegQueryValue(sPath)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			return tipUtil:QueryRegValue(sRegRoot, sRegPath, sRegKey or "") or ""
		end
	end
	return ""
end


function RegSetValue(sPath, value)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			return tipUtil:SetRegValue(sRegRoot, sRegPath, sRegKey or "", value or "")
		end
	end
	return false
end


function LoadTableFromFile(strDatFilePath)
	local tResult = nil

	if IsRealString(strDatFilePath) and tipUtil:QueryFileExists(strDatFilePath) then
		local tMod = XLLoadModule(strDatFilePath)
		if type(tMod) == "table" and type(tMod.GetSubTable) == "function" then
			local tDat = tMod.GetSubTable()
			if type(tDat) == "table" then
				tResult = tDat
			end
		end
	end
	
	return tResult
end

function PopTipWnd(OnCreateFunc)
	local bSuccess = false
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate("TipMainWnd", "HostWndTemplate" )
	local frameHostWnd = nil
	if frameHostWndTemplate then
		frameHostWnd = frameHostWndTemplate:CreateInstance("GreenWallTipWnd.MainFrame")
		if frameHostWnd then
			local objectTreeTemplate = nil
			objectTreeTemplate = templateMananger:GetTemplate("TipPanelTree", "ObjectTreeTemplate")
			if objectTreeTemplate then
				local uiObjectTree = objectTreeTemplate:CreateInstance("GreenWallTipWnd.MainObjectTree")
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
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd(4)
	end
	
	---初始化托盘
    if frameHostWnd and not g_tipNotifyIcon then
	    InitTrayTipWnd(frameHostWnd)
	end
end

function SetWndForeGround(objHostWnd)
	if not objHostWnd then
		return
	end

	if not IsUserFullScreen() then
		objHostWnd:SetTopMost(true)
		if type(tipUtil.SetWndPos) == "function" then
			local hWnd = objHostWnd:GetWndHandle()
			if hWnd ~= nil then
				TipLog("[SetWndForeGround] success")
				tipUtil:SetWndPos(hWnd, 0, 0, 0, 0, 0, 0x0043)
			end
		end
	elseif type(tipUtil.GetForegroundProcessInfo) == "function" then
		local hFrontHandle, strPath = tipUtil:GetForegroundProcessInfo()
		if hFrontHandle ~= nil then
			objHostWnd:BringWindowToBack(hFrontHandle)
		end
	end
end


function ShowMainTipWnd(objMainWnd)
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local bHideMainPage = FetchValueByPath(tUserConfig, {"tConfig", "HideMainPage", "bState"})
	
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
		SetWndForeGround(objMainWnd)
	end
	
	objMainWnd:SetTitle("绿盾广告管家")
	SendStartupReport(true)
end


function ShowPopupWndByName(strWndName, bSetTop)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd(tostring(strWndName))
	if frameHostWnd == nil then
		TipLog("[ShowPopupWindow] GetHostWnd failed: "..tostring(strWndName))
		return
	end

	if not IsUserFullScreen() then
		if type(tipUtil.SetWndPos) == "function" then
			local hWnd = frameHostWnd:GetWndHandle()
			if hWnd ~= nil then
				TipLog("[ShowPopupWndByName] success")
				if bSetTop then
					frameHostWnd:SetTopMost(true)
					tipUtil:SetWndPos(hWnd, 0, 0, 0, 0, 0, 0x0043)
				else
					tipUtil:SetWndPos(hWnd, -2, 0, 0, 0, 0, 0x0043)
				end
			end
		end
	elseif type(tipUtil.GetForegroundProcessInfo) == "function" then
		local hFrontHandle, strPath = tipUtil:GetForegroundProcessInfo()
		if hFrontHandle ~= nil then
			frameHostWnd:BringWindowToBack(hFrontHandle)
		end
	end
	
	frameHostWnd:Show(5)
end


function ShowExitRemindWnd()
	ShowPopupWndByName("TipExitRemindWnd.Instance", true)
end


function HideTray()
	if g_tipNotifyIcon then
		g_tipNotifyIcon:Hide()
	end
end


function InitTrayTipWnd(objHostWnd)
    if not objHostWnd then
	    TipLog("[InitTrayTipWnd] para error")
	    return
	end

	--创建托盘
    local tipNotifyIcon = XLGetObject("GS.NotifyIcon")
	if not tipNotifyIcon then
		TipLog("[InitTrayTipWnd] not support NotifyIcon")
	    return
	end
	
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	
	----托盘事件响应
	function OnTrayEvent(event1,event2,event3,event4)
		local strHostWndName = "GSTrayMenuHostWnd.MainFrame"
		local newWnd = hostwndManager:GetHostWnd(strHostWndName)	
				
		--单击右键,创建并显示菜单
		if event3 == 517 then
			if not newWnd then
        		CreateTrayTipWnd(objHostWnd)
			end
		end
		
		--单击左键
		if event3 == 0x0202 then
			ShowMainPanleByTray(objHostWnd)
		end
		
		--点击气泡
		if event3 == 1029 then
			if g_bShowWndByTray then
				ShowMainPanleByTray(objHostWnd)	
			end
		end
		
		--mousemove
		if event3 == 512 then
			g_tipNotifyIcon:ShowNotifyIconTip(false)
			SetNotifyIconState()
		end
	end

	tipNotifyIcon:Attach(OnTrayEvent)
	g_tipNotifyIcon = tipNotifyIcon
	SetNotifyIconState()
	tipNotifyIcon:Show()
end


function ShowMainPanleByTray(objHostWnd)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	if objHostWnd then
		objHostWnd:Show(5)
		SetWndForeGround(objHostWnd)
		local strState = objHostWnd:GetWindowState()
		if tostring(strState) == "min" then
			objHostWnd:BringWindowToTop(true)
		end
		
		local strHostWndName = "TipFilterRemindWnd.Instance"
		local objPopupWnd = hostwndManager:GetHostWnd(strHostWndName)
		if objPopupWnd and objPopupWnd:GetVisible() then
			local hWnd = objPopupWnd:GetWndHandle()
			if hWnd then
				objHostWnd:BringWindowToBack(hWnd)
			end
		end
	end
end


function SetNotifyIconState(strText)
	if not g_tipNotifyIcon then
		return
	end

	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local bFilterOpen = tUserConfig["bFilterOpen"] or false
	local nFilterCount = tUserConfig["nFilterCountOneDay"] or 0
	
	local strState = "正常过滤"
	if not bFilterOpen then
		strState = "停止过滤"
	end
	local strDefaultText = "绿盾广告管家\r\n状态："..strState.."\r\n今日累计过滤："..tostring(nFilterCount).."次"
	
	local strResImageDir = __document .. "\\..\\..\\..\\..\\res"
	local strImageName = "GreenWall.TrayIcon.Close.ico"
	if bFilterOpen then
		strImageName = "GreenWall.TrayIcon.Open.ico"
	end
	
	local strImagePath = strResImageDir.. "\\".. strImageName
	if not tipUtil:QueryFileExists(strImagePath) then
		strImagePath = nil
	end
	
	local strShowText = strText or strDefaultText
	g_tipNotifyIcon:SetIcon(strImagePath, strShowText)
end


function PopupNotifyIconTip(strText, bShowWndByTray)
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local bBubbleRemind = FetchValueByPath(tUserConfig, {"tConfig", "BubbleRemind", "bState"})
	
	if not bBubbleRemind then
		g_bShowWndByTray = false
		return
	end
	
	if IsRealString(strText) and g_tipNotifyIcon then
		g_tipNotifyIcon:ShowNotifyIconTip(true, strText)
	end
	
	g_bShowWndByTray = bShowWndByTray
end


function PopupBubbleOneDay()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local nLastBubbleUTC = tonumber(tUserConfig["nLastBubbleUTC"]) 
	
	if not IsNilString(nLastBubbleUTC) and not CheckTimeIsAnotherDay(nLastBubbleUTC) then
		return
	end
	
	if g_tipNotifyIcon then
		PopupNotifyIconTip("绿盾广告管家\r\n已开始为您过滤骚扰广告", true)
		tUserConfig["nLastBubbleUTC"] = tipUtil:GetCurrentUTCTime()
		SaveConfigToFileByKey("tUserConfig")
	end
end


function CreateTrayTipWnd(objHostWnd)
	local uTempltMgr = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local uHostWndMgr = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local uObjTreeMgr = XLGetObject("Xunlei.UIEngine.TreeManager")

	if uTempltMgr and uHostWndMgr and uObjTreeMgr then
		local uHostWnd = nil
		local strHostWndTempltName = "TipTrayWnd"
		local strHostWndTempltClass = "HostWndTemplate"
		local strHostWndName = "GSTrayMenuHostWnd.MainFrame"
		local uHostWndTemplt = uTempltMgr:GetTemplate(strHostWndTempltName, strHostWndTempltClass)
		if uHostWndTemplt then
			uHostWnd = uHostWndTemplt:CreateInstance(strHostWndName)
		end

		local uObjTree = nil
		local strObjTreeTempltName = "TrayMenuTree"
		local strObjTreeTempltClass = "ObjectTreeTemplate"
		local strObjTreeName = "GSTrayMenuWnd.MainObjectTree"
		local uObjTreeTemplt = uTempltMgr:GetTemplate(strObjTreeTempltName, strObjTreeTempltClass)
		if uObjTreeTemplt then
			uObjTree = uObjTreeTemplt:CreateInstance(strObjTreeName)
		end

		if uHostWnd and uObjTree then
			--函数会阻塞
			local bSucc = ShowPopupMenu(uHostWnd, uObjTree)
			
			if bSucc and uHostWnd:GetMenuMode() == "manual" then
				uObjTreeMgr:DestroyTree(strObjTreeName)
				uHostWndMgr:RemoveHostWnd(strHostWndName)
			end
		end
	end
end


function ShowPopupMenu(uHostWnd, uObjTree)
	uHostWnd:BindUIObjectTree(uObjTree)
					
	local nPosCursorX, nPosCursorY = tipUtil:GetCursorPos()
	if type(nPosCursorX) ~= "number" or type(nPosCursorY) ~= "number" then
		return false
	end
	
	local nScrnLeft, nScrnTop, nScrnRight, nScrnBottom = tipUtil:GetScreenArea()
	
	local objMainLayout = uObjTree:GetUIObject("TrayMenu.Main")
	if not objMainLayout then
	    return false
	end	
		
	local nL, nT, nR, nB = objMainLayout:GetObjPos()				
	local nMenuContainerWidth = nR - nL
	local nMenuContainerHeight = nB - nT
	local nMenuScreenLeft = nPosCursorX
	local nMenuScreenTop = nPosCursorY - nMenuContainerHeight
	TipLog("[ShowTrayCtrlPanel] about to popup menu")
	
	if nMenuScreenLeft+nMenuContainerWidth > nScrnRight - 10 then
		nMenuScreenLeft = nPosCursorX - nMenuContainerWidth
	end
	
	uHostWnd:SetFocus(false) --先失去焦点，否则存在菜单不会消失的bug
	
	--函数会阻塞
	local bOk = uHostWnd:TrackPopupMenu(objHostWnd, nMenuScreenLeft, nMenuScreenTop, nMenuContainerWidth, nMenuContainerHeight)
	TipLog("[ShowPopupMenu] end menu")
	
	return bOk
end

-------------------------------

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


function ReadConfigFromMemByKey(strKey)
	if not IsRealString(strKey) or type(g_tConfigFileStruct[strKey])~="table" then
		return nil
	end

	local tContent = g_tConfigFileStruct[strKey]["tContent"]
	return tContent
end


function GetCfgPathWithName(strCfgName)
	local bOk, strBaseDir = QueryAllUsersDir()
	if not bOk then
		return ""
	end
	
	local strCfgFilePath = tipUtil:PathCombine(strBaseDir, "GreenShield\\"..tostring(strCfgName))
	return strCfgFilePath or ""
end


function GetSpecifyFilterTableFromMem(strTableName)
	local tFilterTable = ReadConfigFromMemByKey("tFilterConfig") or {}

	if not IsRealString(strTableName) then
		return tFilterTable
	end

	return tFilterTable[strTableName]
end


function SaveSpecifyFilterTableToMem(tNewTable, strTableName)
	local tFilterTable = ReadConfigFromMemByKey("tFilterConfig") or {}

	if not IsRealString(strTableName) then
		tFilterTable = tNewTable
	else
		tFilterTable[strTableName] = tNewTable
	end
end


function SaveConfigToFileByKey(strKey)
	if not IsRealString(strKey) or type(g_tConfigFileStruct[strKey])~="table" then
		return
	end

	local strFileName = g_tConfigFileStruct[strKey]["strFileName"]
	local tContent = g_tConfigFileStruct[strKey]["tContent"]
	local strConfigPath = GetCfgPathWithName(strFileName)
	if IsRealString(strConfigPath) and type(tContent) == "table" then
		tipUtil:SaveLuaTableToLuaFile(tContent, strConfigPath)
	end
end


function ReadAllConfigInfo()
	for strKey, tConfig in pairs(g_tConfigFileStruct) do
		local strFileName = tConfig["strFileName"]
		local strCfgPath = GetCfgPathWithName(strFileName)
		local infoTable = LoadTableFromFile(strCfgPath)
		if type(infoTable) ~= "table" then
			TipLog("[ReadAllConfigInfo] GetConfigFile failed! "..tostring(strFileName))
			return false
		end
		
		local tContent = infoTable
		local fnMergeOldFile = tConfig["fnMergeOldFile"]
		if type(fnMergeOldFile) == "function" then
			tContent = fnMergeOldFile(infoTable, strFileName)
		end
		
		tConfig["tContent"] = tContent
	end

	gbLoadCfgSucc = true	
	TipLog("[ReadAllConfigInfo] success!")
	return true
end


function MergeOldUserCfg(tCurrentCfg, strFileName)
	local tOldCfg, strOldCfgPath = GetOldCfgContent(strFileName)
	if type(tOldCfg) ~= "table" then
		return tCurrentCfg
	end
	
	tCurrentCfg["nFilterCountTotal"] = tOldCfg["nFilterCountTotal"] or 0
	tCurrentCfg["nFilterCountOneDay"] = tOldCfg["nFilterCountOneDay"] or 0
	tCurrentCfg["bFilterOpen"] = tOldCfg["bFilterOpen"]
	tCurrentCfg["nLastAutoUpdateUTC"] = tOldCfg["nLastAutoUpdateUTC"]
	tCurrentCfg["nLastBubbleUTC"] = tOldCfg["nLastBubbleUTC"]
	tCurrentCfg["nLastClearUTC"] = tOldCfg["nLastClearUTC"]
	tCurrentCfg["tConfig"] = tOldCfg["tConfig"]
	tCurrentCfg["nLastCommonUpdateUTC"] = tOldCfg["nLastCommonUpdateUTC"]
	
	tipUtil:DeletePathFile(strOldCfgPath)
	return tCurrentCfg
end


function MergeOldFilterCfg(tCurrentCfg, strFileName)
	local tOldCfg, strOldCfgPath  = GetOldCfgContent(strFileName)
	if type(tOldCfg) ~= "table" then
		return tCurrentCfg
	end
	
	tCurrentCfg["tBlackList"] = tOldCfg["tBlackList"]
	tCurrentCfg["tUserWhiteList"] = tOldCfg["tUserWhiteList"]
	
	local tDefWhiteList = tOldCfg["tDefWhiteList"] or {}
	if type(tCurrentCfg["tDefWhiteList"]) ~= "table" then
		tCurrentCfg["tDefWhiteList"] = {}
	end	
	
	for strDomain, tInfo in pairs(tDefWhiteList) do
		if type(tInfo) == "table" then
			tCurrentCfg["tDefWhiteList"][strDomain] = tInfo
		end
	end
	
	tipUtil:DeletePathFile(strOldCfgPath)
	return tCurrentCfg
end


function MergeOldVideoList(tCurrentCfg, strFileName)
	local tOldCfg, strOldCfgPath = GetOldCfgContent(strFileName)
	if type(tOldCfg) ~= "table" then
		return tCurrentCfg
	end
	
	for strDomain, tInfo in pairs(tOldCfg) do
		if type(tInfo) == "table" then
			tCurrentCfg[strDomain] = tInfo
		end
	end
	
	tipUtil:DeletePathFile(strOldCfgPath)
	return tCurrentCfg
end


function GetOldCfgContent(strCurFileName)
	local strOldFileName = strCurFileName..".bak"
	local strOldCfgPath = GetCfgPathWithName(strOldFileName)
	if not IsRealString(strOldCfgPath) or not tipUtil:QueryFileExists(strOldCfgPath) then
		return nil
	end
	
	local tOldCfg = LoadTableFromFile(strOldCfgPath)
	return tOldCfg, strOldCfgPath
end


function IsVideoDomain(strDomain)
	local strDomainWithFix = FormatDomain(strDomain)
	
	if not IsRealString(strDomainWithFix) then
		return false
	end
	
	local tVideoList = ReadConfigFromMemByKey("tVideoList") or {}
	if tVideoList[strDomainWithFix] then
		return true
	else
		return false
	end
end



function SendFileDataToFilterThread()
	local bSucc = SendRuleListtToFilterThread()
	if not bSucc then
		MessageBox(tostring("文件被损坏，请重新安装"))
		return false
	end	

	local bSucc = SendWhiteListToFilterThread()
	if not bSucc then
		return false
	end

	local bSucc = SendVideoListToFilterThread()
	if not bSucc then
		return false
	end
	
	return true
end


function GenDecFilePath(strEncFilePath)
	local strKey = "fvSem9Rt6wvhxmzs"
	local strDecString = tipUtil:DecryptFileAES(strEncFilePath, strKey)
	if type(strDecString) ~= "string" then
		TipLog("[GenDecFilePath] DecryptFileAES failed : "..tostring(strEncFilePath))
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


function GetWebRulePath()
	local strServerWebRulePath = GetCfgPathWithName("ServerDataW.dat")
	if IsRealString(strServerWebRulePath) and tipUtil:QueryFileExists(strServerWebRulePath) then
		return strServerWebRulePath
	end

	local strWebRulePath = GetCfgPathWithName("DataW.dat")
	if IsRealString(strWebRulePath) and tipUtil:QueryFileExists(strWebRulePath) then
		return strWebRulePath
	end
	
	return ""
end


function GetVideoRulePath()
	local strServerVideoRulePath = GetCfgPathWithName("ServerDataV.dat")
	if IsRealString(strServerVideoRulePath) and tipUtil:QueryFileExists(strServerVideoRulePath) then
		return strServerVideoRulePath
	end

	local strVideoRulePath = GetCfgPathWithName("DataV.dat")
	if IsRealString(strVideoRulePath) and tipUtil:QueryFileExists(strVideoRulePath) then
		return strVideoRulePath
	end
	
	return ""
end


function SendRuleListtToFilterThread()
	local strWebRulePath = GetWebRulePath()
	local strVideoRulePath = GetVideoRulePath()
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
		TipLog("[SendRuleListtToFilterThread] LoadWebRules failed")
		return false
	end
	tipUtil:DeletePathFile(strDecWebRulePath)
	
	local strDecVideoRulePath = GenDecFilePath(strVideoRulePath)
	if not IsRealString(strDecVideoRulePath) then
		return false	
	end
	local bSucc = tipUtil:LoadVideoRules(strDecVideoRulePath)
	if not bSucc then
		TipLog("[SendRuleListtToFilterThread] LoadVideoRules failed")
		return false
	end
	tipUtil:DeletePathFile(strDecVideoRulePath)
	
	return true
end


function SendVideoListToFilterThread()
	local tVideoList = ReadConfigFromMemByKey("tVideoList") or {}
	
	for strDomain, tVideoElem in pairs(tVideoList) do
		if IsRealString(strDomain) and type(tVideoElem) == "table" then
			local nLastPopupUTC = tVideoElem["nLastPopupUTC"]
			CheckVideoList(strDomain, nLastPopupUTC)
		end
	end

	return true
end


function CheckVideoList(strDomain, nLastPopupUTC) 
	local bBlackState, bStateOpen = GetVideoDomainState(strDomain)
	
	if bBlackState == 0 then
		AddVideoDomain(strDomain, 0)
	elseif bBlackState == 1 then
		AddVideoDomain(strDomain, 1)
		
	elseif bBlackState == 2 then
		if IsNilString(nLastPopupUTC) or CheckTimeIsAnotherDay(nLastPopupUTC) then
			AddVideoDomain(strDomain, 0)
		else
			AddVideoDomain(strDomain, 2)
		end
	elseif bBlackState == 3 then
		if bStateOpen then
			AddVideoDomain(strDomain, 1)
		else
			AddVideoDomain(strDomain, 2)
		end
	end
end


--针对视频网站
--0 弹过滤提醒窗口
--1 过滤
--2 不过滤
function EnableVideoDomain(strDomain, nState)
	local strDomainWithFix = FormatDomain(strDomain)

	if not IsRealString(strDomainWithFix) or type(nState) ~= "number" then
		return
	end

	tipUtil:UpdateVideoHost(strDomainWithFix, nState)
	TipLog("[UpdateVideoHost] strDomainWithFix: "..tostring(strDomainWithFix).." nState: "..tostring(nState))
end


function SynchDefaultWhiteList(strDomainWithFix, bSetWite)
	local tDefWhiteList = GetSpecifyFilterTableFromMem("tDefWhiteList") or {}
	if type(tDefWhiteList[strDomainWithFix]) == "table" then
		tDefWhiteList[strDomainWithFix]["bState"] = bSetWite
	end
end


function EnableWhiteDomain(strDomain, bSetWite)
	local strDomainWithFix = FormatDomain(strDomain)
	if not IsRealString(strDomainWithFix) or type(bSetWite) ~= "boolean" then
		return
	end

	SynchDefaultWhiteList(strDomainWithFix, bSetWite) --同步用户操作到默认白名单表
	
	tipUtil:UpdateWhiteHost(strDomainWithFix, bSetWite)
	TipLog("[UpdateWhiteHost] strDomainWithFix: "..tostring(strDomainWithFix).." nState: "..tostring(bSetWite))
end


function AddWhiteDomain(strDomain)
	local strDomainWithFix = FormatDomain(strDomain)

	if not IsRealString(strDomainWithFix) then
		return
	end

	tipUtil:AddWhiteHost(strDomainWithFix)
	TipLog("[AddWhiteDomain] strDomainWithFix: "..tostring(strDomainWithFix))
end


function AddVideoDomain(strDomain, nState)
	if not IsRealString(strDomain) or type(nState) ~= "number" then
		return
	end

	tipUtil:AddVideoHost(strDomain, nState)
	TipLog("[AddVideoDomain] strDomain: "..tostring(strDomain).." nState: "..tostring(nState))
end


function FormatDomain(strDomain)
	if not IsRealString(strDomain) then
		return ""
	end

	local strDomainWithFix = string.gsub(strDomain, "^www%.", "")
	return strDomainWithFix
end


function SendWhiteListToFilterThread()
	local tDefWhiteList = GetSpecifyFilterTableFromMem("tDefWhiteList") or {}
	local tUserWhiteList = GetSpecifyFilterTableFromMem("tUserWhiteList") or {}
		
	for key, tWhiteElem in pairs(tDefWhiteList) do
		local strWhiteDomain = key
		local bStateOpen = tWhiteElem["bState"]
		if IsRealString(strWhiteDomain) and bStateOpen then
			AddWhiteDomain(strWhiteDomain)
		end
	end
	
	for key, tWhiteElem in pairs(tUserWhiteList) do
		local strWhiteDomain = tWhiteElem["strDomain"]
		local bStateOpen = tWhiteElem["bState"]
		if IsRealString(strWhiteDomain) and bStateOpen then
			AddWhiteDomain(strWhiteDomain)
		end
	end
	
	return true
end


--0 不在视频列表且不是用户添加的域名
--1 在视频列表，且开启过滤
--2 在视频列表，且关闭过滤
--3 非视频(用户执行添加的域名)
function GetVideoDomainState(strDomain)
	local tBlackList = GetSpecifyFilterTableFromMem("tBlackList")
	for key, tBlackElem in pairs(tBlackList) do
		local strBlackDomain = tBlackElem["strDomain"]
		local bStateOpen = tBlackElem["bState"]
		local bIsVideo = IsVideoDomain(strDomain)
		if IsRealString(strBlackDomain) then
			if not bIsVideo then
				return 3, bStateOpen
			
			elseif string.find(strBlackDomain, strDomain) then
				if bStateOpen then
					return 1, bStateOpen
				else
					return 2, bStateOpen
				end
			end
		end
	end
	
	return 0
end


function CreateMainTipWnd()
	local function OnCreateFuncF(treectrl)
		local rootctrl = treectrl:GetUIObject("root.layout:root.ctrl")
		local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
		local bRet = rootctrl:SetTipData(tUserConfig)			
		if not bRet then
			return false
		end
	
		return true
	end
	
	PopTipWnd(OnCreateFuncF)	
end


function CreatePopupTipWnd()
	for key, tItem in pairs(g_tPopupWndList) do
		local strHostWndName = tItem[1]
		local strTreeName = tItem[2]
		local bSucc = CreateWndByName(strHostWndName, strTreeName)
		
		if not bSucc then
			TipLog("[CreatePopupTipWnd] create wnd failed: "..tostring(strHostWndName))
			local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
			FunctionObj:FailExitTipWnd(5)
			return false
		end
	end
	
	return true
end


function CreateWndByName(strHostWndName, strTreeName)
	local bSuccess = false
	local strInstWndName = strHostWndName..".Instance"
	local strInstTreeName = strTreeName..".Instance"
	
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate(strHostWndName, "HostWndTemplate" )
	if frameHostWndTemplate then
		local frameHostWnd = frameHostWndTemplate:CreateInstance(strInstWndName)
		if frameHostWnd then
			local objectTreeTemplate = nil
			objectTreeTemplate = templateMananger:GetTemplate(strTreeName, "ObjectTreeTemplate")
			if objectTreeTemplate then
				local uiObjectTree = objectTreeTemplate:CreateInstance(strInstTreeName)
				if uiObjectTree then
					frameHostWnd:BindUIObjectTree(uiObjectTree)
					local iRet = frameHostWnd:Create()
					if iRet ~= nil and iRet ~= 0 then
						bSuccess = true
					end
				end
			end
		end
	end

	return bSuccess
end

function SaveConfigInTimer()
	local nTimeSpanInMs = 10 * 60 * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		SaveAllConfig()
	end, nTimeSpanInMs)
end


function StartRunCountTimer()
	local nTimeSpanInSec = 10 * 60 
	local nTimeSpanInMs = nTimeSpanInSec * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		gnLastReportRunTmUTC = tipUtil:GetCurrentUTCTime()
		SendRunTimeReport(nTimeSpanInSec, false)
	end, nTimeSpanInMs)
end


function GetCommandStrValue(strKey)
	local bRet, strValue = false, nil
	local cmdString = tipUtil:GetCommandLine()
	
	if string.find(cmdString, strKey .. " ") then
		local cmdList = tipUtil:CommandLineToList(cmdString)
		if cmdList ~= nil then	
			for i = 1, #cmdList, 2 do
				local strTmp = tostring(cmdList[i])
				if strTmp == strKey 
					and not string.find(tostring(cmdList[i + 1]), "^/") then		
					bRet = true
					strValue = tostring(cmdList[i + 1])
					break
				end
			end
		end
	end
	return bRet, strValue
end


function SendStartupReport(bShowWnd)
	local tStatInfo = {}
	
	local bRet, strSource = GetCommandStrValue("/sstartfrom")
	tStatInfo.strEL = strSource or ""
	
	if not bShowWnd then
		tStatInfo.strEC = "startup"
		tStatInfo.strEA = GetGSMinorVer() or ""
		tStatInfo.strEV = 0   --进入上报
	else
		tStatInfo.strEC = "showui"
		tStatInfo.strEA = GetInstallSrc() or ""
		tStatInfo.strEV = 1   --展示上报
	end
		
	local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
	FunctionObj.TipConvStatistic(tStatInfo)
end


function SendGSStartReport()
	local strCID = GetPeerID()
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
	local strChannelID = GetInstallSrc()
	
	local strUrl = "http://stat.lvdun123.com:8082/?mac=" .. tostring(strMACFix) 
					.."&op=start&cid=" .. (strChannelID)
	TipLog("SendGSStartReport: " .. tostring(strUrl))
	tipAsynUtil:AsynSendHttpStat(strUrl, function() end)
end


function ReportAndExit()
	DestroyMainWnd()
	DestroyPopupWnd()
	HideTray()
	
	local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
	local tStatInfo = {}
			
	SendRunTimeReport(0, true)
	
	tStatInfo.strEC = "exit"	
	tStatInfo.strEA = GetInstallSrc() or ""
	tStatInfo.Exit = true
		
	FunctionObj.TipConvStatistic(tStatInfo)
end


function SendRunTimeReport(nTimeSpanInSec, bExit)
	local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
	local tStatInfo = {}
	tStatInfo.strEC = "runtime"
	tStatInfo.strEA = GetInstallSrc() or ""
	
	local nRunTime = 0
	if bExit and gnLastReportRunTmUTC ~= 0 then
		nRunTime = math.abs(tipUtil:GetCurrentUTCTime() - gnLastReportRunTmUTC)
	else
		nRunTime = nTimeSpanInSec
	end
	tStatInfo.strEV = nRunTime
	
	FunctionObj.TipConvStatistic(tStatInfo)
end


--win8.1对IE11的支持.
function SetIERegValue(strNewRegValue)
	local nMajVer, nMinVer = tipUtil:GetOSVersion()
	if IsNilString(nMajVer) or IsNilString(nMinVer) 
		or nMajVer < 6 or nMinVer < 2 then
		return
	end
	
	TipLog("[SetIERegValue] win 8 or 8.1")
	
	local strRegPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\Main\\Isolation"
	local strRegValue = RegQueryValue(strRegPath)
	if not IsRealString(strRegValue) then
		tipUtil:CreateRegKey("HKEY_CURRENT_USER", "Software\\Microsoft\\Internet Explorer\\Main\\Isolation")
	end
	
	TipLog("[SetIERegValue] strNewRegValue:"..tostring(strNewRegValue))
	RegSetValue(strRegPath, strNewRegValue)
end


function DeleteBrowserCache()
	local t = {
	"%USERPROFILE%\\Local Settings\\Temporary Internet Files\\*.html",
	"%USERPROFILE%\\Local Settings\\Temporary Internet Files\\*.js",
	"%USERPROFILE%\\Local Settings\\Temporary Internet Files\\*.htm",
	"%USERPROFILE%\\Local Settings\\Temporary Internet Files\\*.xml",
	"%USERPROFILE%\\Local Settings\\Temporary Internet Files\\*.css",
	"%USERPROFILE%\\Local Settings\\Temporary Internet Files\\*.swf",
	"%USERPROFILE%\\Local Settings\\Temporary Internet Files\\*.flv",
	"%USERPROFILE%\\Local Settings\\Temporary Internet Files\\*.mp4",
	"%USERPROFILE%\\Local Settings\\Temporary Internet Files\\Content.IE5\\*.*",
	"%USERPROFILE%\\AppData\\Local\\Microsoft\\Windows\\Temporary Internet Files\\*.*",
	"%USERPROFILE%\\AppData\\Local\\Microsoft\\Windows\\Temporary Internet Files\\Content.IE5\\*.*",
	"%USERPROFILE%\\Application Data\\360se\\ie8data\\Temporary Internet Files\\*.*",
	"%USERPROFILE%\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Cache\\*.*",
	"%USERPROFILE%\\Local Settings\\Application Data\\360Chrome\\Chrome\\User Data\\Default\\CacheIE\\*.*",
	"%USERPROFILE%\\Local Settings\\Application Data\\360Chrome\\Chrome\\User Data\\Default\\Cache\\*.*",
	"%USERPROFILE%\\AppData\\Local\\360Chrome\\Chrome\\User Data\\Default\\Cache\\*.*",
	"%USERPROFILE%\\Local Settings\\Application Data\\360Chrome\\Chrome\\User Data\\Default\\CacheIE\\Content.IE5\\*.*",
	"%USERPROFILE%\\Application Data\\SogouExplorer\\Webkit\\Default\\Cache\\*.*",
	"%USERPROFILE%\\AppData\\Roaming\\SogouExplorer\\Webkit\\Default\\Cache\\*.*",
	"%USERPROFILE%\\AppData\\Local\\Opera\\Opera\\cache\\*.*",
	"%USERPROFILE%\\Local Settings\\Application Data\\Opera\\Opera\\cache\\*.*",
	"%USERPROFILE%\\Local Settings\\Application Data\\Opera\\Opera\\application_cache\\mcache\\*.*",
	"%USERPROFILE%\\Local Settings\\Application Data\\Opera\\Opera\\application_cache\\cache_groups.xml",
	"%USERPROFILE%\\Local Settings\\Application Data\\Opera\\Opera\\opcache\\*.*",
	"%USERPROFILE%\\AppData\\Local\\TaoBrowser\\User Data\\Default\\Cache\\*.*",
	"%USERPROFILE%\\AppData\\Local\\TaoBrowser\\User Data\\Default\\JumpListIcons\\*.*",
	"%USERPROFILE%\\Local Settings\\Application Data\\TaoBrowser\\User Data\\Default\\Cache\\*.*",
	"%USERPROFILE%\\Application Data\\Baidu\\browser\\DiskCache\\*.*",
	"%USERPROFILE%\\AppData\\Roaming\\Baidu\\browser\\DiskCache\\*.*",
	"%USERPROFILE%\\Local Settings\\Application Data\\liebao\\User Data\\Default\\Cache\\*.*",
	"%USERPROFILE%\\Local Settings\\Application Data\\liebao\\User Data\\iecache\\Content.IE5\\*.*",
	"%USERPROFILE%\\AppData\\Local\\liebao\\User Data\\Default\\Cache\\*.*",
	"%USERPROFILE%\\AppData\\Local\\liebao\\User Data\\Default\\JumpListIcons\\*.*",
	"%USERPROFILE%\\Application Data\\Letv\\Logg\\*.log",
	"%APPDATA%\\PPStream\\adsys\\*.*",
	"%APPDATA%\\PPStream\\banner\\*.*",
	"%APPDATA%\\PPStream\\notice\\*.*",
	"%APPDATA%\\PPStream\\CLCache\\*.pld",
	"%APPDATA%\\PPStream\\FDSCache\\*.blf",
	"%APPDATA%\\PPLive\\PPTV\\cache\\*.*",
	"%USERPROFILE%\\funshion\\cache\\*.*",
	"%ALLUSERSPROFILE%\\Baofeng\\StormPlayer\\Profiles\\md\\*.*",
	"%ALLUSERSPROFILE%\\Baofeng\\StormPlayer\\Profiles\\vod\\*.*",
	"%ALLUSERSPROFILE%\\Application Data\\Baofeng\\StormPlayer\\Profiles\\md\\*.*",
	"%ALLUSERSPROFILE%\\Application Data\\Baofeng\\StormPlayer\\Profiles\\vod\\*.*",
	"%ALLUSERSPROFILE%\\Baofeng\\Application Data\\StormPlayer\\Profiles\\md\\*.*",
	"%ALLUSERSPROFILE%\\Baofeng\\Application Data\\StormPlayer\\Profiles\\vod\\*.*",
	"%ALLUSERSPROFILE%\\PPLive\\PPTV\\Cache\\pluginad\\*.*",
	"%ALLUSERSPROFILE%\\Application Data\\PPLive\\PPTV\\Cache\\pluginad\\*.*",
	"%ALLUSERSPROFILE%\\Application Data\\PPLive\\PPTV\\screensaver\\*.*",
	"C:\\Program Files\\DuoMi\\dmdeskinfo.exe",
	"%TEMP%\\UUFile\\*.*",
	"%ALLUSERSPROFILE%\\Application Data\\Qiyi\\qiyiclient\\cache\\*.*",
	}
	
	for key, strPattern in pairs(t) do
		local _, _, strEnvHead, strPathBody, strFileName = string.find(strPattern, "^([^\\]*)\\(.*)\\([^\\]*)$")
		
		local strEnvName = tipUtil:ExpandEnvironmentStrings(tostring(strEnvHead))
		local strDir = tipUtil:PathCombine(strEnvName, strPathBody)

		if IsRealString(strDir) and tipUtil:QueryFileExists(strDir) then
			local strFileList = {}
			local strFileList = tipUtil:FindFileList(strDir, strFileName) or {}

			for i, strFile in pairs(strFileList) do
				if tipUtil:QueryFileExists(strFile) then
					tipUtil:DeletePathFile(strFile)
				end
			end
		end
	end
end

function ModifyOSConfig()
	SetIERegValue("PMIL")
	DeleteBrowserCache()
end

function RestoreOSConfig()
	SetIERegValue("PMEM")
end


function DownLoadServerRule(tDownRuleList)
	local nDownFlag = #tDownRuleList
	if nDownFlag == 0 then
		TipLog("[DownLoadServerRule] no rule to down, start tipmain ")
		TipMain()
		return
	end	
	
	for key, tDownInfo in pairs(tDownRuleList) do 
		local strURL = tDownInfo["strURL"]
		local strPath = tDownInfo["strPath"]
	
		NewAsynGetHttpFile(strURL, strPath, false
		, function(bRet, strVideoPath)
			TipLog("[DownLoadServerRule] bRet:"..tostring(bRet)
					.." strVideoPath:"..tostring(strVideoPath))
					
			nDownFlag = nDownFlag - 1 
			if nDownFlag < 1 then
				TipLog("[DownLoadServerRule] download finish, start tipmain ")
				TipMain()
			end

		end, 5*1000)
	end
end


function CheckForceVersion(tForceVersion)
	if type(tForceVersion) ~= "table" then
		return false
	end

	local bRightVer = false
	
	local strCurVersion = GetGSVersion()
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


function DownLoadNewVersion(tNewVersionInfo, fnCallBack)
	local strPacketURL = tNewVersionInfo.strPacketURL
	local strMD5 = tNewVersionInfo.strMD5
	if not IsRealString(strPacketURL) then
		return
	end
	
	local strFileName = GetFileSaveNameFromUrl(strPacketURL)
	if not string.find(strFileName, "%.exe$") then
		strFileName = strFileName..".exe"
	end
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)

	DownLoadFileWithCheck(strPacketURL, strSavePath, strMD5
	, function(bRet, strRealPath)
		TipLog("[DownLoadNewVersion] strOpenLink:"..tostring(strPacketURL)
		        .."  bRet:"..tostring(bRet).."  strRealPath:"..tostring(strRealPath))
				
		if 0 == bRet then
			fnCallBack(strRealPath, tNewVersionInfo)
			return
		end
		
		if 1 == bRet then
			fnCallBack(strSavePath, tNewVersionInfo)
			return
		end
	end)	
end


function CheckCommonUpdateTime(nTimeInDay)
	return CheckUpdateTimeSpan(nTimeInDay, "nLastCommonUpdateUTC")
end

function CheckAutoUpdateTime(nTimeInDay)
	return CheckUpdateTimeSpan(nTimeInDay, "nLastAutoUpdateUTC")
end

function CheckUpdateTimeSpan(nTimeInDay, strUpdateType)
	if type(nTimeInDay) ~= "number" then
		return false
	end
	
	local nTimeInSec = nTimeInDay*24*3600
	local nCurTimeUTC = tipUtil:GetCurrentUTCTime()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local nLastUpdateUTC = tUserConfig[strUpdateType] or 0
	local nTimeSpan = math.abs(nCurTimeUTC - nLastUpdateUTC)
	
	if nTimeSpan > nTimeInSec then
		return true
	end	
	
	return false
end


function SaveCommonUpdateUTC()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nLastCommonUpdateUTC"] = tipUtil:GetCurrentUTCTime()
	SaveConfigToFileByKey("tUserConfig")
end


function SaveAutoUpdateUTC()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nLastAutoUpdateUTC"] = tipUtil:GetCurrentUTCTime()
	SaveConfigToFileByKey("tUserConfig")
end


function TryForceUpdate(tServerConfig)
	local bPassCheck = CheckCommonUpdateTime(1)
	if not bPassCheck then
		TipLog("[TryForceUpdate] CheckCommonUpdateTime failed")
		return		
	end

	local tNewVersionInfo = tServerConfig["tNewVersionInfo"] or {}
	local tForceUpdate = tNewVersionInfo["tForceUpdate"]
	if(type(tForceUpdate)) ~= "table" then
		return 
	end
	
	local strCurVersion = GetGSVersion()
	local strNewVersion = tForceUpdate.strVersion		
	if not IsRealString(strCurVersion) or not IsRealString(strNewVersion)
		or not CheckIsNewVersion(strNewVersion, strCurVersion) then
		return
	end
	
	local tVersionLimit = tForceUpdate["tVersion"]
	local bPassCheck = CheckForceVersion(tVersionLimit)
	TipLog("[TryForceUpdate] CheckForceVersion bPassCheck:"..tostring(bPassCheck))
	if not bPassCheck then
		return 
	end
	
	DownLoadNewVersion(tForceUpdate, function(strRealPath) 
		if not IsRealString(strRealPath) then
			return
		end
		
		SaveCommonUpdateUTC()
		local strCmd = " /write /silent /run"
		tipUtil:ShellExecute(0, "open", strRealPath, strCmd, 0, "SW_HIDE")
	end)
end


function FixStartConfig(tServerConfig)
	local tStartConfig = tServerConfig["tStartConfig"]
	if type(tStartConfig) ~= "table" then
		return
	end
	
	local strStartCfgPath = GetCfgPathWithName("startcfg.ini")
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


function CheckServerRuleFile(tServerConfig)
	local tServerData = FetchValueByPath(tServerConfig, {"tServerData"}) or {}
	
	local strServerVideoURL = FetchValueByPath(tServerData, {"tServerDataW", "strURL"})
	local strServerVideoMD5 = FetchValueByPath(tServerData, {"tServerDataW", "strMD5"})
	local strServerWebURL = FetchValueByPath(tServerData, {"tServerDataV", "strURL"})
	local strServerWebMD5 = FetchValueByPath(tServerData, {"tServerDataV", "strMD5"})
	if not IsRealString(strServerVideoURL) or not IsRealString(strServerWebURL) 
	   or not IsRealString(strServerVideoMD5) or not IsRealString(strServerWebMD5) then
		TipLog("[CheckServerRuleFile] get server rule info failed , start tipmain ")
		TipMain()
		return
	end
	
	local strVideoSavePath = GetCfgPathWithName("ServerDataV.dat")
	local strWebSavePath = GetCfgPathWithName("ServerDataW.dat")
	if not IsRealString(strVideoSavePath) or not tipUtil:QueryFileExists(strVideoSavePath) then
		strVideoSavePath = GetCfgPathWithName("DataV.dat")
	end
	if not IsRealString(strWebSavePath) or not tipUtil:QueryFileExists(strWebSavePath) then
		strWebSavePath = GetCfgPathWithName("DataW.dat")
	end
	
	local strDataVMD5 = tipUtil:GetMD5Value(strVideoSavePath)
	local strDataWMD5 = tipUtil:GetMD5Value(strWebSavePath)
	local tDownRuleList = {}
	
	if tostring(strDataVMD5) ~= strServerVideoMD5 then
		local nIndex = #tDownRuleList+1
		tDownRuleList[nIndex] = {}
		tDownRuleList[nIndex]["strURL"] = strServerVideoURL
		tDownRuleList[nIndex]["strPath"] = GetCfgPathWithName("ServerDataV.dat")
	end
	
	if tostring(strDataWMD5) ~= strServerWebMD5 then
		local nIndex = #tDownRuleList+1
		tDownRuleList[nIndex] = {}
		tDownRuleList[nIndex]["strURL"] = strServerWebURL
		tDownRuleList[nIndex]["strPath"] = GetCfgPathWithName("ServerDataW.dat")
	end
	
	DownLoadServerRule(tDownRuleList)
end


function AnalyzeServerConfig(nDownServer, strServerPath)
	if nDownServer ~= 0 or not tipUtil:QueryFileExists(tostring(strServerPath)) then
		TipLog("[AnalyzeServerConfig] Download server config failed , start tipmain ")
		TipMain()
		return	
	end
	
	local tServerConfig = LoadTableFromFile(strServerPath) or {}
	TryForceUpdate(tServerConfig)
	FixStartConfig(tServerConfig)
	CheckServerRuleFile(tServerConfig)
end


function TryShowNonSysBubble()
	local bRet, strSource = GetCommandStrValue("/sstartfrom")
	if string.lower(tostring(strSource)) == "service" 
		or string.lower(tostring(strSource)) == "bho" then
		ShowPopupWndByName("TipBubbleWnd.Instance", true)
	end
end


function TipMain()
	local bSucc = SendFileDataToFilterThread()
	if not bSucc then
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd(2)
		return
	end
	
	local bSucc = InitGSFilter()
	if not bSucc then
		return
	end 
	
	CreateMainTipWnd()
	CreatePopupTipWnd()
	SaveConfigInTimer()
	
	TryShowNonSysBubble()
end


function PreTipMain() 
	gnLastReportRunTmUTC = tipUtil:GetCurrentUTCTime()
	
	RegisterFunctionObject()
	
	StartRunCountTimer()
	SendGSStartReport()
	SendStartupReport(false)
	
	ModifyOSConfig()	
	
	local bSuccess = ReadAllConfigInfo()	
	if not bSuccess then
		MessageBox(tostring("文件被损坏，请重新安装"))
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd(1)
		return
	end

	DownLoadServerConfig(AnalyzeServerConfig)
end

PreTipMain()
