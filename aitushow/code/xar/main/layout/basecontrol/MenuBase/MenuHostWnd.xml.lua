
function OnEndMenu(self)
	local tree = self:GetBindUIObjectTree()
	if tree then
		self:UnbindUIObjectTree()
	end
	self:FinalClear()
end

function OnFocusChange(self, bFocus)
	local tree = self:GetBindUIObjectTree()
	if tree then
		self:EndMenu()
	end
end
