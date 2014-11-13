local tipUtil = XLGetObject("GS.Util")
local g_bFilterOpen = false

-------事件---
function OnSelect_Update(self)
	ShowUpdateWnd()
end


function OnSelect_Website(self)
	OpenConfigURL("strIndexURL")
end


function OnSelect_Filter(self)
	ChangeFilterState()
	SetFilterText(self)
	
	local tStatInfo = {}
	tStatInfo.strEC = "TrayMenu"
	tStatInfo.strEA = "filteropen"
	
	if g_bFilterOpen then
		tStatInfo.strEL = "1"
	else
		tStatInfo.strEL = "2"
	end
	
	SendReport(tStatInfo)
end


function OnInit_Filter(self)
	SetFilterText(self)
end


function OnSelect_Contact(self)
	OpenConfigURL("strContactURL")
end


function OnSelect_About(self)
	ShowAboutWnd()
end


function OnSelect_Exit(self)
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	tFunctionHelper.ShowExitRemindWnd()
end

-------

function ShowAboutWnd()
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	tFunctionHelper.ShowPopupWndByName("TipAboutWnd.Instance")
end

function ShowUpdateWnd()
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	tFunctionHelper.ShowPopupWndByName("TipUpdateWnd.Instance")
end


function OpenConfigURL(strUrlKey)
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	if type(tFunctionHelper.ReadConfigFromMemByKey) ~= "function" then
		return
	end
	
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local strIndexURL = tUserConfig[strUrlKey]
	if IsRealString(strIndexURL) then
		tipUtil:OpenURL(strIndexURL)
	end	
end


function SetFilterText(self)
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	if type(tFunctionHelper.ReadConfigFromMemByKey("tUserConfig")) ~= "function" then
		return
	end
	
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local bFilterOpen = tUserConfig["bFilterOpen"]
	if bFilterOpen then
		self:SetText("过滤已开启")
	else
		self:SetText("过滤已关闭")
	end	
	
	g_bFilterOpen = bFilterOpen
end


function ChangeFilterState()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd("GreenWallTipWnd.MainFrame")
	if nil == frameHostWnd then
		return
	end
	
	local objTree = frameHostWnd:GetBindUIObjectTree()
	if nil == objTree then
		return
	end
	
	local objRootCtrl = objTree:GetUIObject("root.layout:root.ctrl")
	if nil == objRootCtrl then
		return
	end
	
	local objBodyContainer = objRootCtrl:GetControlObject("TipCtrl.MainWnd.MainBody")
	if nil == objBodyContainer then
		return
	end
	
	local objAdvCntCtrl = objBodyContainer:GetChildObjByCtrlName("ChildCtrl_AdvCount")
	if nil == objAdvCntCtrl then
		return
	end
	objAdvCntCtrl:ChangeSwitchFilter()
end


function SendReport(tStatInfo)
	local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
	if type(FunctionObj.TipConvStatistic) == "function" then
		FunctionObj.TipConvStatistic(tStatInfo)
	end
end


function IsRealString(str)
	return type(str) == "string" and str~=nil
end


