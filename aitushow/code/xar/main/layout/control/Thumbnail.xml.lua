local Helper = XLGetGlobal("Helper")

--data格式即为tPictures格式:{
-- {"szPath"=, "szExt"=, "utcLastWriteTime"=, "uFileSize"=, "uWidth"=, "uHeight"=, "szType"=, "xlhBitmap"=},

function SetData(self, data, index)--返回值为bool，代表是否成功设定 image，设定失败，则需要请求
	local attr = self:GetAttribute()
	if not attr or not "table" == type(data) then
		return
	end
	
	attr.data = data
	-- attr.index = index
	local background = self:GetControlObject("Background")
	local imageObj = self:GetControlObject("Image")
	local extTextObj = self:GetControlObject("ExtText")
	local fileNameObj = self:GetControlObject("FileName")
	
	background:SetVisible(true)
	background:SetChildrenVisible(true)
	
	extTextObj:SetText(data.szExt or "")
	if data.szPath then
		--提取filename
		local fileName = string.match(data.szPath, ".+[\\/]([^?]+)")
		fileNameObj:SetText(fileName)
	end
	
	if data.xlhBitmap then
		imageObj:SetDrawMode(1)
		imageObj:SetBitmap(data.xlhBitmap)
		return true
	else
		if data.szExt and "" ~= data.szExt then
			imageObj:SetDrawMode(0)
			imageObj:SetResID("default_icon."..data.szExt)
		end
	end
	return false
end

function GetIndex(self)
	local attr = self:GetAttribute()
	return attr.index
end

function SetIndex(self, index)
	local attr = self:GetAttribute()
	local container = self:GetOwnerControl()
	local containerAttr = container:GetAttribute()
	local lineCount, columnCount, pageCount, picWidth, picHeight = container:GetPageLayout(self)  
	
	
	if attr.index and attr.lineCount and attr.columnCount then
		if attr.index == index and attr.lineCount == lineCount and attr.columnCount == columnCount then
			-- 无需调整位置
			return
		end
	end
	
	attr.index = index
	attr.lineCount = lineCount
	attr.columnCount = columnCount
	
	local left = ((index-1)%columnCount) * (containerAttr.SpaceH + picWidth)
	local top  = (math.floor((index-1)/columnCount)) * (containerAttr.SpaceV + picHeight)
	
	self:SetObjPos2(left, top, picWidth, picHeight)
	LOG("Thumbnail object SetLayout, id: ",self:GetID(), " left: ", left, " top: ", top, " width: ", picWidth, " height: ", picHeight)
end

function GetData(self)
	local attr = self:GetAttribute()
	return attr.data
end

function Clear(self)
	local attr = self:GetAttribute()
	attr.data = nil
	attr.index = nil
	--清除数据之后，整个Thumbnail变为不可见
	local background = self:GetControlObject("Background")
	background:SetVisible(false)
	background:SetChildrenVisible(false)
end

function SetImage(self, xlhBitmap)
	local imageObj = self:GetControlObject("Image")
	local attr = self:GetAttribute()
	attr.data.xlhBitmap = xlhBitmap
	
	imageObj:SetBitmap(xlhBitmap)
end

function Select(self, bSelect)
	local attr = self:GetAttribute()
	attr.bSelect = bSelect 
	local bkg = self:GetControlObject("Background")
	if bSelect then
		bkg:SetSrcColor("RGBA(246,196,79,255)")
	else
		bkg:SetSrcColor("RGBA(100,66,57,255)")
	end
end

function OnLButtonUp(self)
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	ownerCtrl:FireExtEvent("OnTrySelect", attr.bSelect)
end

function OnInitControl(self)
	local attr = self:GetAttribute()
	attr.bSelect = false
	
	--没有绑定数据之前，整个Thumbnail是不可见的
	local background = self:GetControlObject("Background")
	background:SetVisible(false)
	background:SetChildrenVisible(false)
end
