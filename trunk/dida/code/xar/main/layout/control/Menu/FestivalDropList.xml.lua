local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

local g_tFestivalList = {
	[1] = {
			["strText"] = "yuandan",
			["strCHNText"] = "元旦",
	},
	[2] = {
			["strText"] = "chuxi",
			["strCHNText"] = "除夕",
	},
	[3] = {
			["strText"] = "chunjie",
			["strCHNText"] = "春节",
	},
	[4] = {
			["strText"] = "qingmingjie",
			["strCHNText"] = "清明节",
	},
	[5] = {
			["strText"] = "laodongjie",
			["strCHNText"] = "劳动节",
	},
	[6] = {
			["strText"] = "duanwujie",
			["strCHNText"] = "端午节",
	},
	[7] = {
			["strText"] = "zhongqiujie",
			["strCHNText"] = "中秋节",
	},
	[8] = {
			["strText"] = "guoqingjie",
			["strCHNText"] = "国庆节",
	},
}

--方法
function SetDefaultItemHover(self)
	local objNormalMenu = self:GetControlObject("Menu.Context")
	local objMenuContainer = objNormalMenu:GetControlObject("context_menu")
	local strFestival = GetFestivalFromComboBox(objMenuContainer)
	local nFestIndex = GetFestIndex(strFestival)
		
	if nFestIndex == 0 then
		nFestIndex = 1
	end
	
	local objChild = objMenuContainer:GetItem(nFestIndex)
	objMenuContainer:SetHoverItem(objChild)
end



-------事件---
function OnInitControl(self)
	ShowFestivalList(self)
end

----

function ShowFestivalList(self)
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
	SetDefaultItemHover(self:GetOwnerControl())
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
	attr.Text = tostring(g_tFestivalList[nIndex].strCHNText)
	
	objMenuItem:AttachListener("OnSelect", false, OnSelectFestival)
	return objMenuItem
end


function BindMenuContainer(self, objMenuContainer, nMaxShowHistory, nTotalCount)
	local attr = self:GetAttribute()
	attr.nLinePerPage = 12
	attr.nTotalLineCount = nTotalCount

	self:OnInitControl(objMenuContainer)
end


function OnSelectFestival(objMenuItem)
	local strText = objMenuItem:GetText()
	local objDateSelect = tFunHelper.GetMainCtrlChildObj("DiDa.DateSelectCtrl")
	local objFestivalBox = objDateSelect:GetControlObject("Combobox.Festival")
		
	local attr = objFestivalBox:GetAttribute()
	local nFestIndex = GetFestIndex(strText)
	attr.LeftTextPos = 22
	if nFestIndex>3 then
		attr.LeftTextPos = 14
	end
	
	objFestivalBox:SetText(strText)
	ChangeCalendarState(nFestIndex)
end


function ChangeCalendarState(nFestIndex)
	local strText = g_tFestivalList[nFestIndex].strText
	if not IsRealString(strText) then
		return
	end
	
	local objDateSelect = tFunHelper.GetMainCtrlChildObj("DiDa.DateSelectCtrl")
	local tVacationList = tFunHelper.ReadConfigFromMemByKey("tVacationList") or {}
	
	local strMonth = FetchValueByPath(tVacationList, {"tDefaultVacMap", strText})
	if not IsRealString(strMonth) then
		local strYear = objDateSelect:GetYearText()
		strMonth = FetchValueByPath(tVacationList, {strYear, "tVactionMap", strText})
	end
	
	if IsRealString(strMonth) then
		objDateSelect:SetMonthText(strMonth.."月")
		tFunHelper.UpdateCalendarContent()
	end		
end



function GetFestIndex(strFestival)
	for nIndex, tInfo in ipairs(g_tFestivalList) do
		if tInfo.strCHNText == strFestival then
			return nIndex
		end
	end	
	
	return 0
end


function GetFestivalFromComboBox(objMenuContainer)
	local objDateSelect = tFunHelper.GetMainCtrlChildObj("DiDa.DateSelectCtrl")
	if not objDateSelect then
		return "元旦"
	end
	
	local objFestivalBox = objDateSelect:GetControlObject("Combobox.Festival")
	local strText = objFestivalBox:GetText()
	return strText
end

-----
function IsRealString(str)
	return type(str) == "string" and str~=nil
end


function FetchValueByPath(obj, path)
	local cursor = obj
	for i = 1, #path do
		cursor = cursor[path[i]]
		if cursor == nil then
			return nil
		end
	end
	return cursor
end