-- state 0:normal 1:hover 2:down 3:disable
function SetState(self, newState, force, notani)
	local attr = self:GetAttribute()
	if attr == nil then return end
	if attr.NowState == -1 then
		return
	end
	
	if force or newState ~= attr.NowState then
		local board = self:GetControlObject("Board")
		function AddShotPic()
			local left, top, right, bottom = board:GetObjPos()
			local theRender = XLGetObject("Xunlei.UIEngine.RenderFactory")
			local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
			local xlgraphic = XLGetObject("Xunlei.XLGraphic.Factory.Object")
			local theBitmap = xlgraphic:CreateBitmap("ARGB32", right - left, bottom - top)
			theRender:RenderObject(board, theBitmap)
			
			local newImageObject = objFactory:CreateUIObject("", "ImageObject")
			newImageObject:SetBitmap(theBitmap)
			newImageObject:SetDrawMode(1)
			newImageObject:SetObjPos(left, top, right, bottom)
			self:AddChild(newImageObject)
			return newImageObject
		end
		local bkg = self:GetControlObject("Background")
		if newState == 0 then
			bkg:SetTextureID(attr.NormalBkgID)
		elseif newState == 1 then
			bkg:SetTextureID(attr.HoverBkgID)
		elseif newState == 2 then
			bkg:SetTextureID(attr.DownBkgID)
		elseif newState == 3 then
			bkg:SetTextureID(attr.DisableBkgID)
		end
		attr.NowState = newState
	end
end

function Updata(self)
	local attr = self:GetAttribute()
	SetState(self, attr.NowState, true, true)
end

function Enable(self, enable)
	if enable == nil then
		return
	end
	if enable == 0 then
		enable = false
	end
	
    local attr = self:GetAttribute()
	local text = self:GetControlObject("Recomment")
	attr.Enable = enable
	if not enable then
		self:SetCursorID("IDC_ARROW")
		self:SetState(3, false, true)
		self:SetCaptureMouse(false)
		attr.CaptrueMouse = false
		text:SetTextColorResID(attr.DisableTextColor)
	else
		self:SetCursorID("IDC_HAND")
		self:SetState(0, false, true)
		text:SetTextColorResID(attr.TextColor)
	end
end

function IsEnable(self, enable)
    local attr = self:GetAttribute()
	return attr.Enable
end

function SetText(self, text)
	if text == nil then
		return
	end
	local textObj = self:GetControlObject("Recomment")
	textObj:SetText(text)
end

function GetText(self, text)
	local textObj = self:GetControlObject("Recomment")
	textObj:SetText(text)
end

function SetTextColor(self, color)
    if color == nil then
        return
    end
    local textObj = self:GetControlObject("Recomment")
    local editObj = self:GetControlObject("button.edit")
    textObj:SetTextColorResID(color)
	editObj:SetTextColorID(color)
end

function SetTextFont(self, font)
    if font == nil then
        return
    end
    local textObj = self:GetControlObject("Recomment")
    local editObj = self:GetControlObject("button.edit")
    textObj:SetTextFontResID(font)
    editObj:SetFontID(font)
end

function Show(self, visible)
    local attr = self:GetAttribute()
    attr.Visible = visible
    self:SetVisible(visible)
    self:SetChildrenVisible(visible)
	self:Updata()
end

function IsVisible(self)
    local attr = self:GetAttribute()
    return attr.Visible
end

function OnLButtonDown(self, x, y)
	local attr = self:GetAttribute()
	if attr.Enable then
        self:SetState(2)
        self:SetCaptureMouse(true)
		attr.CaptrueMouse = true
	end
end

function OnLButtonUp(self, x, y)
	local attr = self:GetAttribute()
    local left, top, right, bottom = self:GetObjPos()
    local width, height = right - left, bottom - top
	if attr.Enable then
		self:SetState(0)
		self:SetCaptureMouse(false)
		if attr.CaptrueMouse and x >= 0 and x <= width and y >= 0 and y <= height then
			self:FireExtEvent("OnClick", x, y)
		end
		attr.CaptrueMouse = false
	end
end

function OnMouseMove(self, x, y)
    local left, top, right, bottom = self:GetObjPos()
    local width, height = right - left, bottom - top
    
    local attr = self:GetAttribute()
	if attr.Enable then
		if attr.NowState == 0 then
			self:SetState(1)
		elseif attr.NowState ~= 1 and attr.NowState ~= 2 then
			self:SetState(0)
			self:SetCaptureMouse(false)
			attr.CaptrueMouse = false
		end
	end
	self:FireExtEvent("OnButtonMouseMove", x, y)
end

function OnMouseLeave(self)
	local attr = self:GetAttribute()
	if attr.Enable then
		self:SetState(0)
	end
end

function OnMouseHover( self, x, y )

end

function OnFocusChange( self, focus )
	return true
end

function OnInitControl(self)
    local attr = self:GetAttribute()
	
	if attr.NormalBkgID == nil then
		attr.NormalBkgID = attr.Bkg4In1ID .. ".normal"
		attr.DownBkgID = attr.Bkg4In1ID .. ".down"
		attr.DisableBkgID = attr.Bkg4In1ID .. ".disable"
		attr.HoverBkgID = attr.Bkg4In1ID .. ".hover"
	end
	
	if attr.BoardResID == nil or attr.BoardResID == "" then
		local background = self:GetControlObject("Background")
		background:SetObjPos(0, 0, "father.width", "father.height")
	end
	if attr.ForegroundResID ~= nil and attr.ForegroundResID ~= "" then
		local left, top, right, bottom = self:GetObjPos()
		local width, height = right - left, bottom - top
		local foreground = self:GetControlObject("Foreground")
		foreground:SetTextureID(attr.ForegroundResID)
		foreground:SetObjPos(attr.ForegroundLeftPos, (height - attr.ForegroundHeight) / 2, attr.ForegroundLeftPos + attr.ForegroundWidth, (height + attr.ForegroundHeight) / 2)
	end
	
    self:SetText(attr.Text)
	self:SetTextColor(attr.TextColor)
	self:SetTextFont(attr.TextFont)
	
	SetState(self, 0, true, true)
	self:Enable(attr.Enable)
    self:Show(attr.Visible)
end

function OnEditFocusChange(edit, bFocus)
	if not bFocus and edit:GetVisible() then
		local sText = edit:GetText()
		local ctrl = edit:GetOwnerControl()
		local text = ctrl:GetControlObject("Recomment")
		if sText then
			text:SetText(sText)
			local cx , cy  = text:GetTextExtent()
			local l , t, w, h =text:GetObjPos()
			if(cx > w) then				
				text:SetTextFontResID("default.font")
				edit:SetFontID("default.font")
			end
			
		end
		edit:SetVisible(false)
		text:SetVisible(true)
	end
end

function OnRButtonDbClick(self)
	local edit = self:GetControlObject("button.edit")
	if not edit:GetVisible() then
		local text = self:GetControlObject("Recomment")
		local sText = text:GetText()
		if sText and sText ~= "" then
			edit:SetText(sText)
		end
		text:SetVisible(false)
		edit:SetVisible(true)
		AsynCall(function()
					edit:SetFocus(true) 
					edit:SetSelAll()
					self:SetTextColor("system.blue")
				end)
	end
end

function OnEditKeyDown(self, uChar)
	if uChar == 0x1B then --VK_ESCAPE
		if self:GetVisible() then
			self:SetVisible(false)
			local ctrl = self:GetOwnerControl()
			local text = ctrl:GetControlObject("Recomment")
			text:SetVisible(true)
		end
	elseif uChar == 0x0D then
		if self:GetVisible() then
			local ctrl = self:GetOwnerControl()
			ctrl:SetFocus(true)
		end
	end
end
