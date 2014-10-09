
function OnPopupMenu(self)
	-- _G["gMenu"] = self
	-- local menuTree = self:GetBindUIObjectTree()
	-- local objMainLayout = menuTree:GetUIObject("TrayMenu.Main")
	-- local context = objMainLayout:GetObject("context_menu")
	-- context:AnimateShow()
end

function OnEndMenu(self)
	-- _G["gMenu"] = nil
	-- local tree = self:GetBindUIObjectTree()
	-- if tree then
		-- local objMainLayout = tree:GetUIObject("TrayMenu.Main")
		-- local menu = objMainLayout:GetObject("context_menu")
		-- menu:AnimateHide()
	-- end
end

function OnBindObjectTree(self, menuTree)
	local objMainLayout = menuTree:GetUIObject("TrayMenu.Main")
	local context = objMainLayout:GetObject("TrayMenu.Context")
	context:OnInitControl()	
	
	local objTitle = objMainLayout:GetObject("TrayMenu.Title")
	local left, top, right, bottom = objTitle:GetObjPos()
	objTitle:SetObjPos(left, top, right, bottom)
end
