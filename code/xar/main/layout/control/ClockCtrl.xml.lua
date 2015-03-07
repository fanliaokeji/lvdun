local tipUtil = XLGetObject("GS.Util")
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")

-----事件----
function OnInitControl(self)
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local nCurFiltTime = tUserConfig["nFiltVideoTimeSec"] or 0
	self:SetTime(nCurFiltTime)
end


-----方法-----
function SetTime(self, nTimeInSec)
	local nHour = math.floor(nTimeInSec/3600)
	local nLeft = math.mod(nTimeInSec, 3600)
	local nMin = math.floor(nLeft/60)
	local nSec = math.floor(math.mod(nLeft, 60))
	
	local objHour = self:GetControlObject("ClockCtrl.Hour")
	local objMin = self:GetControlObject("ClockCtrl.Min")
	local objSec = self:GetControlObject("ClockCtrl.Sec")
	
	local strHour = tostring(nHour)
	if nHour < 100 then
		strHour = string.format("%02d", nHour)
	end
	local strMin = string.format("%02d", nMin)
	local strSec = string.format("%02d", nSec)
	
	objHour:SetText(tostring(strHour))
	objMin:SetText(tostring(strMin))
	objSec:SetText(tostring(strSec))
	
	AdjustLayoutPos(self)
end



function AdjustLayoutPos(self)
	local objHour = self:GetControlObject("ClockCtrl.Hour")
	local objLayout = self:GetControlObject("ClockCtrl.Layout")
	
	local nHourL, nHourT, nHourR, nHourB = objHour:GetObjPos()
	local nRealWidth = objHour:GetTextExtent()
	local nNewHourR = nHourL+nRealWidth
	objHour:SetObjPos(nHourL, nHourT, nNewHourR, nHourB)
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objLayout:GetObjPos()
	local nLayoutWidth = nLayoutR-nLayoutL
	objLayout:SetObjPos(nNewHourR, nHourT, nNewHourR+nLayoutWidth, nHourB)
end


-----辅助函数----
function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
end

