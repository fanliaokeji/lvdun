local tipUtil = XLGetObject( "GS.Util" )
local g_bCheckState = false
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")

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
		SaveStateToFile(false)
	end
end


function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end


function OnClickCheckBox(self)
	local bNewState = not g_bCheckState
	SetCheckBoxState(self, bNewState)
	SaveStateToFile(bNewState)
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


function SaveStateToFile(bNewState)
	local strStartCfgPath = tFunctionHelper.GetCfgPathWithName("startcfg.ini")
	if not IsRealString(strStartCfgPath) then
		return
	end

	if bNewState then
		tipUtil:WriteINI("pusher", "noremind", 1, strStartCfgPath)
	else
		tipUtil:WriteINI("pusher", "noremind", 0, strStartCfgPath)
	end	
end


function IsRealString(str)
	return type(str) == "string" and str ~= ""	
end
