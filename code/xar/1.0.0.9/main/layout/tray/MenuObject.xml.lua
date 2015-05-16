
function IterateItems(self, func, revesel)
	if type(func) ~= "function" then
		return
	end
	local attr = self:GetAttribute()
	local keyArr = {}
	for i, v in pairs(attr.ItemList) do
		table.insert(keyArr, i)
	end
	table.sort(keyArr)
	if revesel ~= true then
		for i = 1, #keyArr do
			-- 返回true,中断遍历
			if func(attr.ItemList[ keyArr[ i ] ], keyArr[ i ]) then
				return
			end
		end
	else
		for i = #keyArr, 1, -1 do
			-- 返回true,中断遍历
			if func(attr.ItemList[ keyArr[ i ] ], keyArr[ i ]) then
				return
			end
		end
	end
end

-- 遍历当前菜单项，得出最宽项
function GetMaxWidth(self)
	local attr = self:GetAttribute()
	local pos_x = attr.ItemLeft
	local pos_y = attr.ItemRight
	local max_width = 0
	IterateItems(self, function(item)
		local temp_width = item:GetMinWidth()
		if temp_width ~= nil and item:IsVisible() and max_width < temp_width then
			max_width = temp_width
		end
	end)
	
	return max_width
end


-- 遍历当前菜单项，从上到下设置每项菜单项的位置
function AdjustItemPos( self )
	local attr = self:GetAttribute()
	local pos_x = attr.ItemLeft
	local pos_y = attr.ItemTop
	
	--local max_width = GetMaxWidth( self )  fix
	local max_width = 210
	
	IterateItems(self, function(item)
		--Set item pos when it is visible
		if item:IsVisible() then
			local left, top, right, bottom = item:GetObjPos()
			item:SetObjPos( pos_x, pos_y, pos_x + max_width, pos_y + bottom - top )
			pos_y = pos_y + bottom - top
		end
	end)
	local self_left, self_top, self_right, self_bottom = self:GetObjPos()
	self:SetObjPos( self_left, self_top, self_left + max_width + attr.ItemLeft + attr.ItemRight, self_top + pos_y + attr.ItemBottom )
	if attr.HoverItem then
		local itembkn = self:GetControlObject("ItemBkn")
		local left, top, right, bottom = attr.HoverItem:GetObjPos()
		itembkn:SetObjPos(left, top, right, bottom)
	end
end

function EndMenu(self)
	local menuTree = self:GetOwner()
	local menuHost = menuTree:GetBindHostWnd()
	menuHost:EndMenu(true)
end

function InsertItem( self, index, item )
	local attr = self:GetAttribute()
	if index == 0 then
		index = 1
	elseif index < 0 then
		index = #attr.ItemList + 1
	end
	table.insert(attr.ItemList, index, item)
	self:AddChild( item )
	AdjustItemPos( self )
end

-- 在原菜单后新加项菜单
function AddItem( self, item )
	self:InsertItem(-1, item)
end

function GetItemCount( self )
	local attr = self:GetAttribute()
	local count = 0
	for i, v in pairs(attr.ItemList) do
		count = count + 1
	end
	return count
end

function GetItem( self, index )
	local attr = self:GetAttribute()
	if not index or index < 1 then
		return
	end
	return attr.ItemList[index]
end

function GetItembyID( self, ID )
	if not ID then
		return
	end
	local retItem
	IterateItems(self, function(item, i)
		if item:GetID() == ID then
			retItem = item
			return true
		end
	end)
	return retItem
end

function RemoveItem( self, index )
	local attr = self:GetAttribute()
	if index < 1 then
		return
	end
	self:RemoveChild( attr.ItemList[index] )
	attr.ItemList[ index ] = nil
	AdjustItemPos( self )
end

function OnInitControl(self)
	local attr = self:GetAttribute()
	attr.ItemList = {}
	local count = self:GetChildCount()
	for i = 0, count - 1 do
		local child = self:GetChildByIndex( i )
		local class = child:GetClass()
		if class == "MenuItemObject" then
			local itemAttr = child:GetAttribute()
			local index = itemAttr.Index
			if index == nil or index < 0 then
				index = #attr.ItemList + 1
			elseif index == 0 then
				index = 1
			end
			table.insert(attr.ItemList, index, child)
		end
	end
	
	local bknRes = attr.BknID
	if bknRes ~= nil then
		local bkn = self:GetControlObject("menu.bkg")
		bkn:SetResID(bknRes)
	else
		local bkn = self:GetControlObject("menu.bkg")
		bkn:SetVisible( false )
	end
	
	local shadingID = attr.ShadingID
	if shadingID ~= nil then
		local shading = self:GetControlObject("menu.shading")
		shading:SetResID(shadingID)
		shading:SetVisible( true )
	else
		local shading = self:GetControlObject("menu.shading")
		shading:SetVisible( false )
	end
	AdjustItemPos( self )
end

function OnDestroy(self)
	local attr = self:GetAttribute()
	
	local tm = XLGetObject("Xunlei.UIEngine.TimerManager")
	if attr.ShowSubMenuTimer ~= nil then
		tm:KillTimer( attr.ShowSubMenuTimer )
		attr.ShowSubMenuTimer = nil
	end
end

-- 此函数是MenuItemObject调，设置当前选中项
function SetHoverItem(self, item, show_sub )
	local itembkn = self:GetControlObject("ItemBkn")
	local attr = self:GetAttribute()
	if item == nil then
		if attr.HoverItem == nil or not attr.HoverItem:HasSubMenu() then
			itembkn:SetVisible( false )
			itembkn:SetChildrenVisible( false )
			if attr.HoverItem ~= nil then
				attr.HoverItem:ChangeState( 0 )
			end
			attr.HoverItem = nil
		elseif attr.HoverItem:HasSubMenu() and not attr.HoverItem:IsShowSubMenu() and attr.ShowSubMenuTimer == nil then
			itembkn:SetVisible( false )
			itembkn:SetChildrenVisible( false )
			attr.HoverItem:ChangeState( 0 )
			attr.HoverItem = nil
		end
		return
	end
	if item ~= nil and attr.HoverItem ~= nil and item:GetID() == attr.HoverItem:GetID() then
		return
	end
	
	local menuTree = self:GetOwner()
	
	local tm = XLGetObject("Xunlei.UIEngine.TimerManager")
	
	if attr.ShowSubMenuTimer ~= nil then
		tm:KillTimer( attr.ShowSubMenuTimer )
		attr.ShowSubMenuTimer = nil
	end
	
	local function OnShowSubMenuTimer( tm, id )		
		tm:KillTimer( attr.ShowSubMenuTimer )
		attr.ShowSubMenuTimer = nil
		if item ~= nil then
			item:ShowSubMenu()
		end
	end
	
	if show_sub == nil or show_sub then
		if item:HasSubMenu() then
			attr.ShowSubMenuTimer = tm:SetTimer( OnShowSubMenuTimer, 200 )
		end
	end
	if attr.HoverItem ~= nil and attr.HoverItem:HasSubMenu() then
		attr.HoverItem:EndSubMenu()
	end
	
	local oldItem = attr.HoverItem
	attr.HoverItem = item
	
	if oldItem == nil then
		local left,top,right,bottom = item:GetObjPos() 
		itembkn:SetObjPos(left,top,right,bottom)
		itembkn:SetVisible( true )
		itembkn:SetChildrenVisible( true )
		attr.HoverItem:ChangeState( 1 )
	else
		oldItem:ChangeState( 0 )
		local left, top, right, bottom = attr.HoverItem:GetObjPos()
		itembkn:SetObjPos(left,top,right,bottom)
		itembkn:SetVisible( true )
		itembkn:SetChildrenVisible( true )
		attr.HoverItem:ChangeState( 1 )
	end
	return oldItem
end

function SetBknID( self, id )
	local attr = self:GetAttribute()
	attr.BknID = id
	local bknRes = id
	if bknRes ~= nil then
		local bkn = self:GetControlObject("menu.bkg")
		bkn:SetResID(bknRes)
		bkn:SetVisible( true )
	else
		local bkn = self:GetControlObject("menu.bkg")
		bkn:SetVisible( false )
	end
end

function SetShadingID( self, id )
	local attr = self:GetAttribute()
	attr.ShadingID = id
	local shadingID = id
	if shadingID ~= nil then
		local shading = self:GetControlObject("menu.shading")
		shading:SetResID(shadingID)
		shading:SetVisible( true )
	else
		local shading = self:GetControlObject("menu.shading")
		shading:SetVisible( false )
	end
end

function GetNextEnableItem( self, cur )
	local attr = self:GetAttribute()
	local aboveItems, belowItems = {}, {}
	for i, v in pairs(attr.ItemList) do
		if i >= cur then
			table.insert(belowItems, i)
		else
			table.insert(aboveItems, i)
		end
	end
	table.sort(belowItems)
	for i = 1, #belowItems do
		if attr.ItemList[ belowItems[ i ] ]:IsEnable() and attr.ItemList[ belowItems[ i ] ]:IsVisible() then
			return attr.ItemList[ belowItems[ i ] ]
		end
	end
	table.sort(aboveItems)
	for i = 1, #aboveItems do
		if attr.ItemList[ aboveItems[ i ] ]:IsEnable() and attr.ItemList[ aboveItems[ i ] ]:IsVisible() then
			return attr.ItemList[ aboveItems[ i ] ]
		end
	end
end

function MoveToNextItem( self )
	local attr = self:GetAttribute()
	if attr.HoverItem == nil then
		if GetItemCount(self) >= 1 then
			self:SetHoverItem( GetNextEnableItem( self, 1 ), false )
		end
	else
		local id_2 = attr.HoverItem:GetID()
		local find = false
		IterateItems(self, function(item, i)
			if find then
				self:SetHoverItem( GetNextEnableItem( self, i ), false )
				return true
			end
			if item:GetID() == id_2 then
				find = true
			end
		end)
	end	
end

function GetPrevEnableItem( self, cur )
	local attr = self:GetAttribute()
	local aboveItems, belowItems = {}, {}
	for i, v in pairs(attr.ItemList) do
		if i >= cur then
			table.insert(belowItems, i)
		else
			table.insert(aboveItems, i)
		end
	end
	table.sort(aboveItems)
	for i = #aboveItems, 1, -1 do
		if attr.ItemList[ aboveItems[ i ] ]:IsEnable() and attr.ItemList[ aboveItems[ i ] ]:IsVisible() then
			return attr.ItemList[ aboveItems[ i ] ]
		end
	end
	table.sort(belowItems)
	for i = #belowItems, 1, -1 do
		if attr.ItemList[ belowItems[ i ] ]:IsEnable() and attr.ItemList[ belowItems[ i ] ]:IsVisible() then
			return attr.ItemList[ belowItems[ i ] ]
		end
	end
end

function MoveToPrevItem( self )
	local attr = self:GetAttribute()
	if attr.HoverItem == nil then
		if GetItemCount(self) > 1 then
			self:SetHoverItem( GetPrevEnableItem( self , table.maxn(attr.ItemList), false ) )
		end
	else
		local id_2 = attr.HoverItem:GetID()
		local find = false
		IterateItems(self, function(item, i)
			if item:GetID() == id_2 then
				find = true
			end
			if find then
				self:SetHoverItem( GetPrevEnableItem( self, i ), false )
				return true
			end
		end, true)
	end
end


function OnKeyDown( self, char )
	self:RouteToFather()
end

function GetHoverItem( self )
	local attr = self:GetAttribute()
	return attr.HoverItem
end

-- 通过AccKey找到项
function GetKeyItem( self, key )
	local key_item_list = {}
	local attr = self:GetAttribute()
	IterateItems(self, function(item, i)
		if item:IsEnable() then
			local attr_item = item:GetAttribute()
			if attr_item.AccKey and string.byte(attr_item.AccKey) == key then
				table.insert(key_item_list, item)
			end
		end
	end)
	return key_item_list
end

function GetParentItem(self)
	local attr = self:GetAttribute()
	local father = self:GetOwnerControl()
	local attr = father:GetAttribute()
	return attr.ParentObj
end
