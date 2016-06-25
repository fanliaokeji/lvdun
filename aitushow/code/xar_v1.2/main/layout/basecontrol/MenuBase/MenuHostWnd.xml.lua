
function OnPopupMenu(self)
	_G["gMenu"] = self
	local menuTree = self:GetBindUIObjectTree()
	self:SetFocus(true)
	local userdata  = self:GetUserData()
	if type(userdata) == "table" and type(userdata.OnPopupMenu) == "function" then
		userdata.OnPopupMenu()
	end
end

function OnEndMenu(self)
	local tree = self:GetBindUIObjectTree()
	if tree then
		self:UnbindUIObjectTree()
	end
	self:FinalClear()
	_G["gMenu"] = nil
	local userdata  = self:GetUserData()
	if type(userdata) == "table" and type(userdata.OnEndMenu) == "function" then
		userdata.OnEndMenu()
	end
end

function OnFocusChange(self, bFocus)
	local tree = self:GetBindUIObjectTree()
	if tree then
		self:EndMenu()
	end
end
