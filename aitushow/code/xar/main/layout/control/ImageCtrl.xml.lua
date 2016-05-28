local Helper = XLGetGlobal("Helper")
local graphicUtil = XLGetObject("GRAPHIC.Util")
local OnPosChangeCookie = nil

function AdjustImageBySize(backgroundObj, imageObj, uWidth, uHeight)
	--这里不能用imageObj:GetObjPos,可能imageObj的pos曾经被下面的代码改过
	local imageL, imageT, imageR, imageB = backgroundObj:GetObjPos()
	local imageWidth = imageR - imageL
	local imageHeight = imageB - imageT
	
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

function OnImagePosChange4png(self, oldl, oldt, oldr, oldb, l, t, r, b)
	local pngbkg = self:GetObject("control:PngBkg")
	if pngbkg:GetVisible() then
		pngbkg:SetObjPos(l, t, r, b)
	end
end

function OnImagePosChange(self)
	local ctrl = self:GetOwnerControl()
	local imageObj = ctrl:GetControlObject("Image")
	local gifObj = ctrl:GetControlObject("SeqImageObject.gif")
	local imageContainer = ctrl:GetControlObject("ImageContainer")
	
	
	local attr = ctrl:GetAttribute()
	local curInfo = attr.tPictures and attr.index and attr.tPictures[attr.index]
	local szExt = curInfo and curInfo.szExt 
	local imageWidth, imageHeight = GetObjWH(imageObj)
	local picWidth, picHeight = 0, 0
	if attr.bGif then
		local gif = gifObj:GetGif()
		AdjustImageBySize(imageContainer, gifObj, gif:GetSize())
		gifObj:SetDrawMode(1)
	elseif szExt then
		local bitmap = imageObj:GetBitmap()
		if not bitmap then
			return
		end
	
		_, picWidth, picHeight, _ = bitmap:GetInfo()
		AdjustImageBySize(imageContainer, imageObj, picWidth, picHeight)
		imageObj:SetDrawMode(1)
	end
	
	
	ctrl:FireExtEvent("OnImageSizeChange", imageWidth, imageHeight, picWidth, picHeight)
end

function SetImagePath(self, path)
	LOG("SetImagePath path: ", path)
	local attr = self:GetAttribute()
	local szFolder, filename = string.match(path, "(.+)[\\/]([^?]+)")
	LOG("SetImagePath szFolder: ", szFolder, " filename: ", filename)
	attr.tPictures = graphicUtil:GetDirSupportImgPaths(szFolder)
	attr.index = 1
	
	for i=1, #attr.tPictures do
		if string.upper(attr.tPictures[i].szPath) == string.upper(path) then
			attr.index = i
			break
		end
	end
	LOG("SetImagePath: ", SetImagePath, " index: ", attr.index)
	SetImageByIndex(self, attr.index)
end

function SetImage(self, xlhBitmap)

end

function SetImageByIndex(self, index)
	local attr = self:GetAttribute()
	if not attr.tPictures or not attr.tPictures[index] then
		return false
	end
	
	attr.index = index
	local tImgInfo = attr.tPictures[index]
	local pngbkg = self:GetObject("PngBkg")
	if tImgInfo.szExt == "png" or tImgInfo.fifType == 13 then
		pngbkg:SetVisible(true)
	else
		pngbkg:SetVisible(false)
	end
	local imageObj = self:GetControlObject("Image")
	local imageContainer = self:GetControlObject("ImageContainer")
	local seqImageObject = self:GetControlObject("SeqImageObject.gif")
	local filename = Helper:GetFileNameByPath(tImgInfo.szPath)
	local tree = self:GetOwner()
	local titleCtrl = tree:GetUIObject("FrameWnd.TitleCtrl")
	titleCtrl:SetTitleTextContent(filename)
	
	local rightArrow = self:GetControlObject("RightArrow")
	local leftArrow = self:GetControlObject("LeftArrow")
		
	if GetNextPic(self) then
		rightArrow:Show(true)
	else
		rightArrow:Show(false)
	end
	if GetPrevPic(self) then
		leftArrow:Show(true)
	else
		leftArrow:Show(false)
	end
	
	if tImgInfo.szExt == "gif" then
		seqImageObject:SetVisible(true)
		imageObj:SetVisible(false)
		if not tImgInfo.xlhGif then
			local XGP_Factory = XLGetObject("Xunlei.XGP.Factory")
			local gif = XGP_Factory:LoadGifFromFile(tImgInfo.szPath)
			tImgInfo.xlhGif = gif
			tImgInfo.uWidth, tImgInfo.uHeight = gif:GetSize()
		end
		seqImageObject:SetGif(tImgInfo.xlhGif)
		seqImageObject:Play()
		seqImageObject:SetDrawMode(1)
		AdjustImageBySize(imageContainer, seqImageObject, tImgInfo.uWidth, tImgInfo.uHeight)
		attr.bGif = true
		return
	end
	
	attr.bGif = false
	seqImageObject:SetVisible(false)
	imageObj:SetVisible(true)
	if not tImgInfo.xlhBitmap then
		local requireFiles = {}
		requireFiles[1] = tImgInfo.szPath
		graphicUtil:GetMultiImgInfoByPaths(requireFiles)
	else
		imageObj:SetBitmap(tImgInfo.xlhBitmap)
		
		-- local picObject = Helper.graphicFactory:CreateBitmap("C:\\Users\\mjt\\Desktop\\新建文件夹\\1.png", "RGB32")
		-- SetOnceTimer(function() imageObj:SetBitmap(picObject) end, 3000)
		imageObj:SetDrawMode(1)
		AdjustImageBySize(imageContainer, imageObj, tImgInfo.uWidth, tImgInfo.uHeight)
	end
	
	return true
end

function GetNextPic(self)
	local attr = self:GetAttribute()
	local index = attr.index + 1
	if index <= #attr.tPictures then
		return attr.tPictures[index], index
	else
		return nil
	end
end

function GetPrevPic(self)
	local attr = self:GetAttribute()
	local index = attr.index - 1
	
	if index > 0 then
		return attr.tPictures[index], index
	else
		return nil
	end
end

function DelCurPic(self)
	local attr = self:GetAttribute()
	if attr.index and attr.tPictures and attr.tPictures[attr.index] then
		local picInfo = attr.tPictures[attr.index]
		local MSG = Helper.MessageBox
		local nRet = MSG.MessageBox("确定要删除 \""..Helper:GetFileNameByPath(picInfo.szPath).."\" 吗？", self:GetOwner():GetBindHostWnd())
		if nRet == MSG.ID_YES then
			Helper.tipUtil:DeletePathFile(picInfo.szPath)
			table.remove(attr.tPictures, attr.index)
			if attr.index > #attr.tPictures then
				attr.index = #attr.tPictures
			end
			SetImageByIndex(self, attr.index)
		end
	end
end

local function Rotate(self, angle)
	local attr = self:GetAttribute()
	local index = attr.index
	if not index or not attr.tPictures or not attr.tPictures[index] then
		return
	end
	local xlhBitmap = attr.tPictures[index].xlhBitmap
	if not xlhBitmap then
		return
	end
	
	local newBitmap, newWidth, newHeight = graphicUtil:RotateImgByAngle(xlhBitmap, angle)
	attr.tPictures[index].angle = attr.tPictures[index].angle and (attr.tPictures[index].angle + angle) or angle
	if newBitmap then
		attr.tPictures[index].uHeight = newHeight
		attr.tPictures[index].uWidth = newWidth
		attr.tPictures[index].xlhBitmap = newBitmap
		attr.tPictures[index].angle = attr.tPictures[index].angle
		
		SetImageByIndex(self, index) 
	end
end
function RotateLeft(self)
	Rotate(self, 90)
end

function RotateRight(self)
	Rotate(self, 270)
end

function SetFolderData(self, userData)
	local attr = self:GetAttribute()
	attr.tPictures = userData.tPictures
	
	attr.index = userData.index
	attr.sPath = userData.sPath
	attr.fileName = userData.fileName
	
	SetImageByIndex(self, attr.index)
end

function SetFolder(self, folder)
	
end

function GetObjWH(obj)
	local L, T, R, B = obj:GetObjPos()
	
	return R - L, B - T
end
--如果需要，可以在这里面向外发事件
function OnDragImage(self, event, ...)
	--图片显示不下的时候，才响应拖拽
	local attr = self:GetAttribute()
	local image = self:GetControlObject("Image")
	if attr.bGif then
		image = self:GetControlObject("SeqImageObject.gif")
	end
	local container = self:GetControlObject("ImageContainer")
	-- local bitmap = image and image:GetBitmap()
	-- if not bitmap then return end
	
	local imageContainer = self:GetControlObject("ImageContainer")
	local containerL, containerT, containerR, containerB = imageContainer:GetObjPos()
	local containerWidth, containerHeigth = containerR - containerL, containerB - containerT
	 
	local imageL, imageT, imageR, imageB = image:GetObjPos()
	local imageWidth, imageHeight = imageR - imageL, imageB - imageT
	if imageWidth <= containerWidth and imageHeight <= containerHeigth then
		Helper:LOG("image can show full")
		return
	end
	
	local dragState, cur_x, cur_y = ...
	local attr = self:GetAttribute()
		
	if "draging" == dragState then
		Helper:LOG("dragState draging")
		if not attr.OnLButtonDownX or not attr.OnLButtonDownY then
			Helper:Assert(false, "OnLButtonDownX is nil while draging!!")
			return
		end
		
		local x, y = Helper.tipUtil:GetCursorPos()
		local offsetX, offsetY = x - attr.LastDragPosX, y - attr.LastDragPosY
		attr.LastDragPosX, attr.LastDragPosY = x, y
		
		Helper:LOG("offsetX: "..offsetX.." offsetY: "..offsetY)
		local posL, posT = imageL + offsetX, imageT + offsetY
		
		--防止拖动过界
		if imageWidth >= containerWidth then
			posL = posL<0 and posL or 0
			posL = posL>containerWidth-imageWidth and posL or containerWidth-imageWidth
		else
			posL = imageL
		end
		
		if imageHeight >= containerHeigth then
			posT = posT<0 and posT or 0
			posT = posT>containerHeigth-imageHeight and posT or containerHeigth-imageHeight
		else
			posT = imageT
		end
		
		image:SetObjPos2(posL, posT, imageWidth, imageHeight)
		local containerW, containerH = GetObjWH(container)
		local rectL = -posL
		local rectT = -posT
		self:FireExtEvent("OnImageShowRectChange", rectL, rectT, rectL + containerW, rectT + containerH)
	elseif "start" == dragState then
		Helper:LOG("dragState start")
		if not attr.OnLButtonDownX or not attr.OnLButtonDownY then
			Helper:Assert(false, "OnLButtonDownX is nil while drag start!!")
			return
		end
		--拖拽开始时，记下上次鼠标在屏幕中的位置
		attr.LastDragPosX, attr.LastDragPosY= Helper.tipUtil:GetCursorPos()
	elseif "end" == dragState then--拖拽正常结束
		Helper:LOG("dragState end")
	elseif "cancel" == dragState then--拖拽取消
		Helper:LOG("dragState cancel")
	end
end

function OnImageLButtonDown(self, x, y)
Helper:LOG("OnImageLButtonDown X: "..x.." Y: ".. y)
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	local image = ownerCtrl:GetControlObject("Image")
	image:SetCaptureMouse(true)
	attr.CaptrueMouse = true
	
	attr.OnLButtonDownX = x
	attr.OnLButtonDownY = y
end

function OnImageLButtonUp(self, x, y)
Helper:LOG("OnImageLButtonUp")
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	local image = ownerCtrl:GetControlObject("Image")
	
	if attr.CaptrueMouse and attr.DragState then
		attr.DragState = "end"
		OnDragImage(ownerCtrl, "OnDragImage", attr.DragState, x, y)
		attr.DragState = nil
	end
	
	attr.CaptrueMouse = false
	image:SetCaptureMouse(false)
	
	attr.OnLButtonDownX = nil
	attr.OnLButtonDownY = nil
end

function OnImageMouseMove(self, x, y)
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	
	if not attr.CaptrueMouse then return end
	if not attr.OnLButtonDownX or not attr.OnLButtonDownY then
		return
	end
	
	Helper:LOG("OnImageMouseMove")
	if not attr.DragState then
		if math.abs(attr.OnLButtonDownX - x) >= 3 or math.abs(attr.OnLButtonDownY - y) >= 3 then
			attr.DragState = "start"
			attr.OnLButtonDownX = x
			attr.OnLButtonDownY = y
		end
	else
		attr.DragState = "draging"
	end
	
	if attr.DragState then
		OnDragImage(ownerCtrl, "OnDragImage", attr.DragState, x, y)
	end
end

function OnImageCaptureChange(self, capture)
Helper:LOG("OnImageCaptureChange")
	if capture then
		return
	end
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	if not attr.CaptrueMouse then
		return
	end
	
	if attr.DragState then
		attr.DragState = "cancel"
		OnDragImage(ownerCtrl, "OnDragImage", attr.DragState, x, y)
		attr.DragState = nil
	end
	attr.CaptrueMouse = false
end

function OnImageRButtonUp(self, x, y)
	local curX, curY = Helper.tipUtil:GetCursorPos()
	local tree = self:GetOwner()
	local wnd = tree:GetBindHostWnd()
	local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")	
	local menuTable = GreenShieldMenu.ImageRClickMenu.menuTable
	local menuFunTable = GreenShieldMenu.ImageRClickMenu.menuFunTable
	local userData = {}
	userData.imageCtrl = self:GetOwnerControl()
	Helper:CreateMenu(curX, curY, wnd:GetWndHandle(), menuTable, menuFunTable, userData)
end

-- direction > 0 ：向上滚,放大 < 0：向下滚，缩小
function OnImageMouseWheel(self, x, y, direction, distance)
	local ownerCtrl = self:GetOwnerControl()
	
	
	if not distance or 0 == distance then
		distance = 10
	end
	local curZoomPercent = GetZoomPercent(ownerCtrl)
	if direction > 0 then
		Zoom(ownerCtrl, curZoomPercent + distance)
	else
		curZoomPercent = curZoomPercent - distance
		Zoom(ownerCtrl, curZoomPercent > 10 and curZoomPercent or 10)
	end
end

function GetShowRect(self)
	local imageObj = self:GetControlObject("Image")
	local bitmap = imageObj:GetBitmap()
	if not bitmap then
		return 0,0,0,0
	end
end

function Zoom(self, percent)
	local attr = self:GetAttribute()
	local imageObj = self:GetControlObject("Image")
	local curInfo = attr.tPictures and attr.index and attr.tPictures[attr.index]
	local szExt = curInfo and curInfo.szExt 
	
	local  picWidth, picHeight= 0,0
	if "gif" == szExt then
		imageObj = self:GetControlObject("SeqImageObject.gif")
		local gif = imageObj:GetGif()
		if gif then
			picWidth, picHeight = gif:GetSize()
		else
			return
		end
	else
		local bitmap = imageObj:GetBitmap()
		if bitmap then
			_, picWidth, picHeight,_ = bitmap:GetInfo()
		else
			return
		end
	end
	
	imageObj:SetDrawMode(1)
	local imageContainer = self:GetControlObject("ImageContainer")
	local containerL, containerT, containerR, containerB = imageContainer:GetObjPos()
	local containerWidth, containerHeigth = containerR - containerL, containerB - containerT

	local targetImageWidth, targetImageHeight = picWidth*percent/100, picHeight*percent/100
	local imageL, imageT, imageR, imageB = imageObj:GetObjPos()
	local curImageWidth, curImageHeight = imageR - imageL, imageB - imageT
	local offsetW, offsetH = targetImageWidth- curImageWidth, targetImageHeight - curImageHeight
	-- local offsetL
	if targetImageWidth <= containerWidth and targetImageHeight <= containerHeigth then
		imageL = math.round((containerWidth - targetImageWidth)/2)
		imageR = math.round(imageL + targetImageWidth)
		imageT = math.round((containerHeigth - targetImageHeight) / 2)
		imageB = math.round(imageT + targetImageHeight)
	else
		--支持偏心缩放，即:在缩放过程中，窗口中心处的内容应始终在窗口中心
		-- local 
		
		imageL = imageL - math.round(offsetW/2)
		imageR = imageL + targetImageWidth
		imageT = imageT - math.round(offsetH/2)
		imageB = imageT + targetImageHeight
	end
	imageObj:SetObjPos(imageL, imageT, imageR, imageB)
	self:FireExtEvent("OnImageSizeChange", math.round(targetImageWidth), math.round(targetImageHeight), picWidth, picHeight)
end

function GetZoomPercent(self)
	local attr = self:GetAttribute()
	local imageObj = self:GetControlObject("Image")
	local picWidth, picHeight = 0,0
	if attr.bGif then
		imageObj = self:GetControlObject("SeqImageObject.gif")
		local gif = imageObj:GetGif()
		if not gif then
			return 0
		end
		picWidth, picHeight = gif:GetSize()
	else
		local bitmap = imageObj:GetBitmap()
		if not bitmap then
			return 0
		end
		 _, picWidth, picHeight, _ = bitmap:GetInfo()
	end
	
	local imageL, imageT, imageR, imageB = imageObj:GetObjPos()
	return math.round(100*(imageR - imageL)/picWidth)
end

function OnClickLeftArrow(self)
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	local index = attr.index - 1
	
	SetImageByIndex(ownerCtrl, index)
end

function OnClickRightArrow(self)
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	local index = attr.index + 1
	
	SetImageByIndex(ownerCtrl, index)
end

function OnGetMultiImgInfoCallBack(self, key, tImgInfo)
	--先看是不是当前显示的index
	local attr = self:GetAttribute()
	local tPicData = attr and attr.tPictures and attr.index and attr.tPictures[attr.index]
	if tPicData and tPicData.szPath == tImgInfo.szPath then
		tPicData.xlhBitmap = tImgInfo.xlhBitmap
		tPicData.uWidth = tImgInfo.uWidth
		tPicData.uHeight = tImgInfo.uHeight
		tPicData.fifType = tImgInfo.fifType
		local pngbkg = self:GetObject("PngBkg")
		if tImgInfo.szExt == "png" or tImgInfo.fifType == 13 then
			pngbkg:SetVisible(true)
		else
			pngbkg:SetVisible(false)
		end
		local imageObj = self:GetControlObject("Image")
		local imageContainer = self:GetControlObject("ImageContainer")
		imageObj:SetBitmap(tPicData.xlhBitmap)
		AdjustImageBySize(imageContainer, imageObj, tImgInfo.uWidth, tImgInfo.uHeight)
		imageObj:SetDrawMode(1)
	end
end

function OnInitControl(self)
	graphicUtil:AttachListener(function(key, tImgInfo) OnGetMultiImgInfoCallBack(self, key, tImgInfo) end)
end