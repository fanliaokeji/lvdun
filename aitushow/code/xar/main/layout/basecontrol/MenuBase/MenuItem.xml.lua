local Helper = XLGetGlobal("Helper")

local function UpdateUI(self)
	local attr = self:GetAttribute()
	if nil == attr or nil == attr.itemData then
		return
	end
	
	SetText(self, attr.itemData.text)
	local textObj = self:GetControlObject("menu.item.text")
	local arrow = self:GetControlObject("menu.item.arrow")
	local icon = self:GetControlObject("menu.item.icon")
	
	if attr.bEnable then
		textObj:SetTextColorResID(attr.NormalTextColor)
		--图标资源ID来自于menuTable,因为给个Item的icon都是不同的
		--菜单的创建者应保证，该ID在运行时是有效的
		icon:SetResID(attr.itemData.iconNormalID or "")
		if attr.itemData.SubMenuTable then
			arrow:SetResID(attr.SubMenuArrowNormalBkgID)
		end
	else
		textObj:SetTextColorResID(attr.DisableTextColor)
		icon:SetResID(attr.itemData.iconDisableID or "")
		if attr.itemData.SubMenuTable then
			arrow:SetResID(attr.SubMenuArrowDisableBkgID or "")
		end
	end
end

local function InitAttrData(self)
	local attr = self:GetAttribute()
	
	if attr.itemData.bSplitter then
		attr.bEnable = false
	else
		attr.bEnable = true
	end
	attr.bPreSelect = false
end

--PreSelect状态是指鼠标hover到item上，Select态是指鼠标hover并点击
function PreSelect(self)
	local attr = self:GetAttribute()
	if not attr.bEnable then return end
	
	if attr.HoverBkgID then
		local bkgObj = self:GetControlObject("menu.item.bkg")
		bkgObj:SetTextureID(attr.HoverBkgID)
	end
	
	local tree = self:GetOwner()
	local menuWnd = tree:GetBindHostWnd()
	local userData = menuWnd:GetUserData()
	local menuItemContainerTemplID = userData.menuItemContainerTemplID
	
	local root = tree:GetRootObject()
	local curContainer = self:GetOwnerControl()
	local lastContainer = tree:GetUIObject(curContainer:GetID()..".Sub")
	if lastContainer then
		--上一个item创建的子菜单，应该销毁
		root:RemoveChild(lastContainer)
	end
	
	--如果item含有子菜单，在这时候显示
	local attr = self:GetAttribute()
	local itemData = attr.itemData
	if "table" ~= type(itemData.SubMenuTable) then
		return
	end
	
	local menuItemContainerTempl = Helper.templateManager:GetTemplate(menuItemContainerTemplID, "ObjectTemplate")
	local subContainer = menuItemContainerTempl:CreateInstance(curContainer:GetID()..".Sub")
	
	root:AddChild(subContainer)
	Helper:AddItemToContainer(subContainer, itemData.SubMenuTable, itemData.SubMenuFunTable)
	
	--计算子菜单位置
	local _, curItemT, _, _ = self:GetObjPos()
	local _, curContainerT, curContainerR, _ = curContainer:GetObjPos()
	local x, y = curContainerR, curContainerT + curItemT
	local subContainerL, subContainerT, subContainerR, subContainerB = Helper:CalcMenuPopPosition(x, y, subContainer, curContainer)
		
	subContainer:SetObjPos(subContainerL, subContainerT, subContainerR, subContainerB)
end

function IsPreSelect(self)
	local attr = self:GetAttribute()
	if nil == attr or attr.itemData.bSplitter then
		return
	end
	
	return attr.bPreSelect
end

function CancelSelect(self)
	OnMouseLeave(self)
end

function OnInitControl(self)
	local attr = self:GetAttribute()
	if attr == nil then
		return
	end
	if attr.itemData.bSplitter then
		local bkgObj = self:GetControlObject("menu.item.bkg")
		bkgObj:SetTextureID(attr.SplitterBkgID)
		return
	end
	
	InitAttrData(self)
	UpdateUI(self)
	self:FireExtEvent("OnInit")
end

function OnLButtonUp(self, x, y)
	local attr = self:GetAttribute()
	if nil == attr or attr.itemData.bSplitter then
		return
	end
	
	if attr.bEnable then
		local tree = self:GetOwner()
		local wnd = tree:GetBindHostWnd()
		
		wnd:EndMenu()
		self:FireExtEvent("OnSelect")
	end
end

function OnMouseMove(self)
	local attr = self:GetAttribute()
	if nil == attr or attr.itemData.bSplitter then
		return
	end

	if not attr.bPreSelect then
		attr.bPreSelect = true
		PreSelect(self)
	end
end

function OnMouseLeave(self)
	local attr = self:GetAttribute()
	if nil == attr or attr.itemData.bSplitter then
		return
	end
	local bkgObj = self:GetControlObject("menu.item.bkg")
	bkgObj:SetTextureID(attr.NormalBkgID or "")
	if attr.bPreSelect then
		attr.bPreSelect = false
	end
	
	-- local tree = self:GetOwner()
	-- local root = tree:GetRootObject()
	-- local curContainer = self:GetOwnerControl()
	-- local lastContainer = tree:GetUIObject(curContainer:GetID()..".Sub")
	-- if lastContainer then
		-- 上一个item创建的子菜单，应该销毁
		-- root:RemoveChild(lastContainer)
	-- end
end

function SetEnable(self, bEnable)
	local attr = self:GetAttribute()
	if nil == attr then return end
	
	attr.bEnable = bEnable
	UpdateUI(self)
end

function SetText(self, text)
	local textObj = self:GetControlObject("menu.item.text")
	textObj:SetText(tostring(text))
end

--itemData:{id, text, bSplitter, iconID, hotKey, userData, OnInitFun, OnSelectFun, 
	     -- OnShowSubMenuFun, SubMenuTable = {...},SubMenuFunTable = {...}
--}
--SetItemData的执行早于OnInitControl，界面元素的设置，请在OnInitControl之后进行
function SetItemData(self, itemData)
	local attr = self:GetAttribute()
	if not attr then
		return
	end
	
	attr.itemData = itemData
	if "function" == type(itemData.OnInitFun) and not attr.OnInitFunCookie then
		attr.OnInitFunCookie = self:AttachListener("OnInit", false, function() itemData.OnInitFun(self) end)
	end
	if "function" == type(itemData.OnSelectFun) and not attr.OnSelectFunCookie then
		attr.OnSelectFunCookie = self:AttachListener("OnSelect", false, function() itemData.OnSelectFun(self) end)
	end
end

function GetItemData(self)
	local attr = self:GetAttribute()
	return attr and attr.itemData
end
