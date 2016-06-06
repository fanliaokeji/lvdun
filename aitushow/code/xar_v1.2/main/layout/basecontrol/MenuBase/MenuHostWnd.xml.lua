
function OnPopupMenu(self)
	_G["gMenu"] = self
	local menuTree = self:GetBindUIObjectTree()
	self:SetFocus(true)
end

function OnEndMenu(self)
	local tree = self:GetBindUIObjectTree()
	if tree then
		self:UnbindUIObjectTree()
	end
	self:FinalClear()
	_G["gMenu"] = nil
end

function OnFocusChange(self, bFocus)
	local tree = self:GetBindUIObjectTree()
	if tree then
		self:EndMenu()
	end
end
