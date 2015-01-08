local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil


--方法

function SetClndrInfo(self)

end



---事件
function OnInitLeftBar(self)
	SetCurrentTime(self)
	StartTimer(self)
end



------------------

function SetCurrentTime(objRootCtrl)
	local tDateInfo = os.date("*t")
	if type(tDateInfo) ~= "table" then
		return
	end
	
	local strClockText = string.format("%02d:%02d:%02d", tDateInfo.hour, tDateInfo.min, tDateInfo.sec)
	SetClockText(objRootCtrl, strClockText)
	
	local strWeekText = GetCHNWeekText(tDateInfo.wday)
	SetWeekText(objRootCtrl, strWeekText)
	
	local strDateText = string.format("%02d", tDateInfo.day)
	SetDateText(objRootCtrl, strDateText)
	
	local strYearText = string.format("%d/%02d", tDateInfo.year, tDateInfo.month)
	SetYearText(objRootCtrl, strYearText)
end


function StartTimer(objRootCtrl)
	local timeMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
	timeMgr:SetTimer(function(Itm, id)
			SetCurrentTime(objRootCtrl)
		end, 1000)
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


function SetClockText(self, strText)
	local objClock = self:GetControlObject("LeftBar.ClockText")
	if objClock and IsRealString(strText) then
		objClock:SetText(strText) 
	end
end


function SetWeekText(self, strText)
	local objWeek = self:GetControlObject("LeftBar.Week")
	if objWeek and IsRealString(strText) 
		and objWeek:GetText() ~= strText then
		
		objWeek:SetText(strText) 
	end
end


function SetDateText(self, strText)
	local objDate = self:GetControlObject("LeftBar.Date")
	if objDate and IsRealString(strText) 
		and objDate:GetText() ~= strText then
		
		objDate:SetText(strText) 
	end
end


function SetYearText(self, strText)
	local objYear = self:GetControlObject("LeftBar.Year")
	if objYear and IsRealString(strText) 
		and objYear:GetText() ~= strText then
		
		objYear:SetText(strText) 
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