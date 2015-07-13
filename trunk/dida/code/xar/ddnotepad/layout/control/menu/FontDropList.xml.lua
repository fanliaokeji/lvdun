local Helper = XLGetGlobal("Helper")
local tipUtil = XLGetObject("API.Util")


--方法
function SetDefaultItemHover(self)
	-- local objNormalMenu = self:GetControlObject("Menu.Context")
	-- local objMenuContainer = objNormalMenu:GetControlObject("context_menu")
	
	-- local nBeginYear, nEndYear = tFunHelper.GetYearScale()
	-- local nCurYear = GetYearFromComboBox(objMenuContainer)
	-- local nCurDiff = nCurYear - nBeginYear
	
	-- local nShowDiff = nCurDiff   --展示的时候，一并显示前三年的数字
	-- if nCurDiff > 3 then
		-- nShowDiff = nCurDiff-3
	-- end
	
	-- local nItemHieght = objNormalMenu:GetItemHeight()
	-- local nScrollPos = nShowDiff*nItemHieght

	-- objNormalMenu:SetScrollPos(nScrollPos)
	-- objNormalMenu:MoveItemListPanel(nScrollPos)
	
	-- local objChild = objMenuContainer:GetItem(nCurDiff+1)
	-- objMenuContainer:SetHoverItem(objChild, false)
end


-------事件---
function OnInitControl(self)
	--CreateMenuContainer
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")	
	local menuTemplate = templateMananger:GetTemplate("fontmenu.context", "ObjectTemplate")
	if menuTemplate == nil then
		Helper:Assert(false)
		return nil
	end
	objMenuContainer = menuTemplate:CreateInstance( "context_menu" )
	
	if not objMenuContainer then
		Helper:Assert(false)
		return false
	end
	
	local tabFont = tipUtil:GetSystemAllTTFFont()
	local nTotalCount = 0
	for index=1,#tabFont do
		local objMenuItem = CreateMenuItem(self, tabFont[index])	
		if objMenuItem then
			objMenuContainer:AddChild(objMenuItem)				
			nTotalCount = nTotalCount+1
		end				
	end

	BindMenuContainer(self, objMenuContainer, nMaxShowHistory, nTotalCount)
	SetDefaultItemHover(self:GetOwnerControl())
end


function OnMouseWheel(self, x, y, distance)
	local objNormalMenu = self:GetControlObject("Menu.Context")
	objNormalMenu:ProcessScrollWhenWheel(x, y, distance)
end

function CreateMenuItem(self, itemID)	
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")	
	local objMenuItemTempl = templateMananger:GetTemplate("menu.context.item", "ObjectTemplate")
	if objMenuItemTempl == nil then
		Helper:Assert(false)
		return nil
	end
	
	local objMenuItem = objMenuItemTempl:CreateInstance(tostring(itemID) )
	if not objMenuItem then
		Helper:Assert(false)
		return nil
	end

	local attr = objMenuItem:GetAttribute()
	attr.Text = tostring(itemID)
	
	local fontDropListCtrl = self:GetOwnerControl()
	objMenuItem:AttachListener("OnSelect", false, function() fontDropListCtrl:FireExtEvent("OnSelectFont", itemID) end)
	return objMenuItem
end


function BindMenuContainer(self, objMenuContainer, nMaxShowHistory, nTotalCount)
	local attr = self:GetAttribute()
	attr.nLinePerPage = 10
	attr.nTotalLineCount = nTotalCount

	self:OnInitControl(objMenuContainer)
end


function OnSelectFont(objMenuItem)
	--获取当前字号
	local strText = objMenuItem:GetText()
	
	--发事件,通知设置Edit字体
	local owner = objMenuItem:GetOwnerControl()
	owner:FireExtEvent("OnSelectFont", strText)

end

-----
function RouteToFather(self)
	self:RouteToFather()
end


function IsRealString(str)
	return type(str) == "string" and str ~= nil
end


