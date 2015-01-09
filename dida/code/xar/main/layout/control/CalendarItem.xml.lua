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


function SetTextHoliday(self)
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


---事件
function OnMouseEnter(self)
	local objSelectImg = self:GetControlObject("Calendar.Select")
	objSelectImg:SetVisible(true)
end

function OnMouseLeave(self)
	local objSelectImg = self:GetControlObject("Calendar.Select")
	objSelectImg:SetVisible(false)
end

function OnLButtonUp(self)
	local attr = self:GetAttribute()
	local tClndrContent = attr.tClndrContent
	if type(tClndrContent) ~= "table" then
		return
	end
	
	local objLeftBarCtrl = tFunHelper.GetMainCtrlChildObj("DiDa.LeftBarCtrl") 
	objLeftBarCtrl:SetClndrInfo(tClndrContent)	
end


----
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

