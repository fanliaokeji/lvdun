function Update(self)
	local attr = self:GetAttribute()
	local MarkIcon = self:GetControlObject("MarkIcon")
	local SelectBkg = self:GetControlObject("SelectBkg")
	if attr.HasChild then
		if attr.Open then
			MarkIcon:SetResID("MarkIconOpen")
		else
			MarkIcon:SetResID("MarkIconNormal")
		end
		MarkIcon:SetVisible(true)
	else
		MarkIcon:SetVisible(false)
	end
	if attr.Select then
		SelectBkg:SetVisible(true)
		ActiveImgs(self, true)
	else
		SelectBkg:SetVisible(false)
		ActiveImgs(self, false)
	end
end

function ActiveImgs(self, bActive)
	local MarkIcon = self:GetControlObject("MarkIcon")
	local MainIcon = self:GetControlObject("MainIcon")
	local attr = self:GetAttribute()
	if attr.HasChild then
		if attr.Open then
			MarkIcon:SetResID(bActive and "MarkIconOpen2" or "MarkIconOpen")
		else
			MarkIcon:SetResID(bActive and "MarkIconNormal2" or "MarkIconNormal")
		end
	end
	MainIcon:SetResID(bActive and attr.MainIconHover or attr.MainIcon)
end

function MarkIconOnLButtonDown(self, x, y, flags)
	local onwer = self:GetOwnerControl()
	local attr = onwer:GetAttribute()
	attr.Open = not attr.Open
	onwer:FireExtEvent("OnStateChange", attr.Open)
end

function LeftTreeItemOnLButtonDown(self, x, y, flags)
	if x > 9 then
		self:FireExtEvent("OnSelect")
	else
		local MarkIcon = self:GetControlObject("MarkIcon")
		MarkIconOnLButtonDown(MarkIcon)
	end
	self:RouteToFather()
end

function LeftTreeItemOnMouseEnter(self, x, y, flags)
	if x <= 9 then
		return
	end
	local attr = self:GetAttribute()
	if not attr.Select then
		local HoverBkg = self:GetControlObject("HoverBkg")
		HoverBkg:SetVisible(true)
	end
	self:RouteToFather()
	--ActiveImgs(self, true)
end

function LeftTreeItemOnMouseLeave(self, x, y, flags)
	local HoverBkg = self:GetControlObject("HoverBkg")
	HoverBkg:SetVisible(false)
	self:RouteToFather()
	--ActiveImgs(self, false)
end

function LeftTreeItemOnInitControl(self)
	local HoverBkg = self:GetControlObject("HoverBkg")
	local l, t, r, b = self:GetObjPos()
	--HoverBkg:SetObjPos(-l, t, "father.width+"..l, b)
end

