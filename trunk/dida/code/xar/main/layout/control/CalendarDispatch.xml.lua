local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

--方法  

--strYearMonth：年月查询
function ShowCalendarInfo(objRootCtrl, strYearMonth)
	function ProcessCalendar(tClndrContent)
		if type(tClndrContent) ~= "table" then
			return
		end
		
		local tClndrItem = tClndrContent[36]
		if type(tClndrItem) ~= "table" then
			ShowBigCalendar(objRootCtrl, tClndrContent, strYearMonth)
			return
		end
		
		local bIsInCurMonth = tFunHelper.CheckIsInMonth(tClndrItem, strYearMonth)
		if bIsInCurMonth then
			ShowBigCalendar(objRootCtrl, tClndrContent, strYearMonth)
		else
			ShowSmallCalendar(objRootCtrl, tClndrContent, strYearMonth)
		end
	end
	
	tFunHelper.GetClndrContent(strYearMonth, ProcessCalendar)
end


---事件
function OnInitControl(self)
	InitBigCalendar(self)
	InitSmallCalendar(self)
	
	local strCurYearMonth = os.date("%Y%m")
	ShowCalendarInfo(self, strCurYearMonth)
end

------
function InitBigCalendar(self)
	local objCalendar = self:GetControlObject("CalendarCtrl.Big")
	objCalendar:CreateClndrItemList(6, 7, "CalendarItem.Big")
end


function InitSmallCalendar(self)
	local objCalendar = self:GetControlObject("CalendarCtrl.Small")
	objCalendar:CreateClndrItemList(5, 7, "CalendarItem.Small")
end

function ShowBigCalendar(objRootCtrl, tClndrContent, strYearMonth)
	local objBigCalendar = objRootCtrl:GetControlObject("CalendarCtrl.Big")
	local objSmallCalendar = objRootCtrl:GetControlObject("CalendarCtrl.Small")
		
	objBigCalendar:SetVisible(true)
	objBigCalendar:SetChildrenVisible(true)
	objSmallCalendar:SetVisible(false)
	objSmallCalendar:SetChildrenVisible(false)
	
	objBigCalendar:ShowClndrContent(tClndrContent, strYearMonth)
end


function ShowSmallCalendar(objRootCtrl, tClndrContent, strYearMonth)
	local objBigCalendar = objRootCtrl:GetControlObject("CalendarCtrl.Big")
	local objSmallCalendar = objRootCtrl:GetControlObject("CalendarCtrl.Small")
		
	objBigCalendar:SetVisible(false)
	objBigCalendar:SetChildrenVisible(false)
	objSmallCalendar:SetVisible(true)
	objSmallCalendar:SetChildrenVisible(true)
	
	objSmallCalendar:ShowClndrContent(tClndrContent, strYearMonth)
end


------------------

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

