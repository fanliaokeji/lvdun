local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

function OnCreate(self)
	SetLoseFocusNoHideFlag(true)
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
end

function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:EndDialog(0)
	SetLoseFocusNoHideFlag(false)
end

