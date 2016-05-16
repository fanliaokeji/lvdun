local Helper = XLGetGlobal("Helper")

function OnCreate(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root")
    local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = Helper.tipUtil:GetWorkArea()
	local workWidth = workright - workleft
	local workHeigth = workbottom - worktop
	self:Move( math.floor((workWidth - nLayoutWidth) / 2), math.floor((workHeigth - nLayoutHeight) / 2), nLayoutWidth, nLayoutHeight)

	local imageCtrl = objtree:GetUIObject("FrameWnd.ImageCtrl")
	imageCtrl:SetImagePath("C:\\Users\\mjt\\Pictures\\Pictures\\test2.png")
	local titleCtrl = objtree:GetUIObject("FrameWnd.TitleCtrl")
	titleCtrl:SetTitleTextContent("test2.png")
	
	local imageCtrl = objtree:GetUIObject("FrameWnd.ImageCtrl")
	imageCtrl:AttachListener("OnImageSizeChange", false, OnImageSizeChange)
	
	Helper:AddDropTarget(self)
	local function OnDrop(file)
		imageCtrl:SetImagePath(file)
	end
	Helper:AddListener("OnDrop", function(_, _, file) OnDrop(file) end)
end

function OnImageSizeChange(self, event, imageWith, imageHeight, picWidth, picHeight)
	local objTree = self:GetOwner()
	local titleCtrl = objTree:GetUIObject("FrameWnd.TitleCtrl")
	titleCtrl:SetTitleTextContent("缩放百分比为:"..tostring(math.round(100*imageWith/picWidth)).."%")
end

function OnShowWindow(self, bVisible)
	
end

--退出全屏模式，进入在TitleCtrl里处理
function OnClickCloseBtnWhileFullScreen(self)
	local objTree = self:GetOwner()
	
	--------------------需要恢复的元素--------------------
	--标题栏
	local titleCtrl = objTree:GetUIObject("FrameWnd.TitleCtrl")
	titleCtrl:SetObjPos2(0, 0, "father.width", 30)
	titleCtrl:SetVisible(true)
	titleCtrl:SetChildrenVisible(true)
	local imageCtrl = objTree:GetUIObject("FrameWnd.ImageCtrl")
	imageCtrl:SetObjPos2(3, 30, "father.width - 6", "father.height - 33")
	
	--resizeCtrl
	local resizeCtrl = objTree:GetUIObject("FrameWnd.ResizeFrameWndCtrl")
	resizeCtrl:SetEnable(true)
	resizeCtrl:SetChildrenVisible(true)
	--大背景
	local rootObj = objTree:GetUIObject("root")
	rootObj:SetSrcColor("RGBA(255,255,255,255)")
	
	--------------------需要隐藏的元素--------------------
	--半透明阴影
	local transparentMask = objTree:GetUIObject("TransparentMask")
	transparentMask:SetVisible(false)
	transparentMask:SetChildrenVisible(false)
	--退出全屏按钮
	self:SetVisible(false)
	self:SetChildrenVisible(false)
	
	--恢复窗口位置
	local objHostWnd = objTree:GetBindHostWnd()
	local workleft, worktop, workright, workbottom = Helper.tipUtil:GetWorkArea()
	local workWidth = workright - workleft
	local workHeigth = workbottom - worktop
	objHostWnd:Move( math.floor((workWidth - 875) / 2), math.floor((workHeigth - 515) / 2), 875, 515)
end
