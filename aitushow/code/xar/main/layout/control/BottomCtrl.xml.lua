local Helper = XLGetGlobal("Helper")
local graphicUtil = XLGetObject("GRAPHIC.Util")

local function Rotate(self, angle)
	local tree = self:GetOwner()
	local containerObj = tree:GetUIObject("ThumbnailContainerObj")
	local containerObjAttr = containerObj:GetAttribute()
	local selectedObj = containerObjAttr.pageManager and containerObjAttr.pageManager:GetCurSelectedThumbnails()
	if selectedObj then
		local attr = selectedObj:GetAttribute()
		local xlhBitmap = attr.data and attr.data.xlhBitmap
		if not xlhBitmap then
			return
		end
		
		local newBitmap, newWidth, newHeight = graphicUtil:RotateImgByAngle(xlhBitmap, angle)
		if newBitmap then
			local tImgInfo = {}
			tImgInfo.szPath = attr.data.szPath
			tImgInfo.uHeight = newHeight
			tImgInfo.uWidth = newWidth
			tImgInfo.xlhBitmap = newBitmap
			containerObjAttr.pageManager:OnGetMultiImgInfoCallBack("OnClickRotateLeft", tImgInfo) 
		end
	end
end


function OnClickRotateLeft(self)
	Rotate(self, 90)
end

function OnClickRotateRight(self)
	Rotate(self, -90)
end

function OnClickDelete(self)
	local tree = self:GetOwner()
	local containerObj = tree:GetUIObject("ThumbnailContainerObj")
	local containerObjAttr = containerObj:GetAttribute()
	local selectedObj = containerObjAttr.pageManager and containerObjAttr.pageManager:GetCurSelectedThumbnails()
	
	if selectedObj then
		local attr = selectedObj:GetAttribute()
		local szPath = attr.data and attr.data.szPath
		containerObjAttr.pageManager:DeleteThumbnailByPath(szPath)
		selectedObj:Select(false)
	end
end
