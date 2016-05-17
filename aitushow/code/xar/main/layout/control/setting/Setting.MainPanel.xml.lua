function closeOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(0)
end

function applyOnClick(self)

end

function confirmOnClick(self)
	closeOnClick(self)
end

function cancelOnClick(self)
	closeOnClick(self)
end

function LeftPanelOnSelect(self, evt, text)
	local owner = self:GetOwnerControl()
	local NormalPanel = owner:GetControlObject("NormalPanel")
	local FileAssoPanel = owner:GetControlObject("FileAssoPanel")
	if text == "常规选项" then
		FileAssoPanel:SetVisible(false)
		FileAssoPanel:SetChildrenVisible(false)
		NormalPanel:SetVisible(true)
		NormalPanel:SetChildrenVisible(true)
	else
		NormalPanel:SetVisible(false)
		NormalPanel:SetChildrenVisible(false)
		FileAssoPanel:SetVisible(true)
		FileAssoPanel:SetChildrenVisible(true)
	end
end
