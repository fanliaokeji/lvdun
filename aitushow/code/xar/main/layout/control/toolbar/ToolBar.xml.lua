local tm = XLGetObject("Xunlei.UIEngine.TimerManager")
local timerid = nil
function StartTimer(fn)
	return function()
		StopTimer()
		timerid = tm:SetTimer(fn, 300)
	end
end
function StopTimer()
	if timerid then
		tm:KillTimer(timerid)
		timerid = nil
	end
end
function ToolPanelOnButtonMouseLeave(self, x, y)
	--鼠标移开无条件停止计时器
	StopTimer()
end

function ToolBarOnInitControl(self)
	--初始化为不可见
	self:SetVisible(false)
	self:SetChildrenVisible(false)
	local attr = self:GetAttribute()
	--除了3和8不需要计时器，其它都需要
	attr.timerdo = {
		["panelbutton1"] = StartTimer(function()
			XLMessageBox("panelbutton1")
		end), 
		["panelbutton2"] = StartTimer(function()
			XLMessageBox("panelbutton2")
		end),
		["panelbutton3"] = function()
		end,
		["panelbutton4"] = StartTimer(function()
			XLMessageBox("panelbutton4")
		end),
		["panelbutton5"] = StartTimer(function()
			XLMessageBox("panelbutton5")
		end),
		["panelbutton6"] = StartTimer(function()
			XLMessageBox("panelbutton6")
		end),
		["panelbutton7"] = StartTimer(function()
			XLMessageBox("panelbutton7")
		end),
		["panelbutton8"] = function()
		end,
	}
	
	--除了3和8需要处理点击事件，其它都只需停掉计时器
	attr.btnclick = {
		["panelbutton1"] = StopTimer, 
		["panelbutton2"] = StopTimer, 
		["panelbutton3"] = function()
			XLMessageBox("btnclick:panelbutton3")
		end,
		["panelbutton4"] = StopTimer, 
		["panelbutton5"] = StopTimer, 
		["panelbutton6"] = StopTimer, 
		["panelbutton7"] = StopTimer, 
		["panelbutton8"] = function()
			XLMessageBox("btnclick:panelbutton8")
		end,
	}
end

function ToolPanelOnButtonMouseDown(self)
	local ower = self:GetOwnerControl()
	local attr = ower:GetAttribute()
	local id = self:GetID()
	if type(attr.timerdo) == "table" and id and attr.timerdo[id] then
		attr.timerdo[id]()
	end
end

function ToolBarOnControlMouseLeave(self, x, y, flags)
	self:SetVisible(false)
	self:SetChildrenVisible(false)
end

function ToolPanelOnClick(self, x, y)
	local ower = self:GetOwnerControl()
	local attr = ower:GetAttribute()
	local id = self:GetID()
	if type(attr.btnclick) == "table" and id and attr.btnclick[id] then
		attr.btnclick[id]()
	end
end

function ToolBarOnControlMouseEnter(self, x, y, flags)
	self:SetVisible(true)
	self:SetChildrenVisible(true)
end

