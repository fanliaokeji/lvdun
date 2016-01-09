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
	tFunctionHelper.SetNotifyIconState()
	--SetBkgStyle(objRootCtrl)
end

---事件---

function OnClickSwitchFilter(self)
	local objRootCtrl = self:GetOwnerControl()
	objRootCtrl:ChangeSwitchFilter()
	
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

function OnMouseMoveSwitch(self)
	if g_bFilterOpen then
		self:SetTextureID("GreenWall.SwitchFilter.Open.Hover")
	else
		self:SetTextureID("GreenWall.SwitchFilter.Close.Hover")
	end
end

function OnMouseLeaveSwitch(self)
	if g_bFilterOpen then
		self:SetTextureID("GreenWall.SwitchFilter.Open.Normal")
	else
		self:SetTextureID("GreenWall.SwitchFilter.Close.Normal")
	end
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


---------------------------
function OnInitCountElem(self)
	--
end


function SetElemCount(self, nCount)
	local objLayout1 = self:GetControlObject("AdvCountElem.Number1.Text")
	local objLayout2 = self:GetControlObject("AdvCountElem.Number2.Text")
	if nil == objLayout1 or nil == objLayout2 then
		return
	end

	local objLayoutList = {objLayout1, objLayout2}
	local attr = self:GetAttribute()
	local nCurShowNumber = attr.nCurShowNumber or 1
	local nHideNumber = math.mod(nCurShowNumber, 2) + 1
	
	local objCurShow = objLayoutList[nCurShowNumber]
	local objCurHide = objLayoutList[nHideNumber]
	local nCurCount = objCurShow:GetText()
	if tostring(nCurCount) == tostring(nCount) then
		return
	end
	
	objCurHide:SetText(tostring(nCount))
	local objImage = objCurHide:GetChildByIndex(0)
	local strID = "Number_"..tostring(nCount)
	objImage:SetResID(strID)		
	
	ShowAnimate(objCurShow, objCurHide)
	attr.nCurShowNumber = nHideNumber
end


function ShowAnimate(objCurShow, objCurHide)
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local aniT = templateMananger:GetTemplate("advcount.animation","AnimationTemplate")
	local aniShow = aniT:CreateInstance(nil, "AniShow")
	local aniHide = aniT:CreateInstance(nil, "AniHide")
	aniShow:BindObj(objCurShow)
	aniHide:BindObj(objCurHide)
	
	local objParent = objCurShow:GetParent()
	local nL, nT, nR, nB = objParent:GetObjPos()
	local nWidth = nR - nL
	local nHeight = nB - nT
	
	objCurHide:SetObjPos(nL, nB, nR, nB+nHeight)
	
	aniShow:SetKeyFramePos(nL, nT, nL, nT-nHeight) 
	aniHide:SetKeyFramePos(nL, nB, nL, nT) 
	
	local objtree = objCurShow:GetOwner()
	objtree:AddAnimation(aniShow)
	objtree:AddAnimation(aniHide)
	
	aniShow:Resume()
	aniHide:Resume()
end


function UpdateAdvShow(objRootCtrl)
	local attr = objRootCtrl:GetAttribute()
	local bHasInit = attr.bHasInit

	if not bHasInit then
		TipLog("[UpdateAdvShow] has not init")
		return false
	end

	local nTempAdvCnt = g_nFilterCountOneDay
	TryShowBubble(nTempAdvCnt)
	
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


function TryShowBubble(nAdvCnt)
	local tSpecialCount = {200, 500}

	for nIndex, nSpecialCount in ipairs(tSpecialCount) do
		if nAdvCnt == nSpecialCount then
			local strText = "绿盾广告管家\r\n今日累计过滤广告："..tostring(nSpecialCount).."次"
			tFunctionHelper.PopupNotifyIconTip(strText, true)
		end
	end
end


function Inner_ChangeSwitchFilter(objFilterSwitch)
	if objFilterSwitch == nil or type(objFilterSwitch.SetTextureID) ~= "function" then
		return
	end

	if g_bFilterOpen then
		objFilterSwitch:SetTextureID("GreenWall.SwitchFilter.Close.Normal")
		g_bFilterOpen = false
		tFunctionHelper.SwitchGSFilter(false)
	else
		objFilterSwitch:SetTextureID("GreenWall.SwitchFilter.Open.Normal")
		g_bFilterOpen = true
		tFunctionHelper.SwitchGSFilter(true)
	end
end


function LoadAdvCountCfg(objRootCtrl)
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig")
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


function SaveAdvCfgSepcifyKey(strKey, strValue, bSaveToFile)
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig")
	if type(tUserConfig) ~= "table" then
		return nil
	end
	
	tUserConfig[strKey] = strValue	
	if bSaveToFile then
		tFunctionHelper.SaveConfigToFileByKey("tUserConfig")
	end
end


function SaveAdvCountCfg()
	SaveAdvCfgSepcifyKey("nFilterCountOneDay", g_nFilterCountOneDay, false)
	SaveAdvCfgSepcifyKey("nFilterCountTotal", g_nFilterCountTotal, false)
end

function SaveAdvLastUTC(nLastUTC)
	SaveAdvCfgSepcifyKey("nLastClearUTC", nLastUTC, false)
end

function SaveAdvOpenState()
	SaveAdvCfgSepcifyKey("bFilterOpen", g_bFilterOpen, true)
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
	local nElemWidth  = 32
	local nElemheight = 43
	
	for nIndex=1, g_nElemCount do
		strKey = "AdvCountElem_"..tostring(nIndex)
		local objElem = objFactory:CreateUIObject(strKey, "AdvCountElem")
		if nil == objElem then
			return false
		end
		
		objFather:AddChild(objElem)
		--local nWhiteSpace = (nFatherWidth - g_nElemCount*nElemWidth)/2 + 1
		--local nNewLeft = nFatherWidth - (nWhiteSpace + nIndex*nElemWidth)
		local nNewLeft = (g_nElemCount - nIndex)* nElemWidth
		objElem:SetObjPos(nNewLeft, 0, nNewLeft+nElemWidth, nElemheight)
		
		g_tCountElemList[#g_tCountElemList+1] = objElem		
	end
	
	local attr = objRootCtrl:GetAttribute()
	attr.bHasInit = true
	
	LoadAdvCountCfg(objRootCtrl)
	BeginAnotherDayEvent(objRootCtrl)
	StartAutoUpdateTimer()
	SetBkgStyle(objRootCtrl)
end


function StartAutoUpdateTimer()
	CheckUpdateCond()

	local nTimeSpanInMs = 3600 * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		CheckUpdateCond()
	end, nTimeSpanInMs)
end


function ClearAdvCount(objRootCtrl)
	objRootCtrl:SetAdvCount(0)
	SaveAdvCountCfg()
end


function ReportAdvCountInfo()
	local nMinorVer = tFunctionHelper.GetGSMinorVer()
	
	local tStatInfo = {}
	tStatInfo.strEC = "MainPanel"
	tStatInfo.strEA = nMinorVer or ""
	tStatInfo.strEL = "countoneday"
	tStatInfo.strEV = g_nFilterCountOneDay
	SendReport(tStatInfo)
	
	tStatInfo.strEA = nMinorVer or ""
	tStatInfo.strEL = "counttotal"
	tStatInfo.strEV = g_nFilterCountTotal
	SendReport(tStatInfo)
end


function DoSthAnotherDay(objRootCtrl)
	TipLog("[DoSthAnotherDay] enter, lastutc:"..tostring(g_nLastClearUTC))
	
	if not tFunctionHelper.CheckTimeIsAnotherDay(g_nLastClearUTC) then
		return 
	end
	
	TipLog("[DoSthAnotherDay] begin, lastutc:"..tostring(g_nLastClearUTC))
	
	ReportAdvCountInfo()
	ClearAdvCount(objRootCtrl)
	InitVideoState()
	
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


--隔天将disable状态的video设置为可弹窗
function InitVideoState()
	TipLog("[InitVideoState] enter")
	local tBlackList = tFunctionHelper.GetSpecifyFilterTableFromMem("tBlackList") or {}
	for key, tBlackItem in pairs(tBlackList) do
		if type(tBlackItem) == "table" then
			local strDomain = tBlackItem["strDomain"] or ""
			local bState = tBlackItem["bState"]
			TipLog("[InitVideoState] strDomain :"..tostring(strDomain).." bState: "..tostring(bState))
			if not bState and tFunctionHelper.IsVideoDomain(strDomain) then	
				-- tFunctionHelper.EnableVideoDomain(strDomain, 0)  --取消隔天逻辑20141210
			end		
		end	
	end	
end


--每天自动检查更新
function CheckUpdateCond()
	local tUserCfg = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local bState = FetchValueByPath(tUserCfg, {"tConfig", "AutoUpdate", "bState"}) or false
	if not bState then
		return
	end

	if tFunctionHelper.CheckIsUpdating() then
		TipLog("[CheckUpdateCond] CheckIsUpdating failed,another thread is updating!")
		return
	end
	
	if not tFunctionHelper.CheckCommonUpdateTime(1) then
		TipLog("[CheckUpdateCond] CheckCommonUpdateTime failed")
		return
	end
	
	if not tFunctionHelper.CheckAutoUpdateTime(7) then
		TipLog("[CheckUpdateCond] CheckAutoUpdateTime failed")
		return
	end

	tFunctionHelper.DownLoadServerConfig(CheckAutoUpdate)
end


--自动检查更新
function CheckAutoUpdate(bRet, strCfgPath)
	if bRet ~= 0 or not tipUtil:QueryFileExists(strCfgPath) then
		return
	end
	
	local tServerConfig = tFunctionHelper.LoadTableFromFile(strCfgPath) or {}
	local tNewVersionInfo = tServerConfig["tNewVersionInfo"]
	if type(tNewVersionInfo) ~= "table" then
		return
	end	
	
	TryShowUpdateWnd(tNewVersionInfo)
end


function TryShowUpdateWnd(tNewVersionInfo)
	local strCurVersion = tFunctionHelper.GetGSVersion()
	local strNewVersion = tNewVersionInfo.strVersion		
	
	if not IsRealString(strCurVersion) or not IsRealString(strNewVersion)
		or not tFunctionHelper.CheckIsNewVersion(strNewVersion, strCurVersion) then
		return
	end
	
	DownLoadNewVersion(tNewVersionInfo, PopupUpdateWndForInstall)
end	


function DownLoadNewVersion(tNewVersionInfo, fnCallBack)
	local strPacketURL = tNewVersionInfo.strPacketURL
	local strMD5 = tNewVersionInfo.strMD5
	if not IsRealString(strPacketURL) then
		return
	end
	
	local strFileName = tFunctionHelper.GetFileSaveNameFromUrl(strPacketURL)
	if not string.find(strFileName, "%.exe$") then
		strFileName = strFileName..".exe"
	end
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)

	tFunctionHelper.SetIsUpdating(true)
	
	tFunctionHelper.DownLoadFileWithCheck(strPacketURL, strSavePath, strMD5
	, function(bRet, strRealPath)
		TipLog("[DownLoadNewVersion] strOpenLink:"..tostring(strPacketURL)
		        .."  bRet:"..tostring(bRet).."  strRealPath:"..tostring(strRealPath))
				
		tFunctionHelper.SetIsUpdating(false)
		
		if 0 == bRet then --下载安装包成功
			fnCallBack(strRealPath, tNewVersionInfo)
			return
		end
		
		if 1 == bRet then  --安装包已经存在
			fnCallBack(strSavePath, tNewVersionInfo)
			return
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
	
	tFunctionHelper.SaveCommonUpdateUTC()
	tFunctionHelper.SaveAutoUpdateUTC()
end

local imageCloseAni = nil
local imageOpenAni = nil
function SetBkgStyle(objRootCtrl)
	local objBkgClose = objRootCtrl:GetControlObject("ChildCtrl_AdvCount.MainWnd.FilterClose")
	
	local objBkgOpen = objRootCtrl:GetControlObject("ChildCtrl_AdvCount.MainWnd.FilterOpen")
	local imageOpen = objRootCtrl:GetControlObject("ChildCtrl_AdvCount.Ani.FilterOpen")
	
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local bFilterOpen = tUserConfig["bFilterOpen"]
	
	objBkgClose:SetVisible(false)
	objBkgClose:SetChildrenVisible(false)
	objBkgOpen:SetVisible(true)
	objBkgOpen:SetChildrenVisible(true)
	
	if not imageOpenAni then
		imageOpenAni = Helper.Ani:RunSeqFrameAni(imageOpen, "GreenWall.Seq_ani", nil, 3600, true)
	end
end

function OnClickUserHelper(self)
	local url = "http://www.lvdun123.com/help/index.html"
	tipUtil:ShellExecute(0, "open", url, 0, 0, "SW_SHOWNORMAL")
end

function OnClickRankingList(self)
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local nFilterCount = tonumber(tUserConfig["nFilterCountOneDay"]) or 0
	
	local url = "http://www.lvdun123.com/top/?count="
	url = url..tostring(nFilterCount)
	url = url.."&time="..tostring(os.time())
	
	tipUtil:ShellExecute(0, "open", url, 0, 0, "SW_SHOWNORMAL")
end

function OnClickFeedback(self)
	local url = "http://weibo.com/5458208915/Daws0gc9R"
	tipUtil:ShellExecute(0, "open", url, 0, 0, "SW_SHOWNORMAL")
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
