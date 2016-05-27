
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
	--local MSG = Helper.MessageBox
	--[[local nRet, bCheck = MSG.MessageBoxEx(objHostWnd)
	if MSG.ID_RENAMESAVE == nRet then
		XLMessageBox("ID_RENAMESAVE, bCheck = "..tostring(bCheck))
	elseif MSG.ID_COVEROLD == nRet then
		XLMessageBox("ID_COVEROLD, bCheck = "..tostring(bCheck))
	else
		XLMessageBox("ID_CANCEL bCheck = "..tostring(bCheck))
	end
	local nRet = MSG.MessageBox("您确定要删除这个图片吗？", objHostWnd)
	if MSG.ID_YES == nRet then
		XLMessageBox("ID_YES")
	else
		XLMessageBox("ID_CANCEL")
	end]]--
	--[[local MiniViewer = objTree:GetUIObject("MainWnd.MiniViewer")
	local xgf= XLGetObject("Xunlei.XLGraphic.Factory.Object")
	local bitmap = xgf:CreateBitmap("E:\\0104\\0107\\{DAC88B5E-C5B5-4D63-A58C-D7C8001B9E64}.png", "ARGB32")
	MiniViewer:Init(bitmap)
	MiniViewer:Update(100, 120, 2.8)]]--
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

function OnClickRestoreBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	if not objHostWnd then
		return
	end
	objHostWnd:Show(9)
end

function OnSize(self, stype)
	local ownerTree = self:GetBindUIObjectTree()
	local MaxBtn = ownerTree:GetUIObject("MainWnd.TitleCtrl:FrameWnd.Title.MaxBtn")
	local RestoreBtn = ownerTree:GetUIObject("MainWnd.TitleCtrl:FrameWnd.Title.RestoreBtn")
	
	if "max" == stype then
		RestoreBtn:Show(true)
		MaxBtn:Show(false)
	elseif "restored" == stype then
		MaxBtn:Show(true)
		RestoreBtn:Show(false)
	end
end

function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	if not objHostWnd then
		return
	end
	-- objHostWnd:Show(2)
	--直接退出
	--Helper.tipUtil:Exit()
	objHostWnd:Show(0)
	if not Helper.Setting.GetExitType() then
		Helper.tipUtil:Exit()
	end
end

function OnInitControl(self)
	local ownerTree = self:GetOwner()
	ownerTree:AttachListener("OnBindHostWnd", false, function(tree, hostwnd, isBind) 
		if isBind then
			hostwnd:AttachListener("OnSize", false, OnSize)
		end
	end)
end