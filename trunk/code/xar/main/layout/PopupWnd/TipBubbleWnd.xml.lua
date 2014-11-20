local tipUtil = XLGetObject( "GS.Util" )
local g_bCheckState = false
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
local g_hTimer = nil

function OnCreate( self )
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	local selfleft, selftop, selfright, selfbottom = self:GetWindowRect()
	local wndwidth, wndheight = selfright - selfleft, selfbottom - selftop
	local objtree = self:GetBindUIObjectTree()
	local objRootCtrl = objtree:GetUIObject("root.layout")
	local webleft, webtop, webright, webbottom = objRootCtrl:GetAbsPos()
	local webwidth, webheight = webright - webleft, webbottom - webtop
	local wndleft = workright - webwidth - 20
	local wndtop =  workbottom - webheight
	self:Move(wndleft, wndtop, wndwidth, wndheight)
end


function OnShowWindow(self, bShow)
	if bShow then
		SaveStateToFile(false)
		StartHideTimer(self)
	end
end


function StartHideTimer(objHostWnd)
	EndTimer()

	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local nTimeSpanInSec = tonumber(tUserConfig["nHideBubblePopWndInSec"]) or 5
	
	local nTimeSpanInMs = nTimeSpanInSec * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	g_hTimer = timerManager:SetTimer(function(item, id)
			item:KillTimer(id)
			objHostWnd:Show(0)		
		end, nTimeSpanInMs)
end


function OnMouseEnter( self )
	EndTimer()
end

function OnMouseLeave(self, x, y)
	StartTimerWithCheck(self)
end


function RouteToFather(self)
	self:RouteToFather()
end


function EndTimer()
	if g_hTimer then
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
		timerManager:KillTimer(g_hTimer)
		g_hTimer = nil
	end
end


function StartTimerWithCheck(objCtrl)
	if nil == objCtrl or type(objCtrl.GetOwner) ~= "function" then
		return
	end

	local objTree = objCtrl:GetOwner()
	if nil == objTree then
		return
	end
	
	local objRootCtrl = objTree:GetUIObject("root.layout")	
	local objHostWnd = objTree:GetBindHostWnd()
	local mouseX, mouseY = tipUtil:GetCursorPos()
	local nWndX, nWndY = objHostWnd:ScreenPtToHostWndPt(mouseX, mouseY)
	local nTreeX, nTreeY = objHostWnd:HostWndPtToTreePt(nWndX, nWndY)
	local nEdgeWidth = 1
	local nLeft, nTop, nRight, nBottom = objRootCtrl:GetAbsPos()
	
	if nTreeX > (nLeft+nEdgeWidth) and nTreeX < (nRight-nEdgeWidth) 
		and nTreeY < (nBottom-nEdgeWidth) and nTreeY > (nTop+nEdgeWidth) then
		return
	end

	StartHideTimer(objHostWnd)
end


function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end


function OnClickCheckBox(self)
	local bNewState = not g_bCheckState
	SetCheckBoxState(self, bNewState)
	SaveStateToFile(bNewState)
	
	SetConfigState(not bNewState)
end


function SetCheckBoxState(objCheckBox, bState)
	if type(bState) ~= "boolean" then
		return
	end

	if bState then
		objCheckBox:SetTextureID("GreenWall.BubbleWnd.CheckBox.Check")
	else
		objCheckBox:SetTextureID("GreenWall.BubbleWnd.CheckBox.Empty")
	end
	
	g_bCheckState = bState
end


function SetConfigState(bState)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd("GreenWallTipWnd.MainFrame")
	if nil == frameHostWnd then
		return
	end
	
	local objTree = frameHostWnd:GetBindUIObjectTree()
	if nil == objTree then
		return
	end
	
	local objRootCtrl = objTree:GetUIObject("root.layout:root.ctrl")
	if nil == objRootCtrl then
		return
	end
	
	local objBodyContainer = objRootCtrl:GetControlObject("TipCtrl.MainWnd.MainBody")
	if nil == objBodyContainer then
		return
	end
	
	local objConfigCtrl = objBodyContainer:GetChildObjByCtrlName("ChildCtrl_Config")
	if nil == objConfigCtrl then
		return
	end
	
	objConfigCtrl:SetElemState("FiltInTime", bState)
end


function SaveStateToFile(bNewState)
	local strStartCfgPath = tFunctionHelper.GetCfgPathWithName("startcfg.ini")
	if not IsRealString(strStartCfgPath) then
		return
	end

	if bNewState then
		tipUtil:WriteINI("pusher", "noremind", 1, strStartCfgPath)
	else
		tipUtil:WriteINI("pusher", "noremind", 0, strStartCfgPath)
	end	
end


function IsRealString(str)
	return type(str) == "string" and str ~= ""	
end
