local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local gRootCtrl = nil

local gShowWnd = true
function CheckIsNeedShow()
	return gShowWnd
end

tFunHelper.CheckIsNeedShow = CheckIsNeedShow


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
		self:SetFocus(true)
	end
	
		tFunHelper.TipLog("[OnShowWindow]  bShow:"..tostring(bShow)
				.." focus: "..tostring(self:GetFocus()))

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
	self:Move( workright - nLayoutWidth+4, workbottom - nLayoutHeight+7, nLayoutWidth, nLayoutHeight)
	-- ani:SetKeyFramePos(0, nLayoutHeight, 0, 0) 
	-- objtree:AddAnimation(ani)
	-- ani:Resume()

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

local gbNotHideFlag = false
function SetLoseFocusNoHideFlag(bNotHide)
	gbNotHideFlag = bNotHide
end
XLSetGlobal("SetLoseFocusNoHideFlag", SetLoseFocusNoHideFlag)

local gTimerID = nil
function OnFocusChange(self, bFocus)
	tFunHelper.TipLog("[OnFocusChange]  bFocus:"..tostring(bFocus))
	if gbNotHideFlag then return end 
	if not bFocus then
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")

		self:UpdateWindow()
		self:Show(0)
		
		local nTimeSpanInMs = 200
		if gTimerID ~= nil then
			timerManager:KillTimer(gTimerID)
			gTimerID = nil
		end
		gShowWnd = false
		gTimerID = timerManager:SetTimer(function(item, id)
			timerManager:KillTimer(gTimerID)
			gTimerID = nil
			gShowWnd = true
		end, nTimeSpanInMs)
	end
end



