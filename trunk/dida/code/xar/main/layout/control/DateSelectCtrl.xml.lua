local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil


--方法--
function GetYearText(self)
	local objYearBox = self:GetControlObject("Combobox.Year")
	local strYear = objYearBox:GetText()	
	local _, _, strYearText = string.find(strYear, "(%d*)[^%d]*")
	return strYearText
end

function GetMonthText(self)
	local objMonthBox = self:GetControlObject("Combobox.Month")
	local strMonth = objMonthBox:GetText()	
	local _, _, strMonthText = string.find(strMonth, "(%d*)[^%d]*")
	return strMonthText
end



---事件
function OnInitRootCtrl(self)
		
	
end


--年份
local g_bYearBoxEnter = false
local g_bNotShow = false

function OnLButtonUpYearBox(self)
	-- if g_bNotShow then
		-- g_bNotShow = false
		-- return
	-- end

	local nTopSpan = 21
	tFunHelper.TryDestroyOldMenu(self, "YearMenu")
	tFunHelper.CreateAndShowMenu(self, "YearMenu", nTopSpan)
	
	if g_bYearBoxEnter then
		g_bNotShow = true
	end
end


function OnInitYearBox(self)
	local nCurYear = os.date("%Y")
	local strText = tostring(nCurYear)..tostring("年")
	self:SetText(strText)
end

function OnMouseEnterYearBox(self)
	g_bYearBoxEnter = true
end

function OnMouseLeaveYearBox(self)
	g_bYearBoxEnter = false
end


--月份
function OnLButtonUpMonthBox(self)
	local nTopSpan = 21
	tFunHelper.TryDestroyOldMenu(self, "MonthMenu")
	tFunHelper.CreateAndShowMenu(self, "MonthMenu", nTopSpan)
end


function OnInitMonthBox(self)
	local nCurMonth = os.date("%m")
	local strMonth = string.format("%1d", nCurMonth)
	local strText = tostring(strMonth)..tostring("月")
	self:SetText(strText)
end


--假期
function OnLButtonUpFestivalBox(self)
	local nTopSpan = 21
	tFunHelper.TryDestroyOldMenu(self, "FestivalMenu")
	tFunHelper.CreateAndShowMenu(self, "FestivalMenu", nTopSpan)
end


function OnInitFestivalBox(self)
	self:SetText("假期安排")
end


--返回今天
function OnLClickToday(self)
	
end



------------------

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end