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

function DownLoadGS(strUrl, strCmd)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local strPacketURL = strUrl or "http://down.lvdun123.com/client/GsSetup_0006.exe"
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strFileName = string.match(strPacketURL, "/([^/]+exe)")
	if not IsRealString(strFileName) then
		strFileName = "GsSetup_0006.exe"
	end
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)
	
	local strStamp = FunctionObj.GetTimeStamp()
	local strURLFix = strPacketURL..strStamp
	if string.find(strPacketURL, "?", 1, true) then
		strStamp = string.gsub(strStamp, "%?", "&")
		strURLFix = strPacketURL..strStamp
	end
	
	FunctionObj.NewAsynGetHttpFile(strURLFix, strSavePath, false
	, function(bRet, strRealPath)
			FunctionObj.TipLog("[DownLoadGS] bRet:"..tostring(bRet)
					.." strRealPath:"..tostring(strRealPath))
					
			if 0 ~= bRet then
				return
			end
			
			tipUtil:ShellExecute(0, "open", strRealPath, strCmd or "/s /run /setboot" , 0, "SW_HIDE")
	end)
end

--如果记录时间是今天则不满足， 否则满足
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

--距离所有tip弹出间隔1小时， 距离lvdun推广tip间隔7天
function CheckLvdunHistory()
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	local nNow = tipUtil:GetCurrentUTCTime() or 0
	local nLastAll = tUserConfig["nAllTipLastPopUTC"]
	local nLastLd = tUserConfig["nLvdunTipLastPopUTC"]
	local function check_poptime(last, step)
		if type(last) ~= "number" then return true end
		return nNow-last > step
	end
	return check_poptime(tonumber(nLastAll), 3600) and check_poptime(tonumber(nLastLd), 7*24*3600)
end

--是否安装绿盾
function CheckLvdunHasInstall()
	local strGSPath = FunctionObj.RegQueryValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\GreenShield\\Path")
	FunctionObj.TipLog("CheckLvdunHasInstall strGSPath = "..tostring(strGSPath))
	if IsRealString(strGSPath) and tipUtil:QueryFileExists(strGSPath) then
		return true
	end
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


--[[
{
	tVersion = {"26-27"},--日历版本区间
	nDelayMins = 2*60,--延时时间。
	text = "richtext",--tip类型，用文本来标识。有以下类型：richtext 富文本; lvdun 推广绿盾。
}
]]--
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
	local txt = string.lower(tostring(tTipInfo["text"]))
	local nCurUTC = tipUtil:GetCurrentUTCTime()
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	for i, info in ipairs(tTipInfo) do
		if type(tUserConfig["nTipLastPopUTC_"..(i-1)]) == "number" and not CheckTipHistory("nTipLastPopUTC_"..(i-1)) and nLaunchUTC > tUserConfig["nTipLastPopUTC_"..(i-1)]  then
			nLaunchUTC = tUserConfig["nTipLastPopUTC_"..(i-1)]
			FunctionObj.TipLog("nLaunchUTC use nTipLastPopUTC_"..(i-1))
		end
		local nStep = nCurUTC - nLaunchUTC
		FunctionObj.TipLog("i = "..i..", nStep = "..nStep)
		--富文本tip, 今天没有弹过， 满足开机延时
		--绿盾推广tip， 未安装绿盾，距离所有tip弹出间隔1小时， 距离lvdun推广tip间隔7天
		if not CheckTipWndExist() and CheckTipVersion(info["tVersion"]) and type(info["nDelayMins"]) == "number" then
			if txt == "richtext" and CheckTipHistory("nTipLastPopUTC_"..i) and nStep >= info["nDelayMins"]*60 then
				return i, info
			elseif text == "lvdun" and not CheckLvdunHasInstall() and CheckLvdunHistory() then 
				return i, info
			end
		end
	end
end

--保存最后弹出时间
local function save_lastpoptime(idx, info)
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	local txt = string.lower(tostring(info["text"]))
	tUserConfig["nAllTipLastPopUTC"] = tipUtil:GetCurrentUTCTime())
	tUserConfig["nTipLastPopUTC_"..idx] = tUserConfig["nAllTipLastPopUTC"]
	if txt == "lvdun" then tUserConfig["nLvdunTipLastPopUTC"] = tUserConfig["nAllTipLastPopUTC"] end
	FunctionObj.SaveConfigToFileByKey("tUserConfig")
end

--弹出富文本tip
function ShowRichtextTip(idx, info)
	FunctionObj.TipLog("ShowRichtextTip, enter")
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
	save_lastpoptime(idx, info)
end

--弹出绿盾推广tip
function ShowLvdunTip(idx, info)
	FunctionObj.TipLog("ShowLvdunTip, enter")
	--点击确认回调
	local function tip_callback_ok()
		DownLoadGS(info["url"], info["cmd"])
	end
	--点击取消回调
	local function tip_callback_cancel()
	end
	--创建tip界面...
	save_lastpoptime(idx, info)
end

function ShowTip(idx, info)
	local txt = string.lower(tostring(info["text"]))
	FunctionObj.TipLog("ShowTip, enter text = "..txt)
	if txt  == "richtext" then
		ShowRichtextTip(idx, info)
	elseif txt == "lvdun" then
		ShowLvdunTip(idx, info)
	else
		FunctionObj.TipLog("ShowTip, error. text is wrong, no match")
	end
end

local obj = {}
obj.ShowTip = ShowTip
obj.GetTipPopInfo = GetTipPopInfo
XLSetGlobal("DiDa.TipHelper", obj)