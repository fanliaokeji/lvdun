local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

function OnCreate( self )
	PopupInDeskRight(self)
end

function OnClickCancle(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end


function OnClickQuit(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	tFunHelper.KillClockWindow()
	tFunHelper.ReportAndExit()
end

function PopupInMainWndCenter(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
    local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local objMainWnd = tFunHelper.GetMainWndInst()
	local nMainWndLeft, nMainWndTop, nMainWndRight, nMainWndBottom = objMainWnd:GetWindowRect()
	local left = (nMainWndRight-nMainWndLeft-nLayoutWidth)/2
	local top = (nMainWndBottom-nMainWndTop-nLayoutHeight)/2
	self:Move( nMainWndLeft+left, nMainWndTop+top, nLayoutWidth, nLayoutHeight)
	return true
end

function PopupInDeskRight(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
    local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move( workright - nLayoutWidth, workbottom - nLayoutHeight, nLayoutWidth, nLayoutHeight)
	return true
end
