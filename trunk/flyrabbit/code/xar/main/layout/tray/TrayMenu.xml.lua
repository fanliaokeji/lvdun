local tFunctionHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunctionHelper.tipUtil
local g_bFilterOpen = false

-------事件---
--设置开机启动
local g_bHasAutoStup = true
function OnSelect_Sysboot(self)
	local bHasAutoStup = g_bHasAutoStup
	local strExePath = tFunctionHelper.GetExePath()
	local strRegPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\FlyRabbit"
	
	if not bHasAutoStup then 
		if IsRealString(strExePath) and tipUtil:QueryFileExists(strExePath) then
			local strCommandline = "\""..strExePath.."\"".." /sstartfrom sysboot /embedding"
			bRetCode = tFunctionHelper.RegSetValue(strRegPath, strCommandline)
		end
	else
		tFunctionHelper.RegDeleteValue(strRegPath)
	end
end


function OnInit_Sysboot(self)
	local bHasAutoStup = false
	local strExePath = tFunctionHelper.GetExePath()
	local szCmdLine = tFunctionHelper.RegQueryValue("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\FlyRabbit") or ""
	if IsRealString(szCmdLine) 
		and string.find(string.lower(szCmdLine), string.lower(tostring(strExePath)), 1, true) then
		bHasAutoStup = true  -- 已经开机启动
	end
	
	ShowCheckIco(self, bHasAutoStup)
	g_bHasAutoStup = bHasAutoStup
end


function OnSelect_PictureSet(self)
	
end


function OnSelect_OfficialWeb(self)
	OpenConfigURL("strIndexURL")
end


function OnSelect_Update(self)
	ShowUpdateWnd()
end


function OnSelect_About(self)
	ShowAboutWnd()
end


function OnSelect_Exit(self)
	-- ShowExitRemindWnd()
	tFunctionHelper.ReportAndExit()
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


