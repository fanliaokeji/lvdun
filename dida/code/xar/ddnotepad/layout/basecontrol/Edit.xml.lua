function SetRange(self, nMin, nMax)
	if nMin == nil then
		nMin = 0
	end
	
	if nMax == nil then
		nMax = 0
	end
	if nMin > nMax then
		local temp = nMin
		nMin = nMax
		nMax = nMin
	end
	local attr = self:GetAttribute()
	attr.Min = nMin
	attr.Max = nMax
end
function SetState(self, state)
    local attr = self:GetAttribute()
    if attr.NowState ~= state then
        local bkg = self:GetControlObject("newedit.bkg")
		local edit = self:GetControlObject("newedit.edit")
        attr.NowState = state
        if attr.NowState == 0 then
			if not attr.auto_set_text then 
				RemoveTip(self)
			end
            if attr.NormalBkgID then
				bkg:SetTextureID(attr.NormalBkgID)
			end
			edit:SetTextColorID(attr.Color)
        elseif attr.NowState == 1 then
			if attr.IsFloat then
				if not attr.auto_set_text then 
					RemoveTip(self)
				end
				self:AddTip("可输入范围"..attr.Min.."到"..attr.Max)
			end
            bkg:SetTextureID(attr.HoverBkgID)
			edit:SetTextColorID(attr.HoverColor)
        elseif attr.NowState == 2 then
            bkg:SetTextureID(attr.DisableBkgID)
			edit:SetTextColorID(attr.DisableColor)
        end
    end
end

function SetFocus( self, focus )
    local edit = self:GetControlObject("newedit.edit")
	edit:SetFocus( focus )
end

function GetFocus( self )
    local edit = self:GetControlObject("newedit.edit")
	return self:GetChildByIndex(0):GetFocus() or edit:GetFocus()
end

function SetText(self, text)
	local attr = self:GetAttribute()
    local edit = self:GetControlObject("newedit.edit")
	if edit then
		if attr then
			attr.Text = text
			edit:SetText(text)
		end
	end
	if not text or "" == text then
		if not attr.Focus and attr.TextHint then
			local texthintobj = self:GetControlObject("text.hint")
			texthintobj:SetVisible(true)
		end
	else
		local texthintobj = self:GetControlObject("text.hint")
		texthintobj:SetVisible(false)
	end
end

function SetSel( self, start_pos, end_pos )
    local edit = self:GetControlObject("newedit.edit")
	edit:SetSel( start_pos, end_pos )
end

function GetSel( self )
    local edit = self:GetControlObject("newedit.edit")
	return edit:GetSel()
end

function GetIsPassword( self )
    local edit = self:GetControlObject("newedit.edit")
	return edit:GetIsPassword()
end
function SetIsPassword( self, b)
	local edit = self:GetControlObject("newedit.edit")
	edit:SetIsPassword(b)
end
function GetText(self)
    local edit = self:GetControlObject("newedit.edit")
	local ownerattr = edit:GetOwnerControl():GetAttribute()
	local text = edit:GetText()
    return text
end

function GetSelText(self)
    local edit = self:GetControlObject("newedit.edit")
	local selText = edit:GetSelText()
    return selText
end

function SetReadOnly(self, enable)
    local edit = self:GetControlObject("newedit.edit")
    edit:SetReadOnly(enable)
end

function SetMultiline(self, enable)
    local edit = self:GetControlObject("newedit.edit")
    edit:SetMultiline(enable)
end

function GetReadOnly(self, enable)
    local edit = self:GetControlObject("newedit.edit")
    return edit:GetReadOnly()
end

function GetMultiline(self, enable)
    local edit = self:GetControlObject("newedit.edit")
    return edit:GetMultiline()
end

function SetEnable(self, enable)
    local attr = self:GetAttribute()
    attr.Enable = enable
    local edit = self:GetControlObject("newedit.edit")
    edit:SetReadOnly(not attr.Enable)
    edit:SetNoCaret(not attr.Enable)
    if attr.Enable then
        self:SetState(0)
    else
        self:SetState(2)
    end
end

function GetEnable(self)
	local attr = self:GetAttribute()
	return attr.Enable
end
function SetTextHintFont(fontid)
	local attr = self:GetAttribute()
	attr.TextHintFont = fontid
	self:GetControlObject("text.hint"):SetTextFontResID(attr.TextHintFont)
end
function SetTextHintColor(colorid)
	local attr = self:GetAttribute()
	attr.TextHintColor = colorid
	self:GetControlObject("text.hint"):SetTextColorResID(attr.TextHintColor)
end

function RemoveTip(self)
	local attr = self:GetAttribute()
	if attr.IsTipPoping and attr.__fPopTipsFunction then
		attr.__fPopTipsFunction()
		attr.IsTipPoping = false
	end
end

function AddTip(self, text, type_)

	local attr = self:GetAttribute()
	if attr.__fPopTipsFunction then
		local left,top,right,bottom = self:GetObjPos()
		local height = bottom - top
		attr.__fPopTipsFunction(text,0, self, 0, 0-height -10,true)
		attr.IsTipPoping = true
	end
end

function OnBind(self)
    local attr = self:GetAttribute()
    local left, top, right, bottom = self:GetObjPos()
    local width = right - left
    local height = bottom - top
    local bkg = self:GetControlObject("newedit.bkg")
    if attr.NormalBkgID then
		bkg:SetTextureID(attr.NormalBkgID)
	end
    local edit = self:GetControlObject("newedit.edit")
    
	if attr.Text ~= nil then
		edit:SetText(attr.Text)
	end
	
	if not attr.EditCustom then
		attr.EditLeft = 0
		attr.EditTop = 4
		attr.EditWidth = width-1
		attr.EditHeight = height - 4
	end
	--这里SetObjPos给了具体数值，父窗口发生变化时edit无法随之变化，暂时注掉
    -- edit:SetObjPos(attr.EditLeft, attr.EditTop, ""..attr.EditLeft.."+"..attr.EditWidth, ""..attr.EditTop.."+"..attr.EditHeight)
    self:SetEnable(attr.Enable)
end

function OnMouseMove(self)
    local control = self:GetOwnerControl()
	
    local attr = control:GetAttribute()
    if not attr.Enable then
        return
    end
    if attr.NowState==0 then
        --control:SetState(1)
    end
end

function Edit__OnMouseLeave(self)
    local control = self:GetOwnerControl()
    local attr = control:GetAttribute()
    if not attr.Enable then
        return
    end
    control:SetState(0)
end

function Edit__OnMouseMove(self)
    local control = self:GetOwnerControl()
    local attr = control:GetAttribute()
    if not attr.Enable then
        return
    end
    if attr.NowState==0 then
        control:SetState(1)
    end
end

function OnMouseLeave(self)
    local control = self:GetOwnerControl()
    local attr = control:GetAttribute()
    if not attr.Enable then
        return
    end
   --control:SetState(0)
end

function Edit__OnSelChange(self, selRangeMin, selRangeMax, selType)
	local control = self:GetOwnerControl()
	control:FireExtEvent("OnSelChange", selRangeMin, selRangeMax, selType)
end

function Edit__OnChange(self)
    local control = self:GetOwnerControl()
    local text = self:GetText()
	local ownerattr = control:GetAttribute()
	if not ownerattr.auto_set_text then 
		RemoveTip(control)
	end
	ownerattr.auto_set_text = false
	if ownerattr.Focus ~= true then
		-- return
	end
	if ownerattr.IsNumber then
	
		if text == "0" and ownerattr.Min > 0 then
			control:AddTip( "输入值必须大于等于"..ownerattr.Min .. "！")
			control:SetText("")
			return
		end

		if text ~= nil then
			for i = 1, string.len( text ) do
				if string.byte( text, i ) < string.byte( "0" ) or string.byte( text, i ) > string.byte( "9" ) then
					if i ~= 1 or string.byte( text, i ) ~= string.byte("-") then
						control:AddTip( "必须输入数字字符！")
						self:SetText( ownerattr.Text )
						return
					end
				end
			end
			if string.byte(text, 1) == string.byte("-") and ownerattr.Min >= 0 then
				control:SetText("")
				control:AddTip("输入值必须大于等于"..ownerattr.Min)
				return
			end
			if ownerattr.Min ~= 0 or ownerattr.Max ~= 0 then
				if ownerattr.Min < ownerattr.Max then
					if tonumber( text ) == nil or text == "" or tonumber( text ) < ownerattr.Min then
						control:AddTip( "输入值必须大于等于"..ownerattr.Min .. "！")
						return
					elseif  tonumber( text ) ~= nil and tonumber( text ) > ownerattr.Max then
						control:AddTip( "输入值必须小于等于"..ownerattr.Max .. "！")
						self:SetText( ownerattr.Text )
						return
					end
				end
			end
		end
	else
		if ownerattr.MaxLength > 0 then
			if string.len( text ) > ownerattr.MaxLength then
				control:AddTip( "最大输入字符个数为"..ownerattr.MaxLength  .. "！")
				self:SetText( ownerattr.Text )
				return
			end
		end
	end
	if ownerattr.IsFloat then
		if text ~= nil then
			local haspoint = false
			for i = 1, string.len( text ) do
				if string.byte( text, i ) < string.byte( "0" ) or string.byte( text, i ) > string.byte( "9" ) then
					if not haspoint and i > 1 and string.byte( text, i ) == string.byte(".") and  string.byte( text, i-1 ) >= string.byte("0") and string.byte( text, i-1 ) < string.byte("9") or i == 1 and string.byte( text, i ) == string.byte("-")  then
					else 
						control:AddTip( "必须输入数字字符！")
						self:SetText( ownerattr.Text )
						return 
					end
					if ownerattr.Precision then
						if string.byte( text, i ) == string.byte(".") and string.len( text ) - i > ownerattr.Precision then
							control:AddTip( "保留小数点后"..ownerattr.Precision.."位")
							self:SetText( ownerattr.Text )
							return
						end
					end
					if string.byte( text, i ) ~= string.byte("-") then
						haspoint = true
					end
				end
			end
			if 2 == string.len( text ) and string.byte(text, 1) == string.byte("0") and string.byte(text, 2) ~= string.byte(".") or 3 == string.len( text ) and string.byte(text, 1) == string.byte("-") and string.byte(text, 2) == string.byte("0") and string.byte(text, 3) ~= string.byte(".") then
				control:AddTip( "必须输入数字字符！")
				self:SetText( ownerattr.Text )
				return
			end
			if string.byte(text, 1) == string.byte("-") and ownerattr.Min >= 0 then
				control:SetText("")
				control:AddTip("输入值必须大于等于"..ownerattr.Min)
				return
			end
			if  string.byte(text, 1) == string.byte("-") and 1 == string.len( text ) then
				return
			end
			if ownerattr.Min ~= 0 or ownerattr.Max ~= 0 then
				if ownerattr.Min < ownerattr.Max then
					if tonumber(text) == nil or text == "" then
						self:SetText( "" )
						return
					end
					if  tonumber( text ) < ownerattr.Min then
						control:AddTip( "输入值必须大于等于"..ownerattr.Min .. "！")
						self:SetText( ownerattr.Text )
						return
					elseif   tonumber( text ) > ownerattr.Max then
						control:AddTip( "输入值必须小于等于"..ownerattr.Max .. "！")
						self:SetText( ownerattr.Text )
						return
					end
				end
			end
		end
	else
		if ownerattr.MaxLength > 0 then
			if string.len( text ) > ownerattr.MaxLength then
				control:AddTip( "最大输入字符个数为"..ownerattr.MaxLength  .. "！")
				self:SetText( ownerattr.Text )
				return
			end
		end
	end
	local lastText = ownerattr.Text
	ownerattr.Text = text
    control:FireExtEvent("OnEditChange", text, lastText)
end

function Edit__OnFocusChange(self, focus)
    local owner = self:GetOwnerControl()
    local ownerattr = owner:GetAttribute()
	local lastText = ownerattr.Text
    if focus then
        RemoveTip(owner)
		if ownerattr.Enable and not ownerattr.ReadOnly and ownerattr.HitSelAll then
			AsynCall( function()
						self:SetSelAll()
					  end )
		end
		ownerattr.Focus = true
    else
		ownerattr.Focus = false
        RemoveTip(owner)
		if ownerattr.Enable then
			local text = self:GetText()
			local ownerattr = owner:GetAttribute()
			if ownerattr.IsNumber then
				if text ~= nil then
					if ownerattr.Min ~= 0 or ownerattr.Max ~= 0 then
						if ownerattr.Min < ownerattr.Max then
							if tonumber( text ) == nil or tonumber( text ) < ownerattr.Min then
								text = ""..ownerattr.Min
								self:SetText( text )
								ownerattr.auto_set_text = true
								owner:FireExtEvent("OnEditChange", text, lastText)
							elseif  tonumber( text ) > ownerattr.Max then
								text = ""..ownerattr.Max
								ownerattr.auto_set_text = true
								self:SetText( text )
								owner:FireExtEvent("OnEditChange", text, lastText)
							end
						end
					end
				end
			end
			if ownerattr.IsFloat then
				if tonumber( text ) == nil or string.byte(text, string.len(text) ) ==  string.byte(".") then
					if ownerattr.Max ~= nil and ownerattr.Min ~= nil then
						self:SetText( (ownerattr.Max + ownerattr.Min) / 2 )
						owner:FireExtEvent("OnEditChange", (ownerattr.Max + ownerattr.Min) / 2 , lastText)
					end
				end
			end
			owner:SetState(0)
		else
			owner:SetState(2)
		end
		if text ~= nil then
			ownerattr.Text = text
		end
    end
	
	if ownerattr.TextHint then
		local texthintobj = owner:GetControlObject("text.hint")
		
		if focus then
			texthintobj:SetVisible(false)
		else
			local text = self:GetText()
			if not text or "" == text then
				texthintobj:SetVisible(true)
			end
		end
	end
	
    owner:FireExtEvent("OnEditFocusChange", focus)
end

function Edit__OnChar(self, char, counts, flag)
    if not self:GetMultiline() then
        return
    end
end

function Edit_OnKeyDown(self, uChar, uRepeatCount, uFlags)
	local owner = self:GetOwnerControl()
	owner:FireExtEvent("OnEditKeyDown", uChar, uRepeatCount, uFlags)
end

function Edit_OnKeyUp(self, uChar, uRepeatCount, uFlags)
	local owner = self:GetOwnerControl()
	owner:FireExtEvent("OnEditKeyUp", uChar, uRepeatCount, uFlags)
end

function Edit__OnMouseWheel(self, x, y, distance, flags)
	local owner = self:GetOwnerControl()
	owner:FireExtEvent("OnEditMouseWheel", x, y, distance, flags)
end

function OnInitControl(self)
    local attr = self:GetAttribute()
	
	-- TipFunction
	if attr.PopTipsFunction and attr.PopTipsFunction ~= "" then
		local f = loadstring("return " .. attr.PopTipsFunction)()
		if type(f) == "function" then
			attr.__fPopTipsFunction = f
		end
	end
    local edit = self:GetControlObject("newedit.edit")
	edit:SetFontID( attr.Font )
	edit:SetTextColorID( attr.Color)
    edit:SetReadOnly(attr.ReadOnly)
    edit:SetMultiline(attr.Multiline)
	edit:SetReadOnly(not attr.Enable)
	edit:SetNoCaret(not attr.Enable)
	edit:SetAllowHSB( attr.AllowHSB )
	edit:SetAllowVSB( attr.AllowVSB )
	edit:SetIsPassword(attr.Password)
	attr.auto_set_text = false
	edit:SetDisableIME(attr.Disableime)
	if attr.Enable then
        self:SetState(0)
	else
        self:SetState(2)
	end
	edit:SetWordWrap(attr.WordWrap)
	edit:SetAutoUrlDetect(attr.AutoUrlDetect)
	if attr.ViewinsetLeft ~= nil and attr.ViewinsetRight ~= nil and attr.ViewinsetTop ~= nil and attr.ViewinsetButtom ~= nil then
		edit:SetViewInset( attr.ViewinsetLeft, attr.ViewinsetTop, attr.ViewinsetRight, attr.ViewinsetButtom )
	end
	
	-- text hint 
	local texthintobj = self:GetControlObject("text.hint")
	if attr.TextHint then
		texthintobj:SetText(attr.TextHint)
		if not attr.Text or "" == attr.Text then
			texthintobj:SetVisible(true)
		end
	end
	
	if attr.TextHintFont then
		texthintobj:SetTextFontResID(attr.TextHintFont)
	end
	if attr.TextHintColor then
		texthintobj:SetTextColorResID(attr.TextHintColor)
	end
end

function OnFocusChange( self, status )
	if status then
		local edit = self:GetOwnerControl():GetControlObject("newedit.edit")
		edit:SetFocus( true )
	end
end

function Control_OnFocusChange( self, status )
	if status then
		local edit = self:GetControlObject("newedit.edit")
		edit:SetFocus( true )
	end
end

function OnVScroll(self, int1 ,int2)
	local edit = self:GetOwnerControl():GetControlObject("newedit.edit")
	local pos = self:GetScrollPos()
	if int2 == 1 then
		self:SetScrollPos( pos - 15, true )
	elseif int2 == 2 then
		self:SetScrollPos( pos + 15, true )
	end
	edit:SetScrollPos(true, self:GetScrollPos())
end

function OnHScroll( self, int1, int2)
	local edit = self:GetOwnerControl():GetControlObject("newedit.edit")
	local pos = self:GetScrollPos()
	if int2 == 1 then
		self:SetScrollPos( pos - 15, true )
	elseif int2 == 2 then
		self:SetScrollPos( pos + 15, true )
	end
	edit:SetScrollPos(false, self:GetScrollPos())
end

function OnGetSBIdealSize(self, vertical)
	
	if vertical then
		return 23,0
	else
		return 0,23
	end
	
end

function OnSetSBRect(self, vertical, left, top, right, bottom)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")
		if self:GetScrollVisible( false ) then
			var:SetObjPos(left,top,right,bottom+23)
		else
			var:SetObjPos(left,top,right,bottom)
		end
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")
		if self:GetScrollVisible( true ) then
			var:SetObjPos(left,top,right+7,bottom)
		else
			var:SetObjPos(left,top,right,bottom)
		end
	end
end

function OnSetSBVisible(self, vertical, visible)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")	
		var:Show(visible)
		if not visible then
			var:SetScrollRange( 0, 0 )
		end
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")	
		var:Show(visible)
		if not visible then
			var:SetScrollRange( 0, 0 )
		end
	end
end

function OnGetSBVisible(self, vertical)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")	
		return var:IsShow()
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")	
		return var:IsShow()
	end
end

function OnGetSBEnable(self, vertical)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")	
		return var:GetEnable()
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")	
		return var:GetEnable()
	end
end

function OnSetSBEnable(self, vertical, enable)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")
		var:SetEnable(enable)
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")
		var:SetEnable(enable)
	end
end

function OnGetSBPageSize(self, vertical)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")	
		return var:GetPageSize()
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")	
		return var:GetPageSize()
	end
end

function OnSetSBPageSize(self, vertical, pagesize)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")	
		var:SetPageSize(pagesize, true)
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")	
		var:SetPageSize(pagesize, true)
	end
end

function OnSetSBRange(self, vertical, minrange, maxrange)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")	
		var:SetScrollRange(minrange, maxrange - var:GetPageSize(), true)
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")	
		var:SetScrollRange(minrange, maxrange - var:GetPageSize()+8, true)
	end
end

function OnGetSBRange(self, vertical)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")
		return var:GetScrollRange()
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")
		return var:GetScrollRange()
	end
end

function OnSetSBPos(self, vertical, pos)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")
		var:SetScrollPos(pos, true)
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")
		var:SetScrollPos(pos, true)
	end
end

function OnGetSBPos(self, vertical)
	if vertical then
		local var = self:GetOwnerControl():GetControlObject("vsb")
		return var:GetScrollPos()
	else
		local var = self:GetOwnerControl():GetControlObject("hsb")
		return var:GetScrollPos()
	end
end

function Edit_OnRButtonUp( self, x, y, flag )
	local attr = self:GetOwnerControl():GetAttribute()
	if attr.ShowMenu and not self:GetIsPassword() then
		XLSetGlobal("xmp.cur.edit",self)
		local x, y = XMP.XmpPre.self:GetCursorPos()
		local wnd = self:GetOwner():GetBindHostWnd()
		XMP.WndFactory.CreateMenuEx(x, y, "Singleton.MenuHostWnd", "tree.edit.menu.context", "context_menu", wnd:GetWndHandle())
	end
end

function Undo_OnInit( self )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		if not edit:CanUndo() or edit:GetReadOnly() then
			self:SetEnable( 0 )
		end
	end
end

function Undo_OnSelect( self, name, id )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		edit:Undo()
	end
end

function Cut_OnInit( self )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		local start, end_ = edit:GetSel()
		if start == end_ or edit:GetReadOnly() then
			self:SetEnable( 0 )
		end
	end
end

function Cut_OnSelect( self, name, id )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		edit:Cut()
	end
end

function Copy_OnInit( self )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		local start, end_ = edit:GetSel()
		if start == end_ then
			self:SetEnable( 0 )
		end
	end
end

function Copy_OnSelect( self, name, id )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		edit:Copy()
	end
end

function Paste_OnInit( self )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		if edit:GetReadOnly() then
			self:SetEnable( 0 )
			return
		end
	end

	local clipbrdText = XMP.XmpPre.self:Call("GetClipbrdData")
	if not clipbrdText or clipbrdText == "" then
		self:SetEnable( 0 )
	end
end

function Paste_OnSelect( self, name, id )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		edit:Paste()
	end
end

function Delete_OnInit( self )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		local start, end_ = edit:GetSel()
		if start == end_ or edit:GetReadOnly() then
			self:SetEnable( 0 )
		end
	end
end

function Delete_OnSelect( self )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		edit:Clear()
	end
end


function SelAll_OnInit( self )
    local edit = XLGetGlobal( "xmp.cur.edit" )
    if edit ~= nil then
        local text = edit:GetText()
        if text:len() == 0 then
            self:SetEnable(0)
        end
    end
end

function SelAll_OnSelect( self, name, id )
	local edit = XLGetGlobal( "xmp.cur.edit" )
	if edit ~= nil then
		edit:SetSelAll()
	end
end

function AppendText( self, text, noscroll, canundo )
    local edit = self:GetControlObject("newedit.edit")
	if edit ~= nil then
		edit:AppendText( text, noscroll, canundo )
	end
end

function GetLineCount( self )
	local edit = self:GetControlObject("newedit.edit")
	local count = 0
	if edit ~= nil then
		count = edit:GetLineCount()
	end
	return count
end

function GetLine( self, idx )
	local edit = self:GetControlObject("newedit.edit")
	local text
	if edit ~= nil and idx >= 0 then
		text = edit:GetLine(idx)
	end
	return text
end

function OnScrollBarMouseWheel( self, name, x, y, distance )
	local ThumbPos = self:GetThumbPos()
    self:SetThumbPos(ThumbPos - distance/10)
end
function LineIndex( self, line )
	local edit = self:GetControlObject("newedit.edit")
	if edit ~= nil then
		return edit:LineIndex(line)
	end
	return -1
end

function PosFromChar( self, charindex )
	local edit = self:GetControlObject("newedit.edit")
	if edit ~= nil then
		return edit:PosFromChar(charindex)
	end
	return 0, 0
end

function ScrollCaret( self )
	local edit = self:GetControlObject("newedit.edit")
	if edit ~= nil then
		return edit:ScrollCaret(charindex)
	end
end


function SetIsNumber(self,isnumber)
	local edit = self:GetControlObject("newedit.edit")
	if edit then
		edit:SetIsNumber(isnumber)
	end
end

function SetViewInset(self,...)
	local left,top,right,buttom = ...
	local edit = self:GetControlObject("newedit.edit")
	edit:SetViewInset(left,top,right,buttom)
end
function SetTextColor(self,colorid)
	local edit = self:GetControlObject("newedit.edit")
	edit:SetTextColorID(colorid)
end
