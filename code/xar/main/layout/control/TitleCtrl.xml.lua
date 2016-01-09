local tipUtil = XLGetObject("GS.Util")
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
local Helper =  XLGetGlobal("Helper")
-----事件----
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

function OnClickConfigBtn(self)
	-- local objRootCtrl = self:GetOwnerControl()
	-- objRootCtrl:SetCaptionStyle(false)
	-- local bOpenSucc = tFunctionHelper.OpenPanel("ChildCtrl_Config")
	
	-- if not bOpenSucc then
		-- tFunctionHelper.OpenPanel("ChildCtrl_AdvCount")
		-- objRootCtrl:SetCaptionStyle(true)
	-- end
	
	local wndTree = self:GetOwner()
	local wnd = wndTree:GetBindHostWnd()
	local maskWnd, maskWndTree = Helper:CreateTransparentMask(wnd)
	
	--创建“设置“窗口之前，先创建一个半透明蒙层（Mask）窗口覆盖到主窗口上，再以Mask窗口为父窗口创建”设置“窗口
	--此时”设置“窗口已经成为主窗口的 孙子 窗口
	local userData = {["maskWndParent"] = wnd, ["parentWnd"] = maskWnd}
	Helper:CreateModalWnd("ConfigWnd", "ConfigWndTree", maskWnd:GetWndHandle(), userData)
end

function OnClickHomePage(self)
	local url = "http://www.lvdun123.com"
	tipUtil:ShellExecute(0, "open", url, 0, 0, "SW_SHOWNORMAL")
end

function OnClickShare(self)
	local wndTree = self:GetOwner()
	local wnd = wndTree:GetBindHostWnd()
	local maskWnd, maskWndTree = Helper:CreateTransparentMask(wnd)
	
	--创建“设置“窗口之前，先创建一个半透明蒙层（Mask）窗口覆盖到主窗口上，再以Mask窗口为父窗口创建”设置“窗口
	--此时”设置“窗口已经成为主窗口的 孙子 窗口  所以，比较好的方法就是：“不要跟强势的场景处在同一组内”
	local userData = {["parentWnd"] = maskWnd, ["maskWndParent"] = wnd}
	Helper:CreateModalWnd("ShareWnd", "ShareWndTree", maskWnd:GetWndHandle(), userData)
end


-----方法-----
function SetCaptionStyle(objRootCtrl, bMainPanel)
	if bMainPanel then
		objRootCtrl:SetCaptionText("绿盾广告管家")
		objRootCtrl:SetCaptionColor("system.white")
		SetBkgVisible(objRootCtrl, false)
	else
		objRootCtrl:SetCaptionText("设置")
		objRootCtrl:SetCaptionColor("caption.black")
		SetBkgVisible(objRootCtrl, true)
	end
	
	SetBtnStyle(objRootCtrl, bMainPanel)
end


function SetCaptionText(self, strText)
	local objCaptionText = self:GetControlObject("TipCtrl.Caption.Text")
	objCaptionText:SetText(strText)
end

function SetCaptionColor(self, strColor)
	local objCaptionText = self:GetControlObject("TipCtrl.Caption.Text")
	objCaptionText:SetTextColorResID(strColor)
end

-----辅助函数----
function SetBkgVisible(objRootCtrl, bVisible)
	local objBkg = objRootCtrl:GetControlObject("TipCtrl.Bkg")
	local objShadow = objRootCtrl:GetControlObject("TipCtrl.MainWnd.TopShadow")
	objBkg:SetVisible(bVisible)
	objShadow:SetVisible(bVisible)
end

function SetBtnStyle(objRootCtrl, bMainPanel)
	local objCloseBtn = objRootCtrl:GetControlObject("TipCtrl.Caption.CloseBtn")
	local objMinBtn = objRootCtrl:GetControlObject("TipCtrl.Caption.BtnMin")
	local objConfig = objRootCtrl:GetControlObject("TipCtrl.Caption.Config")
	
	local attrClose = objCloseBtn:GetAttribute()
	local attrMin = objMinBtn:GetAttribute()
	local attrConfig = objConfig:GetAttribute()
	
	if bMainPanel then
		attrClose.NormalBkgID = "GreenWall.MainWnd.BtnClose.Normal"
		attrMin.NormalBkgID = "GreenWall.MainWnd.BtnMin.Normal"
		attrConfig.NormalBkgID = "GreenWall.MainWnd.Config.Normal"
	else
		attrClose.NormalBkgID = "GreenWall.BtnClose.Normal"
		attrMin.NormalBkgID = "GreenWall.BtnMin.Normal"
		attrConfig.NormalBkgID = "GreenWall.Config.Normal"
	end
	
	objCloseBtn:SetState(0, true)  --force
	objMinBtn:SetState(0, true)
	objConfig:SetState(0, true)
	
	tFunctionHelper.UpdateWindow()
end


function HideWndToTray(objUIElement)
	local objTree = objUIElement:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end

function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@TitleCtrl: " .. tostring(strLog))
	end
end
