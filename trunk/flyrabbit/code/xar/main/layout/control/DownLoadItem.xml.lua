local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")
local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

--方法
function SetFileContent(self, tFileContent)
	if type(tFileContent) ~= "table" then
		return
	end

	SetFileName(self, tFileContent.strFileName)
	SetFileStateUI(self, tFileContent)
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
	
	-- local objProgress = self:GetOwnerControl():GetControlObject("DownLoad.ProgressBar")
	
	-- local nProg = 1
	-- local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	-- timerManager:SetTimer(function(item, id)
		-- objProgress:SetProgress(nProg)
		-- nProg = nProg+1
	-- end, 100)

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

function SetFileName(objRootCtrl, strFileName)
	local objFileName = objRootCtrl:GetControlObject("DownLoad.FileName")
	objFileName:SetText(strFileName)
end


function SetFileSize(objRootCtrl, tFileContent)
	local nDownSizeInKB = tFileContent.nDownSizeInKB
	local nFileSizeInKB = tFileContent.nFileSizeInKB
	
	if tonumber(nDownSizeInKB) == nil or tonumber(nFileSizeInKB) == nil then
		return
	end
	
	local nPercent = 0
	if nFileSizeInKB ~= 0 then
		nPercent = nDownSizeInKB*100/nFileSizeInKB 
	end
	
	local strPercent = string.format("%.1f", nPercent)
	local strDownSize = tFunHelper.FormatFileSize(tFileContent.nDownSizeInKB)
	local strFileSize = tFunHelper.FormatFileSize(tFileContent.nFileSizeInKB)
	
	local strText = strPercent.."%   "..strDownSize.." / "..strFileSize
	SetFileStateText(objRootCtrl, strText)
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
	if type(tFileContent) ~= "table" or
		tonumber(tFileContent.nFileState) == nil or tFileContent.nFileState < 0 then
		return
	end

	SetFileSize(objRootCtrl, tFileContent)
	ShowErrorBkg(objRootCtrl, false)
	ShowProgressBar(objRootCtrl, true)
	
	local nFileState = tFileContent.nFileState
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

