local Helper = XLGetGlobal("Helper")
local MessageBox = Helper.MessageBox

function GetCheckBoxState(self, nRet)
	local checkbox = self:GetObject("control:checkbox")
	if checkbox then
		local attr = checkbox:GetAttribute()
		if attr.Select then
			nRet = nRet + MessageBox.ID_CHECK
		else
			nRet = nRet + MessageBox.ID_UNCHECK
		end
	end
	return nRet
end

function closeOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(GetCheckBoxState(self, MessageBox.ID_CANCEL))
end

function MessageBoxOnInitControl(self)
end

function noOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(GetCheckBoxState(self, MessageBox.ID_CANCEL))
end

function yesOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	hostwnd:EndDialog(GetCheckBoxState(self, MessageBox.ID_YES))
end

