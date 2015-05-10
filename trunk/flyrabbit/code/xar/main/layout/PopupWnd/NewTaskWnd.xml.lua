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
	end
end

function OnClose(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:EndDialog(0)
end


function OnFocusURL(self, bFocus)
	SetEditBkg(self, bFocus)
			
	if not bFocus then
		local objRootCtrl = self:GetOwnerControl()
		AsynCall(function ()
			local strText = self:GetText()
			if IsRealString(strText) then
				local strSaveName = GetFileSaveNameFromUrl(strText)
				if IsRealString(strSaveName) then
					local objRootLayout = self:GetOwnerControl()
					local objFileNameEdit = objRootLayout:GetControlObject("NewTask.FileName.edit")
					objFileNameEdit:SetText(strSaveName)
				end
			end
			ShowDiskInfo(objRootCtrl)
			SetFileSizeFromUI(objRootCtrl)
		end)
	end
end


function OnFocusSavePath(self, bFocus)
	SetEditBkg(self, bFocus)
	
	if not bFocus then
		local objRootCtrl = self:GetOwnerControl()
		AsynCall(function ()
			ShowDiskInfo(objRootCtrl)
		end)
	end
end


function OnFocusFileName(self, bFocus)
	SetEditBkg(self, bFocus)

end


function SetEditBkg(self, isfocus)
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
	if IsRealString(strDir) then
		local objRootLayout = self:GetOwnerControl()
		local objDirEdit = objRootLayout:GetControlObject("NewTask.SavePath.edit")
		objDirEdit:SetText(strDir)
	end
end


function OnShowWindow(self, bVisible)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
	
	ResetAllText(objRootLayout)
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
	    ShowDescMessage(objRootCtrl, "请填写完整信息")
		return false
	end
	
	-- local bSucc, nFileSizeInKB = tRabbitFileList:GetFileSizeWithUrlInKB(strURL)
	-- if not bSucc then
		-- ShowDescMessage(objRootCtrl, "请检查下载链接")
		-- return false
	-- end
	
	local tFileItem = {}
	tFileItem.strICOPath = ""
	tFileItem.hTaskHandle = -1
	
	tFileItem.tDownLoadConfig = {}
	tFileItem.tDownLoadConfig.strFileName = strFileName
	tFileItem.tDownLoadConfig.nFileSizeInKB = nFileSizeInKB or 0
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


function ShowDiskInfo(objRootCtrl)
	local strFileSize = "文件大小未知,"
	local attr = objRootCtrl:GetAttribute()
	if IsRealString(attr.strFileSize) then
		strFileSize = "文件大小"..tostring(attr.strFileSize)..","
	end
	-- local bSucc, strSize = GetFileSizeFromUI(objRootCtrl)	
	-- if bSucc then
		-- strFileSize = "文件大小"..tostring(strSize)..","
	-- end

	local strDiskInfo = "未知路径"
	local objDirEdit = objRootCtrl:GetControlObject("NewTask.SavePath.edit")
	local strPath = objDirEdit:GetText()
	local _, _, strDiskName = string.find(tostring(strPath), "^(.):\\.*")
	local bRet, strFormatSize = GetDiskSizeFromUI(objRootCtrl)
			
	if bRet and IsRealString(strDiskName) then
		strDiskInfo = strDiskName.."盘剩余空间:"..strFormatSize
	end
	
	local strMessage = strFileSize..strDiskInfo
	ShowDescMessage(objRootCtrl,strMessage)
end


function SetFileSizeFromUI(objRootCtrl)
	local objURLEdit = objRootCtrl:GetControlObject("NewTask.Url.edit")	
	local strURL = objURLEdit:GetText()
	tRabbitFileList:AsynGetFileSizeWithUrlInKB(strURL,function(iRet, nFileSizeInKB)
		if iRet == 0 and nFileSizeInKB > 0 then
			local strSize = tFunHelper.FormatFileSize(nFileSizeInKB)
			local attr = objRootCtrl:GetAttribute()
			attr.strFileSize = strSize
			ShowDiskInfo(objRootCtrl)
		end
	end)
	-- local bSucc, nFileSizeInKB = tRabbitFileList:GetFileSizeWithUrlInKB(strURL)
	-- local strSize = ""
	
	-- if bSucc then
		-- local strSize = tFunHelper.FormatFileSize(nFileSizeInKB)
	-- end
	
	-- return bSucc, strSize
end


function GetDiskSizeFromUI(objRootCtrl)
	local bRet = false
	local objDirEdit = objRootCtrl:GetControlObject("NewTask.SavePath.edit")
	local strPath = objDirEdit:GetText()
	
	local strFormatSize = ""
	local nDiskSizeInKB = tFunHelper.GetDiskSizeInKB(strPath)
	if nDiskSizeInKB >= 0 then
		strFormatSize = tFunHelper.FormatFileSize(nDiskSizeInKB)
		bRet = true
	end
	
	return bRet, strFormatSize
end


function ShowDescMessage(objRootCtrl, strMessage)
	local objFileDesc = objRootCtrl:GetControlObject("NewTask.FileDesc.Text")
	objFileDesc:SetText(strMessage)
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

function GetFileSaveNameFromUrl(url)
	local _, _, strFileName = string.find(tostring(url), ".*/(.*)$")
	if not IsRealString(strFileName) then
		return url
	end
	local npos = string.find(strFileName, "?", 1, true)
	if npos ~= nil then
		strFileName = string.sub(strFileName, 1, npos-1)
	end
	return strFileName
end