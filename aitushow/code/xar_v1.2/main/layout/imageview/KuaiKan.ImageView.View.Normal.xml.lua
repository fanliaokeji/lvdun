local Status_Loading = 1
local Status_Normal = 2
local Status_Scaling = 3
local Status_Resizing = 4
local g_ScaleIndex = 0


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

function LeftWard_ShowBtn(self, bShow)
	local btn = self:GetControlObject("client.btn")
	SetObjVisible(btn, bShow)
end


function Ward_OnBtnClick(self)
	local owner = self:GetOwnerControl()
	owner:FireExtEvent("OnBtnClick")
end

function RightWard_ShowBtn(self, bShow)
	local btn = self:GetControlObject("client.btn")
	SetObjVisible(btn, bShow)
end


function View_ResetInfo(self, bMustSrcSize)
	local attr = self:GetAttribute()
	if not attr.DocObj then
		return
	end
	
	local viewLayoutObj = self:GetControlObject("client.view.layout")
	local l,t,r,b = viewLayoutObj:GetObjPos()
	local width, height = r-l, b-t
	local srcWidth, srcHeight = attr.DocObj:GetSrcBitmapSize()
	if srcHeight/srcWidth > 2 then 
		attr.IsLongImage = true
	else
		attr.IsLongImage = false
	end
	if width >= srcWidth and height >= srcHeight then
		attr.AdaptedScreenRatio = 1.0
	else
		attr.AdaptedScreenRatio = math.min(width/srcWidth, height/srcHeight)
	end
	
	attr.ScaleRatio = attr.AdaptedScreenRatio
	
	attr.ViewSamePosPercentX = 0.5
	attr.BitmapSamePosPercentX = 0.5
	attr.BitmapSamePosPercentY = 0.5
	
	if bMustSrcSize and attr.IsLongImage and attr.AdaptedScreenRatio < 1.0 then 
		attr.ScaleRatio = 1.0
		attr.ViewSamePosPercentY = 1.0+ (srcHeight/2-height)/height
	else
		attr.ViewSamePosPercentY = 0.5
	end
	
	attr.ShowBitmapLeftPos = 0
	attr.ShowBitmapTopPos = 0
	attr.ShowBitmapRightPos = 0
	attr.ShowBitmapBottomPos = 0
	self:FireExtEvent("OnScaleRatioChanged")
end

function View_GetAdaptedScreenRatio(self)
	local attr = self:GetAttribute()
	return attr.AdaptedScreenRatio
end

function View_OnPosChange(self)
	local attr = self:GetAttribute()
	if not attr.DocObj then
		return
	end
	self:ResetInfo(false)
	
	self:UpdateView(nil, true)
end

function View_OnInitControl(self, bitmap, bitmapWidth, bitmapHeight, bUpdate)
	local attr = self:GetAttribute()
	attr.ScaleRatio = 1.0	
	attr.AdaptedScreenRatio = 1.0
	local ShowScaleObj = self:GetControlObject("client.scaleShow")
	SetObjVisible(ShowScaleObj, false)
	
end

function View_GetScaleRatio(self)
	local attr = self:GetAttribute()
	return attr.ScaleRatio
end

local g_ScaleRatioList = {}
function Scale(self, ScaleRatio)
	local attr = self:GetAttribute()
	local function OnTimer(timerManager, timerID)
		if g_ScaleRatioList[1] then
			self:UpdateView(g_ScaleRatioList[1], true)
			table.remove(g_ScaleRatioList , 1)
			self:FireExtEvent("OnScaleRatioChanged")
		else
			timerManager:KillTimer(timerID)
		end		
	end
	if g_ScaleRatioList[1] then
		table.insert(g_ScaleRatioList, ScaleRatio)
	else
		table.insert(g_ScaleRatioList, ScaleRatio)
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
		timerManager:SetTimer(OnTimer ,10)
	end
end


function View_SetScaleRatio(self, ScaleRatio)
	local attr = self:GetAttribute()
	do
		local oldRatio = attr.ScaleRatio
		attr.ScaleRatio = ScaleRatio
		if not self:IsParamValid() then
			attr.ScaleRatio = oldRatio
			return
		end
		self:UpdateView(nil, true)
		self:ShowScaleInfo()
		self:FireExtEvent("OnScaleRatioChanged")
		
		local viewObj = self:GetControlObject("client.view")
		if attr.ScaleRatio > attr.AdaptedScreenRatio then
			viewObj:SetCursorID("cur_hand_normal")
		else
			viewObj:SetCursorID("IDC_ARROW")
		end
		return
	end
	
	local curRatio = attr.ScaleRatio
	if g_ScaleRatioList[1] then 
		curRatio = g_ScaleRatioList[1]
		g_ScaleRatioList = {}
		table.insert(g_ScaleRatioList, curRatio)
	end
	local offset = ScaleRatio - curRatio
	attr.ScaleRatio = ScaleRatio
	if offset < 0 then
		local newOffSet = offset / 6
		for i=1, 5 do
			Scale(self, curRatio + i*newOffSet)
		end
		Scale(self, ScaleRatio)
	elseif offset > 0 then
		local newOffSet = offset / 6
		for i=1, 5 do
			Scale(self, curRatio + i*newOffSet)
		end
		Scale(self, ScaleRatio)
	end
end

function View_ResetDoc(self, docObj)
	local attr = self:GetAttribute()
	local viewBkgObj = self:GetControlObject("client.view.bkg")
	viewBkgObj:SetObjPos2(0, 0, 0, 0)
	if not docObj then 
		attr.DocObj = nil
		attr.ScaleRatio = 1.0	
		attr.AdaptedScreenRatio = 1.0
		
		local viewObj = self:GetControlObject("client.view")
		viewObj:SetObjPos2(0,0,"father.width","father.height")
		viewObj:SetBitmap(nil)
		g_ScaleIndex  = g_ScaleIndex + 1
		return
	end
	attr.DocObj = docObj
	self:ResetInfo()
	self:ResetThumbnail()
end

function View_SetBitmap(self, bitmap, l, t, w, h)
	local attr = self:GetAttribute()
	if not bitmap then
		return
	end
	local viewObj = self:GetControlObject("client.view")
	if not viewObj then 
		return
	end
	viewObj:SetObjPos2(l, t, w, h)
	local viewBkgObj = self:GetControlObject("client.view.bkg")
	viewBkgObj:SetObjPos2(l, t, w, h)

	local imageProcessor = Helper.APIproxy.GetLuaImageProcessor()
	viewObj:SetBitmap(imageProcessor:PreMultiplyBitmap(bitmap))
end

function View_IsParamValid(self)
	local attr = self:GetAttribute()
	local viewLayoutObj = self:GetControlObject("client.view.layout")
	local l,t,r,b = viewLayoutObj:GetObjPos()
	local width, height = r-l, b-t
	local ScaleRatio = attr.ScaleRatio
	local srcBitmapWidth, srcBitmapHeight = attr.DocObj:GetSrcBitmapSize()
	local actualWidth, actualHeight = srcBitmapWidth*ScaleRatio, srcBitmapHeight*ScaleRatio
	if actualWidth < 1 or actualHeight < 1 then
		return false
	end
	local TranLeft = attr.ViewSamePosPercentX*width-actualWidth*attr.BitmapSamePosPercentX
	local TranTop = attr.ViewSamePosPercentY*height-actualHeight*attr.BitmapSamePosPercentY
	local TranRight = TranLeft + actualWidth
	local TranBottom = TranTop + actualHeight
	attr.ShowBitmapLeftPos = TranLeft
	attr.ShowBitmapTopPos = TranTop
	attr.ShowBitmapRightPos = TranRight
	attr.ShowBitmapBottomPos = TranBottom
	
	local intersectLeft = math.max(TranLeft, 0)
	local intersectTop = math.max(TranTop, 0)
	local intersectRight = math.min(TranRight, width)
	local intersectBottom = math.min(TranBottom, height)
	if intersectRight > intersectLeft + math.max(3, 3*ScaleRatio) and intersectBottom > intersectTop + math.max(3, 3*ScaleRatio) then
		return true
	else
		return false
	end
end

function View_UpdateView(self, curScaleRatio, bUpdate, bNotUserLowQuality)
	local attr = self:GetAttribute()
	if not attr.DocObj then
		return
	end
	g_ScaleIndex  = g_ScaleIndex + 1
	local viewLayoutObj = self:GetControlObject("client.view.layout")
	local l,t,r,b = viewLayoutObj:GetObjPos()
	local width, height = r-l, b-t
	local ScaleRatio = attr.ScaleRatio
	if curScaleRatio then
		ScaleRatio = curScaleRatio
	end
	local srcBitmapWidth, srcBitmapHeight = attr.DocObj:GetSrcBitmapSize()
	local actualWidth, actualHeight = srcBitmapWidth*ScaleRatio, srcBitmapHeight*ScaleRatio
	local TranLeft = attr.ViewSamePosPercentX*width-actualWidth*attr.BitmapSamePosPercentX
	local TranTop = attr.ViewSamePosPercentY*height-actualHeight*attr.BitmapSamePosPercentY
	local TranRight = TranLeft + actualWidth
	local TranBottom = TranTop + actualHeight
	attr.ShowBitmapLeftPos = TranLeft
	attr.ShowBitmapTopPos = TranTop
	attr.ShowBitmapRightPos = TranRight
	attr.ShowBitmapBottomPos = TranBottom
	local intersectLeft = math.max(TranLeft, 0)
	local intersectTop = math.max(TranTop, 0)
	local intersectRight = math.min(TranRight, width)
	local intersectBottom = math.min(TranBottom, height)
	if intersectLeft >= intersectRight or intersectTop >= intersectBottom then
	else
	end
	local imageLeft = actualWidth*attr.BitmapSamePosPercentX - attr.ViewSamePosPercentX*width + intersectLeft
	local imageTop = actualHeight*attr.BitmapSamePosPercentY - attr.ViewSamePosPercentY*height + intersectTop
	local imageWidth = intersectRight-intersectLeft
	local imageHeiht = intersectBottom-intersectTop
	local actualImageLeft = math.max(imageLeft/ScaleRatio, 0)
	local actualImageTop = math.max(imageTop/ScaleRatio, 0)
	local actualWidth = math.min(imageWidth/ScaleRatio, srcBitmapWidth)
	local actualHeight = math.min(imageHeiht/ScaleRatio, srcBitmapHeight)
	
	local AdaptedBitmap, AdaptedBitmapWidth, AdaptedBitmapHeight = attr.DocObj:GetAdaptedBitmap()
	local AdaptedLeft = actualImageLeft/srcBitmapWidth * AdaptedBitmapWidth
	local AdaptedTop = actualImageTop/srcBitmapHeight * AdaptedBitmapHeight
	local AdaptedWidth = math.floor(actualWidth/srcBitmapWidth * AdaptedBitmapWidth + 0.5)
	if AdaptedLeft + AdaptedWidth > AdaptedBitmapWidth then
		AdaptedLeft = AdaptedBitmapWidth - AdaptedWidth
	end
	local AdaptedHeight =  math.floor(actualHeight/srcBitmapHeight * AdaptedBitmapHeight + 0.5)
	if AdaptedTop + AdaptedHeight > AdaptedBitmapHeight then
		AdaptedTop = AdaptedBitmapHeight - AdaptedHeight
	end
	local imageProcessor = Helper.APIproxy.GetLuaImageProcessor()
	if AdaptedWidth == 0 or AdaptedHeight == 0 then
		return
	end
	local newBitmap, newWidth, newHeight = imageProcessor:ClipSubBindBitmap(AdaptedBitmap, AdaptedLeft, AdaptedTop, AdaptedWidth, AdaptedHeight)
	if newBitmap then
		if newWidth ~= intersectRight - intersectLeft then
			if ScaleRatio <= AdaptedBitmapWidth/srcBitmapWidth or AdaptedBitmapWidth/srcBitmapWidth == 1 then
				if bNotUserLowQuality then
					local scaleBitmap, newWidth, newHeight = imageProcessor:RescaleImage(newBitmap, math.floor(intersectRight - intersectLeft+0.5), math.floor(intersectBottom-intersectTop+0.5), false, 7)
					if scaleBitmap then
						self:SetBitmap(scaleBitmap, math.floor(intersectLeft+0.5), math.floor(intersectTop+0.5), newWidth, newHeight)
					end
					local clientObj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
					clientObj:SetStatus(Status_Normal)
				else
					local scaleBitmap, newWidth, newHeight = imageProcessor:RescaleImage(newBitmap, math.floor(intersectRight - intersectLeft+0.5), math.floor(intersectBottom-intersectTop+0.5), false, 3)
					if scaleBitmap then
						self:SetBitmap(scaleBitmap, math.floor(intersectLeft+0.5), math.floor(intersectTop+0.5), newWidth, newHeight)
					end
					if not attr.bMouseDown then
						local CurIndex = g_ScaleIndex
						local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
						local function OnTimer()
							if CurIndex == g_ScaleIndex then
								local scaleBitmap, newWidth, newHeight = imageProcessor:RescaleImage(newBitmap, math.floor(intersectRight - intersectLeft+0.5), math.floor(intersectBottom-intersectTop+0.5), false, 7)
								if scaleBitmap then
									self:SetBitmap(scaleBitmap, math.floor(intersectLeft+0.5), math.floor(intersectTop+0.5), newWidth, newHeight)
								end
								local clientObj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
								clientObj:SetStatus(Status_Normal)
								
								Helper.APIproxy.Lua_Gc()
							end						
						end
						timerManager:SetOnceTimer(OnTimer , 200)
					end
				end	
			else
				local clientObj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
				
				actualWidth = math.floor(actualWidth+0.5)
				actualHeight = math.floor(actualHeight+0.5)
				if actualImageLeft + actualWidth > srcBitmapWidth then
					actualImageLeft = srcBitmapWidth - actualWidth
				end
				if actualImageTop + actualHeight > srcBitmapHeight then
					actualImageTop = srcBitmapHeight - actualHeight
				end
				local item = {}
				item.ViewL = math.floor(intersectLeft+0.5)
				item.ViewT = math.floor(intersectTop+0.5)
				item.ViewW = math.floor(intersectRight - intersectLeft+0.5)
				item.ViewH = math.floor(intersectBottom-intersectTop+0.5)
				item.ClipL = actualImageLeft
				item.ClipT = actualImageTop
				item.ClipW = actualWidth
				item.ClipH = actualHeight
				item.CurIndex = g_ScaleIndex
				
				local srcBitmap =  attr.DocObj:GetSrcBitmap()
				if bNotUserLowQuality then
					clientObj:SetStatus(Status_Scaling)
					if srcBitmap then
						self:AsynHandleScale(srcBitmap, item)
					else
						clientObj:SetScaleCallBackParam(item)
					end
				else 
					if math.abs(ScaleRatio-1.0) < 0.00001 and srcBitmap then
						self:AsynHandleScale(srcBitmap, item)
					else
						local newBitmap, newWidth, newHeight = imageProcessor:RescaleImage(newBitmap, math.floor(intersectRight - intersectLeft+0.5), math.floor(intersectBottom-intersectTop+0.5), false, 3)
						if newBitmap then
							self:SetBitmap(newBitmap, math.floor(intersectLeft+0.5), math.floor(intersectTop+0.5), newWidth, newHeight)
						end
						if not attr.bMouseDown then
							clientObj:SetStatus(Status_Scaling)
							local CurIndex = g_ScaleIndex
							local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
							local function OnTimer()
								if CurIndex == g_ScaleIndex then
									local srcBitmap =  attr.DocObj:GetSrcBitmap()
									if srcBitmap then
										self:AsynHandleScale(srcBitmap, item)
									else
										clientObj:SetScaleCallBackParam(item)
									end
								end
							end
							timerManager:SetOnceTimer(OnTimer , 200)		
						end
					end
				end
			end
		else
			newBitmap = newBitmap:Clone()
			self:SetBitmap(newBitmap, intersectLeft, intersectTop, intersectRight-intersectLeft, intersectBottom-intersectTop)
		end
	end
	if bUpdate then
		local hostWnd = Helper.Selector.select("", "", "Kuaikan.MainWnd.Instance")
		Helper.APIproxy.ForceUpdateWndShow(hostWnd:GetWndHandle())
	end

	local clientObj = self:GetOwnerControl()
	local thumbnailView = clientObj:GetThumbnailView()
	thumbnailView:UpdateLayer(actualImageLeft, actualImageTop, actualWidth, actualHeight)
	
	Helper.APIproxy.Lua_Gc()
end

function View_AsynHandleScale(self, bitmap, item)
	if item.CurIndex ~= g_ScaleIndex then
		return
	end
	local ScaleBitmapOP = Helper.APIproxy.CreateScaleBitmapOP()
	ScaleBitmapOP:SetParam(bitmap, item)
	local cookieId = 0
	cookieId = ScaleBitmapOP:AttachListener("OnOperationComplete", function (errorCode, bitmap, l,t,w,h,index)
		if errorCode == 0 then
			if index == g_ScaleIndex then
				local clientObj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
				clientObj:SetStatus(Status_Normal)
				self:SetBitmap(bitmap, l,t,w,h)
				Helper.APIproxy.Lua_Gc()
			else
			end
			bitmap = nil
		else
			if index == g_ScaleIndex then
				local clientObj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
				clientObj:SetStatus(Status_Normal)
			end
		end	
		ScaleBitmapOP:DetachListener("OnOperationComplete", cookieId)
		ScaleBitmapOP:Release()
	end)
	ScaleBitmapOP:AddRef()
	ScaleBitmapOP:Execute(true)
end
function View_OnMouseWheel1(self)
	self:RouteToFather()
end
function View_OnLButtonDbClick(self)
	self:RouteToFather()
end
function View_OnRButtonDown(self)
	self:RouteToFather()
end

function View_IsOutValidRegion(self)
	local attr = self:GetAttribute()
	local viewLayoutObj = self:GetControlObject("client.view.layout")
	local l,t,r,b = viewLayoutObj:GetObjPos()
	local width, height = r-l, b-t
	local ScaleRatio = attr.ScaleRatio
	local srcBitmapWidth, srcBitmapHeight = attr.DocObj:GetSrcBitmapSize()
	
	local actualWidth, actualHeight = srcBitmapWidth*ScaleRatio, srcBitmapHeight*ScaleRatio
	if actualWidth < 1 or actualHeight < 1 then 
		return true
	end
	
	local TranLeft = attr.ViewSamePosPercentX*width-actualWidth*attr.BitmapSamePosPercentX
	local TranTop = attr.ViewSamePosPercentY*height-actualHeight*attr.BitmapSamePosPercentY
	local TranRight = TranLeft + actualWidth
	local TranBottom = TranTop + actualHeight
	if TranTop >= 60 then
		return 1
	end
	if height - TranBottom >= 60 then
		return 2
	end
	return 0
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
	if actionType == "1" then 
		if bCtrlKeyDown then	
			
			if flags < 0 then
				if attr.IsLongImage and attr.ScaleRatio > attr.AdaptedScreenRatio then 
					local ret = View_IsOutValidRegion(self)
					if ret ~= 2 then
						local oldViewSamePosPercentY =  attr.ViewSamePosPercentY 
						attr.ViewSamePosPercentY = attr.ViewSamePosPercentY - 0.1
						if self:IsParamValid() then
							self:UpdateView()
						else
							attr.ViewSamePosPercentY = oldViewSamePosPercentY
						end
					else
						self:FireExtEvent("OnNextFileClick")
					end
				else
					self:FireExtEvent("OnNextFileClick")
				end
			else
				if attr.IsLongImage and attr.ScaleRatio > attr.AdaptedScreenRatio then 
					local ret = View_IsOutValidRegion(self)
					if ret ~= 1 then
						local oldViewSamePosPercentY =  attr.ViewSamePosPercentY 
						attr.ViewSamePosPercentY = attr.ViewSamePosPercentY + 0.1
						if self:IsParamValid() then
							self:UpdateView()
						else
							attr.ViewSamePosPercentY = oldViewSamePosPercentY
						end
					else
						self:FireExtEvent("OnPreFileClick")
					end
				else
					self:FireExtEvent("OnPreFileClick")
				end
			end
			return 
		end
	
	elseif actionType == "2" then 
		if not bCtrlKeyDown then
			if flags < 0 then
				if attr.IsLongImage and attr.ScaleRatio > attr.AdaptedScreenRatio then 
					local ret = View_IsOutValidRegion(self)
					if ret ~= 2 then
						local oldViewSamePosPercentY =  attr.ViewSamePosPercentY 
						attr.ViewSamePosPercentY = attr.ViewSamePosPercentY - 0.1
						if self:IsParamValid() then
							self:UpdateView()
						else
							attr.ViewSamePosPercentY = oldViewSamePosPercentY
						end
					else
						self:FireExtEvent("OnNextFileClick")
					end
				else
					self:FireExtEvent("OnNextFileClick")
				end
			else
				if attr.IsLongImage and attr.ScaleRatio > attr.AdaptedScreenRatio then 
					local ret = View_IsOutValidRegion(self)
					if ret ~= 1 then
						local oldViewSamePosPercentY =  attr.ViewSamePosPercentY 
						attr.ViewSamePosPercentY = attr.ViewSamePosPercentY + 0.1
						if self:IsParamValid() then
							self:UpdateView()
						else
							attr.ViewSamePosPercentY = oldViewSamePosPercentY
						end
					else
						self:FireExtEvent("OnPreFileClick")
					end
				else
					self:FireExtEvent("OnPreFileClick")
				end
			end
			return
		end
	end
	
	if not attr.DocObj then
		return
	end
	local clientObj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	local viewLayoutObj = self:GetControlObject("client.view.layout")
	local l,t,r,b = viewLayoutObj:GetObjPos()
	local width, height = r-l, b-t
	
	local bOutBitmap = false
	if x < attr.ShowBitmapLeftPos then
		x = attr.ShowBitmapLeftPos
		bOutBitmap = true
	end
	if x > attr.ShowBitmapRightPos then
		x = attr.ShowBitmapRightPos
		bOutBitmap = true
	end
	
	if y < attr.ShowBitmapTopPos then
		y = attr.ShowBitmapTopPos
	end
	if y > attr.ShowBitmapBottomPos then
		y = attr.ShowBitmapBottomPos
	end

	
	if bOutBitmap and tonumber(attr.ScaleRatio) == 1 and attr.IsLongImage and attr.AdaptedScreenRatio < 1.0 then 
		local oldViewSamePosPercentY =  attr.ViewSamePosPercentY 
		if flags < 0 then
			attr.ViewSamePosPercentY = attr.ViewSamePosPercentY - 0.1
		else
			attr.ViewSamePosPercentY = attr.ViewSamePosPercentY + 0.1
		end
		if self:IsParamValid() then
			self:UpdateView()
		else
			attr.ViewSamePosPercentY = oldViewSamePosPercentY
		end
		return
	else
		--local MouseScalMode = ConfigHelper.GetConfigItem("MouseScalMode")
		if MouseScalMode == "2" then 
		else	
			attr.ViewSamePosPercentX = x/width
			attr.ViewSamePosPercentY = y/height
			attr.BitmapSamePosPercentX = (x-attr.ShowBitmapLeftPos)/(attr.ShowBitmapRightPos-attr.ShowBitmapLeftPos)
			attr.BitmapSamePosPercentY = (y-attr.ShowBitmapTopPos)/(attr.ShowBitmapBottomPos-attr.ShowBitmapTopPos)
		end
	end
	if flags >0 then
		clientObj:ScaleImage(0.05)
	else
		clientObj:ScaleImage(-0.05)
	end
	return 0, true, true
end

function View_HandleUpDownWard(self, bUp)
	local attr = self:GetAttribute()
	if attr.IsLongImage and attr.ScaleRatio > attr.AdaptedScreenRatio then 
		local oldViewSamePosPercentY =  attr.ViewSamePosPercentY 
		if not bUp then
			attr.ViewSamePosPercentY = attr.ViewSamePosPercentY - 0.1
		else
			attr.ViewSamePosPercentY = attr.ViewSamePosPercentY + 0.1
		end
		if self:IsParamValid() then
			self:UpdateView()
		else
			attr.ViewSamePosPercentY = oldViewSamePosPercentY
		end
	else
		local clientObj = self:GetOwnerControl()
		if bUp then
			clientObj:ScaleImage(0.05)
		else
			clientObj:ScaleImage(-0.05)
		end
	end
end

function View_OnLButtonDown(self, x, y, flags)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	local viewObj = owner:GetControlObject("client.view")
	local l,t,r,b = viewObj:GetObjPos()
	local width, height = r-l, b-t
	if x < 0 or x > width or y < 0 or y > height then
		
		self:RouteToFather()
		return
	end

	owner_attr.bMouseDown = true
	local l,t,r,b = self:GetObjPos()
	owner_attr.XBegin = l+x
	owner_attr.yBegin = t+y
	self:SetCaptureMouse(true)
	self:SetCursorID("cur_hand_grab")
end

function View_OnLButtonUp(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if owner_attr.bMouseDown then
		owner_attr.bMouseDown = false
		owner:UpdateView(nil, true, true)
	end
	self:SetCaptureMouse(false)
	
	if owner_attr.ScaleRatio <= owner_attr.AdaptedScreenRatio then
		self:SetCursorID("IDC_ARROW")
	end
end

function View_OnMouseMove(self, x, y, flags)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if owner_attr.bMouseDown and owner_attr.DocObj then
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
		
		
		local oldViewSamePosPercentX = owner_attr.ViewSamePosPercentX
		local oldViewSamePosPercentY = owner_attr.ViewSamePosPercentY
		owner_attr.ViewSamePosPercentX = owner_attr.ViewSamePosPercentX + Xoffset/width
		owner_attr.ViewSamePosPercentY = owner_attr.ViewSamePosPercentY + Yoffset/height
		if owner:IsParamValid() then
			
			owner:UpdateView(nil, true)
		else
			owner_attr.ViewSamePosPercentX = oldViewSamePosPercentX
			owner_attr.ViewSamePosPercentY = oldViewSamePosPercentY
		end
	end
end

function View_GetViewType(self)
	return 1
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


function View_LeftRotate(self)
	local attr = self:GetAttribute()
	if attr.DocObj then
		attr.DocObj:LeftRotate()
		self:ResetInfo()
		self:ResetThumbnail()
		self:UpdateView(nil, true)
	end
end


function View_RightRotate(self)
	local attr = self:GetAttribute()
	if attr.DocObj then
		attr.DocObj:RightRotate()
		self:ResetInfo()
		self:ResetThumbnail()
		self:UpdateView(nil, true)
		
	end
end

function LeftWard_OnRButtonDown(self)
	
end

function RightWard_OnRButtonDown(self)
	
end

function LeftWard_OnLButtonDbClick(self)
	
end

function RightWard_OnLButtonDbClick(self)
	
end

function View_ShowScaleInfo(self)
	local attr = self:GetAttribute()
	
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	if attr.ShowScaleTimerID then	
		timerManager:KillTimer(attr.ShowScaleTimerID)
		attr.ShowScaleTimerID = nil
	end
	
	local showScaleObj = self:GetControlObject("client.scaleShow")
	local showTextObj = self:GetControlObject("client.scaleShow.text")
	local function OnTimer()
		SetObjVisible(showScaleObj, false)
		showTextObj:SetText("")
		timerManager:KillTimer(attr.ShowScaleTimerID)
		attr.ShowScaleTimerID = nil
	end
	attr.ShowScaleTimerID = timerManager:SetTimer(OnTimer, 700)
	SetObjVisible(showScaleObj, true)
	showTextObj:SetText(string.format("%d", attr.ScaleRatio*100) .. "%")
end

function View_OnDestroy(self)
	local attr = self:GetAttribute()
	if attr.ShowScaleTimerID then
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
		timerManager:KillTimer(attr.ShowScaleTimerID)
		attr.ShowScaleTimerID = nil
	end
end

function View_ResetThumbnail(self)
	local attr = self:GetAttribute()
	
	local clientObj = self:GetOwnerControl()
	local thumbnailView = clientObj:GetThumbnailView()
	local bitmap = attr.DocObj:GetAdaptedBitmap()
	local srcBitmapWidth, srcBitmapHeight = attr.DocObj:GetSrcBitmapSize()
	thumbnailView:SetImage(bitmap, srcBitmapWidth, srcBitmapHeight)
end

function View_HandleThumbnailViewChange(self, offsetX, offsetY)
	local attr = self:GetAttribute()
	offsetX = offsetX*attr.ScaleRatio
	offsetY = offsetY*attr.ScaleRatio
	
	local oldViewSamePosPercentX = attr.ViewSamePosPercentX
	local oldViewSamePosPercentY =  attr.ViewSamePosPercentY 
	
	
	local l,t,r,b = self:GetObjPos()
	local width, height = r-l, b-t
	attr.ViewSamePosPercentX = attr.ViewSamePosPercentX - offsetX/width
	attr.ViewSamePosPercentY = attr.ViewSamePosPercentY - offsetY/height
	if self:IsParamValid() then
		self:UpdateView(nil,true)
	else
		attr.ViewSamePosPercentX = oldViewSamePosPercentX
		attr.ViewSamePosPercentY = oldViewSamePosPercentY
	end
end

function View_HandleThumbnailViewMouseWheel(self, flags)
	local attr = self:GetAttribute()
	local oldViewSamePosPercentY =  attr.ViewSamePosPercentY 
	if flags > 0 then
		attr.ViewSamePosPercentY = attr.ViewSamePosPercentY + 0.2
	else
		attr.ViewSamePosPercentY = attr.ViewSamePosPercentY - 0.2
	end
	if self:IsParamValid() then
		self:UpdateView(nil, true)
	else
		attr.ViewSamePosPercentY = oldViewSamePosPercentY
	end
end

function View_HandleThumbnailViewLButtonDown(self, xPercent, yPercent)
	LOG("KuaikanLog xPercent:" .. xPercent .. "  yPercent:" .. yPercent)
	local attr = self:GetAttribute()
	attr.ViewSamePosPercentX = 0.5
	attr.ViewSamePosPercentY = 0.5
	attr.BitmapSamePosPercentX = xPercent
	attr.BitmapSamePosPercentY = yPercent
	self:UpdateView(nil, true)
end