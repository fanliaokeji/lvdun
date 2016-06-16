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
	
    local tipObj = xarFactory:CreateUIObject("tip","Kuaikan.Toolbar.Tip")
	if not tipObj then
		return
	end
	
	local tipAttr = tipObj:GetAttribute()
	tipAttr.BkgTexture = "toolbar.bkg"
	tipAttr.TextBeginH = 0
	tipAttr.TextBeginV = 1

	tipObj:SetText(attr.TipText)
	tipObj:SetZorder(10000)
	self:AddChild(tipObj)
	attr.TipObj = tipObj

	local width, height = tipObj:GetSize(40, 25)
	local tree = self:GetOwner()
	local hwnd = tree:GetBindHostWnd()
	local wnd_left, wnd_top, wnd_right, wnd_bottom = hwnd:GetWindowRect()
	local abs_left, abs_top, abs_right, abs_bottom = self:GetAbsPos()
	
	local left, top = x, y + 20
	if attr.TipPosIsTopMid then
		left = (abs_right - abs_left - width)/2
		top = 0 - height
	end
	local right, bottom = left + width, top + 22
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

function GetTextExtent(self)
    local text = self:GetControlObject("button.text")
    return text:GetTextExtent()
end

function GetText(self)
    local text = self:GetControlObject("button.text")
    return text:GetText()
end

local function UpdateBkg(self, attr, noAni )	
	local bkg = self:GetControlObject("button.bkg")
	local oldbkg = self:GetControlObject("button.oldbkg")
	local ownerTree = self:GetOwner()
	local obj = self:GetControlObject("button.icon")

	local status = attr.Status
	
	----XLPrint("[carson4] ButtonEx UpdateBkg status = "..tostring(status))	
	
	local texture_id = ""
	if status == 1 then
	    if attr.IsDefaultButton then
		    texture_id = attr.DefaultBkgNormal
		else
		    texture_id = attr.BkgTextureID_Normal
		end
		
		if attr.bFocus == true and attr.BkgTextureID_FocusNormal ~= nil then
			----XLPrint("[carson4] ButtonEx UpdateBkg texture_id = "..attr.BkgTextureID_FocusNormal)	
			texture_id = attr.BkgTextureID_FocusNormal
		end
		
		if attr.IconBitmapID ~= nil and attr.IconBitmap == nil then
			obj:SetResID( attr.IconBitmapID )
		end
	elseif status == 2 then
		texture_id = attr.BkgTextureID_Hover
		if attr.bFocus == true and attr.BkgTextureID_FocusHover ~= nil then
			----XLPrint("[carson4] ButtonEx UpdateBkg texture_id = "..attr.BkgTextureID_FocusHover)	
			texture_id = attr.BkgTextureID_FocusHover
		end
		if attr.IconBitmapID_Hover ~= nil and attr.IconBitmap == nil then
			obj:SetResID( attr.IconBitmapID_Hover )
		end
	elseif status == 3 then
		texture_id = attr.BkgTextureID_Down
		if attr.bFocus == true and attr.BkgTextureID_FocusDown ~= nil then
			----XLPrint("[carson4] ButtonEx UpdateBkg texture_id = "..attr.BkgTextureID_FocusHover)	
			texture_id = attr.BkgTextureID_FocusDown
		end
		if attr.IconBitmapID_Down ~= nil and attr.IconBitmap == nil then
			obj:SetResID( attr.IconBitmapID_Down )
		end
	elseif status == 4 then
		texture_id = attr.BkgTextureID_Disable
		if attr.IconBitmapID_Disable ~= nil and attr.IconBitmap == nil then
			obj:SetResID( attr.IconBitmapID_Disable )
		end
	end
	local old_texture_id = bkg:GetTextureID()
	bkg:SetTextureID(texture_id)
	if noAni == nil or not noAni then
		oldbkg:SetTextureID(old_texture_id)
		oldbkg:SetAlpha(255)
		local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")	
		local aniAlpha = aniFactory:CreateAnimation("AlphaChangeAnimation")
		aniAlpha:BindRenderObj(oldbkg)
		aniAlpha:SetTotalTime(200)
		aniAlpha:SetKeyFrameAlpha(255,0)
		ownerTree:AddAnimation(aniAlpha)
		aniAlpha:Resume()
	end

end


local function UpdateText(self, attr)
	local textObj = self:GetControlObject("button.text")
	if attr.SpliceTextInfoID then
		--自动拼接ID
		local suf = {".normal", ".hover", ".down", ".disable"}


		local status = attr.Status
		local font_id = attr.TextFontID
		if font_id ~= "" then
			font_id = font_id .. suf[status]
			textObj:SetTextFontResID(font_id)
		end
		if attr.Status == 4 then
			textObj:SetTextColorResID(attr.DisableTextColor)
		elseif attr.Status == 2 then
			textObj:SetTextColorResID(attr.TextColorID_Hover or attr.TextColorID)
		else
			local color_id = attr.TextColorID
			if color_id ~= "" then
				color_id = color_id
				textObj:SetTextColorResID(color_id)
			end
		end
	else
		--不拼接ID
		textObj:SetTextFontResID(attr.TextFontID)
		if attr.Status == 4 then
			textObj:SetTextColorResID(attr.DisableTextColor)
		elseif attr.Status == 2 then
			textObj:SetTextColorResID(attr.TextColorID_Hover or attr.TextColorID)
		else
			textObj:SetTextColorResID(attr.TextColorID)
		end
	end
end

function SetTextFontID(self, id)
	local attr = self:GetAttribute()
	attr.TextFontID = id
	UpdateText(self, attr)
end


function SetTextColorID(self, normal, hover)
	local attr = self:GetAttribute()
	attr.TextColorID = normal
	if hover then
		attr.TextColorID_Hover = hover
	end
	UpdateText(self, attr)
end

local function SetStatus(self, status)
	local attr = self:GetAttribute()
	if attr.Status == status then
		return
	end
	attr.Status = status
	UpdateBkg(self, attr)
	UpdateText(self, attr)
end

local function InitPosition(self)
	local attr = self:GetAttribute()
	local left, top, right, bottom = self:GetObjPos()
	local self_width = right - left
	local self_height = bottom - top

	local obj = self:GetControlObject("button.icon")
	left = attr.IconLeftPos
	right = left.."+"..attr.IconWidth
	top = attr.IconTopPos
	bottom = top.."+"..attr.IconHeight
	obj:SetObjPos(left, top, right, bottom)

	obj = self:GetControlObject("button.text")
	left = attr.TextLeftPos
	right = self_width
	top = attr.TextTopPos
	bottom = self_height
	obj:SetObjPos2(left, top, right-left, bottom)
end

function SetEnable(self, enable)
	local attr = self:GetAttribute()
	if not attr then
		return
	end
	
	if enable then
		attr.Status = 1
	else
		attr.Status = 4
	end
	attr.Enable = enable
	UpdateBkg(self, attr, true)
	UpdateText(self, attr )
end


function GetEnable(self)
	local attr = self:GetAttribute()
	if attr.Status ~= 4 then
		return true
	else
		return false
	end
end

function SetObjPos(obj, left, top, right, bottom)
	local pre_left, pre_top, pre_right, pre_bottom = obj:GetObjPos()
	if left == nil then
		left = pre_left
	end
	if top == nil then
		top = pre_top
	end
	if right == nil then
		right = pre_right
	end
	if bottom == nil then
		bottom = pre_bottom
	end
	obj:SetObjPos(left, top, right, bottom)
end

function SetIconPos(self, left, top, right, bottom)
	local obj = self:GetControlObject("button.icon")
	SetObjPos(obj, left, top, right, bottom)
end

function SetTextPos(self, left, top, right, bottom)
	local obj = self:GetControlObject("button.text")
	SetObjPos(obj, left, top, right, bottom)
end

function SetValign(self, align)
	local obj = self:GetControlObject("button.text")
	obj:SetVAlign(align)
end

function SetText(self, text)
	local attr = self:GetAttribute()
	if attr.Text == text then
		return 0
	end

	attr.Text = text
	local obj = self:GetControlObject("button.text")
	local oldWidth, oldHeight = obj:GetTextExtent()
	obj:SetText(text)
	local newWidth, newHeight = obj:GetTextExtent()
	return newWidth - oldWidth
end

function SetBkgTexture(self, texture_id)
	local attr = self:GetAttribute()
	attr.BkgTextureID_Normal = texture_id .. ".normal"
	attr.BkgTextureID_Hover = texture_id .. ".hover"
	attr.BkgTextureID_Down = texture_id .. ".down"
	attr.BkgTextureID_Disable = texture_id .. ".disable"

	UpdateBkg(self, attr, true)
end
function SetBkgTextureEx(self, normal, hover,down, disable)
	local attr = self:GetAttribute()
	attr.BkgTextureID_Normal = normal
	attr.BkgTextureID_FocusNormal = normal
	attr.BkgTextureID_Hover = hover
	attr.BkgTextureID_FocusHover = hover
	attr.BkgTextureID_Down = down
	attr.BkgTextureID_FocusDown = down
	attr.BkgTextureID_Disable = disable
	UpdateBkg(self, attr, true)

end

function SetNormalBkgTexture(self, texture_id)
	local attr = self:GetAttribute()
	attr.BkgTextureID_Normal = texture_id

	UpdateBkg(self, attr, true)
end

function SetHoverBkgTexture(self, texture_id)
	local attr = self:GetAttribute()
	attr.BkgTextureID_Hover = texture_id

	UpdateBkg(self, attr, true)
end

function SetDownBkgTexture(self, texture_id)
	local attr = self:GetAttribute()
	attr.BkgTextureID_Down = texture_id

	UpdateBkg(self, attr, true)
end

function SetDisableBkgTexture(self, texture_id)
	local attr = self:GetAttribute()
	attr.BkgTextureID_Disable = texture_id

	UpdateBkg(self, attr, true)
end

function SetFocusBkgTexture(self, texture_id)
	local attr = self:GetAttribute()
	attr.BkgTextureID_FocusNormal = texture_id .. ".normal"
	attr.BkgTextureID_FocusHover = texture_id .. ".hover"
	attr.BkgTextureID_FocusDown = texture_id .. ".down"

	UpdateBkg(self, attr, true)
end

function SetFocusNormalBkgTexture(self, texture_id)
	local attr = self:GetAttribute()
	attr.BkgTextureID_FocusNormal = texture_id

	UpdateBkg(self, attr, true)
end

function SetFocusHoverBkgTexture(self, texture_id)
	local attr = self:GetAttribute()
	attr.BkgTextureID_FocusHover = texture_id

	UpdateBkg(self, attr, true)
end

function SetFocusDownBkgTexture(self, texture_id)
	local attr = self:GetAttribute()
	attr.BkgTextureID_FocusDown = texture_id

	UpdateBkg(self, attr, true)
end

function  SetIconBitmap(self, bitmap)
	local attr = self:GetAttribute()
	attr.IconBitmap = bitmap
	local obj = self:GetControlObject("button.icon")
	obj:SetBitmap(bitmap)
end
function SetIconImage(self, image_id, hover, down, disable)
	local attr = self:GetAttribute()
	attr.IconBitmapID = image_id
	attr.IconBitmapID_Hover = hover
	if hover == nil then
		attr.IconBitmapID_Hover = image_id
	end
	attr.IconBitmapID_Down = down
	if down == nil then
		attr.IconBitmapID_Down = image_id
	end
	attr.IconBitmapID_Disable = disable
	if disable == nil then
		attr.IconBitmapID_Disable = image_id
	end
	local obj = self:GetControlObject("button.icon")
	obj:SetResID(image_id)
end


function OnBind(self)
	local attr = self:GetAttribute()
	if attr.IconBitmapID_Hover == nil or attr.IconBitmapID_Hover == "" then
		attr.IconBitmapID_Hover = attr.IconBitmapID
	end
	if attr.IconBitmapID_Down == nil or attr.IconBitmapID_Down == "" then
		attr.IconBitmapID_Down = attr.IconBitmapID
	end
	if attr.IconBitmapID_Disable == nil or attr.IconBitmapID_Disable == "" then
		attr.IconBitmapID_Disable = attr.IconBitmapID
	end
	UpdateBkg(self, attr)
	UpdateText(self, attr)
	
	InitPosition(self)
	
	local icon = self:GetControlObject("button.icon")
	if attr.IconBitmapID ~= nil then
		icon:SetResID(attr.IconBitmapID)
	else
		icon:SetResID("")
	end
	
	local textObj = self:GetControlObject("button.text")
	textObj:SetText(attr.Text)
	textObj:SetVAlign(attr.VAlign)
	textObj:SetHAlign(attr.HAlign)
end


function OnPosChange(self, focus)
	InitPosition(self)
	if not focus then
		RemoveTip(self)
	end
	return true
end



function OnLButtonDown(self)
	RemoveTip( self )
	local attr = self:GetAttribute()
	local status = attr.Status
	if status ~= 4 and status ~= 3 then
		self:SetCaptureMouse(true)
		attr.Capture = true
		SetStatus(self, 3)
	end
	return 0, false
end

function OnLButtonUp(self, x, y, flags)
	local attr = self:GetAttribute()
	local status = attr.Status
	
	if attr.Capture then
		self:SetCaptureMouse(false)
		attr.Capture = false
		if status ~= 4 then
			local left, top, right, bottom = self:GetObjPos()
			if x >= 0 and x <= right - left and y >= 0 and y <= bottom - top then
				if attr.Status ~= 2 then
					SetStatus(self, 2)
				end
				self:FireExtEvent("OnButtonClick")
			else
				if attr.Status ~= 1 then
					SetStatus( self, 1 )
				end
			end
		end
	end
end

function OnFocusChange(self,isFocus,lastFocusObj)
	local attr = self:GetAttribute()
	attr.bFocus = isFocus
	UpdateBkg(self, attr, true)
	return 0,true
end

function OnKeyDown(self, char, repeatCount, flags)
	local attr = self:GetAttribute()
	if attr.bFocus == true and self:GetVisible() and self:GetEnable() then
		if char == 13 or char == 32  then
			self:FireExtEvent("OnButtonClick")
		end
	end 	
end


function OnMouseMove(self, x, y )
	local attr = self:GetAttribute()
	local status = attr.Status
	if status ~= 4 then
		local left, top, right, bottom = self:GetObjPos()
		if x >= 0 and x <= right - left and y >= 0 and y <= bottom - top then
			if attr.Capture then
				SetStatus(self, 3)
			else
				SetStatus(self, 2)
			end
		else
			SetStatus(self, 1)
		end
	end
	return 0
end

function OnControlMouseLeave( self )
	local attr = self:GetAttribute()
	if attr.Status ~= 4 then
		SetStatus( self, 1 )
	end
	RemoveTip(self)
	return 0
end

function OnControlMouseEnter( self, x, y )
	if self:GetEnable() then
		AddTip(self, x, y)
	end
	return 0
end

function OnInitControl( self )
	local attr = self:GetAttribute()
	if attr.EffectColorResID ~= nil then
		
		local textObj = self:GetControlObject("button.text")
		textObj:SetEffectType("bright")
		textObj:SetEffectColorResID( attr.EffectColorResID )
		if attr.BkgTextureID_Normal == "siamese.button.left.normal" or attr.BkgTextureID_Normal == "siamese.button.right.normal" then
		    local left, top, right, bottom = textObj:GetObjPos()
		    textobj:SetObjPos(left, top, right, bottom-4)
		end
	end
	
	SetDefaultAnimation(self)
	self:SetEnable(attr.Enable)
	
	if attr.CursorID then
		local bkg = self:GetControlObject("button.bkg")
		local oldbkg = self:GetControlObject("button.oldbkg")
		local icon = self:GetControlObject("button.icon")
		bkg:SetCursorID(attr.CursorID)
		oldbkg:SetCursorID(attr.CursorID)
		icon:SetCursorID(attr.CursorID)
	end
	return true
end

function Show(self, visible)
    local attr = self:GetAttribute()
    attr.Visible = visible
    self:SetVisible(visible)
    self:SetChildrenVisible(visible)
end

function AddTipText(self,newText)
	local attr = self:GetAttribute()
	if attr then
		attr.TipText = newText
	end
end

function GetTipText(self)
	local attr = self:GetAttribute()
	return attr.TipText
end

function SetDefaultButton(self, isdefault)
    local attr = self:GetAttribute()
    if isdefault == attr.IsDefaultButton then
        return
    end
    attr.IsDefaultButton = isdefault
    SetDefaultAnimation(self)
    UpdateBkg(self, attr, true)
end

function OnMouseWheel(self)
	self:RouteToFather()
end