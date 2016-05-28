local Helper = XLGetGlobal("Helper")

--data格式即为tPictures的item格式:{
-- {"szPath"=, "szExt"=, "utcLastWriteTime"=, "uFileSize"=, "uWidth"=, "uHeight"=, "szType"=, "xlhBitmap"=},

function AdjustImageBySize(backgroundObj, imageObj, uWidth, uHeight)
	--这里不能用imageObj:GetObjPos,可能imageObj的pos曾经被下面的代码改过
	local imageL, imageT, imageR, imageB = backgroundObj:GetObjPos()
	local imageWidth = imageR - imageL - 6
	local imageHeight = imageB - imageT - 23
	if uWidth < imageWidth and uHeight < imageHeight then
		--直接1:1展示
		local newTop    = math.round((imageHeight - uHeight)/2)
		local newLeft  = math.round((imageWidth - uWidth)/2)
		
		LOG("SetImage 1:1 newHeight: ", newHeight, " newLeft: ", newLeft)
		imageObj:SetObjPos2(newLeft, newTop, uWidth, uHeight)
		return
	end
	if uWidth/uHeight > imageWidth/imageHeight then--图片是矮、宽型的
		local newHeight = math.round((imageWidth*uHeight)/uWidth)
		--计算居中的高度
		local newTop    = math.round((imageHeight - newHeight)/2)
		
		LOG("SetImage  newHeight: ", newHeight, " newTop: ", newTop)
		imageObj:SetObjPos2(imageL, newTop, imageWidth, newHeight)
	elseif uWidth/uHeight < imageWidth/imageHeight then--图片是高、瘦型的
		local newWidth = math.round((imageHeight*uWidth)/uHeight)
		local newLeft  = math.round((imageWidth - newWidth)/2)
		imageObj:SetObjPos2(newLeft, imageT, newWidth, imageHeight)
		LOG("SetImage  newWidth: ", newWidth, " newLeft: ", newLeft)
	end
end

function SetData(self, data, index)--返回值为bool，代表是否成功设定 image，设定失败，则需要请求
	local attr = self:GetAttribute()
	if not attr or not "table" == type(data) then
		return
	end
	
	attr.data = data
	-- attr.index = index
	local background = self:GetControlObject("Background")
	local imageObj = self:GetControlObject("Image")
	local gifObj = self:GetControlObject("Image.gif")
	local extTextObj = self:GetControlObject("ExtText")
	local fileNameObj = self:GetControlObject("FileName")
	
	background:SetVisible(true)
	background:SetChildrenVisible(true)
	
	extTextObj:SetText(data.szExt or "")
	if "gif" == data.szExt then
		gifObj:SetVisible(true)
		imageObj:SetVisible(false)
		local XGP_Factory = XLGetObject("Xunlei.XGP.Factory")
		local gif = XGP_Factory:LoadGifFromFile(data.szPath)
		gifObj:SetGif(gif)
		gifObj:Play()
		attr.data.xlhGif = gif
		attr.data.uWidth, attr.data.uHeight = gif:GetSize()
		AdjustImageBySize(background, gifObj, attr.data.uWidth, attr.data.uHeight)
		return true--gif不去申请了
	else
		gifObj:SetVisible(false)
		imageObj:SetVisible(true)
	end
	if data.szPath then
		--提取filename
		local fileName = string.match(data.szPath, ".+[\\/]([^?]+)")
		attr.fileName = fileName
		fileNameObj:SetText(fileName)
	end
	
	if data.xlhBitmap then
		imageObj:SetDrawMode(1)
		imageObj:SetBitmap(data.xlhBitmap)
		AdjustImageBySize(background, imageObj, data.uWidth, data.uHeight)
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

function SetImage(self, tImgInfo)
	local imageObj = self:GetControlObject("Image")
	local backgroundObj = self:GetControlObject("Background")
	local attr = self:GetAttribute()
	attr.data.xlhBitmap = tImgInfo.xlhBitmap
	attr.data.uWidth = tImgInfo.uWidth
	attr.data.uHeight = tImgInfo.uHeight
	attr.data.szType = tImgInfo.szType
	
	imageObj:SetDrawMode(1)
	imageObj:SetBitmap(tImgInfo.xlhBitmap)
	--调整宽高比
	if tImgInfo.uWidth and tImgInfo.uHeight then
		AdjustImageBySize(backgroundObj, imageObj, tImgInfo.uWidth, tImgInfo.uHeight)
	end
end

function FormatImageInfo(self)
	local attr = self:GetAttribute()
	if not attr.data then
		return
	end
	
	local fileName = Helper:GetFileNameByPath(attr.data.szPath)
	local info = fileName
	if attr.data.uWidth and attr.data.uHeight then
		info = info.." - "..tostring(attr.data.uWidth).."*"..tostring(attr.data.uHeight)
	end
	if attr.data.uFileSize then
		local sSize = ""
		if attr.data.uFileSize <= 1024 then
			sSize = tostring(attr.data.uFileSize).." B"
		elseif attr.data.uFileSize <= 1024*1024 then
			sSize = tostring(math.round(attr.data.uFileSize/1024)).." KB"
		else
			local size = attr.data.uFileSize/(1024*1024)
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
	if not attr then
		LOG("GetAttribute nil!!!!!!!!!")
	end
	attr.bSelect = bSelect 
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
		infoObj:SetText(sInfo)
	end
	
	ownerCtrl:FireExtEvent("OnSelect", attr.bSelect)
end

function OnLButtonDbClick(self)
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	local tree = self:GetOwner()
	local thumbnailContainer = tree:GetUIObject("ThumbnailContainerObj")
	local thumbnailContainerAttr = thumbnailContainer:GetAttribute()
	
	local MainHostWnd = Helper.Selector.select("", "", "MainWnd.Instance")
	Helper.Listener.LastShowWnd = MainHostWnd
	if MainHostWnd then--隐藏主窗口
		MainHostWnd:Show(0)
	end
	
	local userData = {}
	userData.tPictures = thumbnailContainerAttr.tPictures
	userData.sPath = thumbnailContainerAttr.sPath  --文件夹
	userData.index = attr.index
	userData.fileName = attr.fileName
	local wnd = Helper.hostWndManager:GetHostWnd("ImageWnd.Instance")
	if wnd then
		local wndTree = wnd:GetBindUIObjectTree()
		local imageCtrl = wndTree:GetUIObject("FrameWnd.ImageCtrl")
		imageCtrl:SetFolderData(userData)
		wnd:Show(1)
	else
		wnd = Helper:CreateModelessWnd("ImageWnd","ImageWndTree", nil, userData)
	end
	wnd:BringWindowToTop(true)
end

function OnInitControl(self)
	local attr = self:GetAttribute()
	attr.bSelect = false
	
	--没有绑定数据之前，整个Thumbnail是不可见的
	local background = self:GetControlObject("Background")
	background:SetVisible(false)
	background:SetChildrenVisible(false)
end
