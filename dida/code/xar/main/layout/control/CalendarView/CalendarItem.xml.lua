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
	 SetAllTextColorRes(self, "262624", "999999")
end


function SetAllTextGray(self)
	 SetAllTextColorRes(self, "CCCCCC", "CCCCCC")
end

function SetAllTextWeekend(self)
	 SetAllTextColorRes(self, "orange.weekend", "999999")
end


function SetTextSpecialday(self)
	SetAllTextColorRes(self, "", "FF4A50")
end


function SetCHNTextTermDay(self)
	SetAllTextColorRes(self, "", "FF4A50")
end

local bSeqFrameAniEnd = true

function SetCurrentDayBkg(self, bShowBkg)
	local bIsWorkDay = CheckIsWorkDay(self)
	local objCurDayImg = self:GetControlObject("Calendar.Current")
	
	if bIsWorkDay then
		objCurDayImg:SetResID("DiDa.Canlender.Current.Work.Bitmap")
	else
		-- objCurDayImg:SetTextureID("DiDa.Canlender.Current")
		--
		if bSeqFrameAniEnd then--上次的动画已经做完了
			bSeqFrameAniEnd = false
			tFunHelper.RunSeqFrameAni(objCurDayImg, "DiDa.seq_ani", function() bSeqFrameAniEnd = true end, 1200, false)
		end
	end
	
	objCurDayImg:SetVisible(bShowBkg)

	if bShowBkg and not bIsWorkDay then
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
	objImg:SetChildrenVisible(bShowBkg)
end

function SetWorkBkg(self, bShowBkg)
	local objImg = self:GetControlObject("Calendar.Work")
	objImg:SetVisible(bShowBkg)
	objImg:SetChildrenVisible(bShowBkg)
end


function SetCHNDayPos(objRootCtrl)
	local objDayText = objRootCtrl:GetControlObject("Calendar.Day")
	local objCHNDayText = objRootCtrl:GetControlObject("Calendar.CHNDay")
	
	local DayL, DayT, DayR, DayB = objDayText:GetObjPos()
	local CHNDayL, CHNDayT, CHNDayR, CHNDayB = objCHNDayText:GetObjPos()
	local CHNDayH = 15
	local CHNDayNewT = DayB/2+10
	
	objCHNDayText:SetObjPos(CHNDayL, CHNDayNewT, CHNDayR, CHNDayNewT+CHNDayH)
end


---事件
function OnInitControl(self)
	InitSelectImage(self)
end

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
	
	local objLeftBarCtrl = tFunHelper.GetMainCtrlChildObj("DiDa.CalendarPreView:DiDa.LeftBarCtrl") 
	objLeftBarCtrl:SetClndrInfo(tClndrContent)	
	
	self:SetSelectState(true)
	self:FireExtEvent("OnClick")
end


----
function InitSelectImage(objRootCtrl)
	local attr = objRootCtrl:GetAttribute()
	local SelectBkgResID = attr.SelectBkgResID
	
	local objSelectImg = objRootCtrl:GetControlObject("Calendar.Select")
	if IsRealString(SelectBkgResID) then
		objSelectImg:SetResID(SelectBkgResID)
	end
end

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


function CheckIsWorkDay(objRootCtrl)
	local attr = objRootCtrl:GetAttribute()
	local tContent = attr.tClndrContent
	if type(tContent) ~= "table" then
		return false
	end
	
	local strDate = tContent.solarcalendar
	return tFunHelper.CheckIsWorkDay(strDate)
end
------------------

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

