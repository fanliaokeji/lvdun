function chebox24OnSelect(self, evt, ischeck)
	local owner = self:GetOwnerControl()
	local objcheckbox
	for i = 3, 22 do
		objcheckbox = owner:GetControlObject("chebox"..i)
		if objcheckbox then
			objcheckbox:SetCheck(ischeck, false)
		end
	end
end

