function SetCheck(self, bCheck, bNotFireEvent)
	local button = self:GetControlObject("button")
	local btnattr = button:GetAttribute()
	local attr = self:GetAttribute()
	if attr.Select and attr.Type == "radio" and not bNotFireEvent then
		return
	end
	if bCheck == nil then
		bCheck = not attr.Select
	end
	if bCheck then
		attr.Select = true
		btnattr.NormalBkgID = attr.CheckNormal
		btnattr.HoverBkgID = attr.CheckHover
		btnattr.DownBkgID = attr.CheckHover
		btnattr.DisableBkgID = attr.CheckNormal
	else
		attr.Select = false
		btnattr.NormalBkgID = attr.UnCheckNormal
		btnattr.HoverBkgID = attr.UnCheckHover
		btnattr.DownBkgID = attr.UnCheckHover
		btnattr.DisableBkgID = attr.UnCheckNormal
	end
	button:Updata()
	if not bNotFireEvent then
		self:FireExtEvent("OnSelect", attr.Select)
	end
end

function CheckBoxOnEnableChange(self, iseable)
	local attr = self:GetAttribute()
	local text = self:GetControlObject("text")
	if text then
		if not attr.NormalTextColor and not isEnable then
			attr.NormalTextColor = text:GetTextColorResID()
		end
		if isEnable then
			text:SetTextColorResID(attr.NormalTextColor)
		else
			text:SetTextColorResID("999999")
		end
	end
end

function CheckBoxOnLButtonDown(self, x, y)
	local attr = self:GetAttribute()
	local l, t, r, b = self:GetObjPos()
	local button = self:GetControlObject("button")
	if x >= 0 and y >= 0 and x <= r-l and y <= b-t then
		attr.lbuttondown = true
		button:SetState(2)
	else
		attr.lbuttondown = false
		button:SetState(0)
	end
end

function CheckBoxOnMouseEnter(self)
	local button = self:GetControlObject("button")
	button:SetState(1)
end

function CheckBoxOnMouseLeave(self)
	local attr = self:GetAttribute()
	attr.lbuttondown = false
	local button = self:GetControlObject("button")
	button:SetState(0)
end

function buttonOnClick(self)
	local owner = self:GetOwnerControl()
	owner:SetCheck()
end

function CheckBoxOnLButtonUp(self, x, y)
	local attr = self:GetAttribute()
	local l, t, r, b = self:GetObjPos()
	local button = self:GetControlObject("button")
	if x >= 0 and y >= 0 and x <= r-l and y <= b-t and attr.lbuttondown then
		self:SetCheck()
	end
	attr.lbuttondown = false
	button:SetState(0)
end

function CheckBoxOnInitControl(self)
	local attr = self:GetAttribute()
	self:SetCheck(attr.Select, true)
	local button = self:GetControlObject("button")
	button:SetObjPos2(0, "(father.height-"..attr.ImgHeight..")/2", attr.ImgWidth, attr.ImgHeight)
	local text = self:GetControlObject("text")
	text:SetText(attr.Text)
	--if tFunHelper.IsUACOS() then
	--	text:SetTextFontResID("font.text12.xp")
	--end
	local nNeedLen = text:GetTextExtent()
	text:SetObjPos2(attr.ImgWidth+attr.Space, 0, nNeedLen, "father.height")
	self:SetCursorID("IDC_HAND")
end

