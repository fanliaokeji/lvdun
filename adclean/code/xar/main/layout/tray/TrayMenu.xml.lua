local tFunctionHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunctionHelper.tipUtil
local g_bFilterOpen = false

-------事件---
function OnSelect_Update(self)
	ShowUpdateWnd()
end


function OnSelect_Sysboot(self)
	-- OpenConfigURL("strIndexURL")
end


function OnInit_Sysboot(self)
	
end



function OnSelect_Introduce(self)
	tFunctionHelper.ShowPopupWndByName("TipIntroduceWnd.Instance", true)
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
	ShowExitRemindWnd()
end

-------

function ShowAboutWnd()
	tFunctionHelper.ShowPopupWndByName("TipAboutWnd.Instance", true)
end

function ShowUpdateWnd()
	tFunctionHelper.ShowPopupWndByName("TipUpdateWnd.Instance", true)
end

function ShowExitRemindWnd()
	tFunctionHelper.ShowPopupWndByName("TipExitRemindWnd.Instance", true)
end


function OpenConfigURL(strUrlKey)
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
	if type(tFunctionHelper.ReadConfigFromMemByKey) ~= "function" then
		return
	end
	
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local bFilterOpen = tUserConfig["bFilterOpen"]
	if bFilterOpen then
		-- self:SetText("过滤已开启")
	else
		-- self:SetText("过滤已关闭")
	end	
	
	g_bFilterOpen = bFilterOpen
end


function ChangeFilterState()
	local objAdvCntCtrl = tFunctionHelper.GetMainCtrlChildObj("MainWnd.Low.AdvCount")
	if nil == objAdvCntCtrl then
		return
	end
	objAdvCntCtrl:ChangeSwitchFilter()
end


function SendReport(tStatInfo)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper")
	if type(FunctionObj.TipConvStatistic) == "function" then
		FunctionObj.TipConvStatistic(tStatInfo)
	end
end


function IsRealString(str)
	return type(str) == "string" and str~=nil
end


