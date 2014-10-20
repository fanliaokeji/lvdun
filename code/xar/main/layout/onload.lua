local gTipInfoTab = {}
local gFilterConfigInfo = {}
local gVideoList = {}

local gStatCount = 0
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
}
-----------------

function RegisterFunctionObject(self)
	local function FailExitTipWnd(self, iExitCode)
		ExitTipWnd()
	end
	
	local function TipConvStatistic(tStat)
		local rdRandom = tipUtil:GetCurrentUTCTime()
		local tStatInfo = tStat or {}
		
		local strCID = GetPeerID()
		local strEC = tStatInfo.strEC or ""
		local strEA = tStatInfo.strEA or ""
		local strEL = tStatInfo.strEL or ""
		local strEV = tStatInfo.strEV or ""

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
	obj.GetFailExitCode = GetFailExitCode
	obj.TipConvStatistic = TipConvStatistic
	obj.SaveUserConfigToFile = SaveUserConfigToFile
	obj.GetUserConfigFromFile = GetUserConfigFromFile
	obj.SaveUserConfigToMem = SaveUserConfigToMem
	obj.GetUserConfigFromMem = GetUserConfigFromMem
	obj.GetSpecifyFilterTableFromMem = GetSpecifyFilterTableFromMem
	obj.SaveSpecifyFilterTableToMem = SaveSpecifyFilterTableToMem
	obj.SaveFilterConfigToFile = SaveFilterConfigToFile
	obj.SaveVideoListToFile = SaveVideoListToFile
	obj.GetVideoListFromMem = GetVideoListFromMem
	obj.SaveVideoListToMem = SaveVideoListToMem
	obj.ReportAndExit = ReportAndExit
	obj.ShowPopupWndByName = ShowPopupWndByName
	obj.GetCfgPathWithName = GetCfgPathWithName
	obj.LoadTableFromFile = LoadTableFromFile
	obj.ShowExitRemindWnd = ShowExitRemindWnd
	obj.RegQueryValue = RegQueryValue
	obj.RegSetValue = RegSetValue
	obj.GetGSVersion = GetGSVersion
	obj.CheckTimeIsAnotherDay = CheckTimeIsAnotherDay
	obj.IsDomainInWhiteList = IsDomainInWhiteList
	obj.GetVideoDomainState = GetVideoDomainState
	obj.EnableWhiteDomain = EnableWhiteDomain
	obj.EnableVideoDomain = EnableVideoDomain
	obj.PopupBubbleOneDay = PopupBubbleOneDay
	obj.NewAsynGetHttpFile = NewAsynGetHttpFile
	obj.DownLoadServerConfig = DownLoadServerConfig
	obj.CheckIsNewVersion = CheckIsNewVersion
	obj.IsVideoDomain = IsVideoDomain
	obj.GetFileSaveNameFromUrl = GetFileSaveNameFromUrl

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


function DownLoadServerConfig(fnCallBack)
	local tUserConfig = GetUserConfigFromMem() or {}
	
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
	
	local nTimeInMs = 30*1000
		
	NewAsynGetHttpFile(strConfigURL, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[DownLoadServerConfig] bRet:"..tostring(bRet)
				.." strRealPath:"..tostring(strRealPath))
				
		if 0 == bRet then
			fnCallBack(0, strSavePath)
		else
			fnCallBack(bRet)
		end		
	end, nTimeInMs)

end


function SaveAllConfig()
	if gbLoadCfgSucc then
		SaveUserConfigToFile()
		SaveFilterConfigToFile()
		SaveVideoListToFile()
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
		FunctionObj:FailExitTipWnd(7)
	end
	
	---初始化托盘
    if frameHostWnd and not g_tipNotifyIcon then
	    InitTrayTipWnd(frameHostWnd)
	end
end

function ShowMainTipWnd(objMainWnd)
	local tUserConfig = GetUserConfigFromMem() or {}
	local bHideMainPage = FetchValueByPath(tUserConfig, {"tConfig", "HideMainPage", "bState"})
	
	local bAutoStup = false
	local bRet, strSource = GetCommandStrValue("/embedding")
	if bRet then
		bAutoStup = true
	end
	
	if bHideMainPage or bAutoStup then
		objMainWnd:Show(0)
	else
		objMainWnd:Show(4)
	end
end


function ShowPopupWndByName(strWndName)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd(tostring(strWndName))
	if frameHostWnd == nil then
		TipLog("[ShowPopupWindow] GetHostWnd failed: "..tostring(strWndName))
		return
	end

	if not IsUserFullScreen() then
		frameHostWnd:SetTopMost(true)
	elseif type(tipUtil.GetForegroundProcessInfo) == "function" then
		local hFrontHandle, strPath = tipUtil:GetForegroundProcessInfo()
		if hFrontHandle ~= nil then
			frameHostWnd:BringWindowToBack(hFrontHandle)
		end
	end
	
	frameHostWnd:Show(4)
end


function ShowExitRemindWnd()
	ShowPopupWndByName("TipExitRemindWnd.Instance")
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
	
	SetNotifyIconText(tipNotifyIcon)
	tipNotifyIcon:Show()
	
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
			local strHostWndName = "GreenWallTipWnd.MainFrame"
			local objHostWnd = hostwndManager:GetHostWnd(strHostWndName)
			if not objHostWnd then
				CreateMainTipWnd()
			end
			objHostWnd:BringWindowToTop(true)
		end
		
		--mousemove
		if event3 == 512 then
			g_tipNotifyIcon:ShowNotifyIconTip(false)
			SetNotifyIconText(tipNotifyIcon)
		end
	end

	tipNotifyIcon:Attach(OnTrayEvent)
	g_tipNotifyIcon = tipNotifyIcon
end


function SetNotifyIconText(tipNotifyIcon)
	local tUserConfig = GetUserConfigFromMem() or {}
	local nFilterCount = tonumber(tUserConfig["nFilterCountOneDay"]) or 0
	local bFilterOpen = tUserConfig["bFilterOpen"] or false
	local strState = "正常过滤"
	if not bFilterOpen then
		strState = "停止过滤"
	end
	
	local strText = "绿盾广告管家\r\n状态："..strState.."\r\n今日累计屏蔽："..tostring(nFilterCount).."次"
    tipNotifyIcon:SetIcon(nil,strText)
end


function PopupBubbleOneDay()
	local tUserConfig = GetUserConfigFromMem() or {}
	local nLastBubbleUTC = tonumber(tUserConfig["nLastBubbleUTC"]) 
	local bBubbleRemind = FetchValueByPath(tUserConfig, {"tConfig", "BubbleRemind", "bState"})
	
	if not bBubbleRemind then
		return
	end

	if not IsNilString(nLastBubbleUTC) and not CheckTimeIsAnotherDay(nLastBubbleUTC) then
		return
	end
	
	if g_tipNotifyIcon then
		g_tipNotifyIcon:ShowNotifyIconTip(true, "绿盾广告管家\r\n已为您屏蔽骚扰广告")
		tUserConfig["nLastBubbleUTC"] = tipUtil:GetCurrentUTCTime()
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


function SaveUserConfigToMem(tNewConfig)
	gTipInfoTab = tNewConfig
end

function GetUserConfigFromMem()
	return gTipInfoTab
end

function GetVideoListFromMem()
	return gVideoList
end

function SaveVideoListToMem(tNewVideoList)
	gVideoList = tNewVideoList
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
	if not IsRealString(strTableName) then
		return gFilterConfigInfo
	end

	return gFilterConfigInfo[strTableName]
end

function SaveSpecifyFilterTableToMem(tNewTable, strTableName)
	if not IsRealString(strTableName) then
		gFilterConfigInfo = tNewTable
	else
		gFilterConfigInfo[strTableName] = tNewTable
	end
end

function SaveUserConfigToFile()
	local strUserCfgPath = GetCfgPathWithName("UserConfig.dat")
	if IsRealString(strUserCfgPath) then
		tipUtil:SaveLuaTableToLuaFile(gTipInfoTab, strUserCfgPath)
	end
end

function GetUserConfigFromFile()
	local strUserCfgPath = GetCfgPathWithName("UserConfig.dat")
	local infoTable = LoadTableFromFile(strUserCfgPath)
	return infoTable
end


function GetFilterConfigFromFile()
	local strFilterCfgPath = GetCfgPathWithName("FilterConfig.dat")
	local infoTable = LoadTableFromFile(strFilterCfgPath)
	return infoTable
end

function SaveFilterConfigToFile()
	local strFilterCfgPath = GetCfgPathWithName("FilterConfig.dat")
	if IsRealString(strFilterCfgPath) then
		tipUtil:SaveLuaTableToLuaFile(gFilterConfigInfo, strFilterCfgPath)
	end
end

function GetVideoListFromFile()
	local strVideoListPath = GetCfgPathWithName("VideoList.dat")
	local tVideoList = LoadTableFromFile(strVideoListPath)
	return tVideoList
end

function SaveVideoListToFile()
	local strVideoListPath = GetCfgPathWithName("VideoList.dat")
	if IsRealString(strVideoListPath) then
		tipUtil:SaveLuaTableToLuaFile(gVideoList, strVideoListPath)
	end
end


function ReadAllConfigInfo()	
	local infoTable = GetUserConfigFromFile()
	if type(infoTable) ~= "table" then
		TipLog("[ReadAllConfigInfo] GetUserConfigFromFile failed! ")
		return false
	end
		
	local tFilterTable = GetFilterConfigFromFile()
	if type(tFilterTable) ~= "table" then
		TipLog("[ReadAllConfigInfo] GetFilterConfigFromFile failed! ")
		return false
	end
	
	local tVideoListTable = GetVideoListFromFile()
	if type(tVideoListTable) ~= "table" then
		TipLog("[ReadAllConfigInfo] GetVideoListFromFile failed! ")
		return false
	end
			
	gTipInfoTab = infoTable
	gFilterConfigInfo = tFilterTable
	gVideoList = tVideoListTable
	
	gbLoadCfgSucc = true	
	TipLog("[ReadAllConfigInfo] success!")
	return true
end


function IsVideoDomain(strDomain)
	if not IsRealString(strDomain) then
		return false
	end
	
	local tVideoList = GetVideoListFromMem() or {}
	if tVideoList[strDomain] then
		return true
	else
		return false
	end
end


function SendFileDataToFilterThread()
	local bSucc = SendLazyListToFilterThread()
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


function SendLazyListToFilterThread()
	local strLazyListPath = GetCfgPathWithName("data.dat")
	if not IsRealString(strLazyListPath) or not tipUtil:QueryFileExists(strLazyListPath) then
		return false
	end

	local strAESString = tipUtil:ReadFileToString(strLazyListPath)
	if not strAESString then
		TipLog("[SendLazyListToFilterThread] ReadFileToString failed : "..tostring(strLazyListPath))
		return false
	end
	
	local strKey = "fvSem9Rt6wvhxmzs"
	local strDecString = tipUtil:DecryptFileAES(strLazyListPath, strKey)
	if type(strDecString) ~= "string" then
		TipLog("[SendLazyListToFilterThread] DecryptFileAES failed : "..tostring(strLazyListPath))
		return false
	end
	
	local strTmpDir = tipUtil:GetSystemTempPath()
	if not tipUtil:QueryFileExists(strTmpDir) then
		TipLog("[SendLazyListToFilterThread] GetSystemTempPath failed strTmpDir: "..tostring(strTmpDir))
		return false
	end
	
	local strCfgName = tipUtil:GetTmpFileName() or "data.dat"
	local strCfgPath = tipUtil:PathCombine(strTmpDir, strCfgName)
	tipUtil:WriteStringToFile(strCfgPath, strDecString)
	
	local bSucc = tipUtil:LoadConfig(strCfgPath) 
	if not bSucc then
		TipLog("[SendLazyListToFilterThread] LoadConfig failed strCfgPath: "..tostring(strCfgPath))
		return false
	end
	
	tipUtil:DeletePathFile(strCfgPath)
	return true
end


function SendVideoListToFilterThread()
	local tVideoList = GetVideoListFromMem() or {}
	
	for strDomain, tVideoElem in pairs(tVideoList) do
		if IsRealString(strDomain) and type(tVideoElem) == "table" then
			local nLastPopupUTC = tVideoElem["nLastPopupUTC"]
			
			if not CheckWhiteList(strDomain) then
				CheckVideoList(strDomain, nLastPopupUTC)
			end
		end
	end

	return true
end


function CheckWhiteList(strDomain) 
	if IsDomainInWhiteList(strDomain) then
		AddVideoDomain(strDomain, 2)
		return true
	end
	
	return false
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
	if not IsRealString(strDomain) or type(nState) ~= "number" then
		return
	end

	tipUtil:UpdateVideoHost(strDomain, nState)
	TipLog("[UpdateVideoHost] strDomain: "..tostring(strDomain).." nState: "..tostring(nState))
end


function AddVideoDomain(strDomain, nState)
	if not IsRealString(strDomain) or type(nState) ~= "number" then
		return
	end

	tipUtil:AddVideoHost(strDomain, nState)
	TipLog("[AddVideoDomain] strDomain: "..tostring(strDomain).." nState: "..tostring(nState))
end


function EnableWhiteDomain(strDomain, bSetWite)
	if not IsRealString(strDomain) or type(bSetWite) ~= "boolean" then
		return
	end

	tipUtil:UpdateWhiteHost(strDomain, bSetWite)
	TipLog("[UpdateWhiteHost] strDomain: "..tostring(strDomain).." nState: "..tostring(bSetWite))
end


function AddWhiteDomain(strDomain)
	if not IsRealString(strDomain) then
		return
	end

	tipUtil:AddWhiteHost(strDomain)
	TipLog("[AddWhiteDomain] strDomain: "..tostring(strDomain))
end


function SendWhiteListToFilterThread()
	local tWhiteList = GetSpecifyFilterTableFromMem("tWhiteList") or {}
		
	for key, tWhiteElem in pairs(tWhiteList) do
		local strWhiteDomain = tWhiteElem["strDomain"]
		local bStateOpen = tWhiteElem["bState"]
		if IsRealString(strWhiteDomain) and bStateOpen then
			AddWhiteDomain(strWhiteDomain)
		end
	end
	
	return true
end


function IsDomainInWhiteList(strDomain)
	if not IsRealString(strDomain) then
		return false
	end

	local tWhiteList = GetSpecifyFilterTableFromMem("tWhiteList") or {}
	for key, tWhiteElem in pairs(tWhiteList) do
		local strWhiteDomain = tWhiteElem["strDomain"]
		local bStateOpen = tWhiteElem["bState"]
		if IsRealString(strWhiteDomain) and bStateOpen
			and string.find(strWhiteDomain, strDomain) then
			
			return true
		end
	end
	return false
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
		local bRet = rootctrl:SetTipData(gTipInfoTab)			
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
			FunctionObj:FailExitTipWnd()
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


function GetCommandStrValue(strKey)
	local bRet, strValue = false, nil
	local cmdString = tipUtil:GetCommandLine()

	if string.find(cmdString, strKey .. " ") then
		local cmdList = tipUtil:CommandLineToList(cmdString)
		if cmdList ~= nil then	
			for i = 1, #cmdList, 2 do
				local strTmp = tostring(cmdList[i])
				if strTmp == strKey then
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
	tStatInfo.strEC = "startup"
	tStatInfo.strEA = strSource or ""
	tStatInfo.strEV = GetGSMinorVer() or ""
	
	if bShowWnd then
		tStatInfo.strEL = 0   --进入上报
	else
		tStatInfo.strEL = 1   --展示上报
	end
		
	local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
	FunctionObj.TipConvStatistic(tStatInfo)
end


function ReportAndExit()
	DestroyMainWnd()
	DestroyPopupWnd()
	
	local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
	local tStatInfo = {}
			
	local iLastTime = 0	--失败认为展示时长为0
	local nCurTime = tipUtil:GetCurrentUTCTime()
	local fnGetTipStartTime = XLGetGlobal("GreenWall.GetTipStartTime")
	if type(fnGetTipStartTime) == "function" then
		local iTimeStart = fnGetTipStartTime()
		if type(iTimeStart) == "number" then
			iLastTime = nCurTime - iTimeStart
		end
	end

	tStatInfo.strEC = "exit"
	tStatInfo.strEV = iLastTime
	tStatInfo.Exit = true
		
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


function ModifyOSConfig()
	SetIERegValue("PMIL")
end

function RestoreOSConfig()
	SetIERegValue("PMEM")
end


function TipMain() 
	RegisterFunctionObject()
	SendStartupReport(false)
	
	ModifyOSConfig()
	
	local bSuccess = ReadAllConfigInfo()	
	if not bSuccess then
		MessageBox(tostring("文件被损坏，请重新安装"))
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd()
		return
	end

	local bSucc = SendFileDataToFilterThread()
	if not bSucc then
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd()
		return
	end
	
	CreatePopupTipWnd()
	CreateMainTipWnd()
	SaveConfigInTimer()
	
	SendStartupReport(true)
end


TipMain()
