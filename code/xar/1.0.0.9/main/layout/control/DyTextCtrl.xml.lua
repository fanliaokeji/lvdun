function OnInitControl(self)
	local objText = self:GetControlObject("dynamictext")
	local attr = self:GetAttribute()

	objText:SetText(attr.Text)
	objText:SetTextColorResID(attr.Textcolor)
	objText:SetTextFontResID(attr.Font)
	objText:SetVAlign("center")	
	objText:SetHAlign("center")	
end


function OnMouseEnter(self)
	local objText = self:GetControlObject("dynamictext")
	if IsTextOverFlow(self) then
		ShowFullText(objText)
		SlideText(self)
	end
	
	local attr = self:GetAttribute()
	if IsRealString(attr.FontUnderline) then
		objText:SetTextFontResID(attr.FontUnderline)
	end
end


function OnMouseLeave(self)
	local attr = self:GetAttribute()
	local timeMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
	timeMgr:KillTimer(attr.Timer)
	
	local objText = self:GetControlObject("dynamictext")
	if IsTextOverFlow(self) then
		ShowPartText(objText)
	end
	
	local attr = self:GetAttribute()
	objText:SetTextFontResID(attr.Font)
end


function SetText(self, strText, bSlide)
	local objText = self:GetControlObject("dynamictext")
	objText:SetText(strText)	
	
	if IsTextOverFlow(self) then
		--SetTextLeft(objText)
		
		if bSlide then
			SlideText(self)
		end
	end
end


function ShowFullText(objText)
	local objRootCtrl = objText:GetOwnerControl()
	objRootCtrl:SetLimitChild(true)
	
	local nSuitWidth = objText:GetTextExtent()
	local nLeft, nTop, nRight, nBottom = objText:GetObjPos()
	objText:SetObjPos(0, nTop, nSuitWidth, nBottom)
end

function ShowPartText(objText)
	local objRootCtrl = objText:GetOwnerControl()
	objRootCtrl:SetLimitChild(false)
	
	local nLeft, nTop, nRight, nBottom = objRootCtrl:GetObjPos()
	objText:SetObjPos(0, 0, nRight-nLeft, nBottom-nTop)
	objText:SetVAlign("center")	
	objText:SetHAlign("center")
end


function SetTextColor(self, strColorResID)
	local objText = self:GetControlObject("dynamictext")
	objText:SetTextColorResID(strColorResID)
end


function IsTextOverFlow(self)
	local objText = self:GetControlObject("dynamictext")
	local objLayout = self:GetControlObject("dylayout")
	local nLeft, nTop, nRight, nBottom = objLayout:GetObjPos()
	local nWidth = nRight - nLeft
	local nSuitWidth = objText:GetTextExtent()
	
	if nWidth < nSuitWidth then
		return true
	else
		return false
	end
end


function SlideText(self)
	local objText = self:GetControlObject("dynamictext")
	local objLayout = self:GetControlObject("dylayout")
	local timeMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
	local attr = self:GetAttribute()
	local nTimeSpan = tonumber(attr.timespanms)
	local nTextLeft, nTextTop, nTextRight, nTextBottom = objText:GetObjPos()
	local nLayoutLeft, nLayoutTop, nLayoutRight, nLayoutBottom = objLayout:GetObjPos()
	local nTextWidth = nTextRight - nTextLeft
	local nLayoutWidth = nLayoutRight - nLayoutLeft
	
	local attr = self:GetAttribute()
	local nDiff = 5
	local nShowLeft = nTextLeft
	attr.Timer = timeMgr:SetTimer(function(Itm, id)
		objText:SetObjPos(nShowLeft, nTextTop, nShowLeft+nTextWidth, nTextBottom)
		nShowLeft = nShowLeft - nDiff
		
		if nShowLeft+nTextWidth <= nLayoutWidth-5 then
			nShowLeft = nTextLeft
		end
	end, nTimeSpan)
end


function IsRealString(str)
	return type(str) == "string" and str~=nil
end


