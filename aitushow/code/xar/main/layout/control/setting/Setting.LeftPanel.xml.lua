function SettingLeftPanelOnInitControl(self)
	local item1 = self:GetControlObject("ListItem1")
	ListItem2OnSelect(item1, true)
end

function ListItem2OnSelect(self, ischeck)
	local id = self:GetID()
	local owner = self:GetOwnerControl()
	for i = 1, 2 do
		if id ~= "ListItem"..i then
			local obj = owner:GetControlObject("ListItem"..i)
			local objattr = obj:GetAttribute()
			objattr.Select = false
			obj:Update()
		end
	end
	local attr = self:GetAttribute()
	owner:FireExtEvent("OnSelect", attr.Text)
end

function ListItem1OnSelect(self, ischeck)
	local id = self:GetID()
	local owner = self:GetOwnerControl()
	for i = 1, 2 do
		if id ~= "ListItem"..i then
			local obj = owner:GetControlObject("ListItem"..i)
			local objattr = obj:GetAttribute()
			objattr.Select = false
			obj:Update()
		end
	end
	local attr = self:GetAttribute()
	owner:FireExtEvent("OnSelect", attr.Text)
end

