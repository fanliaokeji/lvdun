local Helper = XLGetGlobal("Helper")
local tipUtil = XLGetObject("API.Util")

function SetDefaultItemHover(self)
	local objNormalMenu = self:GetControlObject("Menu.Context")
	local objMenuContainer = objNormalMenu:GetControlObject("context_menu")
	
	-- iPointSize在初始化font.Combobox.btn的时候就有了
	local regPath = "HKEY_CURRENT_USER\\Software\\mycalendar\\iPointSize"
	local lastFontSize = Helper:QueryRegValue(regPath)
	Helper:Assert("number" == type(lastFontSize), "get lastFontSize error!")
	
	--计算当前字号在菜单中是第几项，
	local tabFontSize = GetFontSizeConfig()
	if "table" ~= type(tabFontSize) then
		Helper:Assert(false, "get GetSystemAllTTFFont error!")
		return
	end
	local indexEx = 1
	for index=1,#tabFontSize do
		if tabFontSize[index] == lastFontSize then
			indexEx = index
			break
		end
	end
	
	--展示的时候，一并显示前三年的数字
	local nItemHieght = objNormalMenu:GetItemHeight()
	local nScrollPos = 0
	if indexEx > 3 then
		nScrollPos = (indexEx - 3) * nItemHieght
	else
		nScrollPos = indexEx * nItemHieght
	end
	
	objNormalMenu:SetScrollPos(nScrollPos)
	objNormalMenu:MoveItemListPanel(nScrollPos)
	
	local objChild = objMenuContainer:GetItem(indexEx)
	objMenuContainer:SetHoverItem(objChild, false)
end

function GetFontSizeConfig()
	local configPath = Helper:GetUserDataDir().."\\mycalendar\\notepadcfg\\defaultcfg.dat"
	local configTable = Helper:LoadLuaTable(configPath)
	
	if "table" ~= type(configTable) then
		configTable = {}
		configTable.FontSize = {8, 9, 11, 12, 14, 16, 18, 20, 24, 28, 32}
		local ret = Helper:SaveLuaTable(configTable, configPath)
		-- XLMessageBox(tostring(ret))
	end
	if not configTable.FontSize then
		configTable.FontSize = {8, 9, 11, 12, 14, 16, 18, 20, 24, 28, 32}
		local ret = Helper:SaveLuaTable(configTable, configPath)
	end
	Helper:Assert(configTable.FontSize, "configTable.FontSize is nil")
	return configTable.FontSize
end

function OnInitControl(self)
	--CreateMenuContainer
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")	
	local menuTemplate = templateMananger:GetTemplate("fontsizemenu.context", "ObjectTemplate")
	if menuTemplate == nil then
		Helper:Assert(false)
		return nil
	end
	objMenuContainer = menuTemplate:CreateInstance( "context_menu" )
	
	if not objMenuContainer then
		Helper:Assert(false)
		return false
	end
	
	local tabFontSize = GetFontSizeConfig()
	local nTotalCount = 0
	for index=1,#tabFontSize do
		local objMenuItem = CreateMenuItem(self, tostring(tabFontSize[index]))	
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
	objMenuItem:AttachListener("OnSelect", false, function() fontDropListCtrl:FireExtEvent("OnSelectFontSize", itemID) end)
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

function RouteToFather(self)
	self:RouteToFather()
end

function IsRealString(str)
	return type(str) == "string" and str ~= nil
end


