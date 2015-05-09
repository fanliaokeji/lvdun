local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")
local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

--方法
function SetFileContent(self, tFileContent)
	if type(tFileContent) ~= "table" or type(tFileContent.tDownLoadConfig) ~= "table" then
		return
	end

	SetFileNameUI(self, tFileContent.tDownLoadConfig.strFileName)
	SetFileStateUI(self, tFileContent)
	StartQueryTimer(self)
end


function SetItemIndex(self, nIndex)
	local attr = self:GetAttribute()
	attr.nItemIndex = nIndex	
end

function GetItemIndex(self, nIndex)
	local attr = self:GetAttribute()
	return attr.nItemIndex
end

function SetSelectState(objRootCtrl, bSelect)
	ShowSelectBkg(objRootCtrl, bSelect)
	
	local attr = objRootCtrl:GetAttribute()
	attr.bSelect = bSelect
end


function UpdateFileItemStyle(objRootCtrl)
	local nIndex = objRootCtrl:GetItemIndex()
	local tFileItem = tRabbitFileList:GetFileItemByIndex(nIndex)
	SetFileStateUI(objRootCtrl, tFileItem)
end


---事件
function OnInitControl(self)
	
end

function OnControlMouseEnter(self)
	ShowSelectBkg(self, true)
end


function OnControlMouseLeave(self)
	local attr = self:GetAttribute()
	if attr.bSelect then
		return
	end

	ShowSelectBkg(self, false)
end


function OnLButtonUp(self)
	self:SetSelectState(true)
	self:FireExtEvent("OnClick")
end


function OnClickStart(self)
	local objRootCtrl = self:GetOwnerControl()
	UpdateFileState(objRootCtrl, tRabbitFileList.FILESTATE_START)
	RouteToFather(self)
	
	StartQueryTimer(objRootCtrl)
end


function OnClickPause(self)
	local objRootCtrl = self:GetOwnerControl()
	UpdateFileState(objRootCtrl, tRabbitFileList.FILESTATE_PAUSE)
	RouteToFather(self)
end


function OnClickDelete(self)
	tFunHelper.ShowModalDialog("TipDeleteTaskWnd", "TipDeleteTaskWnd.Instance", "DeleteTaskTree", "DeleteTaskTree.Instance")	
	RouteToFather(self)
end


function OnClickReDownLoad(self)
	RouteToFather(self)
	local objRootCtrl = self:GetOwnerControl()
	UpdateFileState(objRootCtrl, tRabbitFileList.FILESTATE_START)
end


function OnClickOpenFile(self)
	local objRootCtrl = self:GetOwnerControl()
	local nIndex = objRootCtrl:GetItemIndex()
	local strFilePath = tRabbitFileList:GetFilePath(nIndex)
	if IsRealString(strFilePath) and tipUtil:QueryFileExists(strFilePath) then
		local bRet = tipUtil:ShellExecute(0, "open", strFilePath, "", 0, "SW_SHOW")
	end	
end


function OnClickOpenDir(self)
	local objRootCtrl = self:GetOwnerControl()
	local nIndex = objRootCtrl:GetItemIndex()
	local strFilePath = tRabbitFileList:GetFilePath(nIndex)
	if not IsRealString(strFilePath) then
		return
	end
	
	local strDir = tFunHelper.GetFileDirFromPath(strFilePath)
	if IsRealString(strDir) and tipUtil:QueryFileExists(strDir) then
		tipUtil:ShellExecute(0, "open", strDir, "", 0, "SW_SHOW")
	end	
end


----
function StartQueryTimer(objRootCtrl)
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	local attr = objRootCtrl:GetAttribute()
	
	local nIndex = objRootCtrl:GetItemIndex()
	local tFileItem = tRabbitFileList:GetFileItemByIndex(nIndex)
	local nState = tRabbitFileList:GetFileItemState(nIndex)
	
	if nState ~= tRabbitFileList.FILESTATE_START then
		return
	end
	
	if attr.hQueryTimer ~= nil then
		timerManager:KillTimer(attr.hQueryTimer)
		attr.hQueryTimer = nil
	end
	
	attr.hQueryTimer = timerManager:SetTimer(function(item, id)
		local bRet, tQueryInfo = tRabbitFileList:QueryTask(tFileItem)

		if bRet and type(tQueryInfo) == "table" then
			UpdateFileInfoAndUI(objRootCtrl, nIndex, tQueryInfo)
		end		
	end, 1*1000)
end


function UpdateFileInfoAndUI(objRootCtrl, nIndex, tQueryInfo)
    UpdateFileName(nIndex, tQueryInfo.szFilename)
	UpdateDownSize(nIndex, tQueryInfo.nTotalDownload)
	UpdateFileSize(nIndex, tQueryInfo.nTotalSize)
	
	local tFileItem = tRabbitFileList:GetFileItemByIndex(nIndex)
	SetFileStateUI(objRootCtrl, tFileItem)
end


function UpdateFileName(nIndex, strQueryName)
	local strFileName = tRabbitFileList:GetFileItemText(nIndex)
	if tostring(strFileName) ~= tostring(strQueryName) then
		tRabbitFileList:SetFileItemText(strQueryName)
	end
end


function UpdateDownSize(nIndex, nQuerySizeInByte)
	local nOldDownSize = tRabbitFileList:GetDownSizeInKB(nIndex)
	local nQuerySizeInKB = 1 
	if nQuerySizeInByte > 1024 then
		nQuerySizeInKB = math.floor(nQuerySizeInByte/1024)
	end
	if nOldDownSize < nQuerySizeInKB then
		tRabbitFileList:SetDownSizeInKB(nIndex, nQuerySizeInKB)
	end
end


function UpdateFileSize(nIndex, nQuerySizeInByte)
	local nOldFileSize = tRabbitFileList:GetFileSizeInKB(nIndex)
	local nQuerySizeInKB = 1 
	if nQuerySizeInByte > 1024 then
		nQuerySizeInKB = math.floor(nQuerySizeInByte/1024)
	end
	if nOldFileSize < nQuerySizeInKB then
		tRabbitFileList:SetFileSizeInKB(nIndex, nQuerySizeInKB)
	end
end


function SetFileNameUI(objRootCtrl, strFileName)
	local objFileName = objRootCtrl:GetControlObject("DownLoad.FileName")
	objFileName:SetText(strFileName)
end


function SetFileSizeUI(objRootCtrl, tFileContent)
	local tDownLoadConfig = tFileContent.tDownLoadConfig

	local nDownSizeInKB = tDownLoadConfig.nDownSizeInKB
	local nFileSizeInKB = tDownLoadConfig.nFileSizeInKB
		
	if tonumber(nDownSizeInKB) == nil or tonumber(nFileSizeInKB) == nil then
		return
	end
	
	local nPercent = 0
	if nFileSizeInKB ~= 0 then
		nPercent = nDownSizeInKB*100/nFileSizeInKB 
	end
	
	local strPercent = string.format("%.1f", nPercent)
	local strDownSize = tFunHelper.FormatFileSize(tDownLoadConfig.nDownSizeInKB)
	local strFileSize = tFunHelper.FormatFileSize(tDownLoadConfig.nFileSizeInKB)
	
	local strText = strPercent.."%   "..strDownSize.." / "..strFileSize
	SetFileStateText(objRootCtrl, strText)
	SetProgressBarState(objRootCtrl, nPercent)
end


function SetFileStateText(objRootCtrl, strText)
	local objFileName = objRootCtrl:GetControlObject("DownLoad.FileSize")
	objFileName:SetText(strText or "")
end


function ShowErrorBkg(objRootCtrl, bShow)
	local objErrorBkg = objRootCtrl:GetControlObject("DownLoad.ErrorBkg")
	objErrorBkg:SetVisible(bShow)
end


function ShowProgressBar(objRootCtrl, bShow)
	local objProgressBar = objRootCtrl:GetControlObject("DownLoad.ProgressBar")
	objProgressBar:SetVisible(bShow)
	objProgressBar:SetChildrenVisible(bShow)
end


function SetFileStateUI(objRootCtrl, tFileContent)
	if type(tFileContent) ~= "table" or type(tFileContent.tDownLoadConfig) ~= "table" then
		return false
	end

	local tDownLoadConfig = tFileContent.tDownLoadConfig

	if tonumber(tDownLoadConfig.nFileState) == nil or tDownLoadConfig.nFileState < 0 then
		return
	end

	SetFileSizeUI(objRootCtrl, tFileContent)
	ShowErrorBkg(objRootCtrl, false)
	ShowProgressBar(objRootCtrl, true)
	
	local nFileState = tDownLoadConfig.nFileState
	local objStartBtn = objRootCtrl:GetControlObject("DownLoad.StartBtn")
	local objPauseBtn = objRootCtrl:GetControlObject("DownLoad.PauseBtn")
	local objReDownLoad = objRootCtrl:GetControlObject("DownLoad.ReDownLoad")
	local objDownLoadFinish = objRootCtrl:GetControlObject("DownLoad.DownLoadFinish")
	
	objStartBtn:SetVisible(false)
	objStartBtn:SetChildrenVisible(false)
	objPauseBtn:SetVisible(false)
	objPauseBtn:SetChildrenVisible(false)
	objReDownLoad:SetVisible(false)
	objReDownLoad:SetChildrenVisible(false)
	objDownLoadFinish:SetVisible(false)
	objDownLoadFinish:SetChildrenVisible(false)
	
	if nFileState == tRabbitFileList.FILESTATE_START then
		objPauseBtn:SetVisible(true)
		objPauseBtn:SetChildrenVisible(true)
	elseif nFileState == tRabbitFileList.FILESTATE_PAUSE then
		objStartBtn:SetVisible(true)
		objStartBtn:SetChildrenVisible(true)
		SetFileStateText(objRootCtrl, "暂停")
		
	elseif nFileState == tRabbitFileList.FILESTATE_FINISH then
		objDownLoadFinish:SetVisible(true)
		objDownLoadFinish:SetChildrenVisible(true)
		ShowProgressBar(objRootCtrl, false)
		
	elseif nFileState == tRabbitFileList.FILESTATE_ERROR then
		objReDownLoad:SetVisible(true)
		objReDownLoad:SetChildrenVisible(true)
		SetFileStateText(objRootCtrl, "目标文件不存在")
		ShowErrorBkg(objRootCtrl, true)
		ShowProgressBar(objRootCtrl, false)
	end
end


function SetProgressBarState(objRootCtrl, nPercent)
	local objProgress = objRootCtrl:GetControlObject("DownLoad.ProgressBar")
	local nProg = nPercent
	objProgress:SetProgress(nProg)
end


function UpdateFileState(objRootCtrl, nFileState)
	local nIndex = objRootCtrl:GetItemIndex()
	tRabbitFileList:SetFileItemState(nIndex, nFileState)
end


function ShowSelectBkg(objRootCtrl, bShow)
	local objSelectBkg = objRootCtrl:GetControlObject("DownLoad.SelectBkg")
	objSelectBkg:SetVisible(bShow)
end

------------------
function RouteToFather(self)
	self:RouteToFather()
end

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

