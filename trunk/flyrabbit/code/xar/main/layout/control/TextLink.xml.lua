function OnMouseHover(self, x, y)
	local ownerControl = self:GetOwnerControl()
	local attr = ownerControl:GetAttribute()
	
	ownerControl:FireExtEvent("OnTextLinkMouseHover", x, y)
end

function OnMouseMove( self )
	local ownerControl = self:GetOwnerControl()
	local attr = ownerControl:GetAttribute()
	attr.MouseIn = true
	ownerControl:FireExtEvent("OnTextLinkMouseMove")
	self:SetTextFontResID( attr.HoverFont )
	self:SetTextColorResID(attr.HoverTextColor)
end

function OnMouseLeave(self)
	local ownerControl = self:GetOwnerControl()
	local attr = ownerControl:GetAttribute()

	attr.MouseIn = false
	self:SetTextFontResID( attr.Font )
	self:SetTextColorResID(attr.TextColor)
	ownerControl:FireExtEvent("OnTextLinkMouseLeave")
end

function OnLButtonDown(self)
    self:SetCaptureMouse(true)
    return 0, true
end

function OnLButtonUp(self)
	local ownerControl = self:GetOwnerControl()
	local attr = ownerControl:GetAttribute()
	
    self:SetCaptureMouse(false)
	ownerControl:FireExtEvent("OnClick")
    -- return 0, true
end

function OnBind(self)
    local control = self:GetOwnerControl()
    local cattr = control:GetAttribute()
	if cattr.Text ~= nil then
		self:SetText(cattr.Text) 
	end
end

function GetText(self)
    local attr = self:GetAttribute()
    return attr.Text
end

function SetText(self, text)
    local attr = self:GetAttribute()
    attr.Text = text
    local t = self:GetControlObject("textlink.text")
    t:SetText(text)
end


function GetTextColorID(self)
    local attr = self:GetAttribute()
    return attr.TextColor
end

function SetTextColorID(self, textColorID, textHoverColorID)
    local attr = self:GetAttribute()
    attr.TextColor = textColorID
    local t = self:GetControlObject("textlink.text")
    t:SetTextColorResID(attr.TextColor)
	if textHoverColorID then
		attr.HoverTextColor = textHoverColorID
	end
end

function GetDisableTextColorID(self)
    local attr = self:GetAttribute()
    return attr.DisableTextColor
end

function SetDisableTextColorID(self, textColorID)
    local attr = self:GetAttribute()
    attr.DisableTextColor = textColorID
end

function GetVAlign(self)
    local attr = self:GetAttribute()
    return attr.VAlign
end

function SetVAlign(self, vAlign)
    local attr = self:GetAttribute()
	if attr.VAlign == vAlign then
		return
	end
	
    attr.VAlign = vAlign
	local t = self:GetControlObject("textlink.text")
	t:SetVAlign(attr.VAlign)
end

function GetHAlign(self)
    local attr = self:GetAttribute()
    return attr.VAlign
end

function SetHAlign(self, hAlign)
    local attr = self:GetAttribute()
	if attr.HAlign == hAlign then
		return
	end
	
    attr.HAlign = hAlign
	local t = self:GetControlObject("textlink.text")
	t:SetHAlign(attr.HAlign)
end

function OnInitControl( self )
    self:SetDefaultRedirect("control")
	local owner = self:GetOwnerControl()
    local attr = owner:GetAttribute()
	self:SetTextFontResID( attr.Font )
	self:SetVAlign(attr.VAlign)
	self:SetHAlign(attr.HAlign)
	if self:GetEnable() then
		self:SetCursorID("IDC_HAND")
		self:SetTextColorResID(attr.TextColor)
	else
		self:SetCursorID("IDC_ARROW")
		self:SetTextColorResID(attr.DisableTextColor)
	end
	
	if attr.NoHand then
		self:SetCursorID("IDC_ARROW")
	end
	
	attr.MouseIn = false
	self:SetMultiline(attr.Multiline)
end

function SetLinkTextEffectColorResID(self,colorRes)
	local textObj = self:GetControlObject("textlink.text")
	textObj:SetEffectType("bright")
	textObj:SetEffectColorResID(colorRes)
end

function GetTextExtent(self)
	local t = self:GetControlObject("textlink.text")
	return t:GetTextExtent()
end

function SetMultilineTextLimitWidth(self,nWidth)
	if nWidth == nil or nWidth == "" then
		return
	end
	local textObj = self:GetControlObject("textlink.text")
	textObj:SetMultilineTextLimitWidth(nWidth)
end

function SetLineGap( self, nGap )
	local textObj = self:GetControlObject("textlink.text")
	textObj:SetLineGap(nGap)
end

function SetMultiline(self,bMulti)
	local textObj = self:GetControlObject("textlink.text")
	textObj:SetMultiline(bMulti)
end

function GetEndEllipsis(self)
	local textObj = self:GetControlObject("textlink.text")
	textObj:GetEndEllipsis()
end

function SetEndEllipsis(self, bEndEllipsis)
	local textObj = self:GetControlObject("textlink.text")
	textObj:SetEndEllipsis(bEndEllipsis)
end

function SetFont( self, font, hoverfont )
	local attr = self:GetAttribute()
	if font ~= nil then
		attr.Font = font
	end
	if hoverfont ~= nil then
		attr.HoverFont = hoverfont
	end
	local textObj = self:GetControlObject("textlink.text")
	if self:GetEnable() and attr.MouseIn then
		textObj:SetTextFontResID( attr.HoverFont )
	else
		textObj:SetTextFontResID( attr.Font )
	end
end

function OnEnableChange(self, enable)
    local attr = self:GetAttribute()
	local t = self:GetControlObject("textlink.text")
	if enable then
		t:SetCursorID("IDC_HAND")
		t:SetTextColorResID(attr.TextColor)
	else
		t:SetCursorID("IDC_ARROW")
		t:SetTextColorResID(attr.DisableTextColor)
	end
end

function OnVisibleChange(self, visible)
	-- self:SetVisible(visible)
	self:SetChildrenVisible(visible)
end

function control_OnInitControl(self)
    self:SetDefaultRedirect("control")
	OnEnableChange(self, self:GetEnable())
end