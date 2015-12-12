local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local Helper = XLGetGlobal("Helper")
local tipUtil = tFunHelper.tipUtil

local OnLayoutChangeCookieEx = false
--strYearMonth：年月查询
function ShowClndrContent(objRootCtrl, tClndrContent, strYearMonth)
	if type(tClndrContent) ~= "table" then
		return
	end
	
	for nIndex, tContent in ipairs(tClndrContent) do
		local strKey = "ClndrItem_"..tostring(nIndex)
		local objCurItem = objRootCtrl:GetControlObject(strKey)
		if objCurItem then
			SetItemText(objCurItem, tContent)
			SetTextColor(objCurItem, tContent, strYearMonth)
			SetVacBackGround(objCurItem, tContent)   --放假或上班的背景
			
			objCurItem:SetContent(tContent)
		end		
	end
	
	OnLayoutChangeCookieEx = OnLayoutChangeCookieEx or Helper:AddListener("OnLayoutChange", function(_, _, curViewIndex)
		if 1 ~= curViewIndex then
			return
		end
		for nIndex, tContent in ipairs(tClndrContent) do
			local strKey = "ClndrItem_"..tostring(nIndex)
			local objCurItem = objRootCtrl:GetControlObject(strKey)
			if objCurItem then
				SetVacBackGround(objCurItem, tContent)   --放假或上班的背景
			end
		end
	end)
	ClearFocusDay(objRootCtrl)
	ClearSelectBkg(objRootCtrl)
	
	local nFocusDayIdxInMonth = tFunHelper.GetFocusDayIdxInMonth(tClndrContent, strYearMonth)
	tFunHelper.UpdateBackTodayStyle(nFocusDayIdxInMonth)
	if nFocusDayIdxInMonth == 0 then
		return
	end
	
	--设置当前日为焦点
	SetFocusDay(objRootCtrl, nFocusDayIdxInMonth)   
	
	local objLeftBarCtrl = tFunHelper.GetMainCtrlChildObj("DiDa.CalendarPreView:DiDa.LeftBarCtrl")
	if objLeftBarCtrl then
		objLeftBarCtrl:SetClndrInfo(tClndrContent[nFocusDayIdxInMonth])
	end
end

function OnInitCalendarCtrl(self)
	-- CreateClndrItemList(self)
	-- InitClndrContent(self)
end

--CalendarItem
function OnClickClndrItem(self)
	local objRootCtrl = self:GetOwnerControl()
	local attr = objRootCtrl:GetAttribute()
	local nLastIndex = attr.SelectItemIndex
	
	if nLastIndex ~= 0 then 
		local strKey = "ClndrItem_"..tostring(nLastIndex)
		local objLastItem = objRootCtrl:GetControlObject(strKey)
		if objLastItem then
			objLastItem:SetSelectState(false)
		end
	end
	
	local nCurIndex = self:GetItemIndex()
	attr.SelectItemIndex = nCurIndex
	if nLastIndex == nCurIndex then
		attr.SelectItemIndex = 0
	end
end

function OnRButtonUpClndrItem(self)
	local attr = self:GetAttribute()
	tClndrContent = attr.tClndrContent
	if type(tClndrContent) ~= "table" then
		return
	end
	local solarcalendar = tClndrContent.solarcalendar
	SetLoseFocusNoHideFlag(true)
	Helper:TryDestroyOldMenu(self, "CalendarMenu", 0, true)
	Helper:CreateAndShowMenu(self, "CalendarMenu", 0, true)
	SetLoseFocusNoHideFlag(false)
	-- XLMessageBox(str)
end

function InitClndrContent(objRootCtrl)
	local strCurYearMonth = os.date("%Y%m")
	objRootCtrl:ShowClndrContent(strCurYearMonth)
end

function CreateClndrItemList(objRootCtrl, nLineNum, nColNum, strTemplateID)
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local objFather = objRootCtrl:GetControlObject("CalendarCtrl.Container")
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")	
	
	local ClndrTemplate = templateMananger:GetTemplate(strTemplateID, "ObjectTemplate")
	if ClndrTemplate == nil then
		return nil
	end
		
	for i=1, nLineNum do
		for j=1, nColNum do
			local nIndex = nColNum*(i-1) + j
			local strKey = "ClndrItem_"..tostring(nIndex)
			
			local objClndrItem = ClndrTemplate:CreateInstance( strKey )
			-- local objClndrItem = objFactory:CreateUIObject(strKey, "CalendarItem")
			objFather:AddChild(objClndrItem)
			
			SetClndrItemPos(objRootCtrl, objClndrItem, i, j)
			objClndrItem:SetItemIndex(nIndex)
			objClndrItem:SetCHNDayPos()
			objClndrItem:AttachListener("OnClick", false, OnClickClndrItem)
			objClndrItem:AttachListener("OnRButtonUp", false, OnRButtonUpClndrItem)
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
	local nNewTop = (nLine-1)*(nHeight+nSpanH)
	
	tFunHelper.TipLog("nWidth: "..tostring(nWidth))
	objClndrItem:SetObjPos2(nNewLeft, nNewTop, nWidth, nHeight)
end

function SetItemText(objCurItem, tClndrItem)
	local strSolarcalendar = tClndrItem.solarcalendar
	local _, _, strDay = string.find(strSolarcalendar, "(%d%d)$")
	local nDay = tonumber(strDay)
	strDay = string.format("%1d", tostring(nDay))
	objCurItem:SetDayText(strDay)
	
	local strCDay = tClndrItem.cday
	objCurItem:SetCHNDayText(strCDay)
	
	local nSpecialdayType, strSpecialText = CheckIsSpecialday(tClndrItem)
	if nSpecialdayType then  --节日、节气
		objCurItem:SetCHNDayText(strSpecialText)
	end
end

function SetTextColor(objCurItem, tClndrItem, strYearMonth)
	local bIsInCurMonth = tFunHelper.CheckIsInMonth(tClndrItem, strYearMonth)
	if not bIsInCurMonth then  --不在当月， 灰化
		objCurItem:SetAllTextGray()
		return
	end
	
	objCurItem:SetAllTextNormal()
	
	local bIsWeekend = CheckIsWeekend(tClndrItem)
	if bIsWeekend then  --周末
		objCurItem:SetAllTextWeekend()
	end
	
	local nSpecialdayType, strSpecialText = CheckIsSpecialday(tClndrItem)
	if nSpecialdayType then  
		objCurItem:SetTextSpecialday()  --公历节日
		
		if nSpecialdayType == 1 or nSpecialdayType == 2 
			or strSpecialText == "元旦" then  --农历节、节气
			objCurItem:SetCHNTextTermDay()
		end		
	end
end

function SetVacBackGround(objCurItem, tClndrItem)	
	objCurItem:SetVacationBkg(false)
	objCurItem:SetWorkBkg(false)
	
	local strDate = tClndrItem.solarcalendar
	local wndTree = objCurItem:GetOwner()
	local notepadPreView = wndTree:GetUIObject("root.layout:root.ctrl:DiDa.NotePadPreView")
	local curDayNoteCount = notepadPreView:CheckDayRecord(strDate)
	if curDayNoteCount > 0 then
		objCurItem:SetHasItem(true)
	else
		objCurItem:SetHasItem(false)
	end
	
	local bIsVacation = tFunHelper.CheckIsVacation(strDate)
	if bIsVacation then
		objCurItem:SetVacationBkg(true)
		return
	end
	
	local bIsVacation = tFunHelper.CheckIsWorkDay(strDate)
	if bIsVacation then
		objCurItem:SetWorkBkg(true)
		return
	end
end

function CheckIsWeekend(tClndrItem)	
	local strWeekday = tClndrItem.weekday
	if tostring(strWeekday) == "六" 
	or tostring(strWeekday) == "日"  then
		return true
	end
	
	return false
end

function CheckIsSpecialday(tClndrItem)
	if IsRealString(tClndrItem.choliday) then  --农历节
		return 1, tClndrItem.choliday
	end
	
	if IsRealString(tClndrItem.sterm) then   --节气
		return 2, tClndrItem.sterm
	end
	
	if IsRealString(tClndrItem.holiday) then   --公历节
		return 3, tClndrItem.holiday
	end
	
	return false
end


local OnLayoutChangeCookie = nil
local OnMainWndShowCookie = nil
local mainWnd = nil

function ClearFocusDay(objRootCtrl)
	local attr = objRootCtrl:GetAttribute()
	local nFocusDayIndex = attr.FocusDayIndex
	
	if nFocusDayIndex ~= 0 then
		local strKey = "ClndrItem_"..tostring(nFocusDayIndex)
		local objLastItem = objRootCtrl:GetControlObject(strKey)
		if objLastItem then
			if OnLayoutChangeCookie then
				Helper:RemoveListener("OnLayoutChange", OnLayoutChangeCookie)
				OnLayoutChangeCookie = nil
			end
			if OnMainWndShowCookie then
				mainWnd:RemoveListener("OnShowWindow", OnMainWndShowCookie)
				OnMainWndShowCookie = nil
			end
		
			objLastItem:SetCurrentDayBkg(false)
		end
	end
end

function SetFocusDay(objRootCtrl, nFocusDayIdx)
	if nFocusDayIdx == 0 then
		return
	end	
	local attr = objRootCtrl:GetAttribute()
	local strKey = "ClndrItem_"..tostring(nFocusDayIdx)
	local objCurItem = objRootCtrl:GetControlObject(strKey)
	if objCurItem then	
		if not OnLayoutChangeCookie then
			OnLayoutChangeCookie = Helper:AddListener("OnLayoutChange", function() objCurItem:SetCurrentDayBkg(true) end)
		end
		if not OnMainWndShowCookie then
			local ownerTree = objRootCtrl:GetOwner()
			mainWnd = ownerTree:GetBindHostWnd()
			OnMainWndShowCookie = mainWnd:AttachListener("OnShowWindow", false, function(_, bShow)
					if bShow then
						objCurItem:SetCurrentDayBkg(true)
					end
			end)
		end
		objCurItem:SetCurrentDayBkg(true)
	end
	
	attr.FocusDayIndex = nFocusDayIdx
end

function ClearSelectBkg(objRootCtrl)
	local attr = objRootCtrl:GetAttribute()
	local nIndex = attr.SelectItemIndex
	
	if nIndex ~= 0 then
		local strKey = "ClndrItem_"..tostring(nIndex)
		local objLastItem = objRootCtrl:GetControlObject(strKey)
		if objLastItem then
			objLastItem:SetSelectState(false)
		end
	end
end

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

