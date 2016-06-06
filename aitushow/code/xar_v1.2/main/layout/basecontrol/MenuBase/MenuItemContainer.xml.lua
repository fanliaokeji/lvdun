local Helper = XLGetGlobal("Helper")

local function AdjustContainerBkg(self)
	local attr = self:GetAttribute()
	local container = self:GetControlObject("container.bkg")
	local itemCount = container:GetChildCount()
	local firstItem = container:GetChildByIndex(0)
	local itemAttr = firstItem:GetAttribute()
	local itemWidth = itemAttr.Width
	
	local bkgWidth = attr.LeftOffSet + attr.RightOffSet + itemWidth
	local bkgHeight = attr.TopOffSet + attr.BottomOffSet + attr.totalItemHeight
	
	self:SetObjPos2(0, 0, bkgWidth, bkgHeight)
	container:SetObjPos2(0, 0, bkgWidth, bkgHeight)
end

--index从1开始
local function CalcItemPosByIndex(self, menuItem, index)
	local attr = self:GetAttribute()
	local itemAttr = menuItem:GetAttribute()
	local itemData = menuItem:GetItemData()
	Helper:Assert(itemData)
	local width = itemData.bSplitter and itemAttr.SplitterWidth or itemAttr.Width
	local height = itemData.bSplitter and itemAttr.SplitterHeight or itemAttr.Height
	
	Helper:Assert("number" == type(width) and "number" == type(height), "未指定item尺寸!")
	
	local L = attr.LeftOffSet
	local T = attr.totalItemHeight + attr.TopOffSet
	local R = L + width
	local B = T + height
	
	return L, T, R, B 
end

local function GetPreSelectItemIndex(self)
	local container = self:GetControlObject("container.bkg")
	if not container:GetChildCount() then
		XLMessageBox("GetChildCount  nil")
	end
	
	for i=0, container:GetChildCount() - 1 do
		local item = container:GetChildByIndex(i)
		if item and item:IsPreSelect() then
			return i
		end
	end
	return nil
end

function OnInitControl(self)
	local attr = self:GetAttribute()
	if not attr then return end
	
	--当前所有item(包括分割线)的高度和，不包括边缘。可以在AdjustContainerBkg和
	--CalcItemPosByIndex的时候省去遍历所有item来计算高度
	attr.totalItemHeight = 0
end

--主要负责安排item的位置、调整菜单大背景
function AddItem(self, menuItem)
	local attr = self:GetAttribute()
	if not attr then return end
	
	local container = self:GetControlObject("container.bkg")
	container:SetTextureID(attr.BkgTexture)
	
	local itemID = menuItem:GetID()
	local tmp = self:GetControlObject(tostring(itemID))
	if tmp then
		Helper:Assert(false, "重复的ItemID!")
		return
	end
	container:AddChild(menuItem)
	
	local itemCount = container:GetChildCount()
	local L, T, R, B  = CalcItemPosByIndex(self, menuItem, itemCount)
	attr.totalItemHeight = attr.totalItemHeight + B - T
	AdjustContainerBkg(self)
	
	menuItem:SetObjPos(L, T, R, B)
end

function PreSelectPrevItem(self)
	local attr = self:GetAttribute()
	if nil == attr then
		return
	end
	
	local container = self:GetControlObject("container.bkg")
	attr.curSelItemIndex = GetPreSelectItemIndex(self)
	if nil == attr.curSelItemIndex then
		attr.curSelItemIndex = 0
	else
		local curSelItem = container:GetChildByIndex(attr.curSelItemIndex)
		curSelItem:CancelSelect()
		attr.curSelItemIndex = attr.curSelItemIndex - 1
	end
	if attr.curSelItemIndex < 0 then
		attr.curSelItemIndex = 0
	end
	local prevItem = container:GetChildByIndex(attr.curSelItemIndex)
	if prevItem then
		prevItem:PreSelect()
	end
end

function PreSelectNextItem(self)
	local attr = self:GetAttribute()
	if nil == attr then
		return
	end
	
	local container = self:GetControlObject("container.bkg")
	attr.curSelItemIndex = GetPreSelectItemIndex(self)
	if nil == attr.curSelItemIndex then
		attr.curSelItemIndex = 0
	else
		local curSelItem = container:GetChildByIndex(attr.curSelItemIndex)
		curSelItem:CancelSelect()
		attr.curSelItemIndex = attr.curSelItemIndex + 1
	end
	if attr.curSelItemIndex >= self:GetChildCount() then
		attr.curSelItemIndex = self:GetChildCount() - 1
	end
	local nextItem = container:GetChildByIndex(attr.curSelItemIndex)
	if nextItem then
		nextItem:PreSelect()
	end
end

function OnKeyDown(self, keyChar)
	local attr = self:GetAttribute()
	if nil == attr then
		return
	end
	
	if     38 == keyChar then --up
		-- PreSelectPrevItem(self) --这里有问题，暂不支持按键
	elseif 40 == keyChar then --down
		-- PreSelectNextItem(self) 
	elseif 39 == keyChar then --right
		   
	elseif 37 == keyChar then --left
		   
	elseif 27 == keyChar then--ESC
		   
	elseif 18 == keyChar then--ALT
		   
	elseif 13 == keyChar then--ENTER
		
	else
		
	end
end