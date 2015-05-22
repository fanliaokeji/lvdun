local g_bHasInit = false

local g_tPanelCtrlList = {
	"ChildCtrl_AdltWebFilter",
	"ChildCtrl_SurfTime",
	"ChildCtrl_AutoLock",
}


---事件---
function OnInitControl(self)
	InitFilterConfig(self)
end


function OnClickAdltWebFilter(self)
	SetMenuSelect(self, 1)
	OpenPanel(self, "ChildCtrl_AdltWebFilter")
end

function OnClickSurfTime(self)
	SetMenuSelect(self, 2)
	OpenPanel(self, "ChildCtrl_SurfTime")
end

function OnClickAutoLock(self)
	SetMenuSelect(self, 3)
	OpenPanel(self, "ChildCtrl_AutoLock")
end


---方法---
function OnShowPanel(self, bShow)
	if not g_bHasInit then
		InitFilterConfig(self)
	end		
end


-------------------------
function InitFilterConfig(self)
	local objRootCtrl = self
	local objMainBodyCtrl = objRootCtrl:GetControlObject("ChildCtrl_ParentControl.MainPanel")
	if objMainBodyCtrl == nil then
		return false
	end
	
	local bSucc = objMainBodyCtrl:InitPanelList(g_tPanelCtrlList)
	if not bSucc then
		return false
	end
	
	local bSucc = objMainBodyCtrl:ChangePanel("ChildCtrl_AdltWebFilter")
	if not bSucc then
		return false
	end
	
	g_bHasInit = true
	return true
end


function OpenPanel(objButton, strNewCtrlName)
	if objButton == nil then
		return
	end

	local objRootCtrl = objButton:GetOwnerControl()
	if objRootCtrl == nil then
		return
	end

	local objMainBodyCtrl = objRootCtrl:GetControlObject("ChildCtrl_ParentControl.MainPanel")
	if objMainBodyCtrl == nil then
		return
	end
	
	local strCurCtrlName = objMainBodyCtrl:GetCurrentCtrlName()
	if strCurCtrlName ~= strNewCtrlName then
		objMainBodyCtrl:ChangePanel(strNewCtrlName)
	end
end



function SetMenuSelect(objText, nIndex)
	local objRootCtrl = objText:GetOwnerControl()
	local objMenuSelect = objRootCtrl:GetControlObject("ChildCtrl_ParentControl.Menu.Select")
	if nil == objMenuSelect then
		return
	end
	
	local nL, nT, nR, nB = objMenuSelect:GetObjPos()
	local nWidth = nR - nL
	local nSpace = 11
	local nNewL = nSpace+(nIndex-1)*nWidth
	objMenuSelect:SetObjPos(nNewL, nT, nNewL+nWidth, nB)
end

