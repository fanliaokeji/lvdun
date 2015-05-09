--[[
	tRabbitFileList = {
		["data"] = {
			["tFileList"] = {
				[1] = tFileItem1,
				[2] = tFileItem2,
			}
		}
	}
	
	tRabbitFileItem = {
		["tDownLoadConfig"] =
		{
			["strFileName"] = "",
			["nFileSizeInKB"] = "",
			["nDownSizeInKB"] = "",
			["nFinishPercent"] = "",
			["strFilePath"] = "",
			["strFileURL"] = "",
			["bIsResume"] = "",
			["nFileState"] = FILESTATE_START | FILESTATE_PAUSE | FILESTATE_FINISH | FILESTATE_ERROR,
		}
		["hTaskHandle"] = -1,		
		["strICOPath"] = "",
	}
	
--]]

local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")
local miniTPUtil = XLGetObject("MiniTP.Util")

local tRabbitFileList = {}
tRabbitFileList.data = {}
tRabbitFileList.data.tFileList = {}

local FILESTATE_NOITEM = 0
local FILESTATE_ERROR = 1
local FILESTATE_PAUSE = 2
local FILESTATE_START = 3
local FILESTATE_FINISH = 4
local FILESTATE_STARTPENDING = 5
local FILESTATE_STOPPENDING = 6


tRabbitFileList.FILESTATE_NOITEM = FILESTATE_NOITEM
tRabbitFileList.FILESTATE_START = FILESTATE_START
tRabbitFileList.FILESTATE_PAUSE = FILESTATE_PAUSE
tRabbitFileList.FILESTATE_FINISH = FILESTATE_FINISH
tRabbitFileList.FILESTATE_ERROR = FILESTATE_ERROR
tRabbitFileList.FILESTATE_STARTPENDING = FILESTATE_STARTPENDING
tRabbitFileList.FILESTATE_STOPPENDING = FILESTATE_STOPPENDING

function tRabbitFileList:GetFileList()
	return self.data.tFileList
end


function tRabbitFileList:GetFileItemByIndex(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetFileItemByIndex] param error")
		return nil
	end

	local tFileList = self:GetFileList()
	return tFileList[nIndex]
end


function tRabbitFileList:PushFileItem(tFileItem)
	if type(tFileItem) ~= "table" then
		Log("[PushFileItem] param error")
		return false
	end
	
	local tDownLoadConfig = tFileItem.tDownLoadConfig
	
	if tDownLoadConfig.nFileState ~= nil and tDownLoadConfig.nFileState == FILESTATE_START then
		local hHandle = tRabbitFileList:CreateTask(tFileItem)
		if hHandle then
			tFileItem.hTaskHandle = hHandle
			tRabbitFileList:QueryTask(tFileItem)
			tRabbitFileList:StartTask(tFileItem)
		end
	end
	
	local tFileList = self:GetFileList()
	tFileList[#tFileList+1] = tFileItem
	
	return true
end


function tRabbitFileList:RemoveFileItem(nFileItemIndex)
	if type(nFileItemIndex) ~= "number" then
		Log("[RemoveFileItem] param error")
		return false
	end

	local tFileList = self:GetFileList()
	if type(tFileList[nFileItemIndex]) ~= "table" then
		Log("[RemoveFileItem] no item :" .. tostring(nFileItemIndex))
		return false
	end
	
	table.remove(tFileList, nFileItemIndex)
	return true
end


function tRabbitFileList:SetFileItemText(nIndex, strFileName)
	if type(nIndex) ~= "number" or not IsRealString(strFileName) then
		Log("[SetFileItemText] param error")
		return false
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[SetFileItemText] GetFileItemByIndex failed"..tostring(nIndex))
		return false
	end
	
	tFileItem.tDownLoadConfig.strFileName = strFileName
	return true
end


function tRabbitFileList:GetFileItemText(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetFileItemText] param error")
		return ""
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[SetFileItemText] GetFileItemByIndex failed"..tostring(nIndex))
		return ""
	end
	
	return tFileItem.tDownLoadConfig.strFileName
end


function tRabbitFileList:SetFileItemState(nIndex, nFileState)
	if type(nIndex) ~= "number" or IsNilString(nFileState) then
		Log("[SetFileItemState] param error")
		return false
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[SetFileItemState] GetFileItemByIndex failed"..tostring(nIndex))
		return false
	end
	
	tFileItem.tDownLoadConfig.nFileState = nFileState
	return true
end


function tRabbitFileList:GetFileItemState(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetFileItemState] param error")
		return -1
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[GetFileItemState] GetFileItemByIndex failed"..tostring(nIndex))
		return -1
	end
	
	return tFileItem.tDownLoadConfig.nFileState
end


function tRabbitFileList:GetFilePath(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetFilePath] param error")
		return ""
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[GetFilePath] GetFileItemByIndex failed"..tostring(nIndex))
		return ""
	end
	
	return tFileItem.tDownLoadConfig.strFilePath
end


function tRabbitFileList:GetFileSizeInKB(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetFileSizeInKB] param error")
		return 0
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[GetFileSizeInKB] GetFileItemByIndex failed"..tostring(nIndex))
		return 0
	end
	
	return tFileItem.tDownLoadConfig.nFileSizeInKB
end


function tRabbitFileList:SetFileSizeInKB(nIndex, nFileSizeInKB)
	if type(nIndex) ~= "number" or type(nFileSizeInKB) ~= "number" then
		Log("[SetFileSizeInKB] param error")
		return false
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[SetFileSizeInKB] GetFileItemByIndex failed"..tostring(nIndex))
		return false
	end
	
	tFileItem.tDownLoadConfig.nFileSizeInKB = nFileSizeInKB
	return true
end


function tRabbitFileList:GetDownSizeInKB(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetDownSizeInKB] param error")
		return 0
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[GetDownSizeInKB] GetFileItemByIndex failed"..tostring(nIndex))
		return 0
	end
	
	return tFileItem.tDownLoadConfig.nDownSizeInKB
end


function tRabbitFileList:SetDownSizeInKB(nIndex, nDownSizeInKB)
	if type(nIndex) ~= "number" or type(nDownSizeInKB) ~= "number" then
		Log("[SetDownSizeInKB] param error")
		return false
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[SetDownSizeInKB] GetFileItemByIndex failed"..tostring(nIndex))
		return false
	end
	
	tFileItem.tDownLoadConfig.nDownSizeInKB = nDownSizeInKB
	return true
end




---文件读写
function tRabbitFileList:LoadListFromFile()
	local tFunHelper = XLGetGlobal("Project.FunctionHelper")
	if type(tFunHelper) ~= "table" then
		return false
	end

	local tFileListFromDisk = tFunHelper.ReadConfigFromMemByKey("tFileList") or {}
	if type(tFileListFromDisk.tConfigList) ~= "table" then
		return false
	end
	
	for nIndex, tConfigItem in ipairs(tFileListFromDisk.tConfigList) do
		local tFileItem = {}
		tFileItem.tDownLoadConfig = tConfigItem
		tFileItem.hTaskHandle = -1

		tRabbitFileList:PushFileItem(tFileItem)
	end

	Log("[LoadListFromFile] load end")
	return true
end


function tRabbitFileList:SaveListToFile()
	local tFunHelper = XLGetGlobal("Project.FunctionHelper")
	if type(tFunHelper) ~= "table" then
		return false
	end

	local tFileList = tRabbitFileList:GetFileList()
	local tFileListFromDisk = tFunHelper.ReadConfigFromMemByKey("tFileList") or {}
	tFileListFromDisk.tConfigList = {}
	
	for nIndex, tFileItem in ipairs(tFileList) do
		tFileListFromDisk.tConfigList[nIndex] = tFileItem.tDownLoadConfig
	end
	
	tFunHelper.SaveConfigToFileByKey("tFileList")
	return true
end


------minitp util
local g_bHasInit = false

function tRabbitFileList:Init()
	if g_bHasInit then
		return true
	end
	
	local bSucc = miniTPUtil:LoadXLDL()
	if not bSucc then
		Log("[Init] LoadXLDL failed")
		return false
	end
	
	local bSucc = miniTPUtil:Init()
	if not bSucc then
		Log("[Init] miniTPUtil Init failed")
		return false
	end
	
	g_bHasInit = true
	Log("[Init] success")
	return true
end


function tRabbitFileList:UnInit()
	if not g_bHasInit then
		Log("[UnInit] miniTPUtil has not init")
		return false
	end
	
	local bSucc = miniTPUtil:UnInit()
	if not bSucc then
		Log("[UnInit] miniTPUtil UnInit failed")
		return false
	end
	
	g_bHasInit = false
	Log("[UnInit] success")
	return true
end


function tRabbitFileList:CreateTask(tFileItem)
	local bPassCheck = CheckFileItemForDownload(tFileItem)
	if not bPassCheck then
		Log("[CreateTask] CheckFileItemForDownload failed")
		return false
	end
	
	local tDownLoadConfig = tFileItem.tDownLoadConfig
	local strRefURL = nil
	local strCookies = nil
	local bOnlyOriginal = nil
	local nDisableAutoRename = 1  --0是不重命名
	
	local hTaskHandle = miniTPUtil:TaskCreate(tDownLoadConfig.strFileURL, tDownLoadConfig.strFilePath
						, tDownLoadConfig.strFileName, strRefURL, strCookies, strCookies
						, bOnlyOriginal, nDisableAutoRename, tDownLoadConfig.bIsResume)
					
	Log("[CreateTask] tFileItem.strFileURL:"..tostring(tDownLoadConfig.strFileURL)
			.."   hTaskHandle: "..tostring(hTaskHandle))
			
	return hTaskHandle
end


function tRabbitFileList:StartTask(tFileItem)
	if type(tFileItem) ~= "table" then
		return false
	end

	local hTaskHandle = tFileItem.hTaskHandle
	
	if hTaskHandle == nil or hTaskHandle == -1 then
		Log("[StartTask] hTaskHandle not valid")
		return false
	end
		
	local bRet = miniTPUtil:TaskStart(hTaskHandle)
	Log("[StartTask] strFileURL: " ..tostring(tFileItem.tDownLoadConfig.strFileURL).." bRet: "..tostring(bRet))
		
	return bRet
end


function tRabbitFileList:PauseTask(tFileItem)
	if type(tFileItem) ~= "table" then
		return false
	end
	
	local hTaskHandle = tFileItem.hTaskHandle
	
	if hTaskHandle == nil or hTaskHandle == -1 then
		Log("[PauseTask] hTaskHandle not valid")
		return false
	end
		
	local bRet = miniTPUtil:TaskPause(hTaskHandle)
	Log("[PauseTask] strFileURL: " ..tostring(tFileItem.tDownLoadConfig.strFileURL).." bRet: "..tostring(bRet))
		
	return bRet
end


function tRabbitFileList:QueryTask(tFileItem)
	if type(tFileItem) ~= "table" then
		return false
	end

	local hTaskHandle = tFileItem.hTaskHandle
	
	if hTaskHandle == nil or hTaskHandle == -1 then
		Log("[QueryTask] hTaskHandle not valid")
		return false
	end
	
	local bRet, tTaskInfo = miniTPUtil:TaskQueryEx(hTaskHandle)
	Log("[QueryTask] strFileURL: " ..tostring(tFileItem.tDownLoadConfig.strFileURL).." bRet: "..tostring(bRet))
	
	return bRet, tTaskInfo
end


function tRabbitFileList:DeleteTask(tFileItem)
	if type(tFileItem) ~= "table" then
		return false
	end

	local hTaskHandle = tFileItem.hTaskHandle
	
	if hTaskHandle == nil or hTaskHandle == -1 then
		Log("[QueryTask] hTaskHandle not valid")
		return false
	end
	
	local bRet = miniTPUtil:TaskDelete(hTaskHandle)
	Log("[QueryTask] strFileURL: " ..tostring(tFileItem.tDownLoadConfig.strFileURL).." bRet: "..tostring(bRet))
	
	return bRet, tTaskInfo
end


--函数阻塞
function tRabbitFileList:GetFileSizeWithUrlInKB(strURL)
	if not IsRealString(strURL) then
		return false, 0
	end
	
	local bRet, nFileSizeInByte = miniTPUtil:GetFileSizeWithUrl(strURL)
	if not bRet then
		return false, 0
	end
	
	local nFileSizeInKB = 1
	if nFileSizeInByte > 1024 then
		nFileSizeInKB = math.floor(nFileSizeInByte/1024)
	end

	return bRet, nFileSizeInKB
end


function GetFileSaveNameFromUrl(url)
	local _, _, strFileName = string.find(tostring(url), ".*/(.*)$")
	local npos = string.find(strFileName, "?", 1, true)
	if npos ~= nil then
		strFileName = string.sub(strFileName, 1, npos-1)
	end
	return strFileName
end


function CheckFileItemForDownload(tFileItem)
	if type(tFileItem) ~= "table" or type(tFileItem.tDownLoadConfig) ~= "table" then
		return false
	end

	local tDownLoadConfig = tFileItem.tDownLoadConfig

	if not IsRealString(tDownLoadConfig.strFileURL) 
		or not IsRealString(tDownLoadConfig.strFilePath) then
		return false
	end

	if not IsRealString(tDownLoadConfig.strFileName) then
		tDownLoadConfig.strFileName =	GetFileSaveNameFromUrl(tDownLoadConfig.strFileURL)
	end
	
	if type(tDownLoadConfig.bIsResume) ~= "boolean" then
		tDownLoadConfig.bIsResume = true
	end
	
	return true
end


XLSetGlobal("Project.RabbitFileList", tRabbitFileList)


------------------
function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function Log(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@RabbitFileHelper_Log: " .. tostring(strLog))
	end
end
