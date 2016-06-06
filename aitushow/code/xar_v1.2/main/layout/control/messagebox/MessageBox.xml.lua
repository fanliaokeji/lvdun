local Helper = XLGetGlobal("Helper")
local MessageBox = Helper.MessageBox

function closeOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(MessageBox.ID_CANCEL)
end

function MessageBoxOnInitControl(self)
end

function noOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(MessageBox.ID_CANCEL)
end

function yesOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(MessageBox.ID_YES)
end

