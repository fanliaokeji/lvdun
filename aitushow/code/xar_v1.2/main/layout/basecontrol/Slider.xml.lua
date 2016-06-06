--NowState 0:Normal, 1:Hover, 2:Down, 3:Disable

function SetState(self, newState, fource)
	--print(newState)
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	if attr.BtnState == -1 then
		return
	end
	
	if fource or newState ~= attr.BtnState then
		local ownerTree = self:GetOwner()
		local oldBkg = ctrl:GetControlObject("button.oldbkg")
		local bkg = ctrl:GetControlObject("button.bkg")
		
		local oldBkgID = oldBkg:GetTextureID()
		oldBkg:SetTextureID(bkg:GetTextureID())
		oldBkg:SetAlpha(255)
		if newState == 0 then
			bkg:SetTextureID(attr.BtnNormalBkgID and attr.BtnNormalBkgID or "")
		elseif newState == 1 then
			bkg:SetTextureID(attr.BtnHoverBkgID and attr.BtnHoverBkgID or "")
		elseif newState == 2 then
			bkg:SetTextureID(attr.BtnDownBkgID and attr.BtnDownBkgID or "")
		elseif newState == 3 then
			bkg:SetTextureID(attr.BtnDisableBkgID and attr.BtnDisableBkgID or "")
		end

		if oldBkgID and oldBkgID ~= "" and self:GetOwner() then
			-- __Animation.RunAlphaAni_Control(oldBkg, 255, 0)
			-- __Animation.RunAlphaAni_Control(bkg, 0, 255)
		end
		
		attr.BtnState = newState
	end
end

function UpdateBkg(ctrl, bAni)
    local attr = ctrl:GetAttribute()
	local left, top, right, bottom = ctrl:GetObjPos()
	local width, height = right - left, bottom - top
	
	--可能还要分横竖两种情况
	local nLeftWidth
	if attr.RightBkgID ~= "" then 
		nLeftWidth = width * (attr.Value - attr.Min) / (attr.Max - attr.Min)
	else
		nLeftWidth = width
	end
	local nBufferWidth = width * attr.BufferValue / (attr.Max - attr.Min)
	local father = ctrl:GetControlObject("SliderLayout")
	local father_left, father_top, father_right, father_bottom = father:GetObjPos()
	--XLMessageBox(height)
	local ownerTree = ctrl:GetOwner()
	local leftPart = ctrl:GetControlObject("LeftPart")
	if not bAni then
		leftPart:SetObjPos(0, 0-father_top, nLeftWidth, height-father_top)
	else
		local lpLeft, lpTop, lpRight, lpBottom = leftPart:GetObjPos()
		-- __Animation.RunPosAni_Control(leftPart, lpLeft, lpTop, lpRight, lpBottom, 0, 0-father_top, nLeftWidth, height-father_top, nil, 200)
	end
	
	local rightPart = ctrl:GetControlObject("RightPart")
	if not bAni then
		rightPart:SetObjPos(nLeftWidth, 0-father_top, width, height-father_top)
	else
		local rpLeft, rpTop, rpRight, rpBottom = rightPart:GetObjPos()
		-- __Animation.RunPosAni_Control(rightPart, rpLeft, rpTop, rpRight, rpBottom, nLeftWidth, 0-father_top, width, height-father_top, nil, 200)
	end
	
	local bufferPart = ctrl:GetControlObject("BufferPart")
	bufferPart:SetObjPos(nLeftWidth, 0-father_top, nBufferWidth, height-father_top)
	
	local leftBkn = ctrl:GetControlObject("LeftBkn")
	leftBkn:SetObjPos(0, 0, width, height)
	
	local rightBkn = ctrl:GetControlObject("RightBkn")
	rightBkn:SetObjPos(0, 0, width, height)
	
	local bufferBkn = ctrl:GetControlObject("BufferBkn")
	bufferBkn:SetObjPos(0, 0, width, height)
end

function UpdateBtnPos(ctrl)
    local attr = ctrl:GetAttribute()
    local btn = ctrl:GetControlObject("SliderButton")
    local left, top, right, bottom = ctrl:GetObjPos()
	local father = ctrl:GetControlObject("SliderLayout")
	local father_left, father_top, father_right, father_bottom = father:GetObjPos()
	
    local width, height = right - left, bottom - top
	local left, top, right, bottom = btn:GetObjPos()
    if attr.Type == 0 then
        local centerPos = width * (attr.Value - attr.Min) / (attr.Max - attr.Min)
		--XLMessageBox(attr.Value)
        --btn:SetObjPos(centerPos - (attr.BtnWidth / 2), 0 - (attr.BtnHeight - height) / 2, centerPos + (attr.BtnWidth / 2), (attr.BtnHeight + height) / 2)
		if attr.Grid then
			local newValue = math.floor(centerPos * (attr.Max - attr.Min) / (width * attr.Step)) * attr.Step + attr.Min
			centerPos = math.floor((newValue - attr.Min) * width / (attr.Max - attr.Min) + 0.5)
			--btn:SetObjPos(centerPos - (attr.BtnWidth / 2), math.floor((height - attr.BtnHeight) / 2 - father_top), centerPos + (attr.BtnWidth / 2), math.floor((attr.BtnHeight + height) / 2 - father_top))
			local newLeft = centerPos - (attr.BtnWidth / 2)
			local newTop = math.floor((height - attr.BtnHeight) / 2 - father_top)
			local newRight = centerPos + (attr.BtnWidth / 2)
			local newBottom = math.floor((attr.BtnHeight + height) / 2 - father_top)
			
			-- __Animation.RunPosAni_Control(btn, left, top, right, bottom, newLeft, newTop, newRight, newBottom)
		else
			--btn:SetObjPos(centerPos - (attr.BtnWidth / 2), math.ceil((height - attr.BtnHeight) / 2 - father_top), centerPos + (attr.BtnWidth / 2), math.floor((attr.BtnHeight + height) / 2 - father_top))
			btn:SetObjPos2(centerPos - (attr.BtnWidth / 2), math.ceil((height - attr.BtnHeight) / 2 - father_top), attr.BtnWidth, attr.BtnHeight)
		end
    else
        local centerPos = (height - attr.BtnHeight) * (attr.Value - attr.Min) / (attr.Max - attr.Min)
        --btn:SetObjPos(0 - (attr.BtnWidth - width) / 2, centerPos - (attr.BtnHeight / 2), (attr.BtnWidth + width) / 2, centerPos + (attr.BtnHeight / 2))
		--btn:SetObjPos(math.floor((width - attr.BtnWidth) / 2), centerPos, math.floor((attr.BtnWidth + width) / 2), centerPos + attr.BtnHeight)
		btn:SetObjPos2(math.floor((width - attr.BtnWidth) / 2), centerPos,attr.BtnWidth, attr.BtnHeight)
    end
	UpdateBkg(ctrl)
end

function UpdatePos(ctrl)
    local attr = ctrl:GetAttribute()
    if attr.Value < attr.Min then
        attr.Value = attr.Min
    end
    if attr.Value > attr.Max then
        attr.Value = attr.Max
    end
    UpdateBtnPos(ctrl)
    --ctrl:FireExtEvent("OnSliderPosChange", attr.Value)
end

function PosToValue(self, pos)
    local attr = self:GetAttribute()
    local left, top, right, bottom = self:GetObjPos()
    local width, height = right - left, bottom - top
	
    if attr.Type == 0 then
        if pos < 0 then
            pos = 0
        elseif pos > width then
            pos = width
        end
		return pos * (attr.Max - attr.Min) / width + attr.Min, pos
    else
        if pos < 0 then
            pos = 0
        elseif pos > height then
            pos = height
        end
		return pos * (attr.Max - attr.Min) / height + attr.Min, pos
    end
end

function SetBtnPos(ctrl, newPos, bAni)
    local attr = ctrl:GetAttribute()
    local left, top, right, bottom = ctrl:GetObjPos()
    local width, height = right - left, bottom - top
	
	local newValue = 0
	newValue, newPos = ctrl:PosToValue(newPos)

    local btn = ctrl:GetControlObject("SliderButton")
	local left, top, right, bottom = btn:GetObjPos()
    if attr.Type == 0 then
        --btn:SetObjPos(newPos - (attr.BtnWidth / 2), 0 - (attr.BtnHeight - height) / 2, newPos + (attr.BtnWidth / 2), (attr.BtnHeight + height) / 2)
		if attr.Grid then
			if newValue == attr.Value then
				return
			end
			newValue = math.floor(newPos * (attr.Max - attr.Min) / (width * attr.Step)) * attr.Step + attr.Min
			newPos = math.floor((newValue - attr.Min) * width / (attr.Max - attr.Min) + 0.5)
			--btn:SetObjPos(centerPos - (attr.BtnWidth / 2), math.floor((height - attr.BtnHeight) / 2 - father_top), centerPos + (attr.BtnWidth / 2), math.floor((attr.BtnHeight + height) / 2 - father_top))
			local newLeft, newTop, newRight, newBottom = newPos - (attr.BtnWidth / 2), top, newPos + attr.BtnWidth / 2, bottom
			
			--XMP.LOG("btn left:", newLeft, ", top:", newTop, ", right:", newRight, ", bottom:", newBottom)
			-- __Animation.RunPosAni_Control(btn, left, top, right, bottom, newLeft, newTop, newRight, newBottom)
		else
			if not bAni then
				--btn:SetObjPos(newPos - (attr.BtnWidth / 2), top, newPos + attr.BtnWidth / 2, bottom)
				btn:SetObjPos2(newPos - (attr.BtnWidth / 2), top,attr.BtnWidth, attr.BtnHeight)
			else
				local btnLeft, btnTop, btnRight, btnBottom = btn:GetObjPos()
				-- __Animation.RunPosAni_Control(btn, btnLeft, btnTop, btnRight, btnBottom, newPos - (attr.BtnWidth / 2), top, newPos + attr.BtnWidth / 2, bottom, nil, 200)
			end
		end
    else
        --btn:SetObjPos(0 - (attr.BtnWidth - width) / 2, newPos - (attr.BtnHeight / 2), (attr.BtnWidth + width) / 2, newPos + (attr.BtnHeight / 2))
		if attr.Grid then
			btn:SetObjPos(left, newPos - (attr.BtnHeight / 2), right, newPos + attr.BtnHeight / 2)
		else
			btn:SetObjPos(left, newPos - (attr.BtnHeight / 2), right, newPos + attr.BtnHeight / 2)
		end
    end
	local bFireEvent = false
	if newValue ~= attr.Value then
		attr.Value = newValue
		bFireEvent = true
	end
	UpdateBkg(ctrl, bAni)
	if bFireEvent then
		ctrl:FireExtEvent("OnSliderPosChange", attr.Value)
	end
end

function SetValue(self, newValue, update)
    local attr = self:GetAttribute()
	if not attr then
        return
    end
	
    if attr.Value == newValue and update then
        return
    end
    
    if newValue < attr.Min then
        newValue = attr.Min
    elseif newValue > attr.Max then
        newValue = attr.Max
    end
    
    attr.Value = newValue
    UpdatePos(self)
	--self:FireExtEvent("OnSliderPosChange", attr.Value)
end

function GetValue(self)
    local attr = self:GetAttribute()
    return attr.Value
end

function SetBufferValue(self, newValue)
    local attr = self:GetAttribute()
	if not attr then
        return
    end
	
    if attr.BufferValue == newValue then
        return
    end
    
    if newValue < attr.Min then
        newValue = attr.Min
    elseif newValue > attr.Max then
        newValue = attr.Max
    end
    attr.BufferValue = newValue
	
	local left, top, right, bottom = self:GetObjPos()
	local width, height = right - left, bottom - top
	
	local nLeftWidth = (width - attr.BtnWidth) * attr.Value / (attr.Max - attr.Min);
	local nBufferWidth = (width - attr.BtnWidth) * attr.BufferValue / (attr.Max - attr.Min);
	
	local bufferPart = self:GetControlObject("BufferPart")
	bufferPart:SetObjPos(nLeftWidth, 0, nBufferWidth, height)
	
	local bufferBkn = self:GetControlObject("BufferBkn")
	bufferBkn:SetObjPos(-nLeftWidth, 0, width - nLeftWidth, height)
end

function GetBufferValue(self)
    local attr = self:GetAttribute()
    return attr.BufferValue
end

function SetRange(self, min, max)
    local attr = self:GetAttribute()
    if min < 0 or max < 0 then
        return false
    end
    if min > max then
        return false
    end
    attr.Min = min
    attr.Max = max
    UpdatePos(self)
	return true
end

function GetRange(self)
    local attr = self:GetAttribute()
    return attr.Min, attr.Max
end

function Show(self, visible)
    local attr = self:GetAttribute()
    attr.Visible = visible
	
	self:SetVisible(attr.Visible)
	self:SetChildrenVisible(attr.Visible)
	if not attr.Visible and attr.TrackMouse then
		attr.TrackMouse = false
		self:SetCaptureMouse(false)
	end
end

function IsVisible(self)
    local attr = self:GetAttribute()
    return attr.Visible
end

function OnPosChange(self)
    UpdateBtnPos(self)
end

function Enable(self, bEnable)
    local attr = self:GetAttribute()
	attr.Enable = bEnable
	local SliderLayout = self:GetControlObject("SliderLayout")
	local left = self:GetControlObject("LeftBkn")
	local right = self:GetControlObject("RightBkn")
	local buffer = self:GetControlObject("BufferBkn")
    local button = self:GetControlObject("SliderButton")
	if attr.Enable then
		SliderLayout:SetCursorID("IDC_HAND")
		button:SetCursorID("IDC_HAND")
		if attr.LeftBkgID then left:SetTextureID(attr.LeftBkgID) end
		if attr.RightBkgID then right:SetTextureID(attr.RightBkgID) end
		if attr.BufferBkgID then buffer:SetTextureID(attr.BufferBkgID) end
		SetState(button, 0, true)
	else
		SliderLayout:SetCursorID("IDC_ARROR")
		button:SetCursorID("IDC_ARROR")
		if attr.DisableLeftBkgID then
			left:SetTextureID(attr.DisableLeftBkgID)
		end
		if attr.DisableRightBkgID then
			right:SetTextureID(attr.DisableRightBkgID)
		end
		if attr.DisableBufferBkgID then
			buffer:SetTextureID(attr.DisableBufferBkgID)
		end
		SetState(button, 3, true)
	end
end

function IsEnable(self)
    local attr = self:GetAttribute()
	return attr.Enable
end

function SetUserData(self,UserData)
	local attr = self:GetAttribute()
	attr.UserData = UserData
end
function GetUserData(self)
	return self:GetAttribute().UserData
end
--[[
-- 这两个可以放在外部
function GetDataFromValue(self,value)
	local attr = self:GetAttribute()
	if attr.data == nil then return value end
	if value < 1 then value = 1 end
	if value > #attr.data then value = #attr.data end
	return attr.data[value]
end

function GetValueFromData(self,data)
	local attr = self:GetAttribute()
	if attr.data == nil then return data end
	for v,k in ipairs(attr.data) do
		if data == k then
			return v
		end
	end
end
--]]
function SetStep(self, step)
	local attr = self:GetAttribute()
	attr.Step = step
end

function GetStep(self, step)
	local attr = self:GetAttribute()
	return attr.Step
end

function OnBtnLButtonDown(self, x, y)
    local ctrl = self:GetOwnerControl()
    local attr = ctrl:GetAttribute()
    if not attr.Enable then
        return 0, true
    end
	
	SetState(self, 2)
    local left, top, right, bottom = self:GetObjPos()
	if attr.Type == 0 then
		SetBtnPos(ctrl, x + left)
	else
		SetBtnPos(ctrl, y + top)
	end
	--self:FireExtEvent("OnSliderPosChange", attr.Value)
	
    attr.TrackMouse = true
    self:SetCaptureMouse(true)
	return 0, true
end

function OnBtnLButtonUp(self, x, y, flags)
    local ctrl = self:GetOwnerControl()
    local attr = ctrl:GetAttribute()
	if attr.Enable then
		if attr.BtnState == 2 then
			SetState(self, 0)
		end
		self:SetCaptureMouse(false)
		attr.TrackMouse = false
	end
    return 0, true
end

function OnBtnMouseMove(self, x, y, flag)
    local ctrl = self:GetOwnerControl()
    local attr = ctrl:GetAttribute()
	--print(x, y)
	
	local left, top, right, bottom = self:GetObjPos()
	ctrl:FireExtEvent("OnSliderMouseMove", left + x, top + y)
	
    if attr.Enable then
        if attr.BtnState == 0 and not attr.TrackMouse then
			SetState(self, 1)
			return 0, true
        end
		if not attr.TrackMouse then
			return 0, true
		end
		if BitAnd(flag, 1) == 0 then
			return 0, true
		end
		if attr.BtnAni then
			return 0, true
		end
		local centerWidth, centerHeight = (left + right) / 2, (top + bottom) / 2

		local offset = 0
		if attr.Type == 0 then
			x = left + x
			offset = x - centerWidth
		else
			y = top + y
			offset = y - centerHeight
		end
		
		if offset == 0 then
			return 0, true
		end
		
		local btn = ctrl:GetControlObject("SliderButton")

		local centerWidth, centerHeight = (left + right) / 2, (top + bottom) / 2
		--SetState(btn, 0)
		if attr.Type == 0 then
			SetBtnPos(ctrl, centerWidth + offset)
		else
			SetBtnPos(ctrl, centerHeight + offset)
		end
		--self:FireExtEvent("OnSliderPosChange", attr.Value)
	end
    return 0, true
end

function OnBtnMouseLeave(self)
    local ctrl = self:GetOwnerControl()
    local attr = ctrl:GetAttribute()
    if attr.Enable and not attr.TrackMouse then
		SetState(self, 0)
	end
	ctrl:FireExtEvent("OnSliderMouseLeave")
end

function Slider_OnInitControl(self)
	local attr = self:GetAttribute()
	
	local ctrl_left, ctrl_top, ctrl_right, ctrl_bottom = self:GetObjPos()
	local layout = self:GetControlObject("SliderLayout")
	if attr.SliderHeight ~= nil then
		local bottom_height = (attr.BtnHeight - ctrl_bottom + ctrl_top) / 2
		local bkg_top = ctrl_bottom - ctrl_top + bottom_height - attr.SliderHeight
		layout:SetObjPos(0, bkg_top, ctrl_right - ctrl_left, ctrl_bottom - ctrl_top + bottom_height)
	else
		layout:SetObjPos(0, -2, ctrl_right - ctrl_left, ctrl_bottom - ctrl_top +2 )
	end
	if attr.BtnBkg4In1ID then
		attr.BtnNormalBkgID =  attr.BtnBkg4In1ID .. ".normal"
		attr.BtnDownBkgID =  attr.BtnBkg4In1ID .. ".down"
		attr.BtnHoverBkgID =  attr.BtnBkg4In1ID .. ".hover"
		attr.BtnDisableBkgID =  attr.BtnBkg4In1ID .. ".disable"
	end
	local left = self:GetControlObject("LeftBkn")
	local right = self:GetControlObject("RightBkn")
	local buffer = self:GetControlObject("BufferBkn")
    local button = self:GetControlObject("SliderButton")
	if attr.Enable then
		if attr.LeftBkgID then left:SetTextureID(attr.LeftBkgID) end
		if attr.RightBkgID then right:SetTextureID(attr.RightBkgID) end
		if attr.BufferBkgID then buffer:SetTextureID(attr.BufferBkgID) end
		SetState(button, 0, true)
	else
		if attr.DisableLeftBkgID then
			left:SetTextureID(attr.DisableLeftBkgID)
		end
		if attr.DisableRightBkgID then
			right:SetTextureID(attr.DisableRightBkgID)
		end
		if attr.DisableBufferBkgID then
			buffer:SetTextureID(attr.DisableBufferBkgID)
		end
		SetState(button, 3, true)
	end
	self:SetValue(attr.Value)
    self:Show(attr.Visible)
end

function Slider_OnLButtonDown(self, x, y, flag)
	self = self:GetOwnerControl()
	local owner = self:GetOwner()
    local attr = self:GetAttribute()
    if not attr.Enable then
        return
    end
    local left, top, right, bottom = self:GetObjPos()
    local width, height = right - left, bottom - top
    if attr.Type == 0 then
        if x < 0 then
            x = 0
        elseif x > width then
            x = width
        end
    else
        if y < 0 then
            y = 0
        elseif y > height then
            y = height
        end
    end
	
    local btn = self:GetControlObject("SliderButton")
	local btnLeft, btnTop, btnRight, btnBottom = btn:GetObjPos()
	SetState(btn, 2)
	btn:SetCaptureMouse(true)
	attr.TrackMouse = true
	if attr.Type == 0 then
		SetBtnPos(self, x, true)
	else
		SetBtnPos(self, y, true)
	end
	
	self:FireExtEvent("OnSliderLButtonDown", true, x, y, flags)
end

function Slider_OnMouseMove(self, x, y)
	self = self:GetOwnerControl()
	local attr = self:GetAttribute()
	local btn = self:GetControlObject("SliderButton")
	self:FireExtEvent("OnSliderMouseMove", x, y)
	
	if attr.TrackMouse then
		SetBtnPos(self, x)
		return
	end
	if attr.Enable and attr.BtnState ~= 1 then
		SetState(btn, 1)
	end
end

function Slider_OnMouseLeave(self, x, y)
	self = self:GetOwnerControl()
	local attr = self:GetAttribute()
	if attr.Enable then
		local btn = self:GetControlObject("SliderButton")
		SetState(btn, 0)
	end
	self:FireExtEvent("OnSliderMouseLeave")
end
