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
	
	PlayGif(objRootCtrl)
end


function OnShowWindow(self, bShow)
	if bShow then
		StartHideTimer(self)
	else
		if g_bCheckState then
			SaveUserTickToFile()
		end
	end
end


function PlayGif(objRootCtrl)
	local objGif = objRootCtrl:GetObject("FilterBubble.GifBkg")
	if objGif then
		objGif:Play()
	end
end


function StartHideTimer(objHostWnd)
	EndTimer()

	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local nTimeSpanInSec = tonumber(tUserConfig["nHideFilterBubbleInSec"]) or 5
	
	local nTimeSpanInMs = nTimeSpanInSec * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	g_hTimer = timerManager:SetTimer(function(item, id)
			item:KillTimer(id)
			objHostWnd:Show(0)		
		end, nTimeSpanInMs)
end


function OnClickBubblePanel(self)
	ShowMainPanel()
	HideBubble(self)
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
	HideBubble(self)
end


function OnClickCheckBox(self)
	local bNewState = not g_bCheckState
	SetCheckBoxState(self, bNewState)
end


function SetCheckBoxState(objCheckBox, bState)
	if type(bState) ~= "boolean" then
		return
	end

	if bState then
		objCheckBox:SetTextureID("GreenWall.FilterBubble.CheckBox.Check")
	else
		objCheckBox:SetTextureID("GreenWall.FilterBubble.CheckBox.Empty")
	end
	
	g_bCheckState = bState
end


function ShowMainPanel()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local strHostWndName = "GreenWallTipWnd.MainFrame"
	local objHostWnd = hostwndManager:GetHostWnd(strHostWndName)
	if objHostWnd then
		objHostWnd:Show(5)
		tFunctionHelper.SetWndForeGround(objHostWnd)
		local strState = objHostWnd:GetWindowState()
		if tostring(strState) == "min" then
			objHostWnd:BringWindowToTop(true)
		end
	end
end


function HideBubble(objUIElem)
	local objTree = objUIElem:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end


function SaveUserTickToFile()
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nNoShowFilterBubble"] = tipUtil:GetCurrentUTCTime()
	tFunctionHelper.SaveConfigToFileByKey("tUserConfig")
end


function IsRealString(str)
	return type(str) == "string" and str ~= ""	
end
