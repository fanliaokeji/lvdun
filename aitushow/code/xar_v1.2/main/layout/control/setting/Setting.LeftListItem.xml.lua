function Update(self)
	local attr = self:GetAttribute()
	local SelectBkg = self:GetControlObject("SelectBkg")
	local MarkIcon = self:GetControlObject("MarkIcon")
	local TextObj = self:GetControlObject("MainText")
	TextObj:SetText(attr.Text)
	if attr.Select then
		SelectBkg:SetVisible(true)
		MarkIcon:SetVisible(true)
	else
		SelectBkg:SetVisible(false)
		MarkIcon:SetVisible(false)
	end
end
function SettingLeftListItemOnMouseEnter(self, x, y, flags)
	local attr = self:GetAttribute()
	if attr.Select then
		return
	end
	local HoverBkg = self:GetControlObject("HoverBkg")
	HoverBkg:SetVisible(true)
end

function SettingLeftListItemOnMouseLeave(self, x, y, flags)
	local HoverBkg = self:GetControlObject("HoverBkg")
	HoverBkg:SetVisible(false)
end

function SettingLeftListItemOnInitControl(self)
	self:Update()
end

function SettingLeftListItemOnLButtonDown(self, x, y, flags)
	local attr = self:GetAttribute()
	if attr.Select then return end
	attr.Select = true
	self:Update()
	SettingLeftListItemOnMouseLeave(self)
	self:FireExtEvent("OnSelect")
end

