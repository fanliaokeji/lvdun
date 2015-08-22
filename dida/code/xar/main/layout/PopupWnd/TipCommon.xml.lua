local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil


function ReportGoogle(strKey)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local tStatInfo = {}
	tStatInfo.strEL = FunctionObj.GetInstallSrc() or ""
	tStatInfo.strEA = FunctionObj.GetMinorVer() or ""
	tStatInfo.strEV = 1
	tStatInfo.strEC = strKey
	FunctionObj.TipConvStatistic(tStatInfo)
end

local gHostWnd = nil
local gHoldTime = 0
local gHoldTimer = nil
function EndTimer()
	if gHostWnd ~= nil then
		if gHoldTimer then
			local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
			timerManager:KillTimer(gHoldTimer)
			gHoldTimer = nil
		end
	end
end

function StartTimer()
	EndTimer()
	if gHostWnd ~= nil then
		gHoldTime = 0
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
		local tData = gHostWnd:GetUserData()
		local nHoldMins = tData[2]["nHoldMins"]
		gHoldTimer = timerManager:SetTimer(function(item, id)
			gHoldTime = gHoldTime + 1
			if not gHostWnd:GetVisible() or gHostWnd:GetWindowState() == "hide" then--点击tip导致隐藏
				item:KillTimer(id)
			elseif type(nHoldMins) == "number" and gHoldTime >= nHoldMins*60 and nHoldMins > 0 then
				item:KillTimer(id)
				MoveOutOfDesk(gHostWnd)
				gHostWnd:Show(0)
				local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
				if gHostWnd then
					local objtree = gHostWnd:GetBindUIObjectTree()
					gHostWnd:UnbindUIObjectTree()
					local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")
					objtreeManager:DestroyTree(objtree)
					gHostWnd:Destroy()
					hostwndManager:RemoveHostWnd("TipCommon.Instance")
				end
				ReportGoogle("tipautoclose_"..tData[1])
			end
		end, 1000)
	end
end

function OnMouseMove(self )
	EndTimer()
end

function OnMouseLeave(self, x, y)
	StartTimer()
end

function OnCloseBtnClick(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	MoveOutOfDesk(objHostWnd)
	objHostWnd:Show(0)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	if objHostWnd then
		local tData = objHostWnd:GetUserData()
		ReportGoogle("tipmanualclose_"..tData[1])
		local objtree = objHostWnd:GetBindUIObjectTree()
		objHostWnd:UnbindUIObjectTree()
		local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")
		objtreeManager:DestroyTree(objtree)
		objHostWnd:Destroy()
		hostwndManager:RemoveHostWnd("TipCommon.Instance")
	end
end

function MoveOutOfDesk(hostwnd)
	--移到桌面之外
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	local selfleft, selftop, selfright, selfbottom = hostwnd:GetWindowRect()
	local wndwidth, wndheight = selfright - selfleft, selfbottom - selftop
	hostwnd:Move( workright - wndwidth - 7, workbottom - wndheight, wndwidth, wndheight)
	return wndwidth, wndheight
end

function OnShowWindow(self, bShow)
	if bShow then
		gHostWnd = self
		--移到桌面之外
		local wndwidth, wndheight = MoveOutOfDesk(self)
		--开启动画进入屏幕
		local objtree = self:GetBindUIObjectTree()
		local obj = objtree:GetUIObject("root.layout")
		local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
		
		local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")
		local posAni = aniFactory:CreateAnimation("PosChangeAnimation")
		posAni:SetTotalTime(800)
		posAni:BindObj(obj)
		posAni:SetKeyFramePos(0, wndheight, 0, 0)
		objtree:AddAnimation(posAni)
		posAni:Resume()
		local tData = gHostWnd:GetUserData()
		ReportGoogle("tipshow_"..tData[1])
		StartTimer()
		self:SetEnable(true)
	else
		local objtree = self:GetBindUIObjectTree()
		local layout = objtree:GetUIObject("root.layout")
		local browserParent = layout:GetObject("TipCtrl.MainWnd.Bkg:TipCtrl.MainWnd.MainPanel:TipCtrl.MainWnd.Web")
		if browserParent then
			tFunHelper:TipLog("OnShowWindow find browserParent")
			local browser = browserParent:GetControlObject("browser")
			if browser then
				tFunHelper:TipLog("OnShowWindow bShow = false, hide browser")
				browserParent:RemoveChild(browser)
			end
		end
	end
end