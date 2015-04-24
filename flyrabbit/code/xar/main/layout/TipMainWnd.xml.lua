local tFunHelper = XLGetGlobal("Project.FunctionHelper")
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
XLSetGlobal("Project.GetTipStartTime", GetTipStartTime)

function OnShowWindow(self, bShow)
	if bShow then
		gTipStartTime = tipUtil:GetCurrentUTCTime()
	end
end


function OnSize(self, _type, width, height)	
	local objTree = self:GetBindUIObjectTree()
	local objRootCtrl = objTree:GetUIObject("root.layout:root.ctrl")
	objRootCtrl:SetObjPos(0, 0, width, height)
end


--to do left
function PopupInDeskRight(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
    local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	-- local aniT = templateMananger:GetTemplate("tip.pos.animation","AnimationTemplate")
	-- local ani = aniT:CreateInstance()
	-- ani:BindObj(objRootLayout)
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move( workright - nLayoutWidth+3, workbottom - nLayoutHeight+2, nLayoutWidth, nLayoutHeight)
	-- ani:SetKeyFramePos(0, nLayoutHeight, 0, 0) 
	-- objtree:AddAnimation(ani)
	-- ani:Resume()

	return true
end


function SetWindowProfile(self)
	local objNoShadowLayout = tFunHelper.GetMainCtrlChildObj("WithoutShadow")
	if not objNoShadowLayout then
		return
	end
	
	local l, t, r, b = objNoShadowLayout:GetObjPos()
	self:SetBorder(l, t*2, l, 0)
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	local nScreenW = workright - workleft + l*2
	local nScreenH = workbottom - worktop + t*3
	self:SetMaxTrackSize(nScreenW, nScreenH)
end


function OnCreate( self )
	PopupInDeskRight(self)
	-- SetWindowProfile(self)
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


