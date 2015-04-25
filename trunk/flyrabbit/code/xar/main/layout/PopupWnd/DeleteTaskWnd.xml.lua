local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")
local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local tipAsynUtil = tFunHelper.tipAsynUtil

function OnCreate( self )
	PopupInMainWndCenter(self)
end

----
function SetData(self, tData)
	local textobj = self:GetControlObject("DeleteTask.MainInfo") 
	if IsRealString(tData["text"]) then
		textobj:SetText(tData["text"])
	end
	local attr = self:GetAttribute()
	attr.okfunc = tData["okfunc"]
	attr.selectfunc = tData["selectfunc"]
end

function MoveWindowToCenter(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	PopupInMainWndCenter(objHostWnd)
end
---

function OnClose(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:EndDialog(0)
end

function OnSelect(self)
	local objRootCtrl = self:GetOwnerControl()
	local RootAttr = objRootCtrl:GetAttribute()
	local attr = self:GetAttribute()
	if attr.ForegroundResID == "DeleteTask.Select.Texture" then
		attr.ForegroundResID = "DeleteTask.UnSelect.Texture"
		RootAttr.bDeleteFile = false
	else
		attr.ForegroundResID = "DeleteTask.Select.Texture"
		RootAttr.bDeleteFile = true
	end
	local foreground = self:GetControlObject("Foreground")
	foreground:SetTextureID(attr.ForegroundResID)
end

function OnClickOK(self)
	local ctrl = self:GetOwnerControl()
	local atrlattr = ctrl:GetAttribute()
	if type(atrlattr.okfunc) == "function" then
		atrlattr.okfunc()
	end
	local selectctrl = ctrl:GetControlObject("DeleteTask.SelectBtn")
	local selattr = selectctrl:GetAttribute()
	if selattr.ForegroundResID == "DeleteTask.Select.Texture" then
		if type(atrlattr.selectfunc) == "function" then
			atrlattr.selectfunc()
		end
	end
	
	DeleteSelectFileItem(ctrl)
	OnClose(self)
end

function OnShowWindow(self, bVisible)
end

--------------------------
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


function DeleteSelectFileItem(objRootCtrl)
	local objFileItem = tFunHelper.GetSelectItemObject()
	if not objFileItem then
		return
	end
		
	local nIndex = objFileItem:GetItemIndex()
	
	local attr = objRootCtrl:GetAttribute()
	if attr.bDeleteFile then
		local strFilePath = tRabbitFileList:GetFilePath(nIndex)
		tipUtil:DeletePathFile(strFilePath)
	end
	
	tRabbitFileList:RemoveFileItem(nIndex)
	tFunHelper.UpdateFileList()
end


----------
function IsRealString(str)
	return type(str) == "string" and str~=""
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@TipUpdateWnd: " .. tostring(strLog))
	end
end

