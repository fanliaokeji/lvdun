
function SetTitleTextContent(self, text)
	local titleText = self:GetControlObject("FrameWnd.Title.TitleText")
	titleText:SetText(text)
end

function OnClickSetting(self)
	--[[local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	local imageCtrl = objTree:GetUIObject("FrameWnd.ImageCtrl")
	local curZoomPercent = imageCtrl:GetZoomPercent()
	imageCtrl:Zoom(curZoomPercent + 10)]]--
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	Helper:CreateModalWnd("SettingWnd","SettingWndTree", objHostWnd)
end

--进入全屏模式，退出在FrameWnd里处理
function OnClickFullScreen(self)
	local objTree = self:GetOwner()
	
	--------------------需要消失的元素--------------------
	--标题栏
	local titleCtrl = objTree:GetUIObject("FrameWnd.TitleCtrl")
	titleCtrl:SetObjPos2(0, 0, "father.width", 0)
	titleCtrl:SetVisible(false)
	titleCtrl:SetChildrenVisible(false)
	local imageCtrl = objTree:GetUIObject("FrameWnd.ImageCtrl")
	imageCtrl:SetObjPos2(0, 0, "father.width", "father.height")
	--resizeCtrl
	local resizeCtrl = objTree:GetUIObject("FrameWnd.ResizeFrameWndCtrl")
	resizeCtrl:SetEnable(false)
	resizeCtrl:SetChildrenVisible(false)
	--大背景
	local rootObj = objTree:GetUIObject("root")
	rootObj:SetSrcColor("RGBA(0,0,0,0)")
	
	--------------------需要出现的元素--------------------
	--半透明阴影
	local transparentMask = objTree:GetUIObject("TransparentMask")
	transparentMask:SetVisible(true)
	transparentMask:SetChildrenVisible(true)
	transparentMask:SetDestColor("RGBA(0,0,0,180)")
	transparentMask:SetSrcColor("RGBA(0,0,0,180)")
	--退出全屏按钮
	local closeBtnWhileFullScreen = objTree:GetUIObject("FrameWnd.CloseBtnWhileFullScreen")
	closeBtnWhileFullScreen:SetVisible(true)
	closeBtnWhileFullScreen:SetChildrenVisible(true)
	
	--记录窗口大小
	--将窗口设成全屏大小
	local objHostWnd = objTree:GetBindHostWnd()
	local workleft, worktop, workright, workbottom = Helper.tipUtil:GetWorkArea()
	objHostWnd:Move(workleft, worktop, workright - workleft, workbottom - worktop)
end

function OnClickMinBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	if not objHostWnd then
		return
	end
	objHostWnd:Show(2)
end

function OnClickMaxBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	if not objHostWnd then
		return
	end
	objHostWnd:Show(3)
end

function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	if not objHostWnd then
		return
	end
end