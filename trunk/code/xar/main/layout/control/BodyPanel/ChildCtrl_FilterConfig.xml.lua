local g_bHasInit = false

local g_tPanelCtrlList = {
	"ChildCtrl_AdvFilter",
	"ChildCtrl_WhiteList",
	"ChildCtrl_ParentControl",
}


---事件---
function OnClickAdvFilter(self)
	SetMenuSelect(self, 1)
	OpenPanel(self, "ChildCtrl_AdvFilter")
end

function OnClickWhiteList(self)
	SetMenuSelect(self, 2)
	OpenPanel(self, "ChildCtrl_WhiteList")
end

function OnClickParentControl(self)
	SetMenuSelect(self, 3)
	OpenPanel(self, "ChildCtrl_ParentControl")
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
	local objMainBodyCtrl = objRootCtrl:GetControlObject("TChildCtrl_FilterConfig.MainPanel")
	if objMainBodyCtrl == nil then
		return false
	end
	
	local bSucc = objMainBodyCtrl:InitPanelList(g_tPanelCtrlList)
	if not bSucc then
		return false
	end
	
	local bSucc = objMainBodyCtrl:ChangePanel("ChildCtrl_AdvFilter")
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

	local objMainBodyCtrl = objRootCtrl:GetControlObject("TChildCtrl_FilterConfig.MainPanel")
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
	local objMenuSelect = objRootCtrl:GetControlObject("ChildCtrl_FilterConfig.Menu.Select")
	if nil == objMenuSelect then
		return
	end
	
	local nL, nT, nR, nB = objMenuSelect:GetObjPos()
	local nWidth = nR - nL
	local nSpace = 11
	local nNewL = nSpace+(nIndex-1)*nWidth
	objMenuSelect:SetObjPos(nNewL, nT, nNewL+nWidth, nB)
end









