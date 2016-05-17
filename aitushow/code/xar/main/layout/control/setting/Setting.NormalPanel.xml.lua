function OnSelectSaveOp(self, ischeck)
	local ower = self:GetOwnerControl()
	local id = self:GetID()
	for i = 7, 10 do
		if id ~= "radio"..i then
			local obj = ower:GetControlObject("radio"..i)
			obj:SetCheck(false, true)
		end
	end
end

function OnSelectExitOp(self, ischeck)
	local ower = self:GetOwnerControl()
	local id = self:GetID()
	for i = 13, 14 do
		if id ~= "radio"..i then
			local obj = ower:GetControlObject("radio"..i)
			obj:SetCheck(false, true)
		end
	end
end

