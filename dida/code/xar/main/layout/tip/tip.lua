local tipUtil = XLGetObject("API.Util")
local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

function CheckTipVersion(tForceVersion)
	FunctionObj.TipLog("CheckTipVersion enter")
	if type(tForceVersion) ~= "table" then
		FunctionObj.TipLog("CheckTipVersion leave return true")
		return true
	end

	local bRightVer = false
	
	local strCurVersion = FunctionObj.GetDiDaVersion()
	local _, _, _, _, _, strCurVersion_4 = string.find(strCurVersion, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
	local nCurVersion_4 = tonumber(strCurVersion_4)
	if type(nCurVersion_4) ~= "number" then
		FunctionObj.TipLog("CheckTipVersion leave return "..tostring(bRightVer))
		return bRightVer
	end
	for iIndex = 1, #tForceVersion do
		local strRange = tForceVersion[iIndex]
		local iPos = string.find(strRange, "-")
		if iPos ~= nil then
			local lVer = tonumber(string.sub(strRange, 1, iPos - 1))
			local hVer = tonumber(string.sub(strRange, iPos + 1))
			if lVer ~= nil and hVer ~= nil and nCurVersion_4 >= lVer and nCurVersion_4 <= hVer then
				bRightVer = true
				break
			end
		else
			local verFlag = tonumber(strRange)
			if verFlag ~= nil and nCurVersion_4 == verFlag then
				bRightVer = true
				break
			end
		end
	end
	FunctionObj.TipLog("CheckTipVersion leave return "..tostring(bRightVer))
	return bRightVer
end

function CheckTipHistory(strKey)
	FunctionObj.TipLog("CheckTipHistory enter")
	if not IsRealString(strKey) then
		return 
	end
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	if not tUserConfig[strKey] then
		FunctionObj.TipLog("CheckTipHistory leave return true")
		return true
	end
	if type(tUserConfig[strKey]) == "number" and FunctionObj.CheckTimeIsAnotherDay(tUserConfig[strKey]) then
		FunctionObj.TipLog("CheckTipHistory leave return true2")
		return true
	end
	FunctionObj.TipLog("CheckTipHistory leave return false")
	return false
end

function CheckTipWndExist()
	FunctionObj.TipLog("CheckTipWndExist enter")
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local strPopupInst = "TipCommon.Instance"
	local objPopupWnd = hostwndManager:GetHostWnd(strPopupInst)
	local bRet = false
	if objPopupWnd then
		if objPopupWnd:GetVisible() or objPopupWnd:GetWindowState() ~= "hide" then
			bRet = true
		end
	end
	FunctionObj.TipLog("CheckTipWndExist leave return "..tostring(bRet))
	return bRet
end

function GetTipPopInfo(tSvrData, nLaunchUTC)
	FunctionObj.TipLog("GetTipPopInfo enter, type(tSvrData) = "..type(tSvrData))
	if type(tSvrData) ~= "table" or type(nLaunchUTC) ~= "number" then
		return
	end
	local tTipInfo = tSvrData["tTipInfo"]
	FunctionObj.TipLog("GetTipPopInfo: type(tTipInfo) = "..type(tTipInfo))
	if type(tTipInfo) ~= "table" then
		return
	end
	local nCurUTC = tipUtil:GetCurrentUTCTime()
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	for i, info in ipairs(tTipInfo) do
		if type(tUserConfig["nTipLastPopUTC_"..(i-1)]) == "number" and not CheckTipHistory("nTipLastPopUTC_"..(i-1)) and nLaunchUTC > tUserConfig["nTipLastPopUTC_"..(i-1)]  then
			nLaunchUTC = tUserConfig["nTipLastPopUTC_"..(i-1)]
			FunctionObj.TipLog("nLaunchUTC use nTipLastPopUTC_"..(i-1))
		end
		local nStep = nCurUTC - nLaunchUTC
		FunctionObj.TipLog("i = "..i..", nStep = "..nStep)
		if not CheckTipWndExist() and CheckTipVersion(info["tVersion"]) and type(info["nDelayMins"]) == "number" and CheckTipHistory("nTipLastPopUTC_"..i) and nStep >= info["nDelayMins"]*60 then
			return i, info
		end
	end
end

function ShowTip(idx, info)
	FunctionObj.CreateWndByName("TipCommon", "TipCommonTree")
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local strPopupInst = "TipCommon.Instance"
	local objPopupWnd = hostwndManager:GetHostWnd(strPopupInst)
	if not objPopupWnd then
		return
	end 
	--移到桌面之外
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	local selfleft, selftop, selfright, selfbottom = objPopupWnd:GetWindowRect()
	local wndwidth, wndheight = selfright - selfleft, selfbottom - selftop
	objPopupWnd:Move( workright - wndwidth - 7, workbottom - wndheight, wndwidth, wndheight)
	FunctionObj.TipLog("objPopupWnd:Move l = "..(workright - wndwidth - 7)..", t = "..(workbottom - wndheight)..", w = "..wndwidth..", wndheight = "..wndheight)
	objPopupWnd:SetUserData({idx, info})
	local objTree = objPopupWnd:GetBindUIObjectTree()
	local layout = objTree:GetUIObject("root.layout")
	local browser = layout:GetObject("TipCtrl.MainWnd.Bkg:TipCtrl.MainWnd.MainPanel:TipCtrl.MainWnd.Web")
	if not browser then
		return
	end
	browser:Navigate(info["link"])
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nTipLastPopUTC_"..idx] = tipUtil:GetCurrentUTCTime()
	FunctionObj.SaveConfigToFileByKey("tUserConfig")
end

local obj = {}
obj.ShowTip = ShowTip
obj.GetTipPopInfo = GetTipPopInfo
XLSetGlobal("DiDa.TipHelper", obj)