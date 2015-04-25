local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local tipAsynUtil = tFunHelper.tipAsynUtil

function OnCreate( self )
	PopupInDeskRight(self)
	
	local objMainWnd = tFunHelper.GetMainWndInst()
	if objMainWnd:GetVisible() then
		PopupInMainWndCenter(self)
	end	
	
	SetShowText(self)
end

function OnClose(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:EndDialog(0)
end

function SetData(self, tData)
	local textobj1 = self:GetControlObject("About.MainInfo")
	local textobj2 = self:GetControlObject("About.MainInfo2")
	if IsRealString(tData["text1"]) then
		textobj1:SetText(tData["text1"])
	end
	if IsRealString(tData["text2"]) then
		textobj2:SetText(tData["text2"])
	end
end

function OnShowWindow(self, bVisible)
end

--------------------------
function PopupInDeskRight(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
    local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move( workright - nLayoutWidth - 7, workbottom - nLayoutHeight-5, nLayoutWidth, nLayoutHeight)
	return true
end


function PopupInMainWndCenter(objSelfWnd)
	local objtree = objSelfWnd:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local objMainWnd = tFunHelper.GetMainWndInst()
	local workleft, worktop, workright, workbottom = objMainWnd:GetWindowRect()
	local nMainWndW = workright - workleft
	local nMainWndH = workbottom - worktop
	
	objSelfWnd:Move((nMainWndW-nLayoutWidth)/2+workleft, (nMainWndH-nLayoutHeight)/2+worktop, nLayoutWidth, nLayoutHeight)
end


function SetShowText(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")

	local objVersion = objRootLayout:GetControlObject("About.MainInfo")
	if not objVersion then
		return
	end
	
	local strVersion = tFunHelper.GetProjectVersion()
	local strText = "版本号 ： V"..tostring(strVersion).." 正式版"
	objVersion:SetText(strText)
end


function IsRealString(str)
	return type(str) == "string" and str~=""
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@TipUpdateWnd: " .. tostring(strLog))
	end
end

