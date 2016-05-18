function closeOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end

function MessageBoxOnInitControl(self)
end

function noOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end

function yesOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local usedata = hostwnd:GetUserData()
	if type(usedata) == "table" then
		for _, v in ipairs(usedata) do
			if type(v) == "function" then
				v()
				break
			end
		end
	end
	hostwnd:EndDialog(0)
end

