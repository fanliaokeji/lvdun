local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

--方法
function ShowClndrContent(objRootCtrl, strYearMonth)
	local tClndrContent = tFunHelper.GetClndrContent(strYearMonth)
	
	for nIndex, tContent in ipairs(tClndrContent) do
		local strKey = "ClndrItem_"..tostring(nIndex)
		local objCurItem = objRootCtrl:GetControlObject(strKey)
		if objCurItem then
			SetItemText(objCurItem, tContent)
			SetTextColor(objCurItem, tContent)
			SetBackGround(objCurItem, tContent)
			
			objCurItem:SetContent(tContent)
		end		
	end
end



---事件
function OnInitCalendarCtrl(self)
	CreateClndrItem(self)
	InitClndrContent(self)
end


------
function CreateClndrItem(objRootCtrl)
	local nLineNum = 6
	local nColNum = 7
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local objFather = objRootCtrl:GetControlObject("CalendarCtrl.Container")
	
	for i=1, nLineNum do
		for j=1, nColNum do
			local nIndex = nColNum*(i-1) + j
			local strKey = "ClndrItem_"..tostring(nIndex)
			local objClndrItem = objFactory:CreateUIObject(strKey, "CalendarItem")
			objFather:AddChild(objClndrItem)
			
			SetClndrItemPos(objRootCtrl, objClndrItem, i, j)
		end	
	end
end


function SetClndrItemPos(objRootCtrl, objClndrItem, nLine, nCol)
	local attr = objRootCtrl:GetAttribute()
	local nWidth = attr.ItemWidth
	local nHeight = attr.ItemHeight
	local nSpanW = attr.ItemSpanW
	local nSpanH = attr.ItemSpanH
	
	local nNewLeft = (nCol-1)*(nWidth+nSpanW)
	local nNewTop = (nLine-1)*(nHeight+nSpanH)+nLine-1
	if nLine > 2 then
		nNewTop = (nLine-1)*(nHeight+nSpanH)+nLine-2 --微调
	end
	
	objClndrItem:SetObjPos(nNewLeft, nNewTop, nNewLeft+nWidth, nNewTop+nHeight)
end


function InitClndrContent(objRootCtrl)
	local strCurYearMonth = os.date("%Y%m")
	objRootCtrl:ShowClndrContent(strCurYearMonth)
end


function SetItemText(objCurItem, tClndrItem)
	local strSolarcalendar = tClndrItem.solarcalendar
	local _, _, strDay = string.find(strSolarcalendar, "(%d%d)$")
	local nDay = tonumber(strDay)
	strDay = string.format("%1d", tostring(nDay))
	objCurItem:SetDayText(strDay)
	
	local strCDay = tClndrItem.cday
	objCurItem:SetCHNDayText(strCDay)
end


function SetTextColor(objCurItem, tClndrItem)
	local bIsInCurMonth = CheckIsInCurMonth(tClndrItem)
	if not bIsInCurMonth then  --不在当月， 灰化
		objCurItem:SetAllTextGray()
		return
	end
	
	local bIsWeekend = CheckIsWeekend(tClndrItem)
	if bIsWeekend then  --周末
		objCurItem:SetAllTextWeekend()
		return
	end
	
	objCurItem:SetAllTextNormal()
	
	local bIsHoliday = CheckIsHoliday(tClndrItem)
	if bIsHoliday then  --节日
		objCurItem:SetTextHoliday()
	end
end


function SetBackGround(objCurItem, tClndrItem)
	

end

			
function CheckIsInCurMonth(tClndrItem)		
	return true
end

function CheckIsWeekend(tClndrItem)		
	return false
end

function CheckIsHoliday(tClndrItem)		
	return false
end


------------------

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

