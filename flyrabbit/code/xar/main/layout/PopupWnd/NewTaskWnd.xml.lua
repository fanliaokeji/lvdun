local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")
local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local tipAsynUtil = tFunHelper.tipAsynUtil

function OnCreate( self )
	SetWindowPos(self)
end

---
function SetData(self, tData)
	local attr = self:GetAttribute()
	attr.okfunc = tData["okfunc"]
	local url = self:GetControlObject("NewTask.Url.edit")
	local savepath = self:GetControlObject("NewTask.SavePath.edit")
	local filename = self:GetControlObject("NewTask.FileName.edit")
	local filedesc = self:GetControlObject("NewTask.FileDesc.Text")
	if url and IsRealString(tData["url"]) then
		url:SetText(tData["url"])
	end
	if savepath and IsRealString(tData["savepath"]) then
		savepath:SetText(tData["savepath"])
	end
	if filename and IsRealString(tData["filename"]) then
		filename:SetText(tData["filename"])
	end
	if filedesc and IsRealString(tData["filedesc"]) then
		filedesc:SetText(tData["filedesc"])
	end
end

function MoveWindowToCenter(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	SetWindowPos(objHostWnd)
end

function OnClickOK(self)
	local ctrl = self:GetOwnerControl()
	local atrlattr = ctrl:GetAttribute()
	if type(atrlattr.okfunc) == "function" then
		atrlattr.okfunc()
	end
	
	local bRet = CreateNewTask(ctrl)
	if bRet then
		OnClose(self)
	else
		ShowErrorMessage(ctrl)
	end
end

function OnClose(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end

function OnEditFocusChange(self, isfocus)
	local bkg = self:GetParent()
	if isfocus then
		if bkg then
			if bkg:GetID() == "NewTask.Url.Bkg" then
				bkg:SetTextureID("edit.251.select")
			else
				bkg:SetTextureID("edit.229.select")
			end
		end
	else
		if bkg then
			if bkg:GetID() == "NewTask.Url.Bkg" then
				bkg:SetTextureID("edit.251.normal")
			else
				bkg:SetTextureID("edit.229.normal")
			end
		end
	end
end

function OnPopDirSelectDialog(self)
	local strDir = tFunHelper.OpenFolderDialog()
end


function OnShowWindow(self, bVisible)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
	
	ResetAllText(objRootLayout)
end

------
function SetWindowPos(objSelfWnd)
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


function ResetAllText(objRootLayout)
	local objURLEdit = objRootLayout:GetControlObject("NewTask.Url.edit")
	local objDirEdit = objRootLayout:GetControlObject("NewTask.SavePath.edit")
	local objFileNameEdit = objRootLayout:GetControlObject("NewTask.FileName.edit")
	
	objURLEdit:SetText("")
	objFileNameEdit:SetText("")
	
	local strSaveDir = tFunHelper.GetDefaultSaveDir()
	objDirEdit:SetText(strSaveDir)	
end


function CreateNewTask(objRootCtrl)
	local objURLEdit = objRootCtrl:GetControlObject("NewTask.Url.edit")
	local objDirEdit = objRootCtrl:GetControlObject("NewTask.SavePath.edit")
	local objFileNameEdit = objRootCtrl:GetControlObject("NewTask.FileName.edit")
	
	local strURL = objURLEdit:GetText()
	local strSaveDir = objDirEdit:GetText()
	local strFileName = objFileNameEdit:GetText()
	
	if not IsRealString(strURL) or not IsRealString(strSaveDir) 
	   or not tipUtil:QueryFileExists(strSaveDir) or not IsRealString(strFileName) then
		return false
	end
	
	local tFileItem = {}
	tFileItem.strFileName = strFileName
	tFileItem.strICOPath = ""
	tFileItem.nFileSizeInKB = 0
	tFileItem.nDownSizeInKB = 0
	tFileItem.nFinishPercent = 0
	tFileItem.strFilePath = tipUtil:PathCombine(strSaveDir, strFileName)
	tFileItem.strFileURL = strURL
	tFileItem.nFileState = tRabbitFileList.FILESTATE_START
		
	tRabbitFileList:PushFileItem(tFileItem)
	tFunHelper.UpdateFileList()
	
	return true
end


function ShowErrorMessage(objRootCtrl, strMessage)
	local objFileDesc = objRootCtrl:GetControlObject("NewTask.FileDesc.Text")
	objFileDesc:SetText("请填写完整信息")
end

-----------

function IsRealString(str)
	return type(str) == "string" and str~=""
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@TipUpdateWnd: " .. tostring(strLog))
	end
end

