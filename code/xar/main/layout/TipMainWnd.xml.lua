local tipUtil = XLGetObject("GS.Util")
local gRootCtrl = nil

-- function EndTimer()
	-- if gRootCtrl ~= nil then
		-- local attr = gRootCtrl:GetAttribute()
		-- if type(attr) == "table" and attr.holdtimer then
			-- local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
			-- timerManager:KillTimer(attr.holdtimer)
			-- attr.holdtimer = nil
		-- end
	-- end
-- end

-- function StartTimer()
	-- EndTimer()
	-- if gRootCtrl ~= nil then
		-- local attr = gRootCtrl:GetAttribute()
		-- if type(attr) == "table" then
			-- local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
			-- attr.holdtime = 0
			-- attr.holdtimer = timerManager:SetTimer(function(item, id)
				-- attr.holdtime = attr.holdtime + 1
				-- if attr.holdtime >= attr.holdseconds and attr.holdseconds > 0 then
					-- item:KillTimer(id)
					-- local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
					-- local frameHostWnd = hostwndManager:GetHostWnd("XmpTipWnd.MainFrame")
					-- if frameHostWnd then
						-- frameHostWnd:Destroy()
					-- end
				-- end
			-- end, 1000)
		-- end
	-- end
-- end

function OnMouseMove( self )
--	EndTimer()
end

function OnMouseLeave(self, x, y)
--	StartTimer()
end

local gTipStartTime = nil
function GetTipStartTime()
	return gTipStartTime
end
XLSetGlobal("GreenWall.GetTipStartTime", GetTipStartTime)

function OnShowWindow(self, bShow)
	if bShow then
		gTipStartTime = tipUtil:GetCurrentUTCTime()
		--StartTimer()
	end
end

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

function OnDestroy( self )
	local objtree = self:GetBindUIObjectTree()
	if objtree ~= nil then
		--EndTimer()
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



