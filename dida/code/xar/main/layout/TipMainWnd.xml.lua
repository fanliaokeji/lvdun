local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local gRootCtrl = nil


function OnClose( self )
	self:Show(0)
	return 0, true
end


local gTipStartTime = nil
function GetTipStartTime()
	return gTipStartTime
end
XLSetGlobal("DiDa.GetTipStartTime", GetTipStartTime)

function OnShowWindow(self, bShow)
	if bShow then
		gTipStartTime = tipUtil:GetCurrentUTCTime()
	end
	self:SetFocus(true)
end


--to do left
function PopupInDeskLeft(self)
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	local nScreenW = workright - workleft
	local nScreenH = workbottom - worktop
	
	local selfleft, selftop, selfright, selfbottom = self:GetWindowRect()
	local wndwidth, wndheight = selfright - selfleft, selfbottom - selftop
	
	local objtree = self:GetBindUIObjectTree()
	local objRootCtrl = objtree:GetUIObject("root.layout:root.ctrl")
	local nRtCtrlL, nRtCtrlT, nRtCtrlR, nRtCtrlB = objRootCtrl:GetAbsPos()
	local nRtCtrlW, nRtCtrlH = nRtCtrlR - nRtCtrlL, nRtCtrlB - nRtCtrlT
	
	local wndleft = (nScreenW-nRtCtrlW)/2-nRtCtrlL
	local wndtop = (nScreenH-nRtCtrlH)/2-nRtCtrlT
	
	self:Move(wndleft, wndtop, wndwidth, wndheight)
end



function OnCreate( self )
	 PopupInDeskLeft(self)
end


function OnDestroy( self )
	local objtree = self:GetBindUIObjectTree()
	if objtree ~= nil then
		self:UnbindUIObjectTree()
		local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")
		objtreeManager:DestroyTree(objtree)
	end
	local wndId = self:GetID()
	if wndId ~= nil then
		local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
		local tempWnd = hostwndManager:GetHostWnd(wndId)
		if tempWnd then
			hostwndManager:RemoveHostWnd(wndId)
		end
	end
end


function OnFocusChange(self, bFocus)
	if not bFocus then
		self:Show(0)
	end
end


