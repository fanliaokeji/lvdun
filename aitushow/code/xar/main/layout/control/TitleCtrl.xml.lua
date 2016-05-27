
function SetTitleTextContent(self, text)
	local titleText = self:GetControlObject("FrameWnd.Title.TitleText")
	titleText:SetText(text)
end

function OnClickSetting(self)
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
	self:Show(false)
	local ownerCtrl = self:GetOwnerControl()
	local RestoreBtn = ownerCtrl:GetControlObject("FrameWnd.Title.RestoreBtn")
	RestoreBtn:Show(true)
end

function OnClickRestoreBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	if not objHostWnd then
		return
	end
	objHostWnd:Show(9)
	self:Show(false)
	local ownerCtrl = self:GetOwnerControl()
	local MaxBtn = ownerCtrl:GetControlObject("FrameWnd.Title.MaxBtn")
	MaxBtn:Show(true)
end

function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	if not objHostWnd then
		return
	end
	local imgctrl = Helper.Selector.select("", "FrameWnd.ImageCtrl", "ImageWnd.Instance")
	local Setting = Helper.Setting
	if imgctrl then
		local attr = imgctrl:GetAttribute()
		attr.index = attr.index or 0
		attr.tPictures = attr.tPictures or {}
		if attr.tPictures[attr.index] and attr.tPictures[attr.index].angle and attr.tPictures[attr.index].angle%360 ~= 0 then
			local MSG = Helper.MessageBox
			local rtype = Setting.GetRotateType()
			if rtype == "askme" then
				local nRet, bCheck = MSG.MessageBoxEx(objHostWnd)
				if MSG.ID_RENAMESAVE == nRet then
					if bCheck then
						Setting.SetRotateType("rensave")
					end
				elseif MSG.ID_COVEROLD == nRet then
					if bCheck then
						Setting.SetRotateType("coverold")
					end
				else
					if bCheck then
						Setting.SetRotateType("nosave")
					end
				end
			elseif rtype == "rensave" then
			elseif rtype == "coverold" then
			else
			end
		end
	end
	--Helper:DestoryModelessWnd("ImageWnd")
	objHostWnd:Show(0)
	local MainHostWnd = Helper.Selector.select("", "", "MainWnd.Instance")
	if MainHostWnd then
		MainHostWnd:BringWindowToTop(true)
	else
		if not Setting.GetExitType() then
			Helper.Tray.Hide()
			Helper.tipUtil:Exit()
		end
	end
end

function OnSize(self, stype)
	local ownerTree = self:GetBindUIObjectTree()
	local MaxBtn = ownerTree:GetUIObject("FrameWnd.TitleCtrl:FrameWnd.Title.MaxBtn")
	local RestoreBtn = ownerTree:GetUIObject("FrameWnd.TitleCtrl:FrameWnd.Title.RestoreBtn")
	
	if "max" == stype then
		RestoreBtn:Show(true)
		MaxBtn:Show(false)
	elseif "restored" == stype then
		MaxBtn:Show(true)
		RestoreBtn:Show(false)
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