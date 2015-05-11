local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local tipAsynUtil = tFunHelper.tipAsynUtil
local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")

function OnCreate( self )
	PopupInDeskCenter(self)
	
	local objMainWnd = tFunHelper.GetMainWndInst()
	if objMainWnd:GetVisible() then
		PopupInMainWndCenter(self)
	end	
	local objtree = self:GetBindUIObjectTree()
	local ctrl = objtree:GetUIObject("root.layout")
	ctrl:SetData(self:GetUserData())
end

function SetData(self, tData)
	local attr = self:GetAttribute()
	attr.okfunc = tData["okfunc"]
	attr.selectfunc = tData["selectfunc"]
	attr.url = tData["url"]
	local name = self:GetControlObject("DownloadFile.FileDesc.Name")
	local size = self:GetControlObject("DownloadFile.FileDesc.Size")
	local ext = string.match(tostring(tData["name"]), "(%..*)$")
	TipLog("SetData, ext = "..tostring(ext))
	local xlgraphicplus = XLGetObject("Xunlei.XGP.Factory") 
	local icon = xlgraphicplus:LoadIconFromFileExt(ext, 32, 32)
	TipLog("SetData type icon = "..type(icon))
	if icon then
		local bmp = icon:GetBitmap()
		local imgobj = self:GetControlObject("DownloadFile.Picture")
		imgobj:SetBitmap(bmp)
		imgobj:SetObjPos2(21, 54, 32, 32)
	end
	if name and IsRealString(tData["name"]) then
		name:SetText(tData["name"])
	end
	if size and IsRealString(tData["size"]) then
		size:SetText(tData["size"])
	else
		size:SetText("未知大小")
	end
	local edit = self:GetControlObject("DownloadFile.DirChange.edit")
	if edit then
		local strSaveDir = tFunHelper.GetDefaultSaveDir()
		edit:SetText(strSaveDir)
	end
end

function OnPopDirSelectDialog(self)
	local strDir = tFunHelper.OpenFolderDialog()
	if IsRealString(strDir) then
		local objRootLayout = self:GetOwnerControl()
		local objDirEdit = objRootLayout:GetControlObject("DownloadFile.DirChange.edit")
		objDirEdit:SetText(strDir)
	end
end

function OnClose(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:EndDialog(0)
end

function OnSelect(self)
	local attr = self:GetAttribute()
	if attr.ForegroundResID == "DeleteTask.Select.Texture" then
		attr.ForegroundResID = "DeleteTask.UnSelect.Texture"
	else
		attr.ForegroundResID = "DeleteTask.Select.Texture"
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
	local selectctrl = ctrl:GetControlObject("DownloadFile.SelectBtn")
	local selattr = selectctrl:GetAttribute()
	if selattr.ForegroundResID == "DeleteTask.Select.Texture" then
		if type(atrlattr.selectfunc) == "function" then
			atrlattr.selectfunc()
		end
	end
	CreateNewTask(ctrl)
	OnClose(self)
	local objMainWnd = tFunHelper.GetMainWndInst()
	if objMainWnd then
		objMainWnd:SetVisible(true)
	end
end

function CreateNewTask(objRootCtrl)
	local atrlattr = objRootCtrl:GetAttribute()
	local objDirEdit = objRootCtrl:GetControlObject("DownloadFile.DirChange.edit")
	local objFileNameEdit = objRootCtrl:GetControlObject("DownloadFile.FileDesc.Name")
	
	local strURL = atrlattr.url
	local strSaveDir = objDirEdit:GetText()
	local strFileName = objFileNameEdit:GetText()
	
	if not IsRealString(strURL) or not IsRealString(strSaveDir) 
	   or not tipUtil:QueryFileExists(strSaveDir) or not IsRealString(strFileName) then
	    --ShowDescMessage(objRootCtrl, "请填写完整信息")
		return false
	end
	
	
	local tFileItem = {}
	tFileItem.strICOPath = ""
	tFileItem.hTaskHandle = -1
	
	tFileItem.tDownLoadConfig = {}
	tFileItem.tDownLoadConfig.strFileName = strFileName
	tFileItem.tDownLoadConfig.nFileSizeInKB = 0
	tFileItem.tDownLoadConfig.nDownSizeInKB = 0
	tFileItem.tDownLoadConfig.nFinishPercent = 0
	tFileItem.tDownLoadConfig.strFilePath = tipUtil:PathCombine(strSaveDir, strFileName)
	tFileItem.tDownLoadConfig.strFileURL = strURL
	tFileItem.tDownLoadConfig.bIsResume = true
	tFileItem.tDownLoadConfig.nFileState = tRabbitFileList.FILESTATE_START

	tRabbitFileList:PushFileItem(tFileItem)
	tFunHelper.UpdateFileList()
	
	return true
end

function OnEditFocusChange(self, isfocus)
	local bkg = self:GetParent()
	if isfocus then
		if bkg then
			bkg:SetTextureID("edit.long.select")
		end
	else
		if bkg then
			bkg:SetTextureID("edit.long.normal")
		end
	end
end

function OnShowWindow(self, bVisible)
end


function PopupInDeskCenter(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
    local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move(workleft + (workright -workleft - nLayoutWidth)/2, worktop + (workbottom - worktop - nLayoutHeight)/2, nLayoutWidth, nLayoutHeight)
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

--------------------------
function IsRealString(str)
	return type(str) == "string" and str~=""
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@DownLoadFileWnd: " .. tostring(strLog))
	end
end

