local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

local g_tFestivalList = {
	[1] = "元旦",
	[2] = "除夕",
	[3] = "春节",
	[4] = "清明节",
	[5] = "劳动节",
	[6] = "端午节",
	[7] = "中秋节",
	[8] = "国庆节",
}

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
	for nIndex=1, #g_tFestivalList do
		local objMenuItem = CreateMenuItem(nIndex)	
		if objMenuItem then
			objMenuContainer:AddChild(objMenuItem)				
			nTotalCount = nTotalCount+1
		end				
	end

	BindMenuContainer(self, objMenuContainer, nMaxShowHistory, nTotalCount)
	SetDefaultFestivalHover(objMenuContainer)
end


function CreateMenuContainer(objNormalMenuCtrl)
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")	
	local menuTemplate = templateMananger:GetTemplate("festivalmenu.context", "ObjectTemplate")
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
	
	local objMenuItem = objMenuItemTempl:CreateInstance( "festival_"..tostring(nIndex) )
	if not objMenuItem then
		return nil
	end

	local attr = objMenuItem:GetAttribute()
	attr.Text = tostring(g_tFestivalList[nIndex])
	
	objMenuItem:AttachListener("OnSelect", false, OnSelectFestival)
	return objMenuItem
end


function BindMenuContainer(self, objMenuContainer, nMaxShowHistory, nTotalCount)
	local attr = self:GetAttribute()
	attr.nLinePerPage = 12
	attr.nTotalLineCount = nTotalCount

	self:OnInitControl(objMenuContainer)
end


function SetDefaultFestivalHover(objMenuContainer)
	local strFestival = GetFestivalFromComboBox(objMenuContainer)
	local nFestIndex = GetFestIndex(strFestival)
		
	if nFestIndex == 0 then
		nFestIndex = 1
	end
	
	local objChild = objMenuContainer:GetItem(nFestIndex)
	objMenuContainer:SetHoverItem(objChild)
end


function OnSelectFestival(objMenuItem)
	local strText = objMenuItem:GetText()
	local objMenu = objMenuItem:GetFather()
	local objNormalMenu = objMenu:GetOwnerControl()
	local objFestivalBox = objNormalMenu:GetRelateObject()
	
	local attr = objFestivalBox:GetAttribute()
	local nFestIndex = GetFestIndex(strText)
	attr.LeftTextPos = 22
	if nFestIndex>3 then
		attr.LeftTextPos = 14
	end
	
	
	objFestivalBox:SetText(strText)
end


function GetFestIndex(strFestival)
	for nIndex, strText in ipairs(g_tFestivalList) do
		if strText == strFestival then
			return nIndex
		end
	end	
	
	return 0
end


function GetFestivalFromComboBox(objMenuContainer)
	local objDateSelect = tFunHelper.GetMainCtrlChildObj("DiDa.DateSelectCtrl")
	local objFestivalBox = objDateSelect:GetControlObject("Combobox.Festival")

	local strText = objFestivalBox:GetText()
	return strText
end

-----
function IsRealString(str)
	return type(str) == "string" and str~=nil
end


