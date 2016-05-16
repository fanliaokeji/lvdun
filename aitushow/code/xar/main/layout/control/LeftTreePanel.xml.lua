local tipUtil = XLGetObject("API.Util")
local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")

local PathHelper = {}
XLSetGlobal("PathHelper", PathHelper)
function PathHelper.GetDeskTopPath()
	local nCSIDL_DESKTOP = 0
	local bALnkExist = true
	local bCLnkExist = true
	local strPath = tipUtil:GetSpecialFolderPathEx(nCSIDL_DESKTOP)
	return tipUtil:FindDirList(strPath)
end

function PathHelper.GetDocumentPath()
	local nCSIDL_PERSONAL = 5
	local bALnkExist = true
	local bCLnkExist = true
	local strPath = tipUtil:GetSpecialFolderPathEx(nCSIDL_PERSONAL)
	return tipUtil:FindDirList(strPath)
end

function PathHelper.GetPicturePath()
	local nCSIDL_MYPICTURES = 0x27
	local bALnkExist = true
	local bCLnkExist = true
	local strPath = tipUtil:GetSpecialFolderPathEx(nCSIDL_MYPICTURES)
	return tipUtil:FindDirList(strPath)
end

function PathHelper.GetDiskList()
	local tAll = {"C:", "D:", "E:", "F:", "G", "H:", "I:", "J:", "K:", "L:", "M:", "N:"}
	local tDisk = {}
	for _, v in ipairs(tAll) do
		if tipUtil:QueryFileExists(v) then
			tDisk[#tDisk+1] = v
		else
			break
		end
	end
	return tDisk
end

PathHelper.SpecialName = {
	["C:"] = "本地磁盘(C:)",
	["D:"] = "本地磁盘(D:)",
	["E:"] = "本地磁盘(E:)",
	["F:"] = "本地磁盘(F:)",
	["G:"] = "本地磁盘(G:)",
	["H:"] = "本地磁盘(H:)",
	["I:"] = "本地磁盘(I:)",
	["J:"] = "本地磁盘(J:)",
	["K:"] = "本地磁盘(K:)",
	["L:"] = "本地磁盘(L:)",
	["M:"] = "本地磁盘(M:)",
	["N:"] = "本地磁盘(N:)",
}
function Dir2TreeView(self, dir, left, params)
	local panelattr = self:GetAttribute()
	panelattr.nodeindex = panelattr.nodeindex or 1
	panelattr.opendirs = panelattr.opendirs or {}
	params = params or {}
	local dirlist = params.path or tipUtil:FindDirList(dir)
	
	local Item = objFactory:CreateUIObject("lefttreenode"..panelattr.nodeindex, "LeftTreeItem")
	Item:SetObjPos2(left, 26*(panelattr.nodeindex-1), "father.width", 22)
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
	else
		attr.Select = false
	end
	local maintext = Item:GetControlObject("MainText")
	maintext:SetText(PathHelper.SpecialName[dir] or string.match(dir, "([^/\\]*)$") or "未知名称")
	Item:Update()
	Item:AttachListener("OnStateChange", false, function(_self, event, bState)
			panelattr.opendirs[dir] = bState
			ClearTree(self)
			BuildTree(self)
		end)
	Item:AttachListener("OnSelect", false, function()
			if attr.HasChild then
				panelattr.opendirs[dir] = not panelattr.opendirs[dir]
			end
			panelattr.selectdir = dir
			ClearTree(self)
			BuildTree(self)
		end)
	local Container = self:GetControlObject("Container")
	Container:SetObjPos(0, 0, "father.width", 26*(panelattr.nodeindex-1)+22)
	Container:AddChild(Item)
	ResetScrollBar(self)
end

function ClearTree(self)
	local Container = self:GetControlObject("Container")
	Container:RemoveAllChild()
	local panelattr = self:GetAttribute()
	panelattr.nodeindex = 1
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
end

function LeftTreePanelOnInitControl(self)
	BuildTree(self)
end

function ResetScrollBar(objRootCtrl)
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
		OnScrollMousePosEvent(objScrollBar)
		fatherctrl:SetObjPos(l, t, "father.width-8", "father.height-51")
	else
		objScrollBar:SetScrollPos(0, true)	
		objScrollBar:SetVisible(false)
		objScrollBar:SetChildrenVisible(false)
		MoveItemListPanel(objRootCtrl, 0)
		fatherctrl:SetObjPos(l, t, "father.width", "father.height-51")
		return true
	end
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
