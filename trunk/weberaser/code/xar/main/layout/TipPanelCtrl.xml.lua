local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local gtAppList = nil

----方法----
function SetTipData(self, infoTab) 
	CreateFilterListener(self)
	self:UpdateMainWndBkg()
	return true
end


function UpdateMainWndBkg(self, bForce)
	local bFilterOpen = tFunHelper.GetFilterState() 
	local objCloseBkg = self:GetControlObject("MainWnd.Up.Bkg.CloseFilter")
	local objOpenBkg = self:GetControlObject("MainWnd.Up.Bkg.OpenFilter")
	if not objOpenBkg:GetVisible() and not objCloseBkg:GetVisible() and not bForce then
		return 
	end
		
	if bFilterOpen then
		objCloseBkg:SetVisible(false)
		objOpenBkg:SetVisible(true)
		objOpenBkg:Play()
	else
		objCloseBkg:SetVisible(true)
		objOpenBkg:SetVisible(false)
		objOpenBkg:Stop()
	end	
end



---事件--
function OnClickCloseBtn(self)
	HideWndToTray(self)
end

function OnClickMinBtn(self)
	local objTree = self:GetOwner()
	if nil == objTree then
		return
	end
	
	local objHostWnd = objTree:GetBindHostWnd()
	if nil == objHostWnd then
		return
	end
	
	objHostWnd:Min()	
end



--监听事件
function CreateFilterListener(objRootCtrl)
	local objFactory = XLGetObject("GSListen.Factory")
	if not objFactory then
		tFunHelper.TipLog("[CreateFilterListener] not support GSListen.Factory")
		return
	end
	
	local objListen = objFactory:CreateInstance()	
	objListen:AttachListener(
		function(key,...)	

			tFunHelper.TipLog("[CreateFilterListener] key: " .. tostring(key))
			
			local tParam = {...}	
			if tostring(key) == "OnFilterResult" then
				OnFilterResult(tParam)
			elseif tostring(key) == "OnCommandLine" then
				OnCommandLine(tParam)
			end
			
			return
		end)
end

-------------
function ShowHostWnd()
	local objHostWnd = tFunHelper.GetMainWndInst()
	if objHostWnd then
		objHostWnd:Show(5)
		objHostWnd:BringWindowToTop(true)
	end
end

function HideWndToTray(objUIElement)
	local objTree = objUIElement:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end

local gnTimerID = nil
local gtTextInfo = {"过滤骚扰广告，还原干净网页", "看视频无广告，杜绝一切骚扰", "广告橡皮擦，去广告就这么简单", idx=1}
function OnInitControlText(self)
	local timerMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
	if not timerMgr then return end
	if gnTimerID == nil and gtTextInfo.idx ~= #gtTextInfo then
		self:SetText(gtTextInfo[gtTextInfo.idx])
		gnTimerID = timerMgr:SetTimer(function()
				if gtTextInfo.idx ~= #gtTextInfo then
					AnimChangeText(self, true)
				else
					timerMgr:KillTimer(gnTimerID)
					gnTimerID = nil
				end
			end, 3000)
	end
end

function AnimChangeText(self, bHide)
	local function onAniFinish(anim, old, new)
		if new == 3 or new == 4 then
			if gtTextInfo.idx == #gtTextInfo then
				timerMgr:KillTimer(gnTimerID)
				gnTimerID = nil
			else
				gtTextInfo.idx = gtTextInfo.idx + 1
				self:SetText(gtTextInfo[gtTextInfo.idx])
				AnimChangeText(self)
			end
		end
	end
	local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")
	local alphaAnim = aniFactory:CreateAnimation("AlphaChangeAnimation")
	local text = self:GetText()
	alphaAnim:BindObj(self)
	alphaAnim:SetTotalTime(1000)
	if bHide then
		alphaAnim:SetKeyFrameAlpha(255, 0)
		alphaAnim:AttachListener(true,onAniFinish)
	else
		alphaAnim:SetKeyFrameAlpha(0, 255)
	end
	local owner = self:GetOwner()
	owner:AddAnimation(alphaAnim)
	alphaAnim:Resume()
end

function ShowMainPage(ctrl, bIsShow)
	local adctrl = ctrl:GetControlObject("MainWnd.Low.AdvCount")
	local leftgif = ctrl:GetControlObject("MainWnd.Up.Bkg.OpenFilter")
	local text = ctrl:GetControlObject("MainWnd.TextAnim")
	adctrl:SetVisible(bIsShow)
	adctrl:SetChildrenVisible(bIsShow)
	leftgif:SetVisible(bIsShow)
	text:SetVisible(bIsShow)
	local gifclose = ctrl:GetControlObject("MainWnd.Up.Bkg.CloseFilter")
	gifclose:SetVisible(bIsShow)
	if bIsShow then
		local bkg = ctrl:GetControlObject("MainWnd.Bkg")
		bkg:SetTextureID("MainWnd.Bkg1")
		ctrl:UpdateMainWndBkg(true)
	end
end

function ShowZanZhu(ctrl, bIsShow)
	if bIsShow then
		local bkg = ctrl:GetControlObject("MainWnd.Bkg")
		bkg:SetTextureID("MainWnd.Bkg2")
	end
	local zztext = ctrl:GetControlObject("MainWnd.Text.ZanZhu")
	local lyzzbtn = ctrl:GetControlObject("MainWnd.Button.LYZZ")
	zztext:SetVisible(bIsShow)
	lyzzbtn:Show(bIsShow)
end

function OnClickZZ(self)
	local ctrl = self:GetOwnerControl()
	ShowMainPage(ctrl, false)
	ShowAppList(ctrl, false)
	ShowZanZhu(ctrl, true)
end

function OnClickSY(self)
	local ctrl = self:GetOwnerControl()
	ShowZanZhu(ctrl, false)
	ShowAppList(ctrl, false)
	ShowMainPage(ctrl, true)
end

function OnClickYY(self)
	local ctrl = self:GetOwnerControl()
	ShowZanZhu(ctrl, false)
	ShowMainPage(ctrl, false)
	AsynCall(
		function()
			if type(gtAppList) == "table" then
				ShowAppList(ctrl, true)
			else
				DownLoadAppList(function(strCfgName)
					gtAppList = LoadAppList(strCfgName)
					ShowAppList(ctrl, true)
				end)
			end
		end)
end

function DownLoadAppList(fnCallBack)
	local tUserConfig = tFunHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	
	local strAppListURL = tUserConfig["strServerAppListURL"]
	if not IsRealString(strAppListURL) then
		fnCallBack("AppList.dat")
		return
	end
	
	local strSavePath = tFunHelper.GetCfgPathWithName("ServerAppList.dat")
	if not IsRealString(strSavePath) then
		fnCallBack("AppList.dat")
		return
	end
	
	local nTimeInMs = 10*1000
	g_bAppListDownLoading = true
	
	tFunHelper.NewAsynGetHttpFile(strAppListURL, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[InitAppCtrl] bRet:"..tostring(bRet)
				.." strRealPath:"..tostring(strRealPath))
		
		if 0 == bRet then
			fnCallBack("ServerAppList.dat")
		else
			fnCallBack("AppList.dat")
		end
		
		g_bAppListDownLoading = false
	end, nTimeInMs)
end


function LoadAppList(strCfgName)
	local strAppListPath = tFunHelper.GetCfgPathWithName(strCfgName)
	if not IsRealString(strAppListPath) or not tipUtil:QueryFileExists(strAppListPath) then
		return {}
	end
	
	local tAppList = tFunHelper.LoadTableFromFile(strAppListPath)
	return tAppList
end

function CreateTextureRes(strName)
	local xarMgr = XLGetObject("Xunlei.UIEngine.XARManager")
	local xar = xarMgr:GetXARByIndex(0)
	local strImgPath = __document .. "\\..\\..\\..\\..\\appimage\\"..strName..".png"
	if xar:GetTexture("AppList."..strName) == nil and tipUtil:QueryFileExists(strImgPath) then
		local texture = xar:CreateTexture({["id"] = "AppList."..strName, ["image"] = strImgPath})
		return true
	end
	return false
end

function ShowAppList(ctrl, bIsShow) 
	local layout = ctrl:GetControlObject("MainWnd.Layout.AppList")
	if not layout and not bIsShow then return end
	if bIsShow then
		local bkg = ctrl:GetControlObject("MainWnd.Bkg")
		bkg:SetTextureID("MainWnd.Bkg3")
	end
	if not layout then
		local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
		layout = objFactory:CreateUIObject("MainWnd.Layout.AppList", "LayoutObject")
		layout:SetObjPos2(24, 50, 655, 298)
		ctrl:AddChild(layout)
		gtAppList = gtAppList or LoadAppList()
		local tmpTextureObj, tmpTextObj
		for idx, info in ipairs(gtAppList) do
			if CreateTextureRes(info["strKeyName"]) and idx <= 24 then
				tmpTextureObj = objFactory:CreateUIObject("AppList.Texture."..info["strKeyName"], "TipAddin.Button")
				tmpTextObj = objFactory:CreateUIObject("AppList.Text."..info["strKeyName"], "TextObject")
				layout:AddChild(tmpTextureObj)
				layout:AddChild(tmpTextObj)
				local h, v = math.modf(idx/8)
				v = v*8
				if v == 0 then
					v = 8
				else
					h = h + 1
				end
				tmpTextureObj:SetObjPos2((v-1)*83+13, (h-1)*110, 48, 48)
				local attr = tmpTextureObj:GetAttribute()
				attr.NormalBkgID = "AppList."..info["strKeyName"]
				attr.HoverBkgID = "AppList."..info["strKeyName"]
				attr.DownBkgID = "AppList."..info["strKeyName"]
				tmpTextureObj:Show(true)
				tmpTextureObj:AttachListener("OnClick", 
									false,
									function(self)
										OpenLinkAfterClick(info)
									end)
				tmpTextObj:SetObjPos2((v-1)*83, (h-1)*110+48+6, 70, 15)
				tmpTextObj:SetHAlign("center")
				tmpTextObj:SetVAlign("center")
				tmpTextObj:SetTextFontResID("font.yahei12")
				tmpTextObj:SetTextColorResID("system.white")
				tmpTextObj:SetText(info["strAppName"])			
			end
		end
	end
	layout:SetVisible(bIsShow)
	layout:SetChildrenVisible(bIsShow)
end

--1:浏览器打开;2:直接下载或运行
function OpenLinkAfterClick(info)
	local nOpenType = info.nOpenType
	local strOpenLink = info.strOpenLink
	local strKeyName = info.strKeyName or ""
	
	if not IsRealString(strOpenLink) then
		return
	end
	
	if nOpenType == 1 then
		tipUtil:OpenURL(strOpenLink)
		SendAppReport(strKeyName, 1)
	elseif nOpenType == 2 then
		OpenSoftware(info)
	end	
end


function OpenSoftware(info)
	local strRegPath = info.strRegPath or ""
	local strExeName = info.strExeName or ""
	local strKeyName = info.strKeyName or ""
	
	local strInstallDir = tFunHelper.RegQueryValue(strRegPath)
	if string.lower(strExeName) == "baidusd.exe" then
		local strVersion = tFunHelper.RegQueryValue(info.strRegVersion)
		strInstallDir = tipUtil:PathCombine(strInstallDir, strVersion)
	end
	local strInstallPath = tipUtil:PathCombine(strInstallDir, strExeName)
	local strPathWithFix = string.gsub(strInstallPath, '"', "")
	if IsRealString(strInstallDir) and tipUtil:QueryFileExists(strPathWithFix) then
		tipUtil:ShellExecute(0, "open", strPathWithFix, 0, 0, "SW_SHOWNORMAL")
		SendAppReport(strKeyName, 2)
	else
		if info.bIsDownLoading then
			return
		end
		DownLoadSoftware(info)
		SendAppReport(strKeyName, 3)
	end
end


function GetFileSaveNameFromUrl(url)
	local _, _, strFileName = string.find(tostring(url), ".*/(.*)$")
	local npos = string.find(strFileName, "?", 1, true)
	if npos ~= nil then
		strFileName = string.sub(strFileName, 1, npos-1)
	end
	return strFileName
end

function DownLoadSoftware(info)
	if info.bIsDownLoading then
		return
	end
	info.bIsDownLoading = true
	
	local strOpenLink = info.strOpenLink
	local strCommand = info.strCommand or ""
	if not IsRealString(strOpenLink) then
		return
	end
	
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strFileName = GetFileSaveNameFromUrl(strOpenLink)	
	if not string.find(strFileName, "%.exe$") then
		strFileName = strFileName..".exe"
	end
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)
	
	tFunHelper.NewAsynGetHttpFile(strOpenLink, strSavePath, false
	, function(bRet, strRealPath)
		tFunHelper.TipLog("[DownLoadSoftware] strOpenLink:"..tostring(strOpenLink)
		        .."  bRet:"..tostring(bRet).."  strRealPath:"..tostring(strRealPath))		
		if 0 ~= bRet then
			attr.bIsDownLoading = false
			return
		end
		
		tipUtil:ShellExecute(0, "open", strRealPath, strCommand, 0, "SW_HIDE")
		attr.bIsDownLoading = false
	end)	
end

-- 1 浏览器打开链接
-- 2 激活软件
-- 3 下载软件 
function SendAppReport(strKeyName, nType)
	local tStatInfo = {}
	tStatInfo.strEC = "AppPanel"
	tStatInfo.strEA = strKeyName
	tStatInfo.strEL = tostring(nType)
	
	if type(tFunHelper.TipConvStatistic) == "function" then
		tFunHelper.TipConvStatistic(tStatInfo)
	end
end

function OnInitControlVersion(self)
	local strVer = tFunHelper.GetProjectVersion()
	local strVerInfo = "版本号：V"..strVer.."正式版"
	self:SetText(strVerInfo)
end

function OnCommandLine(tParam)
	ShowHostWnd()
end

function OnFilterResult(tParam)
	local bFilterSucc = tParam[1]
	if not bFilterSucc then
		return
	end 
		
	local objAdvCount = tFunHelper.GetMainCtrlChildObj("MainWnd.Low.AdvCount")
	if objAdvCount == nil then
		tFunHelper.TipLog("[OnFilterResult] get ChildCtrl_AdvCount failed")
		return
	end
	
	objAdvCount:AddAdvCount()
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

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end


function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
end

