local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")
local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local tipAsynUtil = tFunHelper.tipAsynUtil

function OnCreate( self )
	PopupInDeskRight(self)
	
	local objMainWnd = tFunHelper.GetMainWndInst()
	if objMainWnd:GetVisible() then
		PopupInMainWndCenter(self)
	end	
end

-----
function SetData(self, tData)
	local textobj = self:GetControlObject("DeleteAllTask.MainInfo") 
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
	SetWindowPos(objHostWnd)
end


------

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
	local selectctrl = ctrl:GetControlObject("DeleteAllTask.SelectBtn")
	local selattr = selectctrl:GetAttribute()
	if selattr.ForegroundResID == "DeleteTask.Select.Texture" then
		if type(atrlattr.selectfunc) == "function" then
			atrlattr.selectfunc()
		end
	end
	
	DeleteAllFileItem(ctrl)
	OnClose(self)
end


function OnShowWindow(self, bVisible)

end


------
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


function DeleteAllFileItem(objRootCtrl)
	local attr = objRootCtrl:GetAttribute()
	local tFileList = tRabbitFileList:GetFileList()
	local nTotalFileNum = #tFileList
	
	for nIndex=nTotalFileNum, 1, -1 do
		local objFileItem = tFunHelper.GetFileItemUIByIndex(nIndex)
		if objFileItem then
			objFileItem:StopQueryTimer()
		end
	
		tRabbitFileList:RemoveFileItem(nIndex, attr.bDeleteFile)
	end
	
	tFunHelper.UpdateFileList()
end


--------------------------
function IsRealString(str)
	return type(str) == "string" and str~=""
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@TipUpdateWnd: " .. tostring(strLog))
	end
end

