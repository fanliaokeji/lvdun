local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")
local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local tipAsynUtil = tFunHelper.tipAsynUtil

function OnCreate( self )
	PopupInDeskCenter(self)
	
	local objMainWnd = tFunHelper.GetMainWndInst()
	local state = objMainWnd:GetWindowState()
	if objMainWnd:GetVisible() and (state == "normal" or state == "max") then
		PopupInMainWndCenter(self)
	end
	self:BringWindowToTop(true)
end

---方法
function SetData(self, tData)
	if type(tData) ~= "table" then
		return
	end

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
	if IsRealString(tData["url"]) then
		local name = GetTaskSaveNameFromUrl(tData["url"])
		if filename then
			filename:SetText(name)
		end
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


function BeginDownLoad(self)
	local ctrl = self
	local objDirEdit = ctrl:GetControlObject("NewTask.SavePath.edit")
	local strSaveDir = objDirEdit:GetText()
	if IsRealString(strSaveDir) and not tipUtil:QueryFileExists(strSaveDir) then
		tipUtil:CreateDir(strSaveDir)
	end
	local atrlattr = ctrl:GetAttribute()
	if type(atrlattr.okfunc) == "function" then
		atrlattr.okfunc()
	end
	
	local bRet = CreateNewTask(ctrl)
	if bRet then
		OnClose(self)
	end
end


------事件
function OnClickOK(self)
	local ctrl = self:GetOwnerControl()
	ctrl:BeginDownLoad()
end

function OnClose(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:EndDialog(0)
end


function OnFocusURL(self, bFocus)
	SetEditBkg(self, bFocus)
end

function OnChangeURL(self)
	local text = self:GetText()
	local bRet,strFileName = GetTaskSaveNameFromUrl(text)
	local objRootLayout = self:GetOwnerControl()
	if strFileName ~= nil then
		local objFileNameEdit = objRootLayout:GetControlObject("NewTask.FileName.edit")
		objFileNameEdit:SetText(strFileName)
	end
	if bRet then
		local objRootLayout = self:GetOwnerControl()
		local attr = objRootLayout:GetAttribute()
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
		if attr.nTimerID_GetUrlSize ~= nil then
			timerManager:KillTimer(attr.nTimerID_GetUrlSize)
			attr.nTimerID_GetUrlSize = nil
		end
		attr.nTimerID_GetUrlSize = timerManager:SetTimer(function(item, id)
			timerManager:KillTimer(id)
			attr.nTimerID_GetUrlSize = nil
			SetFileSizeFromUI()
		end, 1*1000)
	else
		local attr = objRootLayout:GetAttribute()
		attr.strFileSize = nil
		ShowDiskInfo(objRootLayout)
	end
end


function OnFocusSavePath(self, bFocus)
	SetEditBkg(self, bFocus)
end


function OnEditRBtnUp(self)
	tFunHelper.TryDestroyOldMenu(self, "RBtnEditMenu", 0, true)
	tFunHelper.CreateAndShowMenu(self, "RBtnEditMenu", 0, true)
end


function OnChangeSavePath(self)
	local objRootLayout = self:GetOwnerControl()
	-- local attr = objRootLayout:GetAttribute()
	-- local strSaveDir = self:GetText()
	-- local _, _, strDiskName = string.find(tostring(strSaveDir), "^(.):\\.*")
	-- if not IsRealString(strDiskName) then
		-- attr.strDiskInfo = nil
		-- ShowDiskInfo(objRootLayout)
		-- return
	-- end
	-- local bRet, strFormatSize = GetDiskSizeFromUI(strDiskName .. ":\\")
	-- if bRet and IsRealString(strFormatSize) then
		-- local strDiskInfo = strDiskName.."盘剩余空间:"..strFormatSize
		-- if attr.strDiskInfo ~= strDiskInfo then
			-- attr.strDiskInfo = strDiskInfo
			-- ShowDiskInfo(objRootLayout)
		-- end
	-- else
		-- attr.strDiskInfo = nil
		-- ShowDiskInfo(objRootLayout)
	-- end
	SetDiskAttribute(objRootLayout)
	ShowDiskInfo(objRootLayout)
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
	objRootLayout:SetData(self:GetUserData())
end

------
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


function ResetAllText(objRootLayout)
	local objURLEdit = objRootLayout:GetControlObject("NewTask.Url.edit")
	local objDirEdit = objRootLayout:GetControlObject("NewTask.SavePath.edit")
	local objFileNameEdit = objRootLayout:GetControlObject("NewTask.FileName.edit")
	
	objURLEdit:SetText("")
	objFileNameEdit:SetText("")
	
	local strSaveDir = tFunHelper.GetDownFileSaveDir()
	objDirEdit:SetText(strSaveDir)	
	
	-- local _, _, strDiskName = string.find(tostring(strSaveDir), "^(.):\\.*")
	-- if IsRealString(strDiskName) then
		-- local bRet, strFormatSize = GetDiskSizeFromUI(strDiskName .. ":\\")
		-- if bRet and IsRealString(strFormatSize) then
			-- strDiskInfo = strDiskName.."盘剩余空间:"..strFormatSize
			-- local attr = objRootLayout:GetAttribute()
			-- attr.strDiskInfo = strDiskInfo
		-- end
	-- end	
	SetDiskAttribute(objRootLayout)
	ShowDiskInfo(objRootLayout)
end

function SetDiskAttribute(objRootLayout)
	local objDirEdit = objRootLayout:GetControlObject("NewTask.SavePath.edit")
	local strSaveDir = objDirEdit:GetText()
	local attr = objRootLayout:GetAttribute()
	if not IsRealString(strSaveDir) then
		attr.strDiskInfo = nil
		return
	end
	local _, _, strDiskName = string.find(tostring(strSaveDir), "^(.):\\.*")
	if not IsRealString(strDiskName) then
		attr.strDiskInfo = nil
		return
	end
	local bRet, strFormatSize = GetDiskSizeFromUI(strDiskName .. ":\\")
	if bRet and IsRealString(strFormatSize) then
		strDiskInfo = strDiskName.."盘剩余空间:"..strFormatSize
		attr.strDiskInfo = strDiskInfo
	else
		attr.strDiskInfo = nil
	end
end

function CreateNewTask(objRootLayout)
	local objURLEdit = objRootLayout:GetControlObject("NewTask.Url.edit")
	local objDirEdit = objRootLayout:GetControlObject("NewTask.SavePath.edit")
	local objFileNameEdit = objRootLayout:GetControlObject("NewTask.FileName.edit")
	
	local strURL = objURLEdit:GetText()
	local strSaveDir = objDirEdit:GetText()
	local strFileName = objFileNameEdit:GetText()
	
	if not IsRealString(strURL) or not IsRealString(strSaveDir) 
	   or not tipUtil:QueryFileExists(strSaveDir) or not IsRealString(strFileName) then
	    ShowDescMessage(objRootLayout, "请填写完整信息")
		return false
	end
	
	local tFileItem = {}
	tFileItem.strICOPath = ""
	tFileItem.hTaskHandle = -1
	
	tFileItem.tDownLoadConfig = {}
	tFileItem.tDownLoadConfig.strFileName = strFileName
	tFileItem.tDownLoadConfig.nFileSizeInByte = nFileSizeInByte or 0
	tFileItem.tDownLoadConfig.nDownSizeInByte = 0
	tFileItem.tDownLoadConfig.nFinishPercent = 0
	tFileItem.tDownLoadConfig.strFileDir = strSaveDir -- tipUtil:PathCombine(strSaveDir, strFileName)
	tFileItem.tDownLoadConfig.strFileURL = strURL
	tFileItem.tDownLoadConfig.bIsResume = true
	tFileItem.tDownLoadConfig.nFileState = tRabbitFileList.FILESTATE_START
	tRabbitFileList:PushFileItem(tFileItem)
	tFunHelper.UpdateFileList()
	
	SendNewTaskReport(strURL)
	tFunHelper.SetUserSetSaveDir(strSaveDir)
	return true
end


function ShowDiskInfo(objRootLayout)
	local strFileSize = "文件大小未知,"
	local attr = objRootLayout:GetAttribute()
	if IsRealString(attr.strFileSize) then
		strFileSize = "文件大小"..tostring(attr.strFileSize)..","
	end

	local strDiskInfo = "未知路径"
	if IsRealString(attr.strDiskInfo) then
		strDiskInfo  = attr.strDiskInfo
	end
	
	local strMessage = strFileSize..strDiskInfo
	ShowDescMessage(objRootLayout,strMessage)
end


function SetFileSizeFromUI()
	objRootLayout = GetRootLayout()
	if objRootLayout == nil then
		return
	end
	local objURLEdit = objRootLayout:GetControlObject("NewTask.Url.edit")	
	local strURL = objURLEdit:GetText()
	tRabbitFileList:AsynGetFileSizeWithUrlInByte(strURL,function(iRet, nFileSizeInByte)
		local objRootLayout = GetRootLayout()
		if iRet == 0 and nFileSizeInByte > 0 then
			local strSize = tFunHelper.FormatFileSize(nFileSizeInByte)
			if objRootLayout == nil then
				return
			end
			local attr = objRootLayout:GetAttribute()
			attr.strFileSize = strSize
			ShowDiskInfo(objRootLayout)
		else
			local attr = objRootLayout:GetAttribute()
			attr.strFileSize = nil
			ShowDiskInfo(objRootLayout)
		end
	end)
end


function GetDiskSizeFromUI(strDiskName)
	local bRet = false
	-- local objDirEdit = objRootLayout:GetControlObject("NewTask.SavePath.edit")
	-- local strPath = objDirEdit:GetText()
	-- if not tipUtil:QueryFileExists(strPath) then
		-- tipUtil:CreateDir(strPath)
	-- end
	local strFormatSize = ""
	local nDiskSizeInByte = tFunHelper.GetDiskSizeInByte(strDiskName)
	if nDiskSizeInByte >= 0 then
		strFormatSize = tFunHelper.FormatFileSize(nDiskSizeInByte)
		bRet = true
	end
	
	return bRet, strFormatSize
end


function ShowDescMessage(objRootLayout, strMessage)
	local objFileDesc = objRootLayout:GetControlObject("NewTask.FileDesc.Text")
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


function GetTaskSaveNameFromUrl(strUrl)
	if not IsRealString(strUrl) then
		return false,""
	end
	local strRealUrl = tRabbitFileList:ParseThunderPrivateUrl(strUrl)
	local _,_,strUrlWithOutProtocol = string.find(strRealUrl,".+://(.+)")
	if not IsRealString(strUrlWithOutProtocol) then
		return false,""
	end
	local _, _, strFileName = string.find(tostring(strUrlWithOutProtocol), ".*/(.*)$")
	if not IsRealString(strFileName) then
		strFileName = "index.html"
	end
	local npos = string.find(strFileName, "?", 1, true)
	if npos ~= nil then
		strFileName = string.sub(strFileName, 1, npos-1)
	end
	return true,strFileName
end


function GetRootLayout()
	local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local objNewTaskWnd = hostWndManager:GetHostWnd("TipNewTaskWnd.Instance")
	if objNewTaskWnd == nil then
		return nil
	end
	local objtree = objNewTaskWnd:GetBindUIObjectTree()
	if objtree == nil then
		return nil
	end
	local objRootLayout = objtree:GetUIObject("root.layout")
	if objRootLayout == nil then
		return nil
	end
	return objRootLayout
end


function SendNewTaskReport(strURL)
	local FunctionObj = XLGetGlobal("Project.FunctionHelper") 
	local tStatInfo = {}
	tStatInfo.strEC = "newtask"
	tStatInfo.strEA = FunctionObj.UrlEncode(strURL)
	tStatInfo.strEL = FunctionObj.GetInstallSrc() or ""
	tStatInfo.strEV = 1
	
	FunctionObj.TipConvStatistic(tStatInfo)
end

