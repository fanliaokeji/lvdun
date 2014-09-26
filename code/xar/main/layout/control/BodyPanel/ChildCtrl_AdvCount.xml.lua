local tipUtil = XLGetObject("GS.Util")
local g_nAdvCount = 0
local g_tCountElemList = {}
local g_nElemCount = 6
local g_bHasInit = false
local g_bFilterOpen = true
local g_nLastBeginFilterUTC = 0

---方法---
function SetAdvCount(self, nCount)
	if type(nCount) ~= "number" then
		return false
	end
	
	g_nAdvCount = nCount
	UpdateAdvShow(self)
end

function AddAdvCount(self, nAddCount)
	local nAdd = nAddCount or 1
	g_nAdvCount = g_nAdvCount + nAdd
	UpdateAdvShow(self)
end


---事件---
function OnClickSwitchFilter(self)
	ChangeSwitchFilter(self)
	SaveAdvOpenState()
end


function OnShowPanel(self, bShow)
	if not g_bHasInit then
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
	if not g_bHasInit then
		TipLog("[UpdateAdvShow] has not init")
		return false
	end

	local nTempAdvCnt = g_nAdvCount
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


function ChangeSwitchFilter(objFilterSwitch)
	if objFilterSwitch == nil or type(objFilterSwitch.SetTextureID) ~= "function" then
		return
	end

	if g_bFilterOpen then
		objFilterSwitch:SetTextureID("GreenWall.SwitchFilter.Close")
		g_bFilterOpen = false
		tipUtil:FGSFilter(false)
	else
		objFilterSwitch:SetTextureID("GreenWall.SwitchFilter.Open")
		g_bFilterOpen = true
		tipUtil:FGSFilter(true)
	end
	
end

function LoadAdvCountCfg(objRootCtrl)
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	if type(tFunctionHelper.GetUserConfigFromMem) ~= "function" then
		return
	end

	local tUserConfig = tFunctionHelper.GetUserConfigFromMem()
	if type(tUserConfig) ~= "table" then
		return nil
	end
	
	local nFilterCount = tonumber(tUserConfig["nFilterCount"])
	if nFilterCount ~= nil then
		SetAdvCount(objRootCtrl, nFilterCount)
	end

	local bFilterOpen = tUserConfig["bFilterOpen"]
	if type(bFilterOpen) == "boolean" then
		g_bFilterOpen = not bFilterOpen  --注意这里用not，在ChangeSwitchFilter时会还原
		ChangeSwitchFilter(objRootCtrl:GetControlObject("ChildCtrl_AdvCount.Switch.Filter"))
	end
	
	g_nLastBeginFilterUTC = tUserConfig["nLastBeginFilterUTC"]
end


function SaveAdvCfgSepcifyKey(strKey, strValue)
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
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
	SaveAdvCfgSepcifyKey("nFilterCount", g_nAdvCount)
end

function SaveAdvLastUTC(nLastUTC)
	SaveAdvCfgSepcifyKey("nLastBeginFilterUTC", nLastUTC)
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
	
	g_bHasInit = true
	
	LoadAdvCountCfg(objRootCtrl)
	ClearCountInRightTime(objRootCtrl)
end


function ClearAdvCount(objRootCtrl)
	local nCheckSpanInSec = 24*3600
	local nLastUTC = tonumber(g_nLastBeginFilterUTC)
	local nCurUTC = tipUtil:GetCurrentUTCTime()
	if nLastUTC == nil or nCurUTC == nil then
		return
	end
	
	TipLog("[ClearAdvCount] nCurUTC: "..tostring(nCurUTC).."  nLastUTC:"..tostring(nLastUTC))
	local nDiff = nCurUTC-nLastUTC
	if nDiff > nCheckSpanInSec then
		objRootCtrl:SetAdvCount(0)
		SaveAdvCountCfg()
		SaveAdvLastUTC(nCurUTC)
		g_nLastBeginFilterUTC = nCurUTC
	end
end


function ClearCountInRightTime(objRootCtrl)
	local nTimeSpanInMs = 1800 * 1000
	
	ClearAdvCount(objRootCtrl)
	
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		ClearAdvCount(objRootCtrl)
	end, nTimeSpanInMs)
end



function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@ChildCtrl_AdvCount: " .. tostring(strLog))
	end
end
