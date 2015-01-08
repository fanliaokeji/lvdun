local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

-------事件---
function OnInitControl(self)
	ShowYearList(self)
end

----

function ShowYearList(self)
	local tUserConfig = tFunHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local objMenuContainer = CreateMenuContainer(self)
	if not objMenuContainer then
		return false
	end
	
	local nTotalCount = 0
	for nIndex=1, 201 do
		local objMenuItem = CreateMenuItem(nIndex)	
		if objMenuItem then
			objMenuContainer:AddChild(objMenuItem)				
			nTotalCount = nTotalCount+1
		end				
	end

	BindMenuContainer(self, objMenuContainer, nMaxShowHistory, nTotalCount)
	SetDefaultYearHover(objMenuContainer)
end


function CreateMenuContainer(objNormalMenuCtrl)
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")	
	local menuTemplate = templateMananger:GetTemplate("yearmenu.context", "ObjectTemplate")
	if menuTemplate == nil then
		return nil
	end
	local objMenu = menuTemplate:CreateInstance( "context_menu" )
	return objMenu
end


function CreateMenuItem(nIndex)	
	local nBaseYear = 1900
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")	
	local objMenuItemTempl = templateMananger:GetTemplate("menu.context.item", "ObjectTemplate")
	if objMenuItemTempl == nil then
		return nil
	end
	
	local objMenuItem = objMenuItemTempl:CreateInstance( "year_"..tostring(nIndex) )
	if not objMenuItem then
		return nil
	end

	local attr = objMenuItem:GetAttribute()
	attr.Text = tostring(nBaseYear+nIndex-1)
	
	objMenuItem:AttachListener("OnSelect", false, OnSelectYear)
	return objMenuItem
end


function BindMenuContainer(self, objMenuContainer, nMaxShowHistory, nTotalCount)
	local attr = self:GetAttribute()
	attr.nLinePerPage = 12
	attr.nTotalLineCount = nTotalCount

	self:OnInitControl(objMenuContainer)
end


function SetDefaultYearHover(objMenuContainer)
	local nBaseYear = 1900
	local nCurYear = GetYearFromComboBox(objMenuContainer)
	local nCurDiff = nCurYear - nBaseYear
	
	local nShowDiff = nCurDiff   --展示的时候，一并显示前三年的数字
	if nCurDiff > 3 then
		nShowDiff = nCurDiff-3
	end
	
	local objNormalMenu = objMenuContainer:GetOwnerControl()
	local nItemHieght = objNormalMenu:GetItemHeight()
	local nScrollPos = nShowDiff*nItemHieght

	objNormalMenu:SetScrollPos(nScrollPos)
	objNormalMenu:MoveItemListPanel(nScrollPos)
	
	local objChild = objMenuContainer:GetItem(nCurDiff+1)
	objMenuContainer:SetHoverItem(objChild, false)
end


function OnSelectYear(objMenuItem)
	local strText = objMenuItem:GetText() .. "年"
	local objMenu = objMenuItem:GetFather()
	local objNormalMenu = objMenu:GetOwnerControl()
	local objYearBox = objNormalMenu:GetRelateObject()
	
	objYearBox:SetText(strText)
end


function GetYearFromComboBox(objMenuContainer)
	local objDateSelect = tFunHelper.GetMainCtrlChildObj("DiDa.DateSelectCtrl")
	local objYearBox = objDateSelect:GetControlObject("Combobox.Year")

	local strText = objYearBox:GetText()
	if not IsRealString(strText) then
		return
	end
	local _, _, strYear = string.find(strText, "(%d*)年")
	return tonumber(strYear)
end

-----
function IsRealString(str)
	return type(str) == "string" and str~=nil
end


