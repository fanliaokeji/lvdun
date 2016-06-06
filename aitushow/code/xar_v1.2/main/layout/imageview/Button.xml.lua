local function RemoveDefaultAnimation(self)
    local attr = self:GetAttribute()
    if attr.defaultani then
        attr.defaultani:Stop()
    end
    local mask = self:GetControlObject("masktexture")
    if mask then
        self:RemoveChild(mask)
    end
end

local function SetDefaultAnimation(self)
    local attr = self:GetAttribute()
    local bkg = self:GetControlObject("button.bkg")
    if attr.IsDefaultButton then
        bkg:SetTextureID(attr.DefaultBkgNormal)
        local objectTree = self:GetOwner()
        if objectTree then
            local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")
	        local ani = aniFactory:CreateAnimation("Kuaikan.Animation.Button.DefaultChange")
	        ani:BindObj(self)
	        ani:SetTotalTime(9999999999)
	        objectTree:AddAnimation(ani)
	        attr.defaultani = ani
	        ani:Resume()
        end
    else
        if attr.defaultani then
            RemoveDefaultAnimation(self)
        end
	end
end

function ButtonChange_BindObj(self, button)
    local attr = self:GetAttribute()
    local battr = button:GetAttribute()
    local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local maskimage = button:GetControlObject("masktexture")
	if not maskimage then
		maskimage = objFactory:CreateUIObject("masktexture", "TextureObject")
		button:AddChild(maskimage)

	end
	maskimage:SetTextureID(battr.DefaultAnimTexture)

	maskimage:SetObjPos(0,0,"father.width", "father.height")
	attr.maskimage = maskimage
	attr.FrameTime = battr.FrameTime
end

function ButtonChange_Action(self)
    local attr = self:GetAttribute()
	local curve = self:GetCurve()
	local totalTime = self:GetTotalTime()
	local runningTime = self:GetRuningTime()
	local truetime = runningTime % attr.FrameTime
	local SingleFrameTime = attr.FrameTime / 2
	local progtime = truetime % SingleFrameTime
	local progress =  curve:GetProgress(progtime / SingleFrameTime)
	if truetime >= 0 and truetime < SingleFrameTime then
	    attr.maskimage:SetAlpha(255*progress)
	else
	    attr.maskimage:SetAlpha(255*(1-progress))
	end
end

function SetState(self, newState, forceUpdate, noAni)
    local attr = self:GetAttribute()
    if forceUpdate or newState ~= attr.NowState then
        local ownerTree = self:GetOwner()
        local oldBkg = self:GetControlObject("button.oldbkg")
		if not oldBkg then return end
        local bkg = self:GetControlObject("button.bkg")
		local old_texture_id = bkg:GetTextureID()
        if newState == 0 then
            if attr.IsDefaultButton then
                bkg:SetTextureID(attr.DefaultBkgNormal)
                SetDefaultAnimation(self)
            else
                bkg:SetTextureID(attr.NormalBkgID)
            end
        elseif newState == 1 then
            if attr.IsDefaultButton then
                RemoveDefaultAnimation(self)
            end
            bkg:SetTextureID(attr.DownBkgID)
        elseif newState == 2 then
                bkg:SetTextureID(attr.DisableBkgID)
        elseif newState == 3 then
            if attr.IsDefaultButton then
                RemoveDefaultAnimation(self)
            end
            bkg:SetTextureID(attr.HoverBkgID)
        end

		if noAni == nil then
            oldBkg:SetTextureID(old_texture_id)
		    oldBkg:SetAlpha(255)
			local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")	
			local aniAlpha = aniFactory:CreateAnimation("AlphaChangeAnimation")
			aniAlpha:BindRenderObj(oldBkg)
			aniAlpha:SetTotalTime(200)
			aniAlpha:SetKeyFrameAlpha(255,0)
			ownerTree:AddAnimation(aniAlpha)
			aniAlpha:Resume()
		end
		attr.NowState = newState
    end
end

function RemoveTip(self)
	local attr = self:GetAttribute()
	local tipObj = attr.TipObj
	if tipObj ~= nil then
		self:RemoveChild(tipObj)
		attr.TipObj = nil
	end
end

function AddTip(self, x, y)
	local attr = self:GetAttribute()
	if attr.TipText == nil or attr.TipText == "" then
		return
	end

	local tipObj = attr.TipObj
	if tipObj ~= nil then
		return
	end

	local xarManager = XLGetObject("Xunlei.UIEngine.XARManager")
	local xarFactory = xarManager:GetXARFactory()
	--local tipObj = xarFactory:CreateUIObject("tip","Kuaikan.NewTask.Tip")
	if not tipObj then
		return
	end
	
	local tipAttr = tipObj:GetAttribute()
	tipAttr.BkgTexture = "texture.tip.normal.bkg"
	tipAttr.TextBeginH = 0
	tipAttr.TextBeginV = 4

	tipObj:SetText(attr.TipText)
	tipObj:SetZorder(100000000)
	self:AddChild(tipObj)
	attr.TipObj = tipObj

	local width, height = tipObj:GetSize(40, 20)
	local tree = self:GetOwner()
	local hwnd = tree:GetBindHostWnd()
	local wnd_left, wnd_top, wnd_right, wnd_bottom = hwnd:GetWindowRect()
	local abs_left, abs_top, abs_right, abs_bottom = self:GetAbsPos()
	
	local left, top = x, y + 20
	if attr.TipPosIsTopMid then
		left = (abs_right - abs_left - width)/2
		top = 0 - height
	end
	local right, bottom = left + width, top + height
	if abs_left + right > wnd_right - wnd_left then
		left = left - ( abs_left + right - ( wnd_right - wnd_left ) ) - 2
		right = left + width
	end
	if abs_top + bottom > wnd_bottom - wnd_top then
		top = top - ( abs_top + bottom - (wnd_bottom - wnd_top) ) - 2
		bottom = top + 20
	end
	tipObj:SetObjPos(left, top, right, bottom)
end


function SetBitmap( self, nor, down, hover, disable )
	local attr = self:GetAttribute()
	attr.NormalBkgID = ""
	if nor ~= nil then
		attr.NormalBkgID = nor
	end
	attr.DownBkgID = ""
	if down ~= nil then
		attr.DownBkgID = down
	end
	attr.DisableBkgID = ""
	if disable ~= nil then
		attr.DisableBkgID = disable
	end
	attr.HoverBkgID = ""
	if hover ~= nil then
		attr.HoverBkgID = hover
	end
	self:SetState(attr.NowState, true, true)
end

function SetText(self, text)
    if text == nil then
        return
    end
    local textObj = self:GetControlObject("button.text")
    textObj:SetText(text)
    local attr = self:GetAttribute()
    attr.Text = text
end

function GetText(self)
    local attr = self:GetAttribute()
	return attr.Text
end

function SetEnable(self, enable)
    local attr = self:GetAttribute()
    attr.Enable = enable
    local bkg = self:GetControlObject("button.bkg")
	local text = self:GetControlObject("button.text")
    if enable then
        if attr.IsDefaultButton then
            bkg:SetTextureID(attr.DefaultBkgNormal)
        else
		    bkg:SetTextureID(attr.NormalBkgID)
		end
		attr.NowState = 0
		text:SetTextColorResID(attr.TextColor)
    else
		local focusrect = self:GetControlObject("focusrectangle")
		focusrect:SetVisible(false)
		bkg:SetTextureID(attr.DisableBkgID)
		attr.NowState = 2
		text:SetTextColorResID(attr.DisableTextColor)
    end
end

function GetEnable(self)
    local attr = self:GetAttribute()
    return attr.Enable
end

function OnLButtonDown(self, x, y)
	RemoveTip( self )
    local attr = self:GetAttribute()
    local left, top, right, bottom = self:GetObjPos()
    local width, height = right - left, bottom - top
    if not attr.UseValidPos then
        attr.ValidWidth = width
        attr.ValidHeight = height
    end
    if attr.Enable then
        if ((x >= attr.ValidLeft) and (x <= attr.ValidLeft + attr.ValidWidth) and (y >= attr.ValidTop) and (y <= attr.ValidTop + attr.ValidHeight)) then
            self:SetState(1)
            self:SetCaptureMouse(true)
            attr.BtnDown = true
        end
    end
	self:FireExtEvent("OnButtonMouseDown")
    return 0
end

function OnLButtonUp(self)
    local attr = self:GetAttribute()
	if not attr then return end
    if attr.Enable then
        if attr.NowState==1 then
			self:SetState(0)
            self:FireExtEvent("OnClick")
        end
        self:SetCaptureMouse(false)
        attr.BtnDown = false
    end
    return 0
end

function OnKeyDown(self, char)
	if char == 32 or char == 13 then
        local attr = self:GetAttribute()
        if attr.Enable and attr.EnterEnable then
			self:FireExtEvent("OnClick")
		end
    end
	return 0
end

function OnKeyUp(self, char)
    if char == 9 then
        self:RouteToFather()
    end
    return 0
end

function OnMouseMove(self, x, y)
    local left, top, right, bottom = self:GetObjPos()
    local width, height = right - left, bottom - top
    
    local attr = self:GetAttribute()
    if not attr.UseValidPos then
        attr.ValidWidth = width
        attr.ValidHeight = height
    end
    if attr.Enable then
        if ((x >= attr.ValidLeft) and (x <= attr.ValidLeft + attr.ValidWidth) and (y >= attr.ValidTop) and (y <= attr.ValidTop + attr.ValidHeight)) then
            if attr.NowState==0 then
                if attr.BtnDown then
                    self:SetState(1)
                else
                    self:SetState(3)
                end
            end
        else
            if attr.ChangeStateWhenLeave then
                self:SetState(0)
            end
        end
        self:FireExtEvent("OnButtonMouseMove", x, y)
    end
    return 0
end

function OnControlMouseLeave(self, x, y)
    local attr = self:GetAttribute()
	if not attr then return end
    if attr.Enable then
        self:SetState(0)
    else
        self:SetState(2)
    end
	RemoveTip(self)
    self:FireExtEvent("OnButtonMouseLeave", x, y)
    return 0
end

function OnControlMouseEnter( self, x, y )
	if self:GetEnable() then
		AddTip(self, x, y)
	end
end

function OnFocusChange( self, focus )
	if not focus then
		RemoveTip(self)
	end
	local attr = self:GetAttribute()
	local focusrect = self:GetControlObject("focusrectangle")
	if focusrect then
		if not attr.ShowFocusRect or not attr.Enable then
			focusrect:SetVisible(false)
		else
			if attr.FocusRectWidth ~= 0 and attr.FocusRectHeight ~= 0 then
				focusrect:SetObjPos(attr.FocusRectLeft, attr.FocusRectTop, attr.FocusRectLeft+attr.FocusRectWidth, attr.FocusRectTop+attr.FocusRectHeight)
			end
			focusrect:SetVisible(focus)
		end
	end
	
end

function OnInitControl(self)
    local attr = self:GetAttribute()
    
    self:SetText(attr.Text)
	self:SetTextColor(attr.TextColor)
	self:SetTextFont(attr.TextFont)
    attr.NowState=0
    local bkg = self:GetControlObject("button.bkg")
    bkg:SetTextureID(attr.NormalBkgID)
    SetDefaultAnimation(self)
	
    self:SetEnable(attr.Enable)
    
    local left, top, right, bottom = self:GetObjPos()
    
    if not attr.ValidLeft then
        attr.ValidLeft = 0
    end
    if not attr.ValidTop then
        attr.ValidTop = 0
    end
    if not attr.ValidWidth then
        attr.ValidWidth = right - left
    end
    if not attr.ValidHeight then
        attr.ValidHeight = bottom - top
    end
    self:Show(attr.Visible)
	
    local oldbkg = self:GetControlObject("button.oldbkg")
	oldbkg:SetObjPos(attr.SpaceMagrin, 0, "father.width-" .. attr.SpaceMagrin, "father.height")
	
    if attr.NormalBkgID == "general.button.normal" then
        local textobj = self:GetControlObject("button.text")
        if textobj then
            textobj:SetObjPos(0,0,"father.width","father.height")
        end
    end
	
	
	if attr.CursorID then
		bkg:SetCursorID(attr.CursorID)
		oldbkg:SetCursorID(attr.CursorID)
	end
	
	
	if attr.Shadow then
		local textObj = self:GetControlObject("button.text")
		textObj:SetEnableShadow(1)
		textObj:SetShadowOffset(attr.ShadowOffsetX, attr.ShadowOffsetY)
		if attr.ShadowColor then
			textObj:SetShadowColorResID(attr.ShadowColor)
		end
	end	
end

function SetSpaceMagrin(self, magrin)
    if magrin == nil then
        return
    end
    local oldbkg = self:GetControlObject("button.oldbkg")
	oldbkg:SetObjPos(magrin, 0, "father.width-" .. magrin, "father.height")
end

function SetTextColor(self, color)
    if color == nil then
        return
    end
    local attr = self:GetAttribute()
	attr.TextColor = color
    local textObj = self:GetControlObject("button.text")
    textObj:SetTextColorResID(color)
end

function SetTextFont(self, font)
    if font == nil then
        return
    end
    local attr = self:GetAttribute()
	attr.TextFont = font
    local textObj = self:GetControlObject("button.text")
    textObj:SetTextFontResID(font)
end

function Show(self, visible)
    local attr = self:GetAttribute()
    attr.Visible = visible
    self:SetVisible(visible)
    self:SetChildrenVisible(visible)
	local focusrect = self:GetControlObject("focusrectangle")
	local nowvisible = focusrect:GetVisible()
	if not attr.ShowFocusRect then
		focusrect:SetVisible(false)
		focusrect:SetAlpha(0)
	end
	self:FireExtEvent("OnButtonShowChange")
end

function IsShow(self)
    local attr = self:GetAttribute()
    return attr.Visible
end

function SetTextPos(self, left, top, width, height)
	local textObj = self:GetControlObject("button.text")
	textObj:SetObjPos(left, top, left + width, top + height)
end

function GetTextWidth(self)
    local textObj = self:GetControlObject("button.text")
	return textObj:GetTextExtent()
end

function AddTipText(self,newText)
	local attr = self:GetAttribute()
	if attr then
		attr.TipText = newText
	end
end

function SetDefaultButton(self, isdefault)
    local attr = self:GetAttribute()
    if isdefault == attr.IsDefaultButton then
        return
    end
    attr.IsDefaultButton = isdefault
    SetDefaultAnimation(self)
    self:SetState(attr.NowState, true)
end

function IsDefaultButton(self)
	local attr = self:GetAttribute()
	return attr.IsDefaultButton
end