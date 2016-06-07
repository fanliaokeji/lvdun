function SetObjVisible(obj, bVisible)
	if not obj then 
		return
	end
	if bVisible == nil then
		bVisible = false
	end
	obj:SetVisible(bVisible)
	obj:SetChildrenVisible(bVisible)
end

function OnInitControl(self)
	SetObjVisible(self, false)
end

function SetImage(self, bitmap, srcWidth, srcHeight)
	if not bitmap then
		return
	end
	local attr = self:GetAttribute()
	attr.SrcWidth = srcWidth
	attr.SrcHeight = srcHeight
	local colorspace, width, height = bitmap:GetInfo()
	local imageProcessor = Helper.APIproxy.GetLuaImageProcessor()
	
	local layoutObj = self:GetControlObject("client.view.layout")
	local viewObj = self:GetControlObject("client.view")
	local l,t,r,b = layoutObj:GetObjPos()
	local viewWidth, viewHeight = r-l, b-t
	if width <= viewWidth and height <= viewHeight then	
		viewObj:SetBitmap(imageProcessor:PreMultiplyBitmap(bitmap))
		viewObj:SetObjPos2((viewWidth-width)/2, (viewHeight-height)/2, width, height)
	else	
		local newBitmap, width, height = imageProcessor:RescaleImage(bitmap, viewWidth, viewHeight, true, 7)
		if newBitmap then
			viewObj:SetBitmap(imageProcessor:PreMultiplyBitmap(newBitmap))
			viewObj:SetObjPos2((viewWidth-width)/2, (viewHeight-height)/2, width, height)
		end
	end
end

function UpdateLayer(self, RegionL, RegionT, RegionW, RegionH)
	local attr = self:GetAttribute()
	local viewObj = self:GetControlObject("client.view")
	local l,t,r,b = viewObj:GetObjPos()
	local width, height = r-l, b-t
	
	local clipL = math.ceil(RegionL/attr.SrcWidth*width)
	local clipT = math.ceil(RegionT/attr.SrcHeight*height)
	local clipW = math.ceil(RegionW/attr.SrcWidth*width)
	local clipH = math.ceil(RegionH/attr.SrcHeight*height)
	if clipW < 1 then
		clipW = 1
	end
	if clipH < 1 then
		clipH = 1
	end
	
	if clipL + clipW > width then
		clipL = width - clipW
	end
	if clipT + clipH > height then
		clipT = height - clipH
	end
	attr.ClipL = clipL
	attr.ClipT = clipT
	attr.ClipW = clipW
	attr.ClipH = clipH
	
	local ShowThumbnail = "1"
	if attr.ClipL ==0 and attr.ClipT == 0 and attr.ClipW == width and attr.ClipH == height then
		if attr.IsMouseDown then
			attr.IsMouseDown = false
			local layerObj = self:GetControlObject("client.view.Layer")
			layerObj:SetCaptureMouse(false)
			layerObj:SetCursorID("IDC_ARROW")
		end
		SetObjVisible(self, false)
		return
	elseif ShowThumbnail == "0" then
		SetObjVisible(self, false)
		return
	else
		SetObjVisible(self, true)
	end
	local imageProcessor = Helper.APIproxy.GetLuaImageProcessor()
	local clipBitmap = imageProcessor:DrawThumbnailViewLayer(width, height, attr.ClipL, attr.ClipT, attr.ClipW, attr.ClipH)
	if clipBitmap then 
		local layerObj = self:GetControlObject("client.view.Layer")
		layerObj:SetBitmap(clipBitmap)
	end
end

function Layer_OnLButtonDown(self, x, y)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if x >= owner_attr.ClipL and x <= owner_attr.ClipL+owner_attr.ClipW and y >= owner_attr.ClipT and y <= owner_attr.ClipT + owner_attr.ClipH then
		owner_attr.StartX = x
		owner_attr.StartY = y
		owner_attr.IsMouseDown = true
		self:SetCaptureMouse(true)
		self:SetCursorID("hand_cursor")
	else
		local l,t,r,b = self:GetObjPos()
		owner:FireExtEvent("OnViewRectLButtonDown", x/(r-l), y/(b-t))
		
		owner_attr.StartX = x
		owner_attr.StartY = y
		owner_attr.IsMouseDown = true
		self:SetCaptureMouse(true)
		self:SetCursorID("hand_cursor")
	end
end

function Layer_OnLButtonUp(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if owner_attr.IsMouseDown then
		owner_attr.IsMouseDown = false
		self:SetCaptureMouse(false)
		self:SetCursorID("IDC_ARROW")
	end
end

function Layer_OnMouseMove(self, x, y)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if owner_attr.IsMouseDown then	
		local l,t,r,b = self:GetObjPos()
		local width, height = r-l, b-t
		local offsetX, offsetY = x - owner_attr.StartX, y - owner_attr.StartY
		
		owner_attr.StartX = x
		owner_attr.StartY = y
		
		local viewObj = owner:GetControlObject("client.view")
		local l,t,r,b = viewObj:GetObjPos()
		local width, height = r-l, b-t
		offsetX = offsetX/width*owner_attr.SrcWidth
		offsetY = offsetY/height*owner_attr.SrcHeight
		owner:FireExtEvent("OnViewRectChanged", offsetX, offsetY)
	end
end	

function OnLButtonDown(self)
	
end

function OnMouseMove(self)
	
end

function Layer_OnMouseWheel(self, x, y, flags)
	local owner = self:GetOwnerControl()
	owner:FireExtEvent("OnViewRectMouseWheel", flags)
end

function Layer_OnMouseWheel1(self)
	self:RouteToFather()
end

function Cation_OnLButtonDown(self, x, y)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	owner_attr.IsCaptionMouseDown = true
	owner_attr.StartX = x
	owner_attr.StartY = y
	self:SetCaptureMouse(true)
end

function Cation_OnLButtonUp(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if owner_attr.IsCaptionMouseDown then
		owner_attr.IsCaptionMouseDown = false
		self:SetCaptureMouse(false)
	end
end

function Cation_OnMouseMove(self, x, y)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if owner_attr.IsCaptionMouseDown then
		local offsetX = x-owner_attr.StartX
		local offsetY = y-owner_attr.StartY
		local l,t,r,b = owner:GetObjPos()
		local width, height = r-l, b-t
		l=l+offsetX
		t=t+offsetY
		if l < 0 then
			l = 0
		end
		if t < 0 then
			t = 0
		end
		local ParentObj = owner:GetParent()
		local pL, pT, pR, pB = ParentObj:GetObjPos()
		local pWidth, pHeight = pR - pL, pB - pT
		if l+width > pWidth then
			l = pWidth-width
		end
		if t+height > pHeight then
			t = pHeight - height
		end
		owner:SetObjPos2(l, t, width, height)
	end
end

function Cation_OnCloseBtnClick(self)
	local owner = self:GetOwnerControl()
	SetObjVisible(owner, false)
end

function UpdateTitle(self, curRatio)
	local titleObj = self:GetControlObject("client.caption.title")
	curRatio = curRatio*100-curRatio*100%1 .. "%"
	titleObj:SetText("缩放至" .. curRatio)
end