
function OnPopupMenu(self)
	_G["gMenu"] = self
	local menuTree = self:GetBindUIObjectTree()
	local context = menuTree:GetUIObject("Menu.Context")
	context:SetFocus(true)
	context:AnimateShow()
end

function OnEndMenu(self)
	local tree = self:GetBindUIObjectTree()
	if tree then
		local menu = tree:GetUIObject("Menu.Context")
		_G["gMenu"] = nil
		menu:AnimateHide(function()
			_G["gMenu"] = nil
		end)
	end
end

function OnBindObjectTree(self, menuTree)
	local context = menuTree:GetUIObject("Menu.Context")
	context:OnInitControl()
end
