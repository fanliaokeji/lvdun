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
	local objDayText = self:GetControlObject("Calendar.Day")
	local objCHNDayText = self:GetControlObject("Calendar.CHNDay")
	
	objDayText:SetTextColorResID("4D4D4D")
	objCHNDayText:SetTextColorResID("808080")
end


function SetAllTextGray(self)
	local objDayText = self:GetControlObject("Calendar.Day")
	local objCHNDayText = self:GetControlObject("Calendar.CHNDay")
	
	objDayText:SetTextColorResID("CCCCCC")
	objCHNDayText:SetTextColorResID("CCCCCC")
end


function SetAllTextWeekend(self)
	local objDayText = self:GetControlObject("Calendar.Day")
	local objCHNDayText = self:GetControlObject("Calendar.CHNDay")
	
	objDayText:SetTextColorResID("59ACFF")
	objCHNDayText:SetTextColorResID("59ACFF")
end


function SetTextHoliday(self)
	local objCHNDayText = self:GetControlObject("Calendar.CHNDay")
	objCHNDayText:SetTextColorResID("FF9933")
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
	 
	
	
end


------------------



function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

