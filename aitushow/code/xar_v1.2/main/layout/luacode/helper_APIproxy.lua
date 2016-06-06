Helper.APIproxy = {}
local APIproxy = Helper.APIproxy
local tipUtil = XLGetObject("API.Util")
--绑定缩略图获取事件
function APIproxy.CreateLoader()
	local obj = XLGetObject("KKImage.ThumbnailLoader.Factory")
	return obj:CreateLoader()
end


function APIproxy.GetLuaImageProcessor()
	return XLGetObject("KKImage.LuaImageProcessor")
end


function APIproxy.GetTipUtil()
	return tipUtil
end

APIproxy.OSUtil = tipUtil

--监控文件夹变化
local g_CurIndex = 0
local g_MonitorList = {}
local g_LastFilePath
local g_LastEventType
APIproxy.FolderMonitorManager = {
	AttachListener = 
		function (Listener)
			if Listener then
				g_CurIndex = g_CurIndex + 1
				g_MonitorList[g_CurIndex] = Listener
				return g_CurIndex
			end
			return -1
		end,
	DetachListener = 
		function (cookieId)
			if cookieId then
				if g_MonitorList[cookieId] then
					g_MonitorList[cookieId] = nil
				end	
			end
		end,
}
function OnFolderMonitorEvent(oldFilePath, newFilePath, eventType)
	LOG("NeoImageLog " .. oldFilePath .. "  " .. newFilePath .. "  " .. eventType)
	if oldFilePath == g_LastFilePath and g_LastEventType == eventType then
		return
	end
	g_LastFilePath = oldFilePath
	g_LastEventType = eventType
	for key, value in pairs(g_MonitorList) do
		value(oldFilePath, newFilePath, eventType)
	end
end
function AttachFolderMonitorEvent()
	local appObj = tipUtil
	local ret = appObj:InitFolderMonitor()
	if ret then	-- 绑定事件
		appObj:AttachDirChangeEvent(OnFolderMonitorEvent)
	end
end
AttachFolderMonitorEvent()

function APIproxy.GetSpecialPathbyWidth(textObj, path, width)
	textObj:SetText(path)
	local textWidth, textHeight = textObj:GetTextExtent()
	if textWidth <= width then
		return path
	end
	
	local pathList = {}
	local tempPath = ""
	local pos = 1
	while pos <= #path do
		local data = string.byte(path, pos)
		if data == 47 or data == 92 then
			table.insert(pathList, tempPath)
			tempPath = ""
		else
			tempPath = tempPath .. string.char(data)
		end
		pos = pos + 1
	end
	if tempPath ~= "" then
		table.insert(pathList, tempPath)
	end
	for i=#pathList-1, 2, -1 do
		pathList[i] = "..."
		local text = pathList[1]
		for j=2, #pathList do
			text = text .. "\\" .. pathList[j]
		end
		textObj:SetText(text)
		local textWidth, textHeight = textObj:GetTextExtent()
		if textWidth <= width then
			break
		end
	end
	local text = pathList[1]
	for j=2, #pathList do
		text = text .. "\\" .. pathList[j]
	end
	
	return text
end

function APIproxy.GetFileNameFromPath(path)
	if type(path) ~= "string" or  path == "" then
		return ""
	end
	local pos = #path
	while pos >= 0 do
		local data = string.byte(path, pos)
		if data == 47 or data == 92 then
			break
		end
		pos = pos - 1
	end
	return string.sub(path, pos+1, #path)
end

function APIproxy.GetRenameFilePath(path)
	if not path  or type(path) ~= "string" or not tipUtil:QueryFileExists(path) then
		return
	end
	local i, newpath = 1, ""
	while true do
		math.randomseed(os.time())
		i = math.random(10000)
		newpath = string.gsub(path, "%.[^%.]*$", "重命名("..i..")%1")
		if newpath and not tipUtil:QueryFileExists(newpath) then
			return newpath
		end
	end
end

function APIproxy.Lua_Gc()
	tipUtil:Lua_Gc()
end


function APIproxy.ForceUpdateWndShow(hWnd)
	tipUtil:ForceUpdateWndShow(hWnd)
end


function APIproxy.GetFileExt(fileName)
	if type(fileName) ~= "string" or fileName == "" then
		return ""
	end
	
	local pos = #fileName
	while pos >= 1 do
		local data = string.byte(fileName, pos)
		if data == 46 then
			break
		end
		pos = pos - 1
	end
	if pos == 0 then
		return ""
	end
	return string.sub(fileName, pos, #fileName)
end


function APIproxy.GetParentPath(path)
	if type(path) ~= "string" or  path == "" then
		return ""
	end
	local pos = #path
	while pos >= 1 do
		local data = string.byte(path, pos)
		if data == 47 or data ==92 then
			break
		end
		pos = pos - 1
	end
	if pos == 0 then
		return ""
	end
	return string.sub(path, 1, pos-1)
end


local optFac = XLGetObject("KKImage.OperationFactory")
function APIproxy.CreateScaleBitmapOP()
	return optFac:CreateOperation(0x0003)
end

function APIproxy.CreateLoadBitmapOP()
	return optFac:CreateOperation(0x0002)
end

function APIproxy.CreateLoadImageFileOP()
	return optFac:CreateOperation(0x0001)
end

function APIproxy.CreateSaveDocFileOP()	
	return optFac:CreateOperation(0x0005)
end

APIproxy.LuaDragDropProcessor = XLGetObject("KKImage.LuaDragDropProcessor")

--排序
function APIproxy.SortImageListFile(self, fileList)
	local sortby, sorttype = Helper.Setting.GetSortConfig()
	if sortby == "name" then -- 文件名
		SortByFileName(fileList, sorttype)
	elseif sortby == "size" then -- 大小
		SortByFileSize(fileList, sorttype)
	elseif sortby == "time" then -- 修改时间
		SortByEditTime(fileList, sorttype)
	elseif sortby == "type" then -- 格式
		SortByFileType(fileList, sorttype)
	end
end

function SortByFileName(fileList, sorttype)
	LOG("SortByFileName entry")
	local function sortFunc(obj1, obj2)
		local isLarger = tipUtil:StrColl(obj2.FileName, obj1.FileName)
		if sorttype == "positive" then
			if isLarger > 0 then
				return true
			else
				return false
			end
		else
			if isLarger > 0 then
				return false
			else
				return true
			end
		end
	end
	table.sort(fileList, sortFunc)
	return fileList
end

function SortByEditTime(fileList, sorttype)
	LOG("SortByEditTime entry")
	local function sortFunc(obj1, obj2)
		local isLarger = tipUtil:StrColl(obj2.LastWriteTime, obj1.LastWriteTime)
		if sorttype == "positive" then
			if isLarger > 0 then
				return true
			else
				return false
			end
		else
			if isLarger >= 0 then
				return false
			else
				return true
			end
		end
	end
	table.sort(fileList, sortFunc)
	return fileList
end

function SortByFileSize(fileList, sorttype)
	LOG("SortByFileSize entry")
	local function sortFunc(obj1, obj2)
		if sorttype == "positive" then
			if tonumber(obj2.FileSize) >  tonumber(obj1.FileSize) then
				return true
			end
			return false
		else
			if tonumber(obj2.FileSize) >  tonumber(obj1.FileSize) then
				return false
			end
			return true
		end
	end
	table.sort(fileList, sortFunc)
	return fileList
end

function SortByFileType(fileList, sorttype)
	LOG("SortByFileType entry")
	local function sortFunc(obj1, obj2)
		local isLarger = tipUtil:StrColl(obj2.ExtName, obj1.ExtName)
		if sorttype == "positive" then
			if isLarger > 0 then
				return true
			else
				return false
			end
			return true
		else
			if isLarger >= 0 then
				return false
			else
				return true
			end
		end
	end
	table.sort(fileList, sortFunc)
	return fileList
end
