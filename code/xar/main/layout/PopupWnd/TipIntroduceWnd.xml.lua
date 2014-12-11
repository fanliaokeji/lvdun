local tipUtil = XLGetObject( "GS.Util" )
local g_nCurPanelIndex = 1
local g_nTimer = nil
local g_nAllPanelCount = 4

function OnCreate( self )
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	local selfleft, selftop, selfright, selfbottom = self:GetWindowRect()
	local wndwidth, wndheight = selfright - selfleft, selfbottom - selftop
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
	local webleft, webtop, webright, webbottom = objRootLayout:GetAbsPos()
	local webwidth, webheight = webright - webleft, webbottom - webtop
	local wndleft = ((workright-workleft)-webwidth)/2-webleft
	local wndtop = ((workbottom-worktop)-webheight)/2-webtop
	self:Move(wndleft, wndtop, wndwidth, wndheight)
end


function OnShowWindow(self, bShow)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
	
	if bShow then
		StartPanelTimer(objRootLayout)
	else
		StopTimer()
	end
end


function OnClickTryBtn(self)
	HideWnd(self)
	tipUtil:OpenURL("www.youku.com")
end

function OnClickCloseBtn(self)
	HideWnd(self)
end

function OnMouseEnter(self)
	StopTimer()
	self:RouteToFather()
end

function RouteToFather(self)
	self:RouteToFather()
end


function OnMouseLeave(self)
	if g_nCurPanelIndex == g_nAllPanelCount then
		return
	end
	
	local objtree = self:GetOwner()
	local objRootLayout = objtree:GetUIObject("root.layout")

	StartPanelTimer(objRootLayout)
end


function OnClickMenu(self, nPosX, nPosY)
	local nIndex = GetPanelIndexByMousePos(self, nPosX)
	if nIndex < 1 or nIndex > g_nAllPanelCount then
		return
	end
	
	local objtree = self:GetOwner()
	local objRootLayout = objtree:GetUIObject("root.layout")
	ShowPanel(objRootLayout, nIndex)
end


function GetPanelIndexByMousePos(objMenu, nPosX)
	local nL, nT, nR, nB = objMenu:GetObjPos()
	local nMenuW = nR - nL
	local nItemW = nMenuW/g_nAllPanelCount
	local nIndex = math.floor(nPosX/nItemW) + 1

	return nIndex
end


function ShowPanel(objRootLayout, nPanelIndex)
	if nPanelIndex == g_nCurPanelIndex then
		return
	end

	local objContainer = objRootLayout:GetObject("TipIntroduce.ContainerCtrl")
	
	local nL, nT, nR, nB = objRootLayout:GetObjPos()
	local nWidth = nR - nL
	local nHeight = nB - nT
	local nCurLeft = 0-(g_nCurPanelIndex-1)*nWidth
	local nDestLeft = 0-(nPanelIndex-1)*nWidth

	objContainer:SetObjPos(nDestLeft, 0, nDestLeft+nWidth*g_nAllPanelCount, nHeight) 
	g_nCurPanelIndex = nPanelIndex
end


function StartPanelTimer(objRootLayout)
	local nStopIndex = 4

	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	local nTimeSpanInMs = 2*1000
	
	g_nTimer = timerManager:SetTimer(
		function(item, id)
		
			local nNextPanel = math.mod(g_nCurPanelIndex+1, g_nAllPanelCount)
			if nNextPanel == 0 then
				nNextPanel = 4
			end
			ShowPanel(objRootLayout, nNextPanel)
			if nNextPanel == nStopIndex then
				StopTimer()
			end
			
	end, nTimeSpanInMs)
end


function StopTimer()
	if g_nTimer then
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
		timerManager:KillTimer(g_nTimer)
	end
end


function HideWnd(objUIElem)
	local objTree = objUIElem:GetOwner()	
	local objRootLayout = objTree:GetUIObject("root.layout")
		
	ShowPanel(objRootLayout, 1)

	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end

