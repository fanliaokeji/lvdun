local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil


--方法
function SetClndrInfo(self, tClndrContent)
	local objRootCtrl = self
	if type(tClndrContent) ~= "table" then
		return
	end

	local strLunarDate = tostring(tClndrContent.cmonth)..tostring(tClndrContent.cday)
	SetTextObjContent(objRootCtrl, "LeftBar.LunarDate", strLunarDate)

	local strChineseYear = tostring(tClndrContent.yearganzhi).."年【"..tostring(tClndrContent.shengxiao).."年】"
	SetTextObjContent(objRootCtrl, "LeftBar.ChineseYear", strChineseYear)
	
	local strChineseDate = tostring(tClndrContent.monthganzhi).."月"..tostring(tClndrContent.dayganzhi).."日"
	SetTextObjContent(objRootCtrl, "LeftBar.ChineseDate", strChineseDate)
	
	local strWeek = "星期"..tostring(tClndrContent.weekday)
	SetTextObjContent(objRootCtrl, "LeftBar.Week", strWeek)

	local _, _, strYear, strMonth, strDay = string.find(tClndrContent.solarcalendar, "(%d%d%d%d)(%d%d)(%d%d)")
	local nMonth = tonumber(strMonth)
	local nDay = tonumber(strDay)
	strMonth = string.format("%02d", nMonth)
	local strYearText = tostring(strYear).."/"..tostring(strMonth)
	SetTextObjContent(objRootCtrl, "LeftBar.Year", strYearText)
	
	strDay = string.format("%02d", nDay)
	SetTextObjContent(objRootCtrl, "LeftBar.Day", strDay)
end



---事件
function OnInitLeftBar(self)
	InitClndrInfo(self)
end


------------------
function InitClndrInfo(objRootCtrl)
	local strCurDate = os.date("%Y%m%d")
	
	tFunHelper.GetClndrContent(strCurDate, 
		function (tClndrContentList)
			if type(tClndrContentList) ~= "table" then
				return
			end
		
			tClndrContent = tClndrContentList[1]
			SetClndrInfo(objRootCtrl, tClndrContent)
		end)
end


--Sunday 是1
function GetCHNWeekText(nWeekNum)
	local tCHNNumMap = {
		[1] = "星期日",
		[2] = "星期一",
		[3] = "星期二",
		[4] = "星期三",
		[5] = "星期四",
		[6] = "星期五",
		[7] = "星期六",
	}

	return tCHNNumMap[nWeekNum]
end


function SetTextObjContent(objRootCtrl, strObjKey, strText)
	local objText = objRootCtrl:GetControlObject(strObjKey)
	if objText and IsRealString(strText) then
		objText:SetText(strText) 
	end
end


function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end