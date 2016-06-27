local Helper = XLGetGlobal("Helper")

--data格式即为tPictures的item格式:{
-- {"FilePath"=, "ExtName"=, "LastWriteTime"=, "uFileSize"=, "ThumbWidth"=, "ThumbHeight"=, "szType"=, "xlhThumb"=},

function AdjustImageBySize(backgroundObj, imageObj, ThumbWidth, ThumbHeight)
	--这里不能用imageObj:GetObjPos,可能imageObj的pos曾经被下面的代码改过
	local imageL, imageT, imageR, imageB = backgroundObj:GetObjPos()
	local imageWidth = imageR - imageL - 6
	local imageHeight = imageB - imageT - 23
	if ThumbWidth < imageWidth and ThumbHeight < imageHeight then
		--直接1:1展示
		local newTop    = math.round((imageHeight - ThumbHeight)/2)
		local newLeft  = math.round((imageWidth - ThumbWidth)/2)
		
		LOG("SetImage 1:1 newHeight: ", newHeight, " newLeft: ", newLeft)
		imageObj:SetObjPos2(newLeft, newTop, ThumbWidth, ThumbHeight)
		return
	end
	if ThumbWidth/ThumbHeight > imageWidth/imageHeight then--图片是矮、宽型的
		local newHeight = math.round((imageWidth*ThumbHeight)/ThumbWidth)
		--计算居中的高度
		local newTop    = math.round((imageHeight - newHeight)/2)
		
		-- LOG("SetImage  newHeight: ", newHeight, " newTop: ", newTop)
		imageObj:SetObjPos2(3, newTop, imageWidth, newHeight)
	elseif ThumbWidth/ThumbHeight < imageWidth/imageHeight then--图片是高、瘦型的
		local newWidth = math.round((imageHeight*ThumbWidth)/ThumbHeight)
		local newLeft  = math.round((imageWidth - newWidth)/2)
		imageObj:SetObjPos2(newLeft, 3, newWidth, imageHeight)
		-- LOG("SetImage  newWidth: ", newWidth, " newLeft: ", newLeft)
	end
end

function SetData(self, data)--返回值为bool，代表是否成功设定 image，设定失败，则需要请求
	local attr = self:GetAttribute()
	if not attr or not "table" == type(data) then
		return
	end
	
	attr.data = data
	Select(self, attr.data.bSelect)
	
	local background = self:GetControlObject("Background")
	local imageObj = self:GetControlObject("Image")
	local extTextObj = self:GetControlObject("ExtText")
	local fileNameObj = self:GetControlObject("FileName")
	local defaultImage = self:GetControlObject("DefaultImage")
	
	background:SetVisible(true)
	background:SetChildrenVisible(true)
	extTextObj:SetText(data.ExtName or "")
	fileNameObj:SetText(data.FileName)
	
	if data.xlhThumb then
		defaultImage:SetVisible(false)
		imageObj:SetVisible(true)
		imageObj:SetDrawMode(1)
		imageObj:SetBitmap(data.xlhThumb)
		AdjustImageBySize(background, imageObj, data.ThumbWidth, data.ThumbHeight)
		return true
	else
		defaultImage:SetVisible(true)
		imageObj:SetVisible(false)
		if data.ExtName and "" ~= data.ExtName then
			defaultImage:SetDrawMode(0)
			defaultImage:SetResID("default_icon"..string.lower(data.ExtName))
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
	
	local containerW, containerH = Helper:GetObjWH(container)
	local newSpaceH = math.round((containerW - columnCount*picWidth)/columnCount)
	local left = ((index-1)%columnCount) * (newSpaceH + picWidth)
	local top  = (math.floor((index-1)/columnCount)) * (containerAttr.SpaceV + picHeight)
	
	self:SetObjPos2(left, top, picWidth, picHeight)
	-- LOG("Thumbnail object SetLayout, id: ",self:GetID(), " left: ", left, " top: ", top, " width: ", picWidth, " height: ", picHeight)
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

function SetImage(self, tImgInfo)
	local imageObj = self:GetControlObject("Image")
	local defaultImage = self:GetControlObject("DefaultImage")
	local backgroundObj = self:GetControlObject("Background")
	local attr = self:GetAttribute()
	attr.data.xlhThumb = tImgInfo.xlhThumb
	attr.data.ThumbWidth = tImgInfo.ThumbWidth
	attr.data.ThumbHeight = tImgInfo.ThumbHeight
	
	imageObj:SetVisible(true)
	imageObj:SetDrawMode(1)
	imageObj:SetResID("")
	imageObj:SetBitmap(tImgInfo.xlhThumb)
	defaultImage:SetResID("")
	--调整宽高比
	if tImgInfo.ThumbWidth and tImgInfo.ThumbHeight then
		AdjustImageBySize(backgroundObj, imageObj, tImgInfo.ThumbWidth, tImgInfo.ThumbHeight)
	end
end

function Rotate(self, isleft)
	local imageObj = self:GetControlObject("Image")
	if imageObj then
		local bitmap = imageObj:GetBitmap()
		if bitmap then
			local newbitmap
			local attr = self:GetAttribute()
			attr.angle = attr.angle or 0
			if isleft then
				newbitmap = Helper.tipUtil:ImageLeftRotate(bitmap)
				attr.angle = attr.angle - 90
			else
				newbitmap = Helper.tipUtil:ImageRightRotate(bitmap)
				attr.angle = attr.angle + 90
			end
			if newbitmap then
				local backgroundObj = self:GetControlObject("Background")
				local w , h = attr.data.ThumbHeight, attr.data.ThumbWidth--每旋转1次反1次
				attr.data.ThumbHeight, attr.data.ThumbWidth = h, w
				imageObj:SetBitmap(newbitmap)
				AdjustImageBySize(backgroundObj, imageObj, w, h)
			end
		end
	end
end

function FormatImageInfo(self)
	local attr = self:GetAttribute()
	if not attr.data then
		return
	end
	
	local fileName = Helper:GetFileNameByPath(attr.data.FilePath)
	local info = fileName
	if attr.data.ThumbWidth and attr.data.ThumbHeight then
		info = info.." - "..tostring(attr.data.ThumbWidth).."*"..tostring(attr.data.ThumbHeight)
	end
	if attr.data.FileSize then
		local FileSize = tonumber(attr.data.FileSize)
		local sSize = ""
		if FileSize <= 1024 then
			sSize = tostring(FileSize).." B"
		elseif FileSize <= 1024*1024 then
			sSize = tostring(math.round(FileSize/1024)).." KB"
		else
			local size = FileSize/(1024*1024)
			size = tostring(size)
			size = string.sub(size,1,4)
			sSize = size.." MB"
		end
		info = info.." - "..tostring(sSize)
	end
	
	return info
end

function Select(self, bSelect)
	local attr = self:GetAttribute()
	if not attr or not attr.data then
		LOG("GetAttribute nil!!!!!!!!!")
	end
	attr.data.bSelect = bSelect 
	attr.bSelect = bSelect--add by wangwei
	-- local bkg = self:GetControlObject("Background")
	local bkg = self:GetControlObject("SelectFrame")
	if bSelect then
		-- bkg:SetSrcColor("RGBA(79,196,246,255)")
		bkg:SetResID("texture.thumbnail.select.bkg")
		
		--更新窗口底部的信息栏，稍后优化这里。
		local tree = self:GetOwner()
		local infoObj = tree:GetUIObject("MainWnd.Info")
		infoObj:SetText(FormatImageInfo(self))
	else
		-- bkg:SetSrcColor("RGBA(57,66,100,255)")
		bkg:SetResID("")
	end
end

function OnLButtonUp(self)
	local ownerCtrl = self:GetOwnerControl()
	local bkg = ownerCtrl:GetControlObject("SelectFrame")
	local attr = ownerCtrl:GetAttribute()
	if attr.bSelect then
		attr.bSelect = false
		-- bkg:SetSrcColor("RGBA(57,66,100,255)")
		bkg:SetResID("")
	else
		attr.bSelect = true
		-- bkg:SetSrcColor("RGBA(79,196,246,255)")
		bkg:SetResID("texture.thumbnail.select.bkg")
		--更新窗口底部的信息栏，稍后优化这里。
		local tree = self:GetOwner()
		local infoObj = tree:GetUIObject("MainWnd.Info")
		local sInfo = FormatImageInfo(ownerCtrl)
		-- XLMessageBox(sInfo)
		if infoObj then
			infoObj:SetText(sInfo)
		end
	end
	
	ownerCtrl:FireExtEvent("OnSelect", attr.bSelect)
end

function OnLButtonDbClick(self)
	local imgctrl = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	if not imgctrl then
		Helper:CreateModelessWnd("Kuaikan.MainWnd","Kuaikan.MainObjTree")
		imgctrl = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	end
	if not imgctrl then
		LOG("OnLButtonDbClick: not imgctrl: ")
		return
	end
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	imgctrl:LoadImageFile(attr.data.FilePath, nil, nil, function() imgctrl:UpdateFileList() end)
	
	local MainHostWnd = Helper.Selector.select("", "", "MainWnd.Instance")
	if MainHostWnd and MainHostWnd:GetWindowState() ~= "hide" then
		Helper.Listener.LastShowWnd = MainHostWnd
		MainHostWnd:Show(0)
	else
		Helper.Listener.LastShowWnd = nil
	end
	local ImgHostWnd = Helper.Selector.select("", "", "Kuaikan.MainWnd.Instance")
	if ImgHostWnd then
		ImgHostWnd:BringWindowToTop(true)
	end
end

function OnInitControl(self)
	local attr = self:GetAttribute()
	attr.bSelect = false
	
	--没有绑定数据之前，整个Thumbnail是不可见的
	local background = self:GetControlObject("Background")
	background:SetVisible(false)
	background:SetChildrenVisible(false)
end
