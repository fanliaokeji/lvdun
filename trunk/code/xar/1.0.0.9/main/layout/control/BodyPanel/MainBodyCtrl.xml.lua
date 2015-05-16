local tipUtil = XLGetObject("GS.Util")

-----事件----



-----方法-----
function InitPanelList(self, tPanelList)
	local objPanelContainer = self:GetControlObject("MainBodyCtrl.MainPanel")
	if objPanelContainer == nil then
		TipLog("[InitPanelList] get objPanelContainer failed ")
		return false
	end

	if type(tPanelList) ~= "table" then
		TipLog("[InitPanelList] para invalid tPanelList")
		return false
	end

	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	if nil == objFactory then
		TipLog("[InitPanelList] ObjectFactory failed")
		return false
	end
	
	for nIndex, strCtrlName in pairs(tPanelList) do 
		local strCtrlKey = GetCtrlKeyByCtrlName(strCtrlName)
		local objChild = objFactory:CreateUIObject(strCtrlKey, strCtrlName)
		if nil == objChild then
			TipLog("[InitPanelList] CreateUIObject failed, strCtrlName:"..tostring(strCtrlName))
			return false
		end
		
		objPanelContainer:AddChild(objChild)
		objChild:SetVisible(false)
		objChild:SetChildrenVisible(false)
	end
	
	return true	
end

function ChangePanel(self, strNewCtrlName)
	local objPanelContainer = self:GetControlObject("MainBodyCtrl.MainPanel")
	if objPanelContainer == nil then
		TipLog("[ChangePanel] get objPanelContainer failed ")
		return false
	end

	if not IsRealString(strNewCtrlName) then
		TipLog("[ChangePanel] para invalid strNewCtrlName: "..tostring(strNewCtrlName))
		return false
	end

	local strCurCtrlName = GetCurrentCtrlName(self)
	local strCurCtrlKey = GetCtrlKeyByCtrlName(strCurCtrlName)
	local objCurPanel = objPanelContainer:GetObject(strCurCtrlKey)
	
	local strNewCtrlKey = GetCtrlKeyByCtrlName(strNewCtrlName)
	local objNewPanel = objPanelContainer:GetObject(strNewCtrlKey)
	if nil == objNewPanel then
		TipLog("[ChangePanel] Panel to Set is nil: "..tostring(strNewCtrlName))
		return false
	end
	
	if nil ~= objCurPanel and type(objCurPanel.OnShowPanel) == "function" then
		objCurPanel:OnShowPanel(false)
	end
	
	if nil ~= objCurPanel then
		objCurPanel:SetVisible(false)
		objCurPanel:SetChildrenVisible(false)
	end
	
	objNewPanel:SetVisible(true)
	objNewPanel:SetChildrenVisible(true)
	objNewPanel:SetObjPos(0, 0, "father.width", "father.height")
	
	if type(objNewPanel.OnShowPanel) == "function" then
		objNewPanel:OnShowPanel(true)
	end
		
	SetCurrentCtrlName(self, strNewCtrlName)
	return true
end

function GetCurrentCtrlObj(self)
	local strCurCtrlName = GetCurrentCtrlName(self)
	local objChild = GetChildObjByCtrlName(self, strCurCtrlName)
	return objChild
end


function GetChildObjByCtrlName(self, strCtrlName)
	local objPanelContainer = self:GetControlObject("MainBodyCtrl.MainPanel")
	if objPanelContainer == nil then
		TipLog("[GetCurrentCtrlObj] get objPanelContainer failed ")
		return nil
	end
	
	local strCurCtrlKey = GetCtrlKeyByCtrlName(strCtrlName)
	local objCurPanel = objPanelContainer:GetObject(strCurCtrlKey)
	
	return objCurPanel
end

-----辅助函数----
function GetCurrentCtrlName(objRootCtrl)
	local attr = objRootCtrl:GetAttribute()
	TipLog("[GetCurrentCtrlName] attr.CurCtrlName:"..tostring(attr.CurCtrlName))
	return attr.CurCtrlName
end

function SetCurrentCtrlName(objRootCtrl, strCtrlName)
	local attr = objRootCtrl:GetAttribute()
	if IsRealString(strCtrlName) then
		attr.CurCtrlName = strCtrlName
	end

	TipLog("[SetCurrentCtrlName] attr.CurCtrlName :"..tostring(attr.CurCtrlName))
end

function GetCtrlKeyByCtrlName(strName)
	local CTRLKEYPREFIX = "MainBody_"
	local strCtrlKey = CTRLKEYPREFIX..tostring(strName)
	return strCtrlKey
end

function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@MainBodyCtrl: " .. tostring(strLog))
	end
end