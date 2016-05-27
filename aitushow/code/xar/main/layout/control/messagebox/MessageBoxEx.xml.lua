local Helper = XLGetGlobal("Helper")
local MessageBox = Helper.MessageBox

function GetCheckState(self)
	local checkbox = self:GetObject("control:chebox")
	local attr = checkbox:GetAttribute()
	return attr.Select
end

function closeOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local nRet = MessageBox.ID_CANCEL
	hostwnd:EndDialog(nRet)
end

function nosaveOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local nRet = MessageBox.ID_NOSAVE
	if GetCheckState(self) then
		nRet = nRet + MessageBox.ID_CHECK
	else
		nRet = nRet + MessageBox.ID_UNCHECK
	end
	hostwnd:EndDialog(nRet)
end

function renamesaveOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local nRet = MessageBox.ID_RENAMESAVE
	if GetCheckState(self) then
		nRet = nRet + MessageBox.ID_CHECK
	else
		nRet = nRet + MessageBox.ID_UNCHECK
	end
	hostwnd:EndDialog(nRet)
end

function coveroldOnClick(self)
	local owner = self:GetOwner()
	local hostwnd = owner:GetBindHostWnd()
	local nRet = MessageBox.ID_COVEROLD
	if GetCheckState(self) then
		nRet = nRet + MessageBox.ID_CHECK
	else
		nRet = nRet + MessageBox.ID_UNCHECK
	end
	hostwnd:EndDialog(nRet)
end

