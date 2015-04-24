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
		["strFileName"] = "",
		["strICOPath"] = "",
		["nFileSizeInKB"] = "",
		["nDownSizeInKB"] = "",
		["nFinishPercent"] = "",
		["strFilePath"] = "",
		["strFileURL"] = "",
		["nFileState"] = FILESTATE_START | FILESTATE_PAUSE | FILESTATE_FINISH | FILESTATE_ERROR,
	}
	
--]]

local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")

local tRabbitFileList = {}
tRabbitFileList.data = {}
tRabbitFileList.data.tFileList = {}

local FILESTATE_START = 1
local FILESTATE_PAUSE = 2
local FILESTATE_FINISH = 3
local FILESTATE_ERROR = 4

tRabbitFileList.FILESTATE_START = FILESTATE_START
tRabbitFileList.FILESTATE_PAUSE = FILESTATE_PAUSE
tRabbitFileList.FILESTATE_FINISH = FILESTATE_FINISH
tRabbitFileList.FILESTATE_ERROR = FILESTATE_ERROR

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
	
	tFileItem.strFileName = strFileName
	return true
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
	
	tFileItem.nFileState = nFileState
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
	
	return tFileItem.nFileState
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
	
	return tFileItem.strFilePath
end



function tRabbitFileList:LoadListFromFile()
	local tFunHelper = XLGetGlobal("Project.FunctionHelper")
	if type(tFunHelper) ~= "table" then
		return false
	end

	local tFileListFromDisk = tFunHelper.ReadConfigFromMemByKey("tFileList") or {}
	self.data.tFileList = tFileListFromDisk.tFileList or {}

	return true
end


function tRabbitFileList:SaveListToFile()
	local tFunHelper = XLGetGlobal("Project.FunctionHelper")
	if type(tFunHelper) ~= "table" then
		return false
	end
	
	local tFileListFromDisk = tFunHelper.ReadConfigFromMemByKey("tFileList") or {}
	local tFileList = self:GetFileList()
	tFileListFromDisk.tFileList = tFileList
	
	tFunHelper.SaveConfigToFileByKey("tFileList")
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
