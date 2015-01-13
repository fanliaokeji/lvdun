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


function SetYearText(self, strYear)
	local objYearBox = self:GetControlObject("Combobox.Year")
	objYearBox:SetText(strYear)
end

function SetMonthText(self, strMonth)
	local objMonthBox = self:GetControlObject("Combobox.Month")
	objMonthBox:SetText(strMonth)
end

function SetFestivalText(self, strFestival)
	local objFestival = self:GetControlObject("Combobox.Festival")
	objFestival:SetText(strFestival)
end


function ResetFestivalText(self)
	local objFestival = self:GetControlObject("Combobox.Festival")
	local attr = objFestival:GetAttribute()
	attr.LeftTextPos = 5
	
	objFestival:SetText("假期安排")
end


---事件
function OnInitRootCtrl(self)
	CreateTimeListener(self)
	self:ResetFestivalText()
end


--年份
local g_nYearTimer = nil
local g_bShowYearMenu = false

function OnFocusYearDropList(self, bFocus)
	if bFocus then
		return
	end

	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	local mouseX, mouseY = tipUtil:GetCursorPos()
	local nWndX, nWndY = objHostWnd:ScreenPtToHostWndPt(mouseX, mouseY)
	local nTreeX, nTreeY = objHostWnd:HostWndPtToTreePt(nWndX, nWndY)

	local nLeft, nTop, nRight, nBottom = self:GetAbsPos()

	if nTreeX > nLeft-5 and nTreeX < nRight+5 and nTreeY < nBottom+5 and nTreeY > nTop-25 then	
		self:SetFocus(true)
		return
	end
	
	self:SetVisible(false)
	self:SetChildrenVisible(false)
	
	local objMenuContext = self:GetControlObject("Menu.Context")
	objMenuContext:SetVisible(false)
	objMenuContext:SetChildrenVisible(false)
end


function OnInitYearDropList(self)
	self:SetVisible(false)
	self:SetChildrenVisible(false)
end


function OnLButtonUpYearBox(self)
	ProcessButtonUp(self, "DropList.Year")
end


function OnInitYearBox(self)
	local nCurYear = os.date("%Y")
	local strText = tostring(nCurYear)..tostring("年")
	self:SetText(strText)
end



--月份
local g_nMonthTimer = nil
local g_bShowMonthMenu = false
function OnLButtonUpMonthBox(self)
	ProcessButtonUp(self, "DropList.Month")	
end


function OnInitMonthBox(self)
	local nCurMonth = os.date("%m")
	local strMonth = string.format("%1d", nCurMonth)
	local strText = tostring(strMonth)..tostring("月")
	self:SetText(strText)
end


--假期
function OnLButtonUpFestivalBox(self)
	ProcessButtonUp(self, "DropList.Festival")
end


--返回今天
function OnLClickToday(self)
	local objRootCtrl = self:GetOwnerControl()
	ResetCtrlText(objRootCtrl)
	tFunHelper.UpdateCalendarContent()
end



------------------
function ProcessButtonUp(objUIElem, strUIKey)
	local objRootCtrl = objUIElem:GetOwnerControl()
	local objDropList = objRootCtrl:GetControlObject(strUIKey)
	local objMenuContext = objDropList:GetControlObject("Menu.Context")
	if objDropList:GetVisible() and objMenuContext:GetVisible() then
		objDropList:SetVisible(false)
		objDropList:SetChildrenVisible(false)
		
		objMenuContext:SetVisible(false)
		objMenuContext:SetChildrenVisible(false)
		return
	end
	
	objDropList:SetVisible(true)
	objDropList:SetChildrenVisible(true)
	
	objMenuContext:SetVisible(true)
	objMenuContext:SetChildrenVisible(true)
	
	objDropList:SetFocus(true)
	objDropList:SetDefaultItemHover()
end


function CreateTimeListener(self)
	local objHostWnd = tFunHelper.GetMainWndInst()
	local WM_TIMECHANGE = 0x001E
	local cookie,ret = objHostWnd:AddInputFilter(false, 
		function(hostwnd, msg,wpram,lparam)
		
			if msg == WM_TIMECHANGE then
				ResetCtrlText(self)	
				tFunHelper.UpdateCalendarContent()				
			end
			return 0, false
		end)
end


function ResetCtrlText(objRootCtrl)
	local strYear = os.date("%Y")
	objRootCtrl:SetYearText(strYear.."年")
	
	local strMonth = os.date("%m")
	local nMonth = tonumber(strMonth)
	strMonth = string.format("%1d", nMonth)
	objRootCtrl:SetMonthText(strMonth.."月")

	objRootCtrl:ResetFestivalText()
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