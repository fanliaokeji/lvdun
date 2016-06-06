function OnPopupMenu(self)
end

function OnEndMenu(self)
end

function OnBindObjectTree(self, menuTree)
	local objMainLayout = menuTree:GetUIObject("TrayMenu.Main")
	local context = objMainLayout:GetObject("TrayMenu.Context")
	context:OnInitControl()	
end