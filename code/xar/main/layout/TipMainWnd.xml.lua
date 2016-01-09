local tipUtil = XLGetObject("GS.Util")
local gRootCtrl = nil


function OnClose( self )
	self:Show(0)
	return 0, true
end


function OnMouseLeave(self, x, y)

end

local gTipStartTime = nil
function GetTipStartTime()
	return gTipStartTime
end
XLSetGlobal("GreenWall.GetTipStartTime", GetTipStartTime)

function OnShowWindow(self, bShow)
	if bShow then
		gTipStartTime = tipUtil:GetCurrentUTCTime()
	end
end


function PopupInDeskRight(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
    local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	--local aniT = templateMananger:GetTemplate("tip.pos.animation","AnimationTemplate")
	--local ani = aniT:CreateInstance()
	--ani:BindObj(objRootLayout)
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move( (workright - nLayoutWidth)/2, (workbottom - nLayoutHeight)/2, nLayoutWidth, nLayoutHeight)
	--ani:SetKeyFramePos(0, nLayoutHeight, 0, 0) 
	--objtree:AddAnimation(ani)
	--ani:Resume()

	return true
end


function OnCreate( self )
	 PopupInDeskRight(self)
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


