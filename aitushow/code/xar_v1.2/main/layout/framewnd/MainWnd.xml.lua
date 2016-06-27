local Helper = XLGetGlobal("Helper")
local PathHelper = Helper.PathHelper


local iWindowPosXReg = "HKEY_CURRENT_USER\\Software\\kuaikan\\iWindowPosX"
local iWindowPosYReg = "HKEY_CURRENT_USER\\Software\\kuaikan\\iWindowPosY"
local iWindowPosDXReg = "HKEY_CURRENT_USER\\Software\\kuaikan\\iWindowPosDX"
local iWindowPosDYReg = "HKEY_CURRENT_USER\\Software\\kuaikan\\iWindowPosDY"
local sLastPathReg = "HKEY_CURRENT_USER\\Software\\kuaikan\\sLastPath"

function OnCreate(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root")
    local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	-- local workleft, worktop, workright, workbottom = Helper.tipUtil:GetWorkArea()
	-- local workWidth = workright - workleft
	-- local workHeigth = workbottom - worktop
	
	local iWindowPosX = UserConfig:Get("iMainWindowPosX")
	local iWindowPosY = UserConfig:Get("iMainWindowPosY")
	local iWindowPosDX = UserConfig:Get("iMainWindowPosDX")
	local iWindowPosDY = UserConfig:Get("iMainWindowPosDY")
	
	local workleft, worktop, workright, workbottom = Helper.tipUtil:GetWorkArea()
		
	if tonumber(iWindowPosX) and tonumber(iWindowPosY) and tonumber(iWindowPosDX) and tonumber(iWindowPosDY) 
		and iWindowPosX < workright and iWindowPosY < workbottom and iWindowPosDX > workleft and iWindowPosDY > worktop then
		self:Move(iWindowPosX, iWindowPosY, iWindowPosDX-iWindowPosX, iWindowPosDY-iWindowPosY)
	else
		--[[local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
		local nLayoutWidth = nLayoutR - nLayoutL
		local nLayoutHeight = nLayoutB - nLayoutT
		self:Move( math.floor((workright - nLayoutWidth)/2), math.floor((workbottom - nLayoutHeight)/2), nLayoutWidth, nLayoutHeight)]]--
		self:Max()--第一次最大化展示
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
	local lastPath = Helper:QueryRegValue(sLastPathReg)
	if lastPath then
		local addressobj = objtree:GetUIObject("MainWnd.AddressEditCtrl")
		local LeftPanel = objtree:GetUIObject("LeftPanel")
		addressobj:SetPath(lastPath, true)
		ImagePool:SetFolder(lastPath)
		LeftPanel:Update(lastPath)
	end
	local SliderCtrl = objtree:GetUIObject("MainWnd.SliderCtrl")
	local lastZoomPercent = UserConfig:Get("ThumbnailZoom", 10)
	SliderCtrl:SetValue(lastZoomPercent)
	
	local thumbnailContainer = objtree:GetUIObject("ThumbnailContainerObj")
	thumbnailContainer:Zoom(lastZoomPercent)
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
	
	leftTreeContainer:SetObjPos2("4", "33", x - 1, "father.height - 30 - 33")
	self:SetObjPos2(x+3, "33", "4", "father.height - 30 - 33")
	thumbnailContainer:SetObjPos2(x + 7, "33", "father.width - "..x.."-7-4", "father.height - 30 - 33")
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
	UserConfig:Set("ThumbnailZoom", pos)
end

local hasSend = false
function SendShowUI()
	if hasSend then return end
	hasSend = true
	--启动上报
	StatUtil.SendStat({
		strEC = "showui",
		strEA = StatUtil.GetMainVer(),
		strEL = "mainclient",
		strEV = 1,
	}) 
end

function OnShowWindow(self, bVisible)
	if bVisible then
		SendShowUI()
		Helper.Tray.HostWnd = self
	end
end

--左侧树选择事件
function LeftPanelOnSelect(self, event, dir)
	local realpath = PathHelper.GetRealPath(dir)
	if not realpath then
		return
	end
	local owner = self:GetOwner()
	local addressobj = owner:GetUIObject("MainWnd.AddressEditCtrl")
	addressobj:SetPath(realpath, true)
	ImagePool:SetFolder(realpath)
	
	Helper:SetRegValue(sLastPathReg, realpath)
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
	ImagePool:SetFolder(realpath)
	
	Helper:SetRegValue(sLastPathReg, realpath)
end

function HeadOnLButtonDown(self, x, y)
	local addressobj = self:GetObject("MainWnd.ThumbnailContainer:ThumbnailContainer.Header:MainWnd.AddressEditCtrl")
	local edit = addressobj:GetObject("EditObj")
	local l, t, r, b = edit:GetAbsPos()
	if x < l or x > r or y < t or y > b then
		addressobj:FocusChange(false)
	end
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
		LOG("OnSizeOnSize  not tree ")
		return
	end
	
	local rootObject = tree:GetRootObject()
	rootObject:SetObjPos(0, 0, width, height)
	
	if "max" == _type or "min" == _type then
		LOG("OnSizeOnSize  _type:  ", _type) -- or "restored" == _type 
		return
	end
	local x, y = self:HostWndPtToScreenPt(self:TreePtToHostWndPt(0, 0))
	
	UserConfig:Set("iMainWindowPosX", x)
	UserConfig:Set("iMainWindowPosY", y)
	UserConfig:Set("iMainWindowPosDX", x+width)
	UserConfig:Set("iMainWindowPosDY", y+height)
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

function OnClickLeftRotateButton(self)
	local ThumbnailContainerObj = self:GetOwner():GetUIObject("ThumbnailContainerObj")
	local attr = ThumbnailContainerObj:GetAttribute()
	if attr and attr.pageManager and attr.pageManager.selectedObj then
		attr.pageManager.selectedObj:Rotate(true)
	end
end

function OnClickRightRotateButton(self)
	local ThumbnailContainerObj = self:GetOwner():GetUIObject("ThumbnailContainerObj")
	local attr = ThumbnailContainerObj:GetAttribute()
	if attr and attr.pageManager and attr.pageManager.selectedObj then
		attr.pageManager.selectedObj:Rotate(false)
	end
end

function OnClickDeleteButton(self)
	local ThumbnailContainerObj = self:GetOwner():GetUIObject("ThumbnailContainerObj")
	local attr = ThumbnailContainerObj:GetAttribute()
	if attr and attr.pageManager and attr.pageManager.selectedObj then
		local obj = attr.pageManager.selectedObj
		local objattr = obj:GetAttribute()
		local filepath  = objattr.data.FilePath
		if filepath and Helper.tipUtil:QueryFileExists(filepath) then
			if Helper.Setting.IsDelRemind() then
				local HostWndHelper = Helper.MessageBox
				local showText = "确认要删除文件\"" .. Helper.APIproxy.GetFileNameFromPath(filepath) .. "\"吗"
				local nRet = HostWndHelper.MessageBox(showText, self:GetOwner():GetBindHostWnd())
				if nRet == HostWndHelper.ID_YES then
					Helper.tipUtil:DelPathFile2RecycleBin(filepath)
				end
			else
				Helper.tipUtil:DelPathFile2RecycleBin(filepath)
			end
		end
	end
end