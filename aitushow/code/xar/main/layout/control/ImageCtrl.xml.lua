local Helper = XLGetGlobal("Helper")
local OnPosChangeCookie = nil

function OnPosChange(self)
	local imageObj = self:GetControlObject("Image")
	local imageContainer = self:GetControlObject("ImageContainer")
	local bitmap = imageObj:GetBitmap()
	if not bitmap then
		return
	end
	
	local containerL, containerT, containerR, containerB = imageContainer:GetObjPos()
	local containerWidth = containerR - containerL
	local containerHeigth = containerB - containerT
	local _, picWidth, picHeight, _ = bitmap:GetInfo()
	
	local imageL, imageT, imageR, imageB, imageWidth, imageHeight = nil, nil, nil, nil, nil, nil
	
	if picWidth/picHeight > containerWidth/containerHeigth then
		--宽扁型的图片，先满足宽度
		if picWidth > containerWidth then
			imageWidth = containerWidth
			imageL = 0
		else
			imageWidth = picWidth
			imageL = math.round((containerWidth - picWidth)/2)
		end
		
		imageR = imageL + imageWidth
		imageHeight = math.round(imageWidth * picHeight / picWidth)
		Helper:Assert(imageHeight <= containerHeigth, "imageHeight must <= containerHeigth!!")
		imageT = math.round((containerHeigth - imageHeight) / 2)
		imageB = imageT + imageHeight
	else
		--瘦长型的图片，先满足高度
		if picHeight > containerHeigth then
			imageHeight = containerHeigth
			imageT = 0
		else
			imageHeight = picHeight
			imageT = math.round((containerHeigth - picHeight) / 2)
		end
		
		imageB = imageT + imageHeight
		imageWidth = math.round(imageHeight * picWidth / picHeight)
		Helper:Assert(imageWidth <= containerWidth, "imageWidth must <= containerWidth!!")
		imageL = math.round((containerWidth - imageWidth) / 2)
		imageR = imageL + imageWidth
	end
	
	imageObj:SetObjPos(imageL, imageT, imageR, imageB)
	self:FireExtEvent("OnImageSizeChange", imageWidth, imageHeight, picWidth, picHeight)
end

function SetImagePath(self, path)
	local imageObj = self:GetControlObject("Image")
	local bitmap = Helper.graphicFactory:CreateBitmap(path, "RGB32")
	
	if not OnMainWndResizeCookie then
		local imageContainer = self:GetControlObject("ImageContainer")
		OnPosChangeCookie = imageContainer:AttachListener("OnPosChange", false, function() OnPosChange(self) end)
		imageObj:SetDrawMode(1)
	end
	
	imageObj:SetBitmap(bitmap)
	OnPosChange(self)
end

function SetFolder(self, folder)
	
end

--如果需要，可以在这里面向外发事件
function OnDragImage(self, event, ...)
	--图片显示不下的时候，才响应拖拽
	local image = self:GetControlObject("Image")
	local bitmap = image and image:GetBitmap()
	if not bitmap then return end
	
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
	Helper:CreateMenu(curX, curY, wnd:GetWndHandle(), menuTable, menuFunTable)
end

-- direction > 0 ：向上滚,放大 < 0：向下滚，缩小
function OnImageMouseWheel(self, x, y, direction, distance)
	local ownerCtrl = self:GetOwnerControl()
	local bitmap = self:GetBitmap()
	if not bitmap then
		return
	end
	
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
	local imageObj = self:GetControlObject("Image")
	local bitmap = imageObj:GetBitmap()
	if not bitmap then
		return 0
	end
	local imageContainer = self:GetControlObject("ImageContainer")
	local containerL, containerT, containerR, containerB = imageContainer:GetObjPos()
	local containerWidth, containerHeigth = containerR - containerL, containerB - containerT
	
	local _, picWidth, picHeight, _ = bitmap:GetInfo()
	
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
	local imageObj = self:GetControlObject("Image")
	local bitmap = imageObj:GetBitmap()
	if not bitmap then
		return 0
	end
	local _, picWidth, picHeight, _ = bitmap:GetInfo()
	local imageL, imageT, imageR, imageB = imageObj:GetObjPos()
	return math.round(100*(imageR - imageL)/picWidth)
end

function OnClickLeftArrow(self)
	XLMessageBox("OnClickLeftArrow")
end

function OnClickRightArrow(self)
	XLMessageBox("OnClickRightArrow")
end
