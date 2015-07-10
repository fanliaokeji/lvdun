
function OnPopupMenu(self)
	_G["gMenu"] = self
	local menuTree = self:GetBindUIObjectTree()
	local context = menuTree:GetRootObject()
	context:SetFocus(true)
	context:AnimateShow()
end

function OnEndMenu(self)
	local tree = self:GetBindUIObjectTree()
	if tree then
		local menu = tree:GetRootObject()
		menu:AnimateHide(function()
			_G["gMenu"] = nil
		end)
	end
end

function OnBindObjectTree(self, menuTree)
	local context = menuTree:GetRootObject()
	context:OnInitControl()
end
