local Helper = XLGetGlobal("Helper")
local PathHelper = Helper.PathHelper
local Tray = Helper.Tray


local iWindowPosXReg = "HKEY_CURRENT_USER\\Software\\kuaikan\\iWindowPosX"
local iWindowPosYReg = "HKEY_CURRENT_USER\\Software\\kuaikan\\iWindowPosY"
local iWindowPosDXReg = "HKEY_CURRENT_USER\\Software\\kuaikan\\iWindowPosDX"
local iWindowPosDYReg = "HKEY_CURRENT_USER\\Software\\kuaikan\\iWindowPosDY"

function OnCreate(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root")
    local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	-- local workleft, worktop, workright, workbottom = Helper.tipUtil:GetWorkArea()
	-- local workWidth = workright - workleft
	-- local workHeigth = workbottom - worktop
	
	local iWindowPosX = Helper:QueryRegValue(iWindowPosXReg)
	local iWindowPosY = Helper:QueryRegValue(iWindowPosYReg)
	local iWindowPosDX = Helper:QueryRegValue(iWindowPosDXReg)
	local iWindowPosDY = Helper:QueryRegValue(iWindowPosDYReg)
	
	local workleft, worktop, workright, workbottom = Helper.tipUtil:GetWorkArea()
		
	if tonumber(iWindowPosX) and tonumber(iWindowPosY) and tonumber(iWindowPosDX) and tonumber(iWindowPosDY) 
		and iWindowPosX < workright and iWindowPosY < workbottom and iWindowPosDX > workleft and iWindowPosDY > worktop then
		self:Move(iWindowPosX, iWindowPosY, iWindowPosDX-iWindowPosX, iWindowPosDY-iWindowPosY)
	else
		local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
		local nLayoutWidth = nLayoutR - nLayoutL
		local nLayoutHeight = nLayoutB - nLayoutT
		self:Move( math.floor((workright - nLayoutWidth)/2), math.floor((workbottom - nLayoutHeight)/2), nLayoutWidth, nLayoutHeight)
	end
	
	-- self:Move( math.floor((workWidth - nLayoutWidth) / 2), math.floor((workHeigth - nLayoutHeight) / 2), nLayoutWidth, nLayoutHeight)

	-- local imageCtrl = objtree:GetUIObject("FrameWnd.ImageCtrl")
	-- imageCtrl:SetImagePath("C:\\Users\\mjt\\Pictures\\Pictures\\test2.png")
	-- local titleCtrl = objtree:GetUIObject("FrameWnd.TitleCtrl")
	-- titleCtrl:SetTitleTextContent("test2.png")
	
	-- local imageCtrl = objtree:GetUIObject("FrameWnd.ImageCtrl")
	-- imageCtrl:AttachListener("OnImageSizeChange", false, OnImageSizeChange)
	
	-- Helper:AddDropTarget(self)
	-- local function OnDrop(file)
		-- imageCtrl:SetImagePath(file)
	-- end
	-- Helper:AddListener("OnDrop", function(_, _, file) OnDrop(file) end)
	Tray.Init(self)
end

local isLButtonDown = false
function OnLButtonDown_ResizeLine(self, x, y)
	self:SetCaptureMouse(true)
	isLButtonDown = true
end

function OnMouseMove_ResizeLine(self, x, y)
	self:SetCursorID("IDC_SIZEWE")
	if not isLButtonDown then
		return
	end
	
	local owner = self:GetOwner()
	local wnd = owner:GetBindHostWnd()
	local x, y = wnd:ScreenPtToHostWndPt(Helper.tipUtil:GetCursorPos())
	x, y = wnd:HostWndPtToTreePt(x, y)
	
	--最大、最小限制
	if x > 400 or x < 100 then
		-- Helper:Assert(false, "x: "..tostring(x))
		return
	end
	local leftTreeContainer = owner:GetUIObject("MainWnd.LeftTreeContainer")
	local thumbnailContainer = owner:GetUIObject("MainWnd.ThumbnailContainer")
	
	leftTreeContainer:SetObjPos2("0", "30", x, "father.height - 30 - 30")
	self:SetObjPos2(x, "30", "2", "father.height - 30 - 30")
	thumbnailContainer:SetObjPos2(x, "30", "father.width - "..x, "father.height - 30 - 30")
end

function OnLButtonUp_ResizeLine(self, x, y)
	self:SetCursorID("IDC_SIZEWE")
	self:SetCaptureMouse(false)
	isLButtonDown = false
end

function OnSliderPosChange(self, event, pos)
	local ownerTree = self:GetOwner()
	local thumbnailContainer = ownerTree:GetUIObject("ThumbnailContainerObj")
	thumbnailContainer:Zoom(pos)
end

function OnShowWindow(self, bVisible)
	
end

--左侧树选择事件
function LeftPanelOnSelect(self, event, dir)
	local realpath = PathHelper.GetRealPath(dir)
	if not realpath then
		return
	end
	local owner = self:GetOwner()
	local addressobj = owner:GetUIObject("MainWnd.AddressEditCtrl")
	local thumbContainerObj = owner:GetUIObject("ThumbnailContainerObj")
	addressobj:SetPath(realpath, true)
	thumbContainerObj:SetFolder(realpath)
end

--地址栏变化事件
function AddressEditCtrlOnPathChanged(self, event, dir)
	local realpath = PathHelper.GetRealPath(dir)
	if not realpath then
		return
	end
	local owner = self:GetOwner()
	local LeftPanel = owner:GetUIObject("LeftPanel")
	LeftPanel:Update(realpath)
end

function OnClickSortButton(self)
	local tree = self:GetOwner()
	local wnd = tree:GetBindHostWnd()
	local wndRectL, wndRectT, wndRectR, wndRectB = wnd:GetWindowRect()
	local curX = wndRectR - 160
	local curY = wndRectT + 100
	local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")	
	local menuTable = GreenShieldMenu.SortMenu.menuTable
	local menuFunTable = GreenShieldMenu.SortMenu.menuFunTable
	
	local userData = {}
	userData.thumbContainerObj = tree:GetUIObject("ThumbnailContainerObj")
	-- Helper:CreateMenu(curX, curY, wnd:GetWndHandle(), menuTable, menuFunTable)
	-- if not userData.thumbContainerObj then XLMessageBox("Not thumbContainerObj") end
	
	local menuItemTemplID = "menu.common.item.template"
	local menuItemContainerTemplID = "menu.combobox.container.template"
    Helper:CreateMenuEx(curX, curY, wnd:GetWndHandle(), menuTable, menuFunTable, userData, menuItemTemplID, menuItemContainerTemplID)

end

function OnSize(self, _type, width, height)
	local tree = self:GetBindUIObjectTree()
	if not tree then
		return
	end
	
	local rootObject = tree:GetRootObject()
	rootObject:SetObjPos(0, 0, width, height)
	local x, y = self:HostWndPtToScreenPt(self:TreePtToHostWndPt(0, 0))
	Helper:SetRegValue(iWindowPosXReg, x)
	Helper:SetRegValue(iWindowPosYReg, y)
	Helper:SetRegValue(iWindowPosDXReg, x+width)
	Helper:SetRegValue(iWindowPosDYReg, y+height)
end

function OnMove(self)
	local state = self:GetWindowState()
	if "max" == state or "min" == state then
		return
	end
	local wndleft,wndtop,wndright,wndbottom = self:GetWindowRect()
	local wndwidth = wndright - wndleft
	local wndheight = wndbottom - wndtop
		
	local x, y = self:HostWndPtToScreenPt(self:TreePtToHostWndPt(0, 0))
	
	Helper:SetRegValue(iWindowPosXReg, x)
	Helper:SetRegValue(iWindowPosYReg, y)
	Helper:SetRegValue(iWindowPosDXReg, x+wndwidth)
	Helper:SetRegValue(iWindowPosDYReg, y+wndheight)
end
