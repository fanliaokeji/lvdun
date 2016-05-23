local tipUtil = XLGetObject("API.Util")
local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
local Helper = XLGetGlobal("Helper")
local PathHelper = Helper.PathHelper

function openparent(attr, dir)
	dir = string.gsub(dir, "([/\\])$", "")
	local parent = string.match(dir, "^(.*)[/\\][^/\\]+$")
	if parent then
		attr.opendirs[parent] = true
		openparent(attr, parent)
	end
end

function Filter(dir)
	if not dir then return end
	dir = string.gsub(dir, "[/\\]$", "")
	return string.upper(dir)
end

function Update(self, dir)
	dir = Filter(dir)
	local attr = self:GetAttribute()
	if attr.selectdir == dir then
		return
	end
	
	--无条件关闭其它分支
	attr.opendirs = {}
	local dirlist = tipUtil:FindDirList(dir)
	if #dirlist > 0 then
		attr.opendirs[dir] = not attr.opendirs[dir]
	end
	
	local vpath  = PathHelper.GetVrPath(dir)
	if vpath == "计算机" then
		if string.find(dir, "^%a:\\USERS") or string.find(dir, "^%a:\\DOCUMENTS AND SETTINGS") then
			return 
		end
	end
	attr.opendirs[vpath] = true
	openparent(attr, dir)
	attr.selectdir = dir
	ClearTree(self)
	BuildTree(self)
end

function Dir2TreeView(self, dir, left, params)
	dir = Filter(dir)
	local panelattr = self:GetAttribute()
	panelattr.nodeindex = panelattr.nodeindex or 1
	panelattr.saveleft = panelattr.saveleft or left
	if panelattr.saveleft < left then
		panelattr.saveleft = left
	end
	panelattr.opendirs = panelattr.opendirs or {}
	params = params or {}
	local dirlist = params.path or tipUtil:FindDirList(dir)
	
	local Item = objFactory:CreateUIObject("lefttreenode"..panelattr.nodeindex, "LeftTreeItem")
	Item:SetObjPos2(left, 26*(panelattr.nodeindex-1), 500, 22)
	panelattr.nodeindex = panelattr.nodeindex + 1
	
	local attr = Item:GetAttribute()
	if #dirlist == 0 then
		attr.HasChild = false
	else
		attr.HasChild = true
	end
	if params["MainIcon"] and params["MainIconHover"] then
		attr.MainIcon = params["MainIcon"]
		attr.MainIconHover = params["MainIconHover"]
	else
		attr.MainIcon = "MainIconFoder"
		attr.MainIconHover = "MainIconFoderHover"
	end

	if panelattr.opendirs[dir] then
		attr.Open = true
		for _, v in ipairs(dirlist) do
			Dir2TreeView(self, v, left+16, (type(params.params) == "table" and params.params or nil))
		end
	else
		attr.Open = false
	end
	if panelattr.selectdir == dir then
		attr.Select = true
		panelattr.SelectItem = Item
	else
		attr.Select = false
	end
	local maintext = Item:GetControlObject("MainText")
	maintext:SetText(PathHelper.SpecialName[dir] or string.match(dir, "([^/\\]*)$") or "未知名称")
	Item:Update()
	Item:AttachListener("OnStateChange", false, function(_self, event, bState)
			panelattr.opendirs[dir] = bState
			ClearTree(self, dir)
			BuildTree(self)
		end)
	Item:AttachListener("OnSelect", false, function()
			if attr.HasChild then
				panelattr.opendirs[dir] = not panelattr.opendirs[dir]
			end
			if panelattr.selectdir ~= dir then
				panelattr.selectdir = dir
				self:FireExtEvent("OnSelect", dir)
			end
			ClearTree(self, dir)
			BuildTree(self)
		end)
	local Container = self:GetControlObject("Container")
	--local conl, _, conr = self:GetControlObject("ContainerBox"):GetObjPos()
	Container:SetObjPos(0, 0, "father.width+"..panelattr.saveleft , 26*(panelattr.nodeindex-1)+22)
	Container:AddChild(Item)
end

--根据dir决定是否超过一定数量清除其它分支
function ClearTree(self, dir)
	local Container = self:GetControlObject("Container")
	Container:RemoveAllChild()
	local panelattr = self:GetAttribute()
	--节点太多会导致卡顿，so节点大于一定值时清除其它打开的分支
	if dir and panelattr.opendirs[dir] and panelattr.nodeindex > 50 then
		panelattr.opendirs = {}
		collectgarbage("collect")
		panelattr.opendirs[dir] = true
		local vpath  = PathHelper.GetVrPath(dir)
		panelattr.opendirs[vpath] = true
		openparent(panelattr, dir)
	end
	panelattr.saveleft = 0
	panelattr.nodeindex = 1
	panelattr.SelectItem = nil
end

function BuildTree(self)
	local tPaths = {
		{"桌面", {MainIcon="MainIconDesk", MainIconHover="MainIconDeskHover", path=PathHelper.GetDeskTopPath()},},
		{"我的文档", {MainIcon="MainIconDocument", MainIconHover="MainIconDocumentHover", path=PathHelper.GetDocumentPath()}},
		{"我的图片", {MainIcon="MainIconPicture", MainIconHover="MainIconPictureHover", path=PathHelper.GetPicturePath()}},
		{"计算机", {MainIcon="MainIconComputer", MainIconHover="MainIconComputerHover", path=PathHelper.GetDiskList(),params={MainIcon="MainIconDisk", MainIconHover="MainIconDiskHover"}},},
	}
	for _, v in ipairs(tPaths) do
		Dir2TreeView(self, v[1], 0, v[2])
	end
	ResetScrollBarV(self)
	ResetScrollBarH(self)
end

function LeftTreePanelOnInitControl(self)
	BuildTree(self)
end

function RouteToFather(self)
	self:RouteToFather()
end

function ContainerOnMouseWheel(self, x, y, distance)
	local Vscroll = self:GetObject("control:listbox.vscroll")
	local Hscroll = self:GetObject("control:listbox.hscroll")
	if Vscroll:GetVisible() then
		OnScrollBarMouseWheel(Vscroll, "",  x, y, distance )
	elseif Hscroll:GetVisible() then
		OnScrollBarMouseWheelH(Hscroll, "",  x, y, distance )
	end
end

function LeftTreePanelOnPosChange(self)
	local VScroll = self:GetObject("listbox.vscroll")
	local HScroll = self:GetObject("listbox.hscroll")
	--local v_visible, h_visible = VScroll:GetVisible(), HScroll:GetVisible()
	--if v_visible then
	ResetScrollBarV(self)
	--end
	--if h_visible then
	ResetScrollBarH(self)
	--end
end

function AdjustScrolPos(self)
	local VScroll = self:GetObject("listbox.vscroll")
	local HScroll = self:GetObject("listbox.hscroll")
	local ContainerBox = self:GetObject("ContainerBox")
	local v_visible, h_visible = VScroll:GetVisible(), HScroll:GetVisible()
	if not v_visible and h_visible then
		ContainerBox:SetObjPos(0, 51, "father.width-1", "father.height")
		return
	end
	local offsetX, offsetY = 0, 0
	if v_visible then
		offsetX = 9
	end
	if h_visible then
		offsetY = 9
	end
	ContainerBox:SetObjPos(0, 51, "father.width-1-"..offsetX, "father.height-"..offsetY)
	VScroll:SetObjPos("father.width - 9", 51, "father.width", "father.height-"..offsetY)
	HScroll:SetObjPos(0, "father.height-9", "father.width-"..offsetX, "father.height")
end

-----------------------------------
--竖向滚动条
-----------------------------------
function ResetScrollBarV(objRootCtrl)
	if objRootCtrl == nil then
		return false
	end
	local objScrollBar = objRootCtrl:GetControlObject("listbox.vscroll")
	local fatherctrl = 	objRootCtrl:GetControlObject("ContainerBox")
	local Container = 	objRootCtrl:GetControlObject("Container")
	local attr = objRootCtrl:GetAttribute()
	local _, _, _, cb = Container:GetObjPos()
	local l, t, r, b = fatherctrl:GetObjPos()
	
	if cb > b-t then
		objScrollBar:SetScrollRange( 0, cb-b+t, true )
		objScrollBar:SetPageSize(b-t, true)
		objScrollBar:SetVisible(true)
		objScrollBar:SetChildrenVisible(true)
		objScrollBar:Show(true)
		--OnScrollMousePosEvent(objScrollBar)
		
		--移动到选中的节点
		local olddis = cb-b+t
		local newdis = olddis
		if attr.SelectItem then
			local sl, st, sr, sb = attr.SelectItem:GetObjPos()
			--当前选中的不在屏幕中间时
			if sb > b-t then
				--将选中的移动到底部
				newdis = sb-b+t
				--再判断是否可以移动到中间
				if olddis - newdis > (b-t)/2 then
					newdis = newdis + (b-t)/2
				else
					newdis = olddis
				end
			else
				newdis = 0
			end
		end
		MoveItemListPanel(objRootCtrl, newdis)
		objScrollBar:SetScrollPos(newdis, true)
	else
		objScrollBar:SetScrollPos(0, true)	
		objScrollBar:SetVisible(false)
		objScrollBar:SetChildrenVisible(false)
		MoveItemListPanel(objRootCtrl, 0)
	end
	AdjustScrolPos(objRootCtrl)
	return true
end

function MoveItemListPanel(objRootCtrl, nScrollPos)
	if not objRootCtrl then
		return
	end
	
	local objContainer = objRootCtrl:GetControlObject("Container")
	if not objContainer then
		return
	end
	
	local nL, nT, nR, nB = objContainer:GetObjPos()
	local nHeight = nB-nT
	local nNewT = 0-nScrollPos
	
	objContainer:SetObjPos(nL, nNewT, nR, nNewT+nHeight)
end

function OnScrollBarMouseWheel(self, name, x, y, distance)
	local objRootCtrl = self:GetOwnerControl()
	local nScrollPos = self:GetScrollPos()	
    if distance > 0 then
		self:SetScrollPos( nScrollPos - 44, true )
    else		
		self:SetScrollPos( nScrollPos + 44, true )
    end

	local nNewScrollPos = self:GetScrollPos()
	MoveItemListPanel(objRootCtrl, nNewScrollPos)
	return true	
end

function OnScrollMousePosEvent(self)
	local objRootCtrl = self:GetOwnerControl()
	local nScrollPos = self:GetScrollPos()
	
	MoveItemListPanel(objRootCtrl, nScrollPos)
end

function OnVScroll(self, fun, _type, pos)
	local objRootCtrl = self:GetOwnerControl()
	local nScrollPos = self:GetScrollPos()	
	--点击向上按钮或上方空白
    if _type ==1 then
        self:SetScrollPos( nScrollPos - 44, true )
	--点击向下按钮或下方空白
    elseif _type==2 then
		self:SetScrollPos( nScrollPos + 44, true )
    end

	local nNewScrollPos = self:GetScrollPos()
	MoveItemListPanel(objRootCtrl, nNewScrollPos)
	return true
end

----------------------------------------------
--以下是横向滚动条
----------------------------------------------
function ResetScrollBarH(objRootCtrl)
	if objRootCtrl == nil then
		return false
	end
	local objScrollBar = objRootCtrl:GetControlObject("listbox.hscroll")
	local ContainerBox = 	objRootCtrl:GetControlObject("ContainerBox")
	local Container = 	objRootCtrl:GetControlObject("Container")
	local attr = objRootCtrl:GetAttribute()
	local cl, _, cr = Container:GetObjPos()
	local l, t, r, b = ContainerBox:GetObjPos()
	local olddis = cr-r+l
	if attr.SelectItem then
		local sl = attr.SelectItem:GetObjPos()
		local maxlen = attr.SelectItem:GetObject("MainText"):GetTextExtent()
		--靠右
		olddis = sl+32+maxlen-r+l
	end
	
	
	if olddis > 0 then
		objScrollBar:SetScrollRange( 0, olddis, true )
		objScrollBar:SetPageSize(r-l, true)
		objScrollBar:SetVisible(true)
		objScrollBar:SetChildrenVisible(true)
		objScrollBar:Show(true)
		--OnScrollMousePosEvent(objScrollBar)
		
		--当ITEM的宽度大于外框的宽度时 让他靠左，否则靠右
		local newdis = olddis
		if attr.SelectItem then
			local sl = attr.SelectItem:GetObjPos()
			if newdis > sl then
				--不完全贴住左侧
				newdis = sl
			end
		end
		
		MoveItemListPanelH(objRootCtrl, newdis)
		objScrollBar:SetScrollPos(newdis, true)
	else
		objScrollBar:SetScrollPos(0, true)	
		objScrollBar:SetVisible(false)
		objScrollBar:SetChildrenVisible(false)
		MoveItemListPanelH(objRootCtrl, 0)
	end
	AdjustScrolPos(objRootCtrl)
	return true
end

function MoveItemListPanelH(objRootCtrl, nScrollPos)
	if not objRootCtrl then
		return
	end
	
	local objContainer = objRootCtrl:GetControlObject("Container")
	if not objContainer then
		return
	end
	
	local nL, nT, nR, nB = objContainer:GetObjPos()
	local nWidth = nR-nL
	local nNewL = 0-nScrollPos
	
	objContainer:SetObjPos(nNewL, nT, nNewL+nWidth, nB)
end

function OnScrollBarMouseWheelH(self, name, x, y, distance)
	local objRootCtrl = self:GetOwnerControl()
	local nScrollPos = self:GetScrollPos()	
    if distance > 0 then
		self:SetScrollPos( nScrollPos - 44, true )
    else		
		self:SetScrollPos( nScrollPos + 44, true )
    end

	local nNewScrollPos = self:GetScrollPos()
	MoveItemListPanelH(objRootCtrl, nNewScrollPos)
	return true	
end

function OnScrollMousePosEventH(self)
	local objRootCtrl = self:GetOwnerControl()
	local nScrollPos = self:GetScrollPos()
	
	MoveItemListPanelH(objRootCtrl, nScrollPos)
end

function OnHScroll(self, fun, _type, pos)
	local objRootCtrl = self:GetOwnerControl()
	local nScrollPos = self:GetScrollPos()	
	--点击向左按钮或上方空白
    if _type ==1 then
        self:SetScrollPos( nScrollPos - 44, true )
	--点击向右按钮或下方空白
    elseif _type==2 then
		self:SetScrollPos( nScrollPos + 44, true )
    end

	local nNewScrollPos = self:GetScrollPos()
	MoveItemListPanelH(objRootCtrl, nNewScrollPos)
	return true
end
