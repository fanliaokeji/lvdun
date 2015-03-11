local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil


--方法
function SetDefaultItemHover(self)
	local objNormalMenu = self:GetControlObject("Menu.Context")
	local objMenuContainer = objNormalMenu:GetControlObject("context_menu")
	
	local nBeginYear, nEndYear = tFunHelper.GetYearScale()
	local nCurYear = GetYearFromComboBox(objMenuContainer)
	local nCurDiff = nCurYear - nBeginYear
	
	local nShowDiff = nCurDiff   --展示的时候，一并显示前三年的数字
	if nCurDiff > 3 then
		nShowDiff = nCurDiff-3
	end
	
	local nItemHieght = objNormalMenu:GetItemHeight()
	local nScrollPos = nShowDiff*nItemHieght

	objNormalMenu:SetScrollPos(nScrollPos)
	objNormalMenu:MoveItemListPanel(nScrollPos)
	
	local objChild = objMenuContainer:GetItem(nCurDiff+1)
	objMenuContainer:SetHoverItem(objChild, false)
end


-------事件---
function OnInitControl(self)
	ShowYearList(self)
end


function OnMouseWheel(self, x, y, distance)
	local objNormalMenu = self:GetControlObject("Menu.Context")
	objNormalMenu:ProcessScrollWhenWheel(x, y, distance)
end

----

function ShowYearList(self)
	local tUserConfig = tFunHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local objMenuContainer = CreateMenuContainer(self)
	if not objMenuContainer then
		return false
	end
	
	local nBeginYear, nEndYear = tFunHelper.GetYearScale()
	local nScale = nEndYear-nBeginYear+1
	local nTotalCount = 0
	for nIndex=1, nScale do
		local objMenuItem = CreateMenuItem(nIndex)	
		if objMenuItem then
			objMenuContainer:AddChild(objMenuItem)				
			nTotalCount = nTotalCount+1
		end				
	end

	BindMenuContainer(self, objMenuContainer, nMaxShowHistory, nTotalCount)
	SetDefaultItemHover(self:GetOwnerControl())
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
	local nBeginYear, nEndYear = tFunHelper.GetYearScale()
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
	attr.Text = tostring(nBeginYear+nIndex-1)
	
	objMenuItem:AttachListener("OnSelect", false, OnSelectYear)
	return objMenuItem
end


function BindMenuContainer(self, objMenuContainer, nMaxShowHistory, nTotalCount)
	local attr = self:GetAttribute()
	attr.nLinePerPage = 12
	attr.nTotalLineCount = nTotalCount

	self:OnInitControl(objMenuContainer)
end


function OnSelectYear(objMenuItem)
	local strText = objMenuItem:GetText() .. " 年"
	local objDateSelect = tFunHelper.GetMainCtrlChildObj("DiDa.DateSelectCtrl")
	objDateSelect:SetYearText(strText)
	objDateSelect:ResetFestivalText()
	
	tFunHelper.UpdateCalendarContent()
end


function GetYearFromComboBox(objMenuContainer)
	local objDateSelect = tFunHelper.GetMainCtrlChildObj("DiDa.DateSelectCtrl")
	if not objDateSelect then
		local strYear = os.date("%Y")
		return tonumber(strYear)
	end
	
	local objYearBox = objDateSelect:GetControlObject("Combobox.Year")

	local strText = objYearBox:GetText()
	if not IsRealString(strText) then
		return
	end
	local _, _, strYear = string.find(strText, "(%d*)[^%d]*")
	return tonumber(strYear)
end

-----
function RouteToFather(self)
	self:RouteToFather()
end


function IsRealString(str)
	return type(str) == "string" and str~=nil
end


