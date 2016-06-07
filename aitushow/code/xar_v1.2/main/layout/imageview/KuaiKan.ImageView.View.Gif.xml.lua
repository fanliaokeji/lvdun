
function View_OnPosChange(self)
	local attr = self:GetAttribute()
	if not attr.DocObj then
		return
	end
	self:ResetInfo()
	self:UpdateView(true)
end

function View_OnInitControl(self, bitmap, bitmapWidth, bitmapHeight, bUpdate)
	local attr = self:GetAttribute()
	attr.ScaleRatio = 1.0
	attr.AdaptedScreenRatio = 1.0
end

function View_UpdateView(self, curScaleRatio, bUpdate, bNotUserLowQuality)
	local attr = self:GetAttribute()
	if not attr.DocObj then
		return
	end	
	local viewLayoutObj = self:GetControlObject("client.view.layout")
	
	local l,t,r,b = viewLayoutObj:GetObjPos()
	local width, height = r-l, b-t
	
	local srcBitmapWidth, srcBitmapHeight = attr.DocObj:GetSrcBitmapSize()
	LOG("KuaikanLog attr.ScaleRatio is " .. attr.ScaleRatio .. " srcBitmap width=" .. srcBitmapWidth .. " height=" .. srcBitmapHeight)
	
	local actualWidth, actualHeight = srcBitmapWidth*attr.ScaleRatio, srcBitmapHeight*attr.ScaleRatio
	LOG("KuaikanLog actualWidth is width=" .. actualWidth .. " height=" .. actualHeight)
	
	local TranLeft = attr.ViewSamePosPercentX*width-actualWidth*attr.BitmapSamePosPercentX
	local TranTop = attr.ViewSamePosPercentY*height-actualHeight*attr.BitmapSamePosPercentY
	local TranRight = TranLeft + actualWidth
	local TranBottom = TranTop + actualHeight
	attr.ShowBitmapLeftPos = TranLeft
	attr.ShowBitmapTopPos = TranTop
	attr.ShowBitmapRightPos = TranRight
	attr.ShowBitmapBottomPos = TranBottom
	
	local viewObj = self:GetControlObject("client.view")
	viewObj:SetObjPos(attr.ShowBitmapLeftPos, attr.ShowBitmapTopPos, attr.ShowBitmapRightPos, attr.ShowBitmapBottomPos)
end

function View_GetViewType(self)
	return 2
end

function View_GetAdaptedScreenRatio(self)
	local attr = self:GetAttribute()
	return attr.AdaptedScreenRatio
end

function View_ResetInfo(self)
	local attr = self:GetAttribute()
	if not attr.DocObj then
		return
	end
	
	local viewLayoutObj = self:GetControlObject("client.view.layout")
	local l,t,r,b = viewLayoutObj:GetObjPos()
	local width, height = r-l, b-t
	local srcWidth, srcHeight = attr.DocObj:GetSrcBitmapSize()
	if width >= srcWidth and height >= srcHeight then
		attr.ScaleRatio = 1.0
	else
		attr.ScaleRatio = math.min(width/srcWidth, height/srcHeight)
	end
	
	attr.MidPosX = width/2	
	attr.MidPosY = height/2	

	attr.ViewSamePosPercentX = 0.5
	attr.ViewSamePosPercentY = 0.5
	attr.BitmapSamePosPercentX = 0.5
	attr.BitmapSamePosPercentY = 0.5
	attr.ShowBitmapLeftPos = 0
	attr.ShowBitmapTopPos = 0
	attr.ShowBitmapRightPos = 0
	attr.ShowBitmapBottomPos = 0
	attr.AdaptedScreenRatio = attr.ScaleRatio
	
	self:FireExtEvent("OnScaleRatioChanged")
end

function View_ResetDoc(self, docObj)
	local attr = self:GetAttribute()
	local viewObj = self:GetControlObject("client.view")
	if not docObj then
		if attr.DocObj then
			viewObj:Stop()
			attr.DocObj = nil
		end		
		viewObj:SetGif(nil)
		return
	else
		
		if attr.DocObj then
			local viewObj = self:GetControlObject("client.view")
			viewObj:Stop()
		end
		attr.DocObj = docObj
		self:ResetInfo()
		
		if attr.DocObj then
			local gifObj = attr.DocObj:GetGifObj()
			local viewObj = self:GetControlObject("client.view")
			viewObj:SetGif(gifObj)
			
			viewObj:Play()
		end
	end
end

function View_OnMouseWheel1(self)
	self:RouteToFather()
end
function View_OnLButtonDbClick(self)
	self:RouteToFather()
end

function View_OnMouseWheel(self, x, y, flags)
	local attr = self:GetAttribute()
	
	local osUtil = Helper.APIproxy.OSUtil
	local bCtrlKeyDown = osUtil:GetKeyState(17)
	if bCtrlKeyDown < 0 then
		bCtrlKeyDown = true
	else
		bCtrlKeyDown = false
	end

	local actionType = "1"
	if actionType then
		if actionType == "1" then 
			if bCtrlKeyDown then	
				if flags < 0 then
					self:FireExtEvent("OnNextFileClick")
				else
					self:FireExtEvent("OnPreFileClick")
				end
				return
			end
		
		elseif actionType == "2" then 
			if not bCtrlKeyDown then
				if flags < 0 then
					self:FireExtEvent("OnNextFileClick")
				else
					self:FireExtEvent("OnPreFileClick")
				end
				return
			end
		end
	end
	
	local clientObj = self:GetOwnerControl()
	local viewLayoutObj = self:GetControlObject("client.view.layout")
	local l,t,r,b = viewLayoutObj:GetObjPos()
	local width, height = r-l, b-t
	
	if x < attr.ShowBitmapLeftPos then
		x = attr.ShowBitmapLeftPos
	end
	if x > attr.ShowBitmapRightPos then
		x = attr.ShowBitmapRightPos
	end
	
	if y < attr.ShowBitmapTopPos then
		y = attr.ShowBitmapTopPos
	end
	if y > attr.ShowBitmapBottomPos then
		y = attr.ShowBitmapBottomPos
	end
	
	--local MouseScalMode = ConfigHelper.GetConfigItem("MouseScalMode")
	if MouseScalMode == "2" then 
	else	
		attr.ViewSamePosPercentX = x/width
		attr.ViewSamePosPercentY = y/height
		attr.BitmapSamePosPercentX = (x-attr.ShowBitmapLeftPos)/(attr.ShowBitmapRightPos-attr.ShowBitmapLeftPos)
		attr.BitmapSamePosPercentY = (y-attr.ShowBitmapTopPos)/(attr.ShowBitmapBottomPos-attr.ShowBitmapTopPos)
	end

	if flags >0 then
		clientObj:ScaleImage(0.05)
	else
		clientObj:ScaleImage(-0.05)
	end
	return 0, true, true
end

function View_HandleUpDownWard(self, bUp)
	local clientObj = self:GetOwnerControl()
	if bUp then
		clientObj:ScaleImage(0.05)
	else
		clientObj:ScaleImage(-0.05)
	end
end


function View_GetScaleRatio(self)
	local attr = self:GetAttribute()
	return attr.ScaleRatio
end

function View_SetScaleRatio(self, ScaleRatio)
	local attr = self:GetAttribute()
	local oldRatio = attr.ScaleRatio
	attr.ScaleRatio = ScaleRatio
	if not self:IsParamValid() then
		attr.ScaleRatio = oldRatio
		return 
	end
	self:UpdateView(false)
	self:FireExtEvent("OnScaleRatioChanged")
end

function View_OnLButtonDown(self, x, y, flags)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	owner_attr.bMouseDown = true
	local l,t,r,b = self:GetObjPos()
	owner_attr.XBegin = l+x
	owner_attr.yBegin = t+y
	self:SetCaptureMouse(true)
end

function View_OnLButtonUp(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if owner_attr.bMouseDown then
		owner_attr.bMouseDown = false
		owner:UpdateView(true, true)
	end
	self:SetCaptureMouse(false)
end

function View_OnMouseMove(self, x, y, flags)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if owner_attr.bMouseDown then
		local l,t,r,b = self:GetObjPos()
		local curX = l+x
		local curY = t+y
		local Xoffset = curX - owner_attr.XBegin
		local Yoffset = curY - owner_attr.yBegin
		if Xoffset == 0 and Yoffset == 0 then	
			return 
		end
		owner_attr.XBegin = curX
		owner_attr.yBegin = curY
		
		local l,t,r,b = owner:GetObjPos()
		local width,height = r-l, b-t
		owner_attr.ViewSamePosPercentX = owner_attr.ViewSamePosPercentX + Xoffset/width
		owner_attr.ViewSamePosPercentY = owner_attr.ViewSamePosPercentY + Yoffset/height
		
		owner:UpdateView(true)
	end
end

function LeftWard_OnControlMouseEnter(self)
	self:ShowBtn(true)
end
function LeftWard_OnControlMouseLeave(self)
	self:ShowBtn(false)
end
function LeftWard_OnBtnClick(self)
	local owner = self:GetOwnerControl()
	owner:FireExtEvent("OnPreFileClick")
end

function RightWard_OnControlMouseEnter(self)
	self:ShowBtn(true)
end

function RightWard_OnControlMouseLeave(self)
	self:ShowBtn(false)
end

function RightWard_OnBtnClick(self)
	local owner = self:GetOwnerControl()
	owner:FireExtEvent("OnNextFileClick")
end

function LeftWard_OnLButtonDown(self)
	return 0, false, true
end

function LeftWard_OnLButtonUp(self)
	return 0, false, true
end

function LeftWard_OnMouseMove(self)
	return 0, false, true
end

function RightWard_OnLButtonDown(self)
	return 0, false, true
end

function RightWard_OnLButtonUp(self)
	return 0, false, true
end

function RightWard_OnMouseMove(self)
	return 0, false, true
end

function LeftWard_OnMouseWheel(self)
	return 0, false, true
end

function RightWard_OnMouseWheel(self)
	return 0, false, true
end

function RightWard_OnLButtonDbClick(self)
	
end

function RightWard_OnRButtonDown(self)
	
end

function View_OnRButtonDown(self)
	self:RouteToFather()
end

function LeftWard_OnRButtonDown(self)
	
end

function LeftWard_OnLButtonDbClick(self)
	
end

function View_IsParamValid(self)
	local attr = self:GetAttribute()
	local viewLayoutObj = self:GetControlObject("client.view.layout")
	
	local l,t,r,b = viewLayoutObj:GetObjPos()
	local width, height = r-l, b-t
	
	local srcBitmapWidth, srcBitmapHeight = attr.DocObj:GetSrcBitmapSize()
	local actualWidth, actualHeight = srcBitmapWidth*attr.ScaleRatio, srcBitmapHeight*attr.ScaleRatio
	local TranLeft = attr.ViewSamePosPercentX*width-actualWidth*attr.BitmapSamePosPercentX
	local TranTop = attr.ViewSamePosPercentY*height-actualHeight*attr.BitmapSamePosPercentY
	local TranRight = TranLeft + actualWidth
	local TranBottom = TranTop + actualHeight	
	if TranLeft >=width or TranTop >= height or TranRight <= 0 or TranBottom <= 0 then
		return false
	else
		return true
	end
end

function View_LeftRotate(self)
	
end

function View_RightRotate(self)
	
end
function View_HandleThumbnailViewChange(self)
	
end

function View_HandleThumbnailViewMouseWheel(self)
	
end