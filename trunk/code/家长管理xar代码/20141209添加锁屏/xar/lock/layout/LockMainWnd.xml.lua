local tipUtil = XLGetObject("GS.Util")


function OnCreate( self )
	SetWindowFullScreen(self)
	SetMainPanleCenter(self)
end


function SetWindowFullScreen(self)
	local nWidth, nHeight = tipUtil:GetScreenSize()
	self:SetMaxTrackSize(nWidth, nHeight)
	self:Move(0, 0, nWidth, nHeight)
	
	local objtree = self:GetBindUIObjectTree()
	local objBlackLayout = objtree:GetUIObject("root.layout:black.layout.caption")
	if not objBlackLayout then
		return
	end
	
	objBlackLayout:SetObjPos(0, 0, nWidth, nHeight)
end


function SetMainPanleCenter(self)
	local nWidth, nHeight = tipUtil:GetScreenSize()
	local objtree = self:GetBindUIObjectTree()
	local objRootCtrl = objtree:GetUIObject("root.layout:root.ctrl")
	if not objRootCtrl then
		return
	end 
	
	local webleft, webtop, webright, webbottom = objRootCtrl:GetAbsPos()
	local webwidth, webheight = webright - webleft, webbottom - webtop
	local wndleft = (nWidth-webwidth)/2-webleft
	local wndtop = (nHeight-webheight)/2-webtop
	objRootCtrl:SetObjPos(wndleft, wndtop, wndleft+webwidth, wndtop+webheight)
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

