local tipUtil = XLGetObject( "GS.Util" )

function OnCreate( self )
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	local selfleft, selftop, selfright, selfbottom = self:GetWindowRect()
	local wndwidth, wndheight = selfright - selfleft, selfbottom - selftop
	local objtree = self:GetBindUIObjectTree()
	local objRootCtrl = objtree:GetUIObject("root.layout")
	local webleft, webtop, webright, webbottom = objRootCtrl:GetAbsPos()
	local webwidth, webheight = webright - webleft, webbottom - webtop
	local wndleft = ((workright-workleft)-webwidth)/2-webleft
	local wndtop = ((workbottom-worktop)-webheight)/2-webtop
	self:Move(wndleft, wndtop, wndwidth, wndheight)
end


function OnShowWindow(self, bVisible)
	if not bVisible then
		return
	end
	
	InitMainWnd(self)	
end

----------------------------------------------------------------

function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end


function OnClickMinBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end


function OnClickEnterBtn(self)
	self:SetVisible(false)
	self:SetChildrenVisible(false)

	local objTree = self:GetOwner()
	local objRootCtrl = objTree:GetUIObject("root.layout")
	local objProgBarLayout = objRootCtrl:GetObject("TipUpdate.Bkg:TipUpdate.ProgressBar.Layout")
	objProgBarLayout:SetVisible(true)
	objProgBarLayout:SetChildrenVisible(true)
	
	SetProgBar(objProgBarLayout)
	
end


-----------
function InitMainWnd(objHostWnd)	
	local objTree = objHostWnd:GetBindUIObjectTree()
	local objRootCtrl = objTree:GetUIObject("root.layout")
	if not objRootCtrl then
		return
	end

	local objProgBarLayout = objRootCtrl:GetObject("TipUpdate.Bkg:TipUpdate.ProgressBar.Layout")
	if not objProgBarLayout then
		return
	end
	
	objProgBarLayout:SetVisible(false)
	objProgBarLayout:SetChildrenVisible(false)
end

function SetProgBar(objProgBarLayout)
	local objProgBar = objProgBarLayout:GetObject("TipUpdate.ProgressBar")
	if not objProgBar then
		return
	end
	local l_nProgress = 0
	

	function DownLoadNewVersion()

		objProgBar:SetProgress(l_nProgress)
	end

	DownLoadNewVersion()
end





