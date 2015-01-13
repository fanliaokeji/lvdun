local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

--方法
function SetDefaultItemHover(self)
	local nBaseMonth = 1
	local nCurMonth = GetMonthFromComboBox()
	local nCurDiff = nCurMonth - nBaseMonth
	
	local objNormalMenu = self:GetControlObject("Menu.Context")
	local objMenuContainer = objNormalMenu:GetControlObject("context_menu")
	local objChild = objMenuContainer:GetItem(nCurDiff+1)
	objMenuContainer:SetHoverItem(objChild, false)
end


-------事件---
function OnInitControl(self)
	ShowMonthList(self)
end

----

function ShowMonthList(self)
	local tUserConfig = tFunHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local objMenuContainer = CreateMenuContainer(self)
	if not objMenuContainer then
		return false
	end
	
	local nTotalCount = 0
	for nIndex=1, 12 do
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
	local menuTemplate = templateMananger:GetTemplate("monthmenu.context", "ObjectTemplate")
	if menuTemplate == nil then
		return nil
	end
	local objMenu = menuTemplate:CreateInstance( "context_menu" )
	return objMenu
end


function CreateMenuItem(nIndex)	
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")	
	local objMenuItemTempl = templateMananger:GetTemplate("menu.context.item", "ObjectTemplate")
	if objMenuItemTempl == nil then
		return nil
	end
	
	local objMenuItem = objMenuItemTempl:CreateInstance( "month_"..tostring(nIndex) )
	if not objMenuItem then
		return nil
	end

	local attr = objMenuItem:GetAttribute()
	attr.Text = tostring(nIndex).."月"
	
	objMenuItem:AttachListener("OnSelect", false, OnSelectMonth)
	return objMenuItem
end


function BindMenuContainer(self, objMenuContainer, nMaxShowHistory, nTotalCount)
	local attr = self:GetAttribute()
	attr.nLinePerPage = 12
	attr.nTotalLineCount = nTotalCount

	self:OnInitControl(objMenuContainer)
end


function OnSelectMonth(objMenuItem)
	local strText = objMenuItem:GetText()
	local objDateSelect = tFunHelper.GetMainCtrlChildObj("DiDa.DateSelectCtrl")
	local objMonthBox = objDateSelect:GetControlObject("Combobox.Month")
	
	local attr = objMonthBox:GetAttribute()
	local _, _, strMonth = string.find(strText, "(%d*)[^%d]*")
	local nMonth = tonumber(strMonth)
	attr.LeftTextPos = 10
	if nMonth>9 then
		attr.LeftTextPos = 6
	end
	
	objMonthBox:SetText(strText)
	objDateSelect:ResetFestivalText()
	tFunHelper.UpdateCalendarContent()
end


function GetMonthFromComboBox()
	local objDateSelect = tFunHelper.GetMainCtrlChildObj("DiDa.DateSelectCtrl")
	if not objDateSelect then
		local strMonth = os.date("%m")
		local nMonth = tonumber(strMonth)
		strMonth = string.format("%1d", nMonth)
		return tonumber(strMonth)
	end
	
	local objMonthBox = objDateSelect:GetControlObject("Combobox.Month")

	local strText = objMonthBox:GetText()
	if not IsRealString(strText) then
		return
	end
	local _, _, strMonth = string.find(strText, "(%d*)[^%d]*")
	return tonumber(strMonth)
end

-----
function IsRealString(str)
	return type(str) == "string" and str~=nil
end


