local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local Helper = XLGetGlobal("Helper")
local tipUtil = tFunHelper.tipUtil

function OnSelect_open(self)
	local clndrItem = GetRelateObject(self)
	local attr = clndrItem:GetAttribute()
	tClndrContent = attr.tClndrContent
	if type(tClndrContent) ~= "table" then
		return
	end
	local sDate = tClndrContent.solarcalendar
	Helper:Assert(sDate)
	local wndTree = clndrItem:GetOwner()
	local notepadPreView = wndTree:GetUIObject("root.layout:root.ctrl:DiDa.NotePadPreView")
	
	local curDayNoteCount, newestTime = notepadPreView:CheckDayRecord(sDate)
	--选中newestTime记事本
	
	--切换到notepad
	SetOnceTimer(function() tFunHelper.ChangeView(2) end, 200)
end

function OnInit_open(self)
	local clndrItem = GetRelateObject(self)
	local attr = clndrItem:GetAttribute()
	tClndrContent = attr.tClndrContent
	if type(tClndrContent) ~= "table" then
		self:SetEnable(false)
		return
	end
	local sDate = tClndrContent.solarcalendar
	Helper:Assert(sDate)
	local wndTree = clndrItem:GetOwner()
	local notepadPreView = wndTree:GetUIObject("root.layout:root.ctrl:DiDa.NotePadPreView")
	
	local curDayNoteCount = notepadPreView:CheckDayRecord(sDate)
	if curDayNoteCount > 0 then
		self:SetEnable(true)
		return
	end
	self:SetEnable(false)
end

function OnSelect_new(self)
	local clndrItem = GetRelateObject(self)
	local attr = clndrItem:GetAttribute()
	tClndrContent = attr.tClndrContent
	if type(tClndrContent) ~= "table" then
		return
	end
	local sDate = tClndrContent.solarcalendar
	Helper:Assert(sDate)
	local wndTree = clndrItem:GetOwner()
	local notepadPreView = wndTree:GetUIObject("root.layout:root.ctrl:DiDa.NotePadPreView")
	
	--添加当天的记事本
	notepadPreView:AddItemByDate(sDate)
	--切换到notepad
	SetOnceTimer(function() tFunHelper.ChangeView(2) end, 200)
end

function OnInit_new(self)
--这个地方暂时这样，若在该天中的23：59：59秒添加之后，再添加是有问题的
	self:SetEnable(true)
end

function OnSelect_cancel(self)
	
end

function OnInit_cancel(self)
	
end

function GetRelateObject(objMenuItem)
	local objTree = objMenuItem:GetOwner()
	-- local objMainLayout = objTree:GetUIObject("Menu.MainLayout")
	local objNormalCtrl = objTree:GetUIObject("Menu.Context")
	return objNormalCtrl:GetRelateObject()
end

