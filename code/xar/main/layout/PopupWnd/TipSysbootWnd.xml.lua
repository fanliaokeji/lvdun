local tipUtil = XLGetObject( "GS.Util" )
local g_bCheckState = true
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
local g_hTimer = nil

function OnCreate( self )
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	local selfleft, selftop, selfright, selfbottom = self:GetWindowRect()
	local wndwidth, wndheight = selfright - selfleft, selfbottom - selftop
	local objtree = self:GetBindUIObjectTree()
	local objRootCtrl = objtree:GetUIObject("root.layout")
	local webleft, webtop, webright, webbottom = objRootCtrl:GetAbsPos()
	local webwidth, webheight = webright - webleft, webbottom - webtop
	local wndleft = workright - webwidth - 20
	local wndtop =  workbottom - webheight
	self:Move(wndleft, wndtop, wndwidth, wndheight)
end


function OnShowWindow(self, bShow)
	if bShow then
		StartHideTimer(self)
	else
		TrySetSysboot()
	end	
end


function StartHideTimer(objHostWnd)
	EndTimer()

	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local nTimeSpanInSec = tonumber(tUserConfig["nHideBubblePopWndInSec"]) or 5
	
	local nTimeSpanInMs = nTimeSpanInSec * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	g_hTimer = timerManager:SetTimer(function(item, id)
			item:KillTimer(id)
			objHostWnd:Show(0)		
		end, nTimeSpanInMs)
end


function OnMouseEnter( self )
	EndTimer()
end

function OnMouseLeave(self, x, y)
	StartTimerWithCheck(self)
end


function RouteToFather(self)
	self:RouteToFather()
end


function EndTimer()
	if g_hTimer then
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
		timerManager:KillTimer(g_hTimer)
		g_hTimer = nil
	end
end


function StartTimerWithCheck(objCtrl)
	if nil == objCtrl or type(objCtrl.GetOwner) ~= "function" then
		return
	end

	local objTree = objCtrl:GetOwner()
	if nil == objTree then
		return
	end
	
	local objRootCtrl = objTree:GetUIObject("root.layout")	
	local objHostWnd = objTree:GetBindHostWnd()
	local mouseX, mouseY = tipUtil:GetCursorPos()
	local nWndX, nWndY = objHostWnd:ScreenPtToHostWndPt(mouseX, mouseY)
	local nTreeX, nTreeY = objHostWnd:HostWndPtToTreePt(nWndX, nWndY)
	local nEdgeWidth = 1
	local nLeft, nTop, nRight, nBottom = objRootCtrl:GetAbsPos()
	
	if nTreeX > (nLeft+nEdgeWidth) and nTreeX < (nRight-nEdgeWidth) 
		and nTreeY < (nBottom-nEdgeWidth) and nTreeY > (nTop+nEdgeWidth) then
		return
	end

	StartHideTimer(objHostWnd)
end


function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end


function OnClickCheckBox(self)
	local bNewState = not g_bCheckState
	SetCheckBoxState(self, bNewState)
end


function SetCheckBoxState(objCheckBox, bState)
	if type(bState) ~= "boolean" then
		return
	end

	if bState then
		objCheckBox:SetTextureID("GreenWall.BubbleWnd.CheckBox.Check")
	else
		objCheckBox:SetTextureID("GreenWall.BubbleWnd.CheckBox.Empty")
	end
	
	g_bCheckState = bState
end


function TrySetSysboot()
	local bHasAutoStup = false
	local strGreenShieldPath = tFunctionHelper.RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\GreenShield\\path")
	local szCmdLine = tFunctionHelper.RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\GreenShield") or ""
	if IsRealString(szCmdLine) 
		and string.find(string.lower(szCmdLine), string.lower(tostring(strGreenShieldPath)), 1, true) then
		bHasAutoStup = true  -- 已经开机启动
	end
	
	if g_bCheckState and not bHasAutoStup then   --g_bCheckState == true 表示需要设置开机启动
		if IsRealString(strGreenShieldPath) and tipUtil:QueryFileExists(strGreenShieldPath) then
			local sCommandline = "\""..strGreenShieldPath.."\"".." /sstartfrom sysboot /embedding"
			bRetCode = tFunctionHelper.RegSetValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\GreenShield", sCommandline)
		end
		
	elseif bHasAutoStup then
		tFunctionHelper.RegDeleteValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\GreenShield")
	end

end


function IsRealString(str)
	return type(str) == "string" and str ~= ""	
end





