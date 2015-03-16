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
	InitLineContainer(objCalendar, 6)
end


function InitSmallCalendar(self)
	local objCalendar = self:GetControlObject("CalendarCtrl.Small")
	objCalendar:CreateClndrItemList(5, 7, "CalendarItem.Small")
	InitLineContainer(objCalendar, 5)
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


function InitLineContainer(objContainer, nLineCount)
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")	
	local nHeight = GetLineSpan(objContainer)
	local strFatherID = objContainer:GetID()
	
	local LineTemplate = templateMananger:GetTemplate("LineTemplate", "ObjectTemplate")
	if LineTemplate == nil then
		return nil
	end
	
	for i=1, nLineCount do
		local strKey = strFatherID.."_Line"..tostring(i)
		local objLine = LineTemplate:CreateInstance( strKey )
		
		objContainer:AddChild(objLine)
		local nTop = nHeight*(i-1)

		objLine:SetLinePoint(1, nTop, "father.width", nTop+5)
		local l, t, r, b = objContainer:GetObjPos()
	end
end


function GetLineSpan(objContainer)
	local attr = objContainer:GetAttribute()
	return attr.ItemHeight
end

------------------

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

