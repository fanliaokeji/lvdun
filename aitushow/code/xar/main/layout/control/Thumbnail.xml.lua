local Helper = XLGetGlobal("Helper")

function SetData(self, data)
	local attr = self:GetAttribute()
	if not attr or not "table" == type(data) then
		return
	end
	
	local imageObj = self:GetControlObject("Image")
	local extTextObj = self:GetControlObject("ExtText")
	local fileNameObj = self:GetControlObject("FileName")
	
	if data.thumbnailBitmap then
		imageObj:SetBitmap(data.thumbnailBitmap)
	end
	if data.ext then
		extTextObj:SetText(data.ext)
	end
	if data.fileName then
		fileNameObj:SetText(data.fileName)
	end
end

function Select(self, bSelect)
	
end

function OnLButtonUp(self)
	
end
