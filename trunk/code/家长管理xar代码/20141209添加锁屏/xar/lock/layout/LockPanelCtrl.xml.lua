local tipUtil = XLGetObject("GS.Util")
local tipAsynUtil = XLGetObject("GS.AsynUtil")
local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
local g_IdenTimerID = nil
local g_strIdenNum = nil
local g_hHook = nil
local g_bIsSendingIden = false

----方法----
function SetTipData(self, infoTab) 
	return true
end

--事件
-----窗口-----
function OnShowLockWnd(self, bShow)
	if not bShow then
		return
	end
	
	local objTree = self:GetBindUIObjectTree()
	local objRootCtrl = objTree:GetUIObject("root.layout:root.ctrl")
	
	ShowPanel(objRootCtrl, "unlock")
	-- ClipCursorInWnd(self)
	HookKeyboard()
end


------解锁界面-------
function OnInputCodeKeyDown(self, nKeyCode)
	local objRootCtrl = self:GetOwnerControl()
	ShowUnlockKeyError(objRootCtrl, false)

	if nKeyCode == 13 then --只处理回车
		local strKey = self:GetText()
		local bPassCheck = FunctionObj.CheckUserKey(strKey)
		if bPassCheck then
			HideLockWnd(self)
		else
			ShowUnlockKeyError(objRootCtrl, true)
		end
		
		return
	end
end


function OnMouseEnterInputBkg(self)
	self:SetTextureID("GreenWall.LockWnd.Input.Hover")
end


function OnMouseLeaveInputBkg(self)
	self:SetTextureID("GreenWall.LockWnd.Input.Normal")
end


function OnClickForgetBtn(self)
	local objRootCtrl = self:GetOwnerControl()
	ShowPanel(objRootCtrl, "resetcode")
end


function RouteToFather(self)
	self:RouteToFather()
end

-------验证码界面------
function OnClickEnterBtn(self)
	local bPassCheck = CheckUserIden(self)
	if bPassCheck then
		local objRootCtrl = self:GetOwnerControl()
		ShowPanel(objRootCtrl, "unlock")		
	end
end


function OnClickCancleBtn(self)
	local objRootCtrl = self:GetOwnerControl()
	ShowPanel(objRootCtrl, "unlock")
end

--短信验证
function OnClickIdenBtn(self)
	local objRootCtrl = self:GetOwnerControl()
	
	if g_bIsSendingIden then
		return 
	end
	
	if g_IdenTimerID ~= nil then
		return
	end
	
	local strIdenNum = GenIdentyNum()
	g_strIdenNum = strIdenNum
	
	SendIdenToServer(objRootCtrl, strIdenNum,
		function(bRet)
			g_bIsSendingIden = false
			if bRet then
				self:SetText("验证码已发送")
				ShowIdenTimer(objRootCtrl, 60)
			else
				self:SetText("网络错误")
			end
		end)
end

---------------辅助函数----------------------------

function CheckUserIden(objUIElem)
	local objRootCtrl = objUIElem:GetOwnerControl()
	local objNewCode = objRootCtrl:GetControlObject("lockwnd.centerpanel.resetcode.inputnew")
	local objIdent = objRootCtrl:GetControlObject("lockwnd.centerpanel.resetcode.inputident")
	
	if not objNewCode or not objIdent then
		return true
	end

	local strNewCode = objNewCode:GetText()
	local strIdent = tostring(objIdent:GetText())
	
	if strIdent ~= tostring(g_strIdenNum) then
		ShowIdenKeyError(objRootCtrl, true, "验证码错误")
		return false
	end
	
	if not IsRealString(strNewCode) or string.len(strNewCode) < 6 
		or string.len(strNewCode) > 12 then 

		ShowIdenKeyError(objRootCtrl, true, "长度为6-12为密码")
		return false
	end
	
	local strEncKey = FunctionObj.EncryptAesBase64(strNewCode)
	FunctionObj.SetUserKey(strEncKey)
	ShowIdenKeyError(objRootCtrl, false, "")
	return true
end


function SetIdenBtnText(objRootCtrl, strText)
	if not IsRealString(strText) then
		return
	end	
	
	local objIdenBtn = objRootCtrl:GetControlObject("lockwnd.centerpanel.resetcode.iden")
	if not objIdenBtn then
		return
	end

	objIdenBtn:SetText(strText)
end


function ShowIdenTimer(objRootCtrl, nSec)
	local objIdensecText = objRootCtrl:GetControlObject("lockwnd.centerpanel.resetcode.idensec")
	if not objIdensecText then
		return
	end
	
	local strSecText = "("..tostring(nSec).."s)"
	objIdensecText:SetText(strSecText)
	objIdensecText:SetVisible(true)
	objIdensecText:SetChildrenVisible(true)
	
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	if g_IdenTimerID ~= nil then
		timerManager:KillTimer(g_IdenTimerID)
		g_IdenTimerID = nil
	end
		
	g_IdenTimerID = timerManager:SetTimer(
		function(item, id)
			local strSecText = "("..tostring(nSec).."s)"
			objIdensecText:SetText(strSecText)
		
			nSec = nSec - 1
			if nSec < 0 then
				item:KillTimer(id)
				timerManager:KillTimer(g_IdenTimerID)
				g_IdenTimerID = nil
				
				objIdensecText:SetVisible(false)
				objIdensecText:SetChildrenVisible(false)
				SetIdenBtnText(objRootCtrl, "重新获取验证")
				return
			end
 	
		end, 1000)
end


function StopIdenTimer(objRootCtrl)
	local objIdensecText = objRootCtrl:GetControlObject("lockwnd.centerpanel.resetcode.idensec")
	if objIdensecText then
		objIdensecText:SetVisible(false)
	end

	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	if g_IdenTimerID ~= nil then
		timerManager:KillTimer(g_IdenTimerID)
		g_IdenTimerID = nil
		XLMessageBox(tostring("kill"))
	end
	
	g_bIsSendingIden = false
	SetIdenBtnText(objRootCtrl, "点击获取验证")
end


function SendIdenToServer(objRootCtrl, strIdenNum, fnCallBack)
	if not IsRealString(strIdenNum) then
		fnCallBack(false)
		return
	end
	
	local strUserPhone = FunctionObj.GetUserPhone()
	if not IsRealString(strUserPhone) then
		fnCallBack(false)
		return
	end
	
	SetIdenBtnText(objRootCtrl, "正在发送...")
	g_bIsSendingIden = true
	
	local strUrl = "http://www.google-analytics.com/"	
	tipAsynUtil:AsynSendHttpStat(strUrl, function(bRet)
		TipLog("[SendIdenToServer]: strUrl: " .. tostring(strUrl).." bRet:"..tostring(bRet))
		if bRet == 0 then
			fnCallBack(true)
		else
			fnCallBack(false)
		end
	end)
	
	return true
end


function GenIdentyNum()
	local nCurTimeUTC = tipUtil:GetCurrentUTCTime()
	math.randomseed(nCurTimeUTC)
	local nIdentyNum = math.random(1, 999999)
	local strIdentyNum = string.format("%06s", 999959)			 
	return "123456"
end


function ClearIdenNum()
	g_strIdenNum = nil
end


function HookKeyboard()
	local hHook = tipUtil:SetLowLevelKeyboardHook()
	g_hHook = hHook
end

function UnHookKeyboard()
	if g_hHook then
		tipUtil.UnhookLowLevelKeyboard(g_hHook)
	end
end


function ClipCursorInWnd(objHostWnd)
	local objTree = objHostWnd:GetBindUIObjectTree()
	local objRootCtrl = objTree:GetUIObject("root.layout:root.ctrl")
	local nL, nT, nR, nB = objRootCtrl:GetAbsPos()
	local nNewL, nNewT, nNewR, nNewB = objHostWnd:HostWndRectToScreenRect(nL, nT, nR, nB)
	
	tipUtil:ClipCursor(nNewL, nNewT, nNewR, nNewB)
end


function ShowPanel(objRootCtrl, strPanelName)
	local objUnlockLayout = objRootCtrl:GetControlObject("lockwnd.centerpanel.unlock")
	local objResetLayout = objRootCtrl:GetControlObject("lockwnd.centerpanel.resetcode")
	
	if objUnlockLayout == nil or objResetLayout == nil then
		return
	end
	
	ClearIdenNum()
	
	local bShowUnlock = true
	if string.lower(tostring(strPanelName)) == "resetcode" then
		bShowUnlock = false
	end
				
	objUnlockLayout:SetVisible(bShowUnlock)
	objUnlockLayout:SetChildrenVisible(bShowUnlock)
	objResetLayout:SetVisible(not bShowUnlock)
	objResetLayout:SetChildrenVisible(not bShowUnlock)
	
	SetUnLockLayoutStyle(objRootCtrl, bShowUnlock)
	SetResetLayoutStyle(objRootCtrl, not bShowUnlock)
end


function SetUnLockLayoutStyle(objRootCtrl, bShow)
	local objEditCode = objRootCtrl:GetControlObject("lockwnd.centerpanel.unlock.inputcode")
	if objEditCode then
		objEditCode:SetText("")
		objEditCode:SetFocus(bShow)
	end
	
	ShowUnlockKeyError(objRootCtrl, false)
end


function SetResetLayoutStyle(objRootCtrl, bShow)
	local objEditIdent = objRootCtrl:GetControlObject("lockwnd.centerpanel.resetcode.inputident")
	local objEditNewCode = objRootCtrl:GetControlObject("lockwnd.centerpanel.resetcode.inputnew")
	
	if objEditIdent then
		objEditIdent:SetText("")
		objEditIdent:SetFocus(bShow)
	end
	
	if objEditNewCode then
		objEditNewCode:SetText("")
	end
	
	ShowIdenKeyError(objRootCtrl, false, "")
	StopIdenTimer(objRootCtrl)
end


function ShowUnlockKeyError(objRootCtrl, bShow)
	local objKeyError = objRootCtrl:GetControlObject("lockwnd.centerpanel.unlock.keyerror")
	if not objKeyError then
		return
	end
	
	objKeyError:SetVisible(bShow)
end


function ShowIdenKeyError(objRootCtrl, bShow, strText)
	local objKeyError = objRootCtrl:GetControlObject("lockwnd.centerpanel.resetcode.keyerror")
	if not objKeyError then
		return
	end
	
	if IsRealString(strText) then
		objKeyError:SetText(strText)
	end
	
	objKeyError:SetVisible(bShow)
end


function HideLockWnd(objUIElem)
	local objTree = objUIElem:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	--tipUtil:ClipCursor()
	UnHookKeyboard()
	
	objHostWnd:Show(0)
end


function IsRealString(str)
	return type(str) == "string" and str ~= ""	
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@greenwall_locktipctrl MainTipLog: " .. tostring(strLog))
	end
end

