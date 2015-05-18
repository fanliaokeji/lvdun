local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local gtAppList = nil

----方法----
function SetTipData(self, infoTab) 
	CreateFilterListener(self)
	self:UpdateMainWndBkg()
	return true
end


function UpdateMainWndBkg(self)
	local bFilterOpen = tFunHelper.GetFilterState() 
	local objCloseBkg = self:GetControlObject("MainWnd.Up.Bkg.CloseFilter")
	local objOpenBkg = self:GetControlObject("MainWnd.Up.Bkg.OpenFilter")
		
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
		ctrl:UpdateMainWndBkg()
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
	--[[AsynCall(
		function()
			ShowAppList(ctrl, true)
		end)]]--
		ShowAppList(ctrl, true)
end

function LoadAppList()
	local strAppListPath = tFunHelper.GetCfgPathWithName("AppList.dat")
	if not IsRealString(strAppListPath) or not tipUtil:QueryFileExists(strAppListPath) then
		return nil
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
			if CreateTextureRes(info["strKeyName"]) then
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
										XLMessageBox("21")
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

