local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local g_nFilterCountOneDay = 0
local g_nFilterCountTotal = 0
local g_tCountElemList = {}
local g_nElemCount = 6
local g_nLastClearUTC = 0

local g_bFilterOpen = true

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
	local objFilterSwitch = objRootCtrl:GetControlObject("AdvCountCtrl.Switch.Filter")
	Inner_ChangeSwitchFilter(objFilterSwitch)
	SaveAdvOpenState()
	tFunHelper.SetNotifyIconState()
	tFunHelper.UpdateMainWndBkg()
end


---事件---
function OnInitAdvCount(self, bShow)
	local attr = self:GetAttribute()
	local bHasInit = attr.bHasInit

	if not bHasInit then
		InitAdvCount(self)
	elseif bShow then
		UpdateAdvShow(self)
	end
end


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


function LoadAdvCountCfg(objRootCtrl)
	local tUserConfig = tFunHelper.ReadConfigFromMemByKey("tUserConfig")
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
	local tUserConfig = tFunHelper.ReadConfigFromMemByKey("tUserConfig")
	if type(tUserConfig) ~= "table" then
		return nil
	end
	
	tUserConfig[strKey] = strValue	
	if bSaveToFile then
		tFunHelper.SaveConfigToFileByKey("tUserConfig")
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
	objFather = objRootCtrl:GetControlObject("AdvCountCtrl.MainWnd.Container")
	if objFather == nil then
		return nil
	end
	
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	if objFactory == nil then
		return nil
	end
	
	local nFatherLeft, nFatherTop, nFatherRight, nFatherBottom = objFather:GetObjPos(objElem)
	local nFatherWidth = nFatherRight - nFatherLeft
	local nElemWidth = 22
	
	for nIndex=1, g_nElemCount do
		strKey = "AdvCountElem_"..tostring(nIndex)
		local objElem = objFactory:CreateUIObject(strKey, "AdvCountElem")
		if nil == objElem then
			return false
		end
		
		local ResID = "Number.Bkg_"..tostring(nIndex)
		local objBkg = objElem:GetControlObject("AdvCountElem.Bkg")
		if objBkg then
			objBkg:SetTextureID(ResID)
		end
		
		objFather:AddChild(objElem)
		local nWhiteSpace = (nFatherWidth - g_nElemCount*nElemWidth)/2
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
	TipLog("[DoSthAnotherDay] enter, lastutc:"..tostring(g_nLastClearUTC))
	
	if not tFunHelper.CheckTimeIsAnotherDay(g_nLastClearUTC) then
		return 
	end
	
	TipLog("[DoSthAnotherDay] begin, lastutc:"..tostring(g_nLastClearUTC))
	
	ReportAdvCountInfo()
	ClearAdvCount(objRootCtrl)
	
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
	if type(tFunHelper.TipConvStatistic) == "function" then
		tFunHelper.TipConvStatistic(tStatInfo)
	end
end



function Inner_ChangeSwitchFilter(objFilterSwitch)
	if objFilterSwitch == nil or type(objFilterSwitch.SetTextureID) ~= "function" then
		return
	end

	if g_bFilterOpen then
		objFilterSwitch:SetTextureID("SwitchFilter.Close")
		g_bFilterOpen = false
		tFunHelper.SwitchADCFilter(false)
	else
		objFilterSwitch:SetTextureID("SwitchFilter.Open")
		g_bFilterOpen = true
		tFunHelper.SwitchADCFilter(true)
	end
end

----------------------------------

function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@AdvCountCtrl: " .. tostring(strLog))
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
