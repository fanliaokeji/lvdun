local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

function OnCreate( self )
	PopupInDeskRight(self)
end

function Restore(objHostWnd, bCancel)
	local tUserData = objHostWnd:GetUserData()
	if type(tUserData) == "table" and type(tUserData["restore"]) == "function" then
		if bCancel then
			tUserData["restore"]()
		else
			if type(tUserData["callback"]) == "function" then
				tUserData["callback"]()
			end
			tUserData["restore"]()
		end
	end
end

function OnClickCancle(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	Restore(objHostWnd, true)
	objHostWnd:Show(0)
end


function OnClickQuit(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	if objHostWnd:GetUserData() then
		Restore(objHostWnd)
		objHostWnd:Show(0)
	else
		tFunHelper.KillClockWindow()
		tFunHelper.ReportAndExit()
	end
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

function OnShowWindow(self, bShow)
	if bShow then
		local tUserData = self:GetUserData()
		if type(tUserData) == "table" then
			PopupInMainWndCenter(self)
		else
			PopupInDeskRight(self)
		end
	end
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
