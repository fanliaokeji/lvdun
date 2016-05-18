function closeOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end

function nosaveOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end

function renamesaveOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local usedata = hostwnd:GetUserData()
	if type(usedata) == "table" and type(usedata["fn_renamesave"]) == "function" then
		usedata["fn_renamesave"]()
	end
	hostwnd:EndDialog(0)
end

function coveroldOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local usedata = hostwnd:GetUserData()
	if type(usedata) == "table" and type(usedata["fn_coverold"]) == "function" then
		usedata["fn_coverold"]()
	end
	hostwnd:EndDialog(0)
end

