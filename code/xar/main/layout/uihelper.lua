local tipUtil = XLGetObject("GS.Util")
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")


function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@uihelper: " .. tostring(strLog))
	end
end


function GetMainWndInst()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local objMainWnd = hostwndManager:GetHostWnd("GreenWallTipWnd.MainFrame")
	return objMainWnd
end


function GetMainCtrlChildObj(strObjName)
	local objMainWnd = GetMainWndInst()
	if not objMainWnd then
		return nil
	end
	
	local objTree = objMainWnd:GetBindUIObjectTree()
	
	if not objTree then
		TipLog("[GetMainCtrlChildObj] get main wnd or tree failed")
		return nil
	end
	
	local objRootCtrl = objTree:GetUIObject("root.layout:root.ctrl")
	if not objRootCtrl then
		TipLog("[GetMainCtrlChildObj] get objRootCtrl failed")
		return nil
	end 

	return objRootCtrl:GetControlObject(tostring(strObjName))
end


function OpenPanel(strNewCtrlName)
	local objMainBodyCtrl = GetMainCtrlChildObj("TipCtrl.MainWnd.MainBody")
	if objMainBodyCtrl == nil then
		return false
	end
	
	local strCurCtrlName = objMainBodyCtrl:GetCurrentCtrlName()
	if strCurCtrlName ~= strNewCtrlName then
		objMainBodyCtrl:ChangePanel(strNewCtrlName)
		return true
	end
	
	return false
end


function IncVideoFiltTime(nVideoIncSec)
	local objMainBody = GetMainCtrlChildObj("TipCtrl.MainWnd.MainBody")
	local objAdvCount = objMainBody:GetChildObjByCtrlName("ChildCtrl_AdvCount")
	local objClockCtrl = objAdvCount:GetControlObject("ChildCtrl_AdvCount.Low.Time")
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local nCurFiltTime = tUserConfig["nFiltVideoTimeSec"] or 0
	local nNewFiltTime = nCurFiltTime+nVideoIncSec
	nNewFiltTime = TryClearTime(nNewFiltTime)
	
	objClockCtrl:SetTime(nNewFiltTime)
	tUserConfig["nFiltVideoTimeSec"] = nNewFiltTime
	tFunctionHelper.SaveConfigToFileByKey("tUserConfig")
end

--满2年清零
function TryClearTime(nTimeInSec)
	local n2YearInSec = 2*365*24*3600
	if nTimeInSec < n2YearInSec then
		return nTimeInSec
	end
	
	return 0
end


function UpdateWindow()
	local objMainWnd = GetMainWndInst()
	objMainWnd:UpdateWindow()
end

--

------------------文件--
local obj = XLGetGlobal("GreenWallTip.FunctionHelper")

obj.OpenPanel = OpenPanel
obj.UpdateWindow = UpdateWindow
obj.GetMainCtrlChildObj = GetMainCtrlChildObj
obj.IncVideoFiltTime = IncVideoFiltTime

XLSetGlobal("GreenWallTip.FunctionHelper", obj)




