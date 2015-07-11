local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

----方法----
function SetTipData(self, infoTab) 
	CreateFilterListener(self)
	return true
end

function OnLButtonDown(self)
	--四个按钮用这一个方法
	local owner = self:GetOwnerControl()
	local calendarBtn = owner:GetControlObject("calendar.btn")
	local notepadBtn  = owner:GetControlObject("notepad.btn")
	local remindBtn   = owner:GetControlObject("remind.btn")
	local forherBtn   = owner:GetControlObject("forher.btn")
	
	calendarBtn:SetZorder(36000)
	notepadBtn:SetZorder(36000)
	remindBtn:SetZorder(36000)
	forherBtn:SetZorder(36000)
	
	self:SetZorder(60000)
	
end

function HideAllView(self)
	--隐藏所有的preview、view
	local owner = self:GetOwnerControl()
	local calendarPreView = owner:GetControlObject("DiDa.CalendarPreView")
	calendarPreView:SetVisible(false)
	calendarPreView:SetChildrenVisible(false)
	local calendarView = owner:GetControlObject("DiDa.CalendarView")
	calendarView:SetVisible(false)
	calendarView:SetChildrenVisible(false)
	
	local notePadPreView = owner:GetControlObject("DiDa.NotePadPreView")
	if notePadPreView then
		notePadPreView:SetVisible(false)
		notePadPreView:SetChildrenVisible(false)
	end
	local notePadView = owner:GetControlObject("DiDa.NotePadView")
	if notePadView then
		notePadView:SetVisible(false)
		notePadView:SetChildrenVisible(false)
	end
	
	local remindPreView = owner:GetControlObject("DiDa.RemindPreView")
	if remindPreView then
		remindPreView:SetVisible(false)
		remindPreView:SetChildrenVisible(false)
	end
	local remindView = owner:GetControlObject("DiDa.RemindView")
	if remindView then
		remindView:SetVisible(false)
		remindView:SetChildrenVisible(false)
	end
end

local curViewIndex = 1
function OnClickCalendar(self)
	--动画调整preview宽度，由213-->185;view宽度由 473--> 503
	if 1 == curViewIndex then
		return
	end
	
	local owner = self:GetOwnerControl()
	local previewLayoutBkg = owner:GetControlObject("preview.layout.bkg")
	local pagenailBkg = owner:GetControlObject("pagenail.bkg")
	local viewLayoutBkg = owner:GetControlObject("view.layout.bkg")
	
	previewLayoutBkg:SetObjPos2(38, 0, 182, 445)
	pagenailBkg:SetObjPos2(185+38-14, math.floor((445 - 293)/2), 25, 293)
	viewLayoutBkg:SetObjPos2(185+38, 0, 503, 445)
	
	HideAllView(self)
	
	--显示日历preview、view
	local calendarPreView = owner:GetControlObject("DiDa.CalendarPreView")
	calendarPreView:SetVisible(true)
	calendarPreView:SetChildrenVisible(true)
	local calendarView = owner:GetControlObject("DiDa.CalendarView")
	calendarView:SetVisible(true)
	calendarView:SetChildrenVisible(true)
	
	curViewIndex = 1
end

local UpdateNotePadListCookie = nil
function OnClickNotepad(self)
	local owner = self:GetOwnerControl()
	local previewLayoutBkg = owner:GetControlObject("preview.layout.bkg")
	local pagenailBkg = owner:GetControlObject("pagenail.bkg")
	local viewLayoutBkg = owner:GetControlObject("view.layout.bkg")
	
	local objectFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local previewLayout = owner:GetControlObject("preview.layout")
	local viewLayout = owner:GetControlObject("view.layout")
	--若上次显示的是日历标签，则需动画调整宽度
	if 1 == curViewIndex then
		previewLayoutBkg:SetObjPos2(38, 0, 213, 445)
		pagenailBkg:SetObjPos2(213+38-14, math.floor((445 - 293)/2), 25, 293)
		viewLayoutBkg:SetObjPos2(213 + 38, 0, 473, 445)
	end
	--隐藏提醒preview、view 以及 她用 preview、view
	HideAllView(self)
	
	--显示notepad preview、view 
	local notePadView = owner:GetControlObject("DiDa.NotePadView")
	if not notePadView then
		notePadView = objectFactory:CreateUIObject("DiDa.NotePadView", "NotePadView")
		notePadView:SetObjPos(0, 0, "father.width", "father.height")
		viewLayout:AddChild(notePadView)
	end
	notePadView:SetVisible(true)
	notePadView:SetChildrenVisible(true)
	
	local notePadPreView = owner:GetControlObject("DiDa.NotePadPreView")
	if not notePadPreView then
		notePadPreView = objectFactory:CreateUIObject("DiDa.NotePadPreView", "TimeLineListCtrl")
		--底部、左边有阴影
		notePadPreView:SetObjPos(0, 0, "father.width - 5", "father.height - 9")
		notePadPreView:AttachListener("OnSelect", false, 
											function(_, _, data) 
													notePadView:SetData(data)
											end)
		notePadPreView:AttachListener("OnClearItem", false, 
											function() 
													notePadView:SetData(nil)
											end)
											
		previewLayout:AddChild(notePadPreView)
	end
	notePadPreView:SetVisible(true)
	notePadPreView:SetChildrenVisible(true)
	
	UpdateNotePadListCookie = UpdateNotePadListCookie or 
								notePadView:AttachListener("UpdateNoteList", false, 
											function() 
													notePadPreView:ReBuildRemindList()
											end)
											
	curViewIndex = 2
end

local UpdateRemindListCookie = nil
function OnClickRemind(self)
	local owner = self:GetOwnerControl()
	local previewLayoutBkg = owner:GetControlObject("preview.layout.bkg")
	local pagenailBkg = owner:GetControlObject("pagenail.bkg")
	local viewLayoutBkg = owner:GetControlObject("view.layout.bkg")
	local objectFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	
	--若上次显示的是日历标签，则需动画调整宽度
	if 1 == curViewIndex then
		previewLayoutBkg:SetObjPos2(38, 0, 213, 445)
		pagenailBkg:SetObjPos2(213+38-14, math.floor((445 - 293)/2), 25, 293)
		viewLayoutBkg:SetObjPos2(213 + 38, 0, 473, 445)
	end
	--隐藏notepad preview、view 以及 她用 preview、view
	HideAllView(self)
	
	--显示提醒preview、view
	local remindView = owner:GetControlObject("DiDa.RemindView")
	local previewLayout = owner:GetControlObject("preview.layout")
	local viewLayout = owner:GetControlObject("view.layout")
	local remindPreView = owner:GetControlObject("DiDa.RemindPreView")
	if not remindView then
		remindView = objectFactory:CreateUIObject("DiDa.RemindView", "RemindPanleCtrl")
		remindView:SetObjPos(0, 0, "father.width", "father.height")
		viewLayout:AddChild(remindView)
	end
	remindView:SetVisible(true)
	remindView:SetChildrenVisible(true)
	
	if not remindPreView then
		remindPreView = objectFactory:CreateUIObject("DiDa.RemindPreView", "TimeLineRemindListCtrl")
		--底部、左边有阴影
		remindPreView:SetObjPos(0, 0, "father.width - 5", "father.height - 9")
		
		remindPreView:AttachListener("OnSelect", false, 
											function(_, _, data) 
													remindView:SetData(data)
											end)									
		previewLayout:AddChild(remindPreView)
	end
	remindPreView:SetVisible(true)
	remindPreView:SetChildrenVisible(true)
	
	UpdateRemindListCookie = UpdateRemindListCookie or 
								remindView:AttachListener("UpdateRemindList", false, 
											function() 
													remindPreView:ReBuildRemindList(data)
											end)
	
	curViewIndex = 3
end

function OnClickForher(self)
	local owner = self:GetOwnerControl()
	local previewLayoutBkg = owner:GetControlObject("preview.layout.bkg")
	local pagenailBkg = owner:GetControlObject("pagenail.bkg")
	local viewLayoutBkg = owner:GetControlObject("view.layout.bkg")
		
	--若上次显示的是日历标签，则需动画调整宽度
	if 1 == curViewIndex then
		previewLayoutBkg:SetObjPos2(38, 0, 213, 445)
		pagenailBkg:SetObjPos2(213+38-14, math.floor((445 - 293)/2), 25, 293)
		viewLayoutBkg:SetObjPos2(213 + 38, 0, 473, 445)
		
		local calendarPreView = owner:GetControlObject("DiDa.CalendarPreView")
		calendarPreView:SetVisible(false)
		calendarPreView:SetChildrenVisible(false)
		local calendarView = owner:GetControlObject("DiDa.CalendarView")
		calendarView:SetVisible(false)
		calendarView:SetChildrenVisible(false)
	end
	--隐藏notepad preview、view 以及 提醒preview、view
	
	--显示她用 preview、view
	curViewIndex = 4
end

---事件--
function OnCloseWnd(self)
	-- tipUtil:Exit()
end

function OnInitControl(self)
	--初始化，默认显示日历
	local objectFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local calendarPreView = objectFactory:CreateUIObject("DiDa.CalendarPreView", "CalendarPreView")
	local previewLayout = self:GetControlObject("preview.layout")
	if calendarPreView then
		calendarPreView:SetObjPos(0, 0, "father.width + 2", "father.height")
		previewLayout:AddChild(calendarPreView)
	end
	local calendarView = objectFactory:CreateUIObject("DiDa.CalendarView", "CalendarView")
	local viewLayout = self:GetControlObject("view.layout")
	if calendarView then
		calendarView:SetObjPos(0, 0, "father.width", "father.height")
		viewLayout:AddChild(calendarView)
	end
end

-- function OnLButtonUp(self, bFocus)
	-- 勿删
-- end

--监听事件
function CreateFilterListener(objRootCtrl)
	local objFactory = XLGetObject("APIListen.Factory")
	if not objFactory then
		tFunHelper.TipLog("[CreateFilterListener] not support APIListen.Factory")
		return
	end
	
	local objListen = objFactory:CreateInstance()	
	objListen:AttachListener(
		function(key,...)	

			tFunHelper.TipLog("[CreateFilterListener] key: " .. tostring(key))
			
			local tParam = {...}	
			if tostring(key) == "OnCommandLine" then
				OnCommandLine(tParam)
			elseif tostring(key) == "OnExplorerNotify" then
				OnExplorerNotify(tParam)
			end
		end)
end

function ShowHostWnd()
	local objHostWnd = tFunHelper.GetMainWndInst()
	if objHostWnd then
		local hWnd = objHostWnd:GetWndHandle()
		if hWnd then
			tipUtil:SetForegroundWindow(hWnd)
		else
			objHostWnd:Show(5)
		end
		
		objHostWnd:SetTopMost(true)
	end
end

function OnCommandLine(tParam)
	if tFunHelper.CheckIsNeedShow() then
		ShowHostWnd()
	end
end

function OnExplorerNotify(tParam)
	local nMessage = tonumber(tParam[1]) or -1
	local nShow = 0
	local nUpdate = 1
	local nAbout = 2
	local nQUIT = 3
	
	if nMessage == nShow then
		if tFunHelper.CheckIsNeedShow() then
			ShowHostWnd()
		end
	elseif nMessage == nUpdate then
		tFunHelper.ShowPopupWndByName("TipUpdateWnd.Instance", true)
	elseif nMessage == nAbout then
		tFunHelper.ShowPopupWndByName("TipAboutWnd.Instance", true)
	elseif nMessage == nQUIT then
		tFunHelper.ShowPopupWndByName("TipExitRemindWnd.Instance", true)
	end
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
end
