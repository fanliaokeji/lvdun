local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

--方法
function SetDayText(self, strText)
	if not IsRealString(strText) then
		return
	end

	local objText = self:GetControlObject("Calendar.Day")
	objText:SetText(strText)
end


function SetCHNDayText(self, strText)
	if not IsRealString(strText) then
		return
	end

	local objText = self:GetControlObject("Calendar.CHNDay")
	objText:SetText(strText)
end


function SetAllTextNormal(self)
	 SetAllTextColorRes(self, "4D4D4D", "808080")
end


function SetAllTextGray(self)
	 SetAllTextColorRes(self, "CCCCCC", "CCCCCC")
end

function SetAllTextWeekend(self)
	 SetAllTextColorRes(self, "59ACFF", "59ACFF")
end


function SetTextSpecialday(self)
	 SetAllTextColorRes(self, "", "FF9933")
end


function SetCurrentDayBkg(self, bShowBkg)
	local objCurDayImg = self:GetControlObject("Calendar.Current")
	objCurDayImg:SetVisible(bShowBkg)

	if bShowBkg then
		SetAllTextColorRes(self, "system.white", "system.white")
	end
end

function SetContent(self, tClndrContent)
	local attr = self:GetAttribute()
	attr.tClndrContent = tClndrContent
end


function SetSelectState(self, bSelect)
	local attr = self:GetAttribute()
	attr.bSelect = bSelect
	ShowSelectImage(self, bSelect)
end


function GetItemIndex(self)
	local attr = self:GetAttribute()
	return attr.nItemIndex
end

function SetItemIndex(self, nIndex)
	local attr = self:GetAttribute()
	attr.nItemIndex = nIndex
end

function SetVacationBkg(self, bShowBkg)
	local objImg = self:GetControlObject("Calendar.Vacation")
	objImg:SetVisible(bShowBkg)
end

function SetWorkBkg(self, bShowBkg)
	local objImg = self:GetControlObject("Calendar.Work")
	objImg:SetVisible(bShowBkg)
end


---事件
function OnMouseEnter(self)
	ShowSelectImage(self, true)
end

function OnMouseLeave(self)
	local attr = self:GetAttribute()
	if attr.bSelect then
		return
	end
	ShowSelectImage(self, false)
end

function OnLButtonUp(self)
	local attr = self:GetAttribute()
	local tClndrContent = attr.tClndrContent
	if type(tClndrContent) ~= "table" then
		return
	end
	
	local objLeftBarCtrl = tFunHelper.GetMainCtrlChildObj("DiDa.LeftBarCtrl") 
	objLeftBarCtrl:SetClndrInfo(tClndrContent)	
	
	self:SetSelectState(true)
	self:FireExtEvent("OnClick")
end


----
function ShowSelectImage(objRootCtrl, bSelect)
	local objSelectImg = objRootCtrl:GetControlObject("Calendar.Select")
	objSelectImg:SetVisible(bSelect)
end

function SetAllTextColorRes(self, strDayClr, strCHNDayClr)
	local objDayText = self:GetControlObject("Calendar.Day")
	local objCHNDayText = self:GetControlObject("Calendar.CHNDay")
	
	if IsRealString(strDayClr) then
		objDayText:SetTextColorResID(strDayClr)
	end
	
	if IsRealString(strCHNDayClr) then
		objCHNDayText:SetTextColorResID(strCHNDayClr)
	end
end



------------------

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

