local Helper = XLGetGlobal("Helper")


function GetObjWH(obj)
	local L, T, R, B = obj:GetObjPos()
	
	return R - L, B - T
end

function OnImageShowRectChange(self,ctrl, ...)
	local objtree = self:GetBindUIObjectTree() 
	local miniView = objtree:GetUIObject("FrameWnd.MiniViewer")
	local image = ctrl:GetControlObject("Image")
	local container = ctrl:GetControlObject("ImageContainer")
	
	local imageW,imageH = GetObjWH(image)
	local containerW,containerH = GetObjWH(container)
	if imageW > containerW or imageH > containerH then
		miniView:SetVisible(true)
		miniView:SetChildrenVisible(true)
		
		miniView:Init(image:GetBitmap())
		local rectL, rectT = ...
		local size_rates = ctrl:GetZoomPercent()
		miniView:Update(rectL, rectT, size_rates/100)
	else
		miniView:SetVisible(false)
		miniView:SetChildrenVisible(false)
	end
end

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

	local userData = self:GetUserData()
	local imageCtrl = objtree:GetUIObject("FrameWnd.ImageCtrl")
	if userData then
		imageCtrl:SetFolderData(userData)
	end
		
	imageCtrl:AttachListener("OnImageSizeChange", false, OnImageSizeChange)
	imageCtrl:SetFocus(true)
	
	Helper:AddDropTarget(self)
	local function OnDrop(file)
		imageCtrl:SetImagePath(file)
	end
	Helper:AddListener("OnDrop", function(_, _, file) OnDrop(file) end)
	
	local miniView = objtree:GetUIObject("FrameWnd.MiniViewer")
	imageCtrl:AttachListener("OnImageShowRectChange", false, function(ctrl,_, ...) OnImageShowRectChange(self,ctrl, ...) end)
end

function OnImageSizeChange(self, event, imageWith, imageHeight, picWidth, picHeight)
	local objTree = self:GetOwner()
	local titleCtrl = objTree:GetUIObject("FrameWnd.TitleCtrl")
	titleCtrl:SetTitleTextContent("缩放百分比为:"..tostring(math.round(100*imageWith/picWidth)).."%")
end

function OnImageMouseWheel(self, x, y, direction, distance)
	if not distance or 0 == distance then
		distance = 10
	end
	local curZoomPercent = self:GetZoomPercent()
	if direction > 0 then
		self:Zoom(curZoomPercent + distance)
	else
		curZoomPercent = curZoomPercent - distance
		self:Zoom(curZoomPercent > 10 and curZoomPercent or 10)
	end
end

function OnShowWindow(self, bVisible)
	if bVisible then
		Helper.Tray.HostWnd = self
	end
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

--工具栏容器进入事件
function ToolBarContainerOnMouseEnter(self, x, y)
	local toolbar = self:GetObject("ToolBar")
	toolbar:SetVisible(true)
	toolbar:SetChildrenVisible(true)
end

--工具栏容器离开事件
function ToolBarContainerOnMouseLeave(self, x, y)
	local toolbar = self:GetObject("ToolBar")
	toolbar:SetVisible(false)
	toolbar:SetChildrenVisible(false)
end

function OnToolBarCommand(self, _, cmd)	
	if "string" ~= type(cmd) then	
		return
	end
	local tree = self:GetOwner()
	local imageCtrl = tree:GetUIObject("FrameWnd.ImageCtrl")
	local curZoomPercent = imageCtrl:GetZoomPercent()
	if     "fangda" == cmd then
		imageCtrl:Zoom(curZoomPercent + 10)
		-- XLMessageBox("OnToolBarCommand cmd: "..tostring(cmd))

	elseif "suoxiao" == cmd then
		curZoomPercent = curZoomPercent - 10
		if curZoomPercent < 10 then
			curZoomPercent = 10
		end
		imageCtrl:Zoom(curZoomPercent)
	elseif "yibiyi" == cmd then
		imageCtrl:Zoom(100)
	elseif "xiayizhang" == cmd then
		local picData, nextIndex = imageCtrl:GetNextPic()
		if nextIndex then
			imageCtrl:SetImageByIndex(nextIndex)
		end
	elseif "shangyizhang" == cmd then
		local picData, prevIndex = imageCtrl:GetPrevPic()
		if prevIndex then
			imageCtrl:SetImageByIndex(prevIndex)
		end
	elseif "zuoxuanzhuan" == cmd then
		imageCtrl:RotateLeft()
	elseif "youxuanzhuan" == cmd then
		imageCtrl:RotateRight()
	elseif "shanchu" == cmd then
		imageCtrl:DelCurPic()
	else
	end
end