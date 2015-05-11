local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")
local tipUtil = tFunHelper.tipUtil

--方法
function UpdateBottomStyle(self)
	local objStartBtn = self:GetControlObject("BottomCtrl.StartBtn")
	local objPauseBtn = self:GetControlObject("BottomCtrl.PauseBtn")
	SetBtnEnableStyle(objStartBtn, false)
	SetBtnEnableStyle(objPauseBtn, false)

	local objFileItem = tFunHelper.GetSelectItemObject()
	if not objFileItem then
		return
	end
	
	local nIndex = objFileItem:GetItemIndex()
	local nState = tRabbitFileList:GetFileItemState(nIndex)
	if nState < 0 then
		return
	end
	
	if nState == tRabbitFileList.FILESTATE_START then
		SetBtnEnableStyle(objPauseBtn, true)
	elseif nState == tRabbitFileList.FILESTATE_PAUSE then
		SetBtnEnableStyle(objStartBtn, true)
	end	
end



---事件
function OnInitControl(self)
	UpdateBottomStyle(self)
end


function OnClickNewTask(self)
	tFunHelper.ShowModalDialog("TipNewTaskWnd", "TipNewTaskWnd.Instance", "NewTaskTree", "NewTaskTree.Instance")
end


function OnClickStartBtn(self)
	UpdateFileState(tRabbitFileList.FILESTATE_START)
	tFunHelper.UpdateFileStateUI()
end

function OnClickPauseBtn(self)
	UpdateFileState(tRabbitFileList.FILESTATE_PAUSE)
	tFunHelper.UpdateFileStateUI()
end


function OnClickFileManage(self)
	local strSaveDir = tFunHelper.GetDownFileSaveDir()
	if not IsRealString(strSaveDir) or not tipUtil:QueryFileExists(strSaveDir) then
		return
	end
	
	tipUtil:ShellExecute(0, "open", strSaveDir, "", 0, "SW_SHOW")
end


function OnClickDeleteAll(self)
	tFunHelper.ShowModalDialog("TipDeleteAllTaskWnd", "TipDeleteAllTaskWnd.Instance", "DeleteAllTaskTree", "DeleteAllTaskTree.Instance")
end


----
function SetBtnEnableStyle(objBtn, bEnable)
	objBtn:SetEnable(bEnable)
	if bEnable then
		objBtn:SetAlpha(255, true)
	else
		objBtn:SetAlpha(40, true)
	end
end


function UpdateFileState(nSetStart)
	local objFileItem = tFunHelper.GetSelectItemObject()
	if not objFileItem then
		return
	end
	
	local nIndex = objFileItem:GetItemIndex()
	tRabbitFileList:SetFileItemState(nIndex, nSetStart)
	
	if nSetStart == tRabbitFileList.FILESTATE_START then
		objFileItem:StartQueryTimer()
	elseif nSetStart == tRabbitFileList.FILESTATE_PAUSE then
		objFileItem:StopQueryTimer()
	end
end


------------------

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

