-- nProgress: 0 ~ 100
function SetProgress(self,nProgress)
	if not tonumber(nProgress) then return end
    local attr = self:GetAttribute()
	attr.progress = tonumber(nProgress)
	nProgress = tonumber(nProgress) / 100
	local pgrBkg = self:GetControlObject("texture.bkg")
	local progress = self:GetControlObject("texture.progress")
	local left, top, right, bottom = pgrBkg:GetObjPos()
	local width = right - left
	local height = bottom - top
	progress:SetObjPos(0, 0, left + width*nProgress, height)
end

function GetProgress(self)
    local attr = self:GetAttribute()
	return attr.progress
end

function MoveLight(self)
	local pgrBkg = self:GetControlObject("texture.bkg")
	if not pgrBkg then
		return
	end
	local left, top, right, bottom = pgrBkg:GetObjPos()
	--光
	local procLight = self:GetControlObject("texture.light")
	local leftL, topL, rightL, bottomL = procLight:GetObjPos()
	widthL = rightL - leftL
	local moveSpeed = 20
	leftL = leftL + moveSpeed
	rightL = rightL + moveSpeed
	if leftL > right then
		leftL = 0 - widthL
		rightL = 0
	end
	procLight:SetObjPos(leftL, topL, rightL, bottomL)
end

function OnInitControl(self)
    local attr = self:GetAttribute()
	self:GetControlObject("texture.bkg"):SetTextureID(attr.BkgID)
	if attr.LightID then
		self:GetControlObject("texture.light"):SetTextureID(attr.LightID)
	end
	self:GetControlObject("texture.progress"):SetTextureID(attr.ProgressID)
	attr.timerid = SetTimer(function() MoveLight(self) end, 200)
end

function OnDestroy(self)
    local attr = self:GetAttribute()
	if attr.timerid then
		KillTimer(attr.timerid)
		attr.timerid = nil
	end
end