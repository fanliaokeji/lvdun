local tFunctionHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunctionHelper.tipUtil
local g_bFilterOpen = false

-------事件---
function OnSelect_Update(self)
	ShowUpdateWnd()
end


--设置开机启动
function OnSelect_Sysboot(self)
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local bLastSetAutoStup = tUserConfig["bUserSetAutoStup"]
	
	local bUserSetAutoStup = not bLastSetAutoStup
	tUserConfig["bUserSetAutoStup"] = bUserSetAutoStup
	
	local strExePath = tFunctionHelper.GetExePath()
	local strRegPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\WebEraser"
	
	if bUserSetAutoStup then 
		if IsRealString(strExePath) and tipUtil:QueryFileExists(strExePath) then
			local strCommandline = "\""..strExePath.."\"".." /sstartfrom sysboot /embedding"
			bRetCode = tFunctionHelper.RegSetValue(strRegPath, strCommandline)
		end
	else
		tFunctionHelper.RegDeleteValue(strRegPath)
	end
	
	tFunctionHelper.SaveConfigToFileByKey("tUserConfig")
end


function OnInit_Sysboot(self)
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local bUserSetAutoStup = tUserConfig["bUserSetAutoStup"]
	ShowCheckIco(self, bUserSetAutoStup)
end


function OnSelect_Filter(self)
	ChangeFilterState()
		
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
	SetFilterIcoState(self)
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


function SetFilterIcoState(self)
	if type(tFunctionHelper.ReadConfigFromMemByKey) ~= "function" then
		return
	end
	
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local bFilterOpen = tUserConfig["bFilterOpen"]
	ShowCheckIco(self, bFilterOpen)

	g_bFilterOpen = bFilterOpen
end


function ChangeFilterState()
	local objAdvCntCtrl = tFunctionHelper.GetMainCtrlChildObj("MainWnd.Low.AdvCount")
	if nil == objAdvCntCtrl then
		return
	end
	objAdvCntCtrl:ChangeSwitchFilter()
end



function ShowCheckIco(objMenuItem, bShowCheck)
	if bShowCheck then
		objMenuItem:SetIconID("MenuItem.Check")
	else
		objMenuItem:SetIconID("MenuItem.UnCheck")
	end
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


