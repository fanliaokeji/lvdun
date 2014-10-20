local tipUtil = XLGetObject("GS.Util")
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
local g_nFilterCountOneDay = 0
local g_nFilterCountTotal = 0
local g_tCountElemList = {}
local g_nElemCount = 6
local g_bFilterOpen = true
local g_nLastClearUTC = 0

---方法---
function SetAdvCount(self, nCount)
	if type(nCount) ~= "number" then
		return false
	end
	
	g_nFilterCountOneDay = nCount
	UpdateAdvShow(self)
end

function AddAdvCount(self, nAddCount)
	local nAdd = nAddCount or 1
	g_nFilterCountOneDay = g_nFilterCountOneDay + nAdd
	g_nFilterCountTotal = g_nFilterCountTotal + nAdd
	UpdateAdvShow(self)
end

function ChangeSwitchFilter(objRootCtrl)
	local objFilterSwitch = objRootCtrl:GetControlObject("ChildCtrl_AdvCount.Switch.Filter")
	Inner_ChangeSwitchFilter(objFilterSwitch)
	SaveAdvOpenState()
end

---事件---
function OnClickSwitchFilter(self)
	Inner_ChangeSwitchFilter(self)
	SaveAdvOpenState()
	
	local tStatInfo = {}
	tStatInfo.strEC = "MainPanel"
	tStatInfo.strEA = "filteropen"
	
	if g_bFilterOpen then
		tStatInfo.strEL = "1"
	else
		tStatInfo.strEL = "2"
	end
	
	SendReport(tStatInfo)
end


function OnShowPanel(self, bShow)
	local attr = self:GetAttribute()
	local bHasInit = attr.bHasInit

	if not bHasInit then
		InitAdvCount(self)
	elseif bShow then
		UpdateAdvShow(self)
	end
end

function OnDestroy(self)
	--SaveAdvCountCfg()
end


---------------------------
function SetElemCount(self, nCount)
	local objImg = self:GetControlObject("AdvCountElem.Number")
	if nil == objImg then
		return
	end
	
	local strID = "Number_"..tostring(nCount)
	objImg:SetResID(strID)		
end

function UpdateAdvShow(objRootCtrl)
	local attr = objRootCtrl:GetAttribute()
	local bHasInit = attr.bHasInit

	if not bHasInit then
		TipLog("[UpdateAdvShow] has not init")
		return false
	end

	local nTempAdvCnt = g_nFilterCountOneDay
	for i=1, g_nElemCount do
		local objElem = g_tCountElemList[i]
		if objElem == nil then
			return false
		end
		
		local nAdvCntToShow = math.mod(nTempAdvCnt, 10)
		objElem:SetElemCount(nAdvCntToShow)
		nTempAdvCnt = math.floor(nTempAdvCnt/10)
	end
	
	SaveAdvCountCfg()
end


function Inner_ChangeSwitchFilter(objFilterSwitch)
	if objFilterSwitch == nil or type(objFilterSwitch.SetTextureID) ~= "function" then
		return
	end

	if g_bFilterOpen then
		objFilterSwitch:SetTextureID("GreenWall.SwitchFilter.Close")
		g_bFilterOpen = false
		tipUtil:GSFilter(false)
	else
		objFilterSwitch:SetTextureID("GreenWall.SwitchFilter.Open")
		g_bFilterOpen = true
		tipUtil:GSFilter(true)
	end
end

function LoadAdvCountCfg(objRootCtrl)
	if type(tFunctionHelper.GetUserConfigFromMem) ~= "function" then
		return
	end

	local tUserConfig = tFunctionHelper.GetUserConfigFromMem()
	if type(tUserConfig) ~= "table" then
		return nil
	end
	
	local nFilterCountTotal = tonumber(tUserConfig["nFilterCountTotal"]) or 0
	g_nFilterCountTotal = nFilterCountTotal
	
	local nFilterCount = tonumber(tUserConfig["nFilterCountOneDay"]) or 0
	SetAdvCount(objRootCtrl, nFilterCount)

	local bFilterOpen = tUserConfig["bFilterOpen"]
	if type(bFilterOpen) == "boolean" then
		g_bFilterOpen = not bFilterOpen  --注意这里用not，在Inner_ChangeSwitchFilter时会还原
		objRootCtrl:ChangeSwitchFilter()
	end
	
	g_nLastClearUTC = tUserConfig["nLastClearUTC"]
end


function SaveAdvCfgSepcifyKey(strKey, strValue)
	if type(tFunctionHelper.SaveUserConfigToMem) ~= "function" then
		return
	end

	if type(tFunctionHelper.GetUserConfigFromMem) ~= "function" then
		return
	end

	local tUserConfig = tFunctionHelper.GetUserConfigFromMem()
	if type(tUserConfig) ~= "table" then
		return nil
	end
	
	tUserConfig[strKey] = strValue	
	tFunctionHelper.SaveUserConfigToMem(tUserConfig)
end


function SaveAdvCountCfg()
	SaveAdvCfgSepcifyKey("nFilterCountOneDay", g_nFilterCountOneDay)
	SaveAdvCfgSepcifyKey("nFilterCountTotal", g_nFilterCountTotal)
end

function SaveAdvLastUTC(nLastUTC)
	SaveAdvCfgSepcifyKey("nLastClearUTC", nLastUTC)
end

function SaveAdvOpenState()
	SaveAdvCfgSepcifyKey("bFilterOpen", g_bFilterOpen)
end


function InitAdvCount(objRootCtrl)
	objFather = objRootCtrl:GetControlObject("ChildCtrl_AdvCount.MainWnd.Container")
	if objFather == nil then
		return nil
	end
	
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	if objFactory == nil then
		return nil
	end
	
	local nFatherLeft, nFatherTop, nFatherRight, nFatherBottom = objFather:GetObjPos(objElem)
	local nFatherWidth = nFatherRight - nFatherLeft
	local nElemWidth = 31
	
	for nIndex=1, g_nElemCount do
		strKey = "AdvCountElem_"..tostring(nIndex)
		local objElem = objFactory:CreateUIObject(strKey, "AdvCountElem")
		if nil == objElem then
			return false
		end
		
		objFather:AddChild(objElem)
		local nWhiteSpace = (nFatherWidth - g_nElemCount*nElemWidth)/2 + 1
		local nNewLeft = nFatherWidth - (nWhiteSpace + nIndex*nElemWidth)
		objElem:SetObjPos(nNewLeft, 0, nNewLeft+nElemWidth, "father.height")
		
		g_tCountElemList[#g_tCountElemList+1] = objElem		
	end
	
	local attr = objRootCtrl:GetAttribute()
	attr.bHasInit = true
	
	LoadAdvCountCfg(objRootCtrl)
	BeginAnotherDayEvent(objRootCtrl)
end


function ClearAdvCount(objRootCtrl)
	objRootCtrl:SetAdvCount(0)
	SaveAdvCountCfg()
end


function ReportAdvCountInfo()
	local tStatInfo = {}
	tStatInfo.strEC = "MainPanel"
	tStatInfo.strEA = "countoneday"
	tStatInfo.strEL = g_nFilterCountOneDay
	SendReport(tStatInfo)
	
	tStatInfo.strEA = "counttotal"
	tStatInfo.strEL = g_nFilterCountTotal
	SendReport(tStatInfo)
end


function DoSthAnotherDay(objRootCtrl)
	if not tFunctionHelper.CheckTimeIsAnotherDay(g_nLastClearUTC) then
		return 
	end
	TipLog("[DoSthAnotherDay] begin, lastutc:"..tostring(g_nLastClearUTC))
	
	ReportAdvCountInfo()
	ClearAdvCount(objRootCtrl)
	CheckUpdateVersion()
	
	local nCurUTC = tipUtil:GetCurrentUTCTime()
	SaveAdvLastUTC(nCurUTC)
	g_nLastClearUTC = nCurUTC
end


function BeginAnotherDayEvent(objRootCtrl)
	local nTimeSpanInMs = 1800 * 1000
	
	DoSthAnotherDay(objRootCtrl)	
	
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		
			DoSthAnotherDay(objRootCtrl)	
		
		end, nTimeSpanInMs)
end


function SendReport(tStatInfo)
	if type(tFunctionHelper.TipConvStatistic) == "function" then
		tFunctionHelper.TipConvStatistic(tStatInfo)
	end
end


--每天自动检查更新
function CheckUpdateVersion()
	local tUserCfg = tFunctionHelper.GetUserConfigFromMem() or {}
	local bState = FetchValueByPath(tUserCfg, {"tConfig", "AutoUpdate", "bState"}) or false
	if not bState then
		return
	end
	
	function TryShowUpdateWnd(bRet, strCfgPath)
		if bRet ~= 0 or not tipUtil:QueryFileExists(strCfgPath) then
			return
		end
		
		local tServerConfig = tFunctionHelper.LoadTableFromFile(strCfgPath) or {}
		local tNewVersionInfo = tServerConfig["tNewVersionInfo"]
		if(type(tNewVersionInfo)) ~= "table" then
			return 
		end
		
		local strCurVersion = tFunctionHelper.GetGSVersion()
		local strNewVersion = tNewVersionInfo.strVersion		
		
		if not IsRealString(strCurVersion) or not IsRealString(strNewVersion)
			or not tFunctionHelper.CheckIsNewVersion(strNewVersion, strCurVersion) then
			return
		end
		
		DownLoadNewVersion(tNewVersionInfo)
		-- tFunctionHelper.ShowPopupWndByName("TipUpdateWnd.Instance")
	end	
	
	tFunctionHelper.DownLoadServerConfig(TryShowUpdateWnd)
end


function DownLoadNewVersion(tNewVersionInfo)
	local strPacketURL = tNewVersionInfo.strPacketURL
	if not IsRealString(strPacketURL) then
		return
	end
	
	local strFileName = tFunctionHelper.GetFileSaveNameFromUrl(strPacketURL)
	if not string.find(strFileName, "%.exe$") then
		strFileName = strFileName..".exe"
	end
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)

	tFunctionHelper.NewAsynGetHttpFile(strPacketURL, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[DownLoadNewVersion] strOpenLink:"..tostring(strPacketURL)
		        .."  bRet:"..tostring(bRet).."  strRealPath:"..tostring(strRealPath))
				
		if 0 == bRet then
			PopupUpdateWndForInstall(strRealPath, tNewVersionInfo)
		end
	end)	
end

function PopupUpdateWndForInstall(strRealPath, tNewVersionInfo)
	if not IsRealString(strRealPath) then
		return
	end

	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local strWndName = "TipUpdateWnd.Instance"
	local objPopupWnd = hostwndManager:GetHostWnd(strWndName)
	if objPopupWnd == nil then
		TipLog("[PopupUpdateWndForInstall] GetHostWnd failed: "..tostring(strWndName))
		return
	end

	local objTree = objPopupWnd:GetBindUIObjectTree()
	local objRootCtrl = objTree:GetUIObject("root.layout")
	if objRootCtrl then
		objRootCtrl:ShowInstallPanel(strRealPath, tNewVersionInfo)
	end
end


----------------------------------

function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@ChildCtrl_AdvCount: " .. tostring(strLog))
	end
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
