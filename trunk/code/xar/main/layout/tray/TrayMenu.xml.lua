local tipUtil = XLGetObject("GS.Util")

-------事件---
function OnSelect_Update(self)

end


function OnSelect_Website(self)
	OpenIndexURL()
end


function OnSelect_Filter(self)
	ChangeFilterState()
	SetFilterText(self)
end


function OnInit_Filter(self)
	SetFilterText(self)
end


function OnSelect_Contact(self)
	OpenIndexURL()
end


function OnSelect_About(self)
	ShowAboutWnd()
end


function OnSelect_Exit(self)
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	tFunctionHelper.TipConvStatistic()
end

-------

function ShowAboutWnd()
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	tFunctionHelper.ShowPopupWndByName("TipAboutWnd.Instance")
end

function OpenIndexURL()
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	if type(tFunctionHelper.GetUserConfigFromMem) ~= "function" then
		return
	end
	
	local tUserConfig = tFunctionHelper.GetUserConfigFromMem() or {}
	local strIndexURL = tUserConfig["strIndexURL"]
	if IsRealString(strIndexURL) then
		tipUtil:OpenURL(strIndexURL)
	end	
end


function SetFilterText(self)
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	if type(tFunctionHelper.GetUserConfigFromMem) ~= "function" then
		return
	end
	
	local tUserConfig = tFunctionHelper.GetUserConfigFromMem() or {}
	local bFilterOpen = tUserConfig["bFilterOpen"]
	if bFilterOpen then
		self:SetText("过滤已开启")
	else
		self:SetText("过滤已关闭")
	end	
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


function IsRealString(str)
	return type(str) == "string" and str~=nil
end


