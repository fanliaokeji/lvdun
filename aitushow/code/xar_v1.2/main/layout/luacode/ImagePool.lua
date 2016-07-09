--创建一个图片池对象，自动继承ObjectBase的AddListener等方法
--ImagePool可直接作为一个对象来用，也可通过 newPoolObj = ImagePool:New()
--的方式，作为类来用
local ImagePool = ObjectBase:New()
XLSetGlobal("ImagePool", ImagePool)

local ThumbnailsLoaderFactory = XLGetObject("KKImage.ThumbnailLoader.Factory")

-- {"FilePath"=, "FileName"=, "LastWriteTime"=, "ExtName"=, "FileSize", "ThumbWidth"=, "ThumbHeight"=, "szType"=, "xlhThumb"=},
ImagePool.tPictures  = nil
ImagePool.curFolder  = nil
ImagePool.sortKey    = nil 
ImagePool.sortSettingToSortKey = {
			["name"]="FileName", 
			["type"]="ExtName", 
			["time"]="LastWriteTime", 
			["size"]="FileSize",
}
ImagePool.bReverse   = nil
ImagePool.thumbnailMaxWidth  = 185
ImagePool.thumbnailMaxHeight = 185
ImagePool.thumbnailCatchPath = Helper.tipUtil:GetTempDir().."\\KuaiKan_{1E27A1DA-16F4-4436-AB6F-A83FCC6DD850}\\KKThumb\\"
ImagePool.thumbnailsLoader = ThumbnailsLoaderFactory:CreateLoader()

--[[
--向外提供的事件：
--OnSetPath 	调用SetFolder，获取到tPictures table之后
--OnDeleteFile  文件被删掉       --由ImagePool负责监视文件夹，产生以下三个事件
--OnAddFile		插入一个文件
--OnRenameFile  重命名文件
--OnPicUpdate	图片取到缩略图、被旋转、缩放、修改等操作后，通知界面更新
--OnSortFinished重新排序后，通知界面刷新
--]]

--向外提供的方法：
function ImagePool:SetFolder(sPath, bMust)
	if not sPath or not Helper.tipUtil:QueryFileExists(sPath) then
		return
	end
	
	if self.curFolder and self.curFolder == sPath and not bMust then
		return
	end
	
	self.curFolder = sPath
	self.tPictures = Helper.tipUtil:GetFiles(sPath)
	self.sortKey = "FileName"
	
	--按照上次的设置排序(GetFiles返回的文件默认是按文件名排序)
	local curSortBy = Helper.Setting.GetSortConfig()
	if self.sortKey ~= self.sortSettingToSortKey[curSortBy] then
		self.sortKey = self.sortSettingToSortKey[curSortBy]
		self:Sort(self.sortKey, false, true)
	end
	
	self:DispatchEvent("OnSetPath", sPath)
	
	--监听该文件夹的变更
	if self.monitorCookie then
		Helper.tipUtil:UnMonitorDirChange(self.monitorCookie)
		LOG("UnMonitorDirChange ")
		self.monitorCookie = nil
	end
	self.monitorCookie = Helper.tipUtil:MonitorDirChange(sPath)
	LOG("MonitorDirChange: monitorCookie: ", self.monitorCookie)
end

function ImagePool:GetPicInfo(indexOrPath)
	--尽量传index
	if "number" == type(indexOrPath) then
		return self.tPictures[indexOrPath], indexOrPath
	elseif "string" == type(indexOrPath) then
		for i=1, #self.tPictures do
			if string.upper(self.tPictures[i].FilePath) == string.upper(indexOrPath) then
				return self.tPictures[i], i
			end
		end
	end
	return nil, nil
end

function ImagePool:GetNextPicInfo(indexOrPath)
	if "number" == type(indexOrPath) then
		if indexOrPath < #self.tPictures then
			return self.tPictures[indexOrPath + 1], indexOrPath + 1
		end
	elseif "string" == type(indexOrPath) then
		for i=1, #self.tPictures do
			if string.upper(self.tPictures[i].FilePath) == string.upper(indexOrPath) then
				if i< #self.tPictures then
					return self.tPictures[i+1], i+1
				end
			end
		end
	end
	return nil, nil
end

function ImagePool:GetPrevPicInfo(indexOrPath)
	if "number" == type(indexOrPath) then
		if indexOrPath > 1 then
			return self.tPictures[indexOrPath - 1], indexOrPath - 1
		end
	elseif "string" == type(indexOrPath) then
		for i=1, #self.tPictures do
			if string.upper(self.tPictures[i].FilePath) == string.upper(indexOrPath) then
				if i > 1 then
					return self.tPictures[i-1], i-1
				end
			end
		end
	end
	return nil, nil
end

--bReverse:是否倒序列，默认正序，也就是小的在前面
--notFireEvent 是否发事件，默认发
function ImagePool:Sort(key, bReverse, notFireEvent)
	if not self.curFolder then
		return
	end
	if key == self.sortKey and bReverse == self.bReverse then
		LOG("Sort key == self.sortKey, do not need sort")
		return
	end
	if key ~= "FileName" and key ~= "LastWriteTime" and key ~= "ExtName" and key ~= "FileSize" then
		LOG("Sort key error: ", key)
		return
	end
	local ret = bReverse and -1 or 1
	local cmpFun = function(itemA, itemB)
		if itemA[key] > itemB[key] then
			return ret
		elseif itemA[key] < itemB[key] then
			return -ret
		else
			return 0
		end
	end
	
	Helper:QuickSort(self.tPictures,1, #self.tPictures, cmpFun)
	self.sortKey = key
	self.bReverse = bReverse
	if not notFireEvent then
		self:DispatchEvent("OnSortFinished", key)
	end
end

--申请连续的一段下标的缩略图
function ImagePool:QueryThumbByRange(indexBegin, indexEnd)
	if indexBegin < 1 or indexEnd > #self.tPictures then
		return
	end
	
	for i=indexBegin, indexEnd do
		local info = self:GetPicInfo(i)
		if not info.bThumbLoading then--防止重复申请
			local thumbnailsFilename = self:GetThumbFileName(info)..".jpg"
			local thumbFilePath = self.thumbnailCatchPath..thumbnailsFilename
			-- LOG("QueryThumbByRange: info.FilePath: ", info.FilePath, " thumbFilePath: ", thumbFilePath)
			self.thumbnailsLoader:LoadThumbnails(info.FilePath, thumbFilePath, self.thumbnailMaxWidth, self.thumbnailMaxHeight)
			info.bThumbLoading = true
		end	
	end
end

--申请下标不连续的缩略图
function ImagePool:QueryThumbByIndexTable(tIndex)
	if "table" ~= type(tIndex) then
		return
	end
	for i=1, #tIndex do
		local info = self:GetPicInfo(tIndex[i])
		if not info.bThumbLoading and not info.xlhThumb then--防止重复申请
			local thumbnailsFilename = self:GetThumbFileName(info)..".jpg"
			local thumbFilePath = self.thumbnailCatchPath..thumbnailsFilename
			self.thumbnailsLoader:LoadThumbnails(info.FilePath, thumbFilePath, self.thumbnailMaxWidth, self.thumbnailMaxHeight)
			info.bThumbLoading = true
		else
			if info.bThumbLoading then
				LOG(info.FileName.." 缩略图正在请求中！！")
			else
				LOG(info.FileName.." 已有缩略图！！无需重复申请")
			end
		end	
	end
end

function ImagePool:IsInCurFolder(sPath)
	if not self.curFolder then
		return
	end
	local b, e = string.find(sPath, self.curFolder)
	
	if e then
		return true
	end
	return false
end

function ImagePool:GetThumbFileName(fileInfo)
	return Helper.tipUtil:GetMd5Str(fileInfo.FilePath..fileInfo.LastWriteTime)
end

--按照现有的排序方式进行插入
function ImagePool:Insert(tInfo)
	if not self.tPictures or #self.tPictures < 1 then
		table.insert(self.tPictures, tInfo)
		return 1, 1
	end
	local index = 1
		
	if not self.bReverse then
		--当前是正序(从小到大)
		for i=1, #self.tPictures do
			if tInfo[self.sortKey] > self.tPictures[i][self.sortKey] then
				index = i
			end
		end
		table.insert(self.tPictures, index+1, tInfo)
		LOG("indsert info to index: ", index)
		return index, #self.tPictures
	else
		--当前是倒序，从大到小
		for i=1, #self.tPictures do
			if tInfo[self.sortKey] < self.tPictures[i][self.sortKey] then
				index = i
			end
		end
		table.insert(self.tPictures, index+1, tInfo)
		LOG("indsert info to index: ", index)
		return index, #self.tPictures
	end
end

function ImagePool:AddFileInfo(sFilePath)
	local fileInfo = Helper.tipUtil:GetFileInfoByPath(sFilePath)
	local insertIndex, endIndex = self:Insert(fileInfo)
	self:QueryThumbByRange(insertIndex, insertIndex)
	
	--插入一个图片后，insertIndex及其之后的item都被影响了，界面需要重新排布
	--假如界面展示的图片不在(insertIndex, endIndex)范围内，则无需重排
	self:DispatchEvent("OnAddFile", insertIndex, endIndex)
end

function ImagePool:DeleteFileInfo(sFilePath)
	local info, index = self:GetPicInfo(sFilePath)
	if not info then
		LOG("Can not find file: sFilePath: ", sFilePath)
		return
	end
	
	table.remove(self.tPictures, index)
	self:DispatchEvent("OnDeleteFile", info, index)
end

function ImagePool:UpdateFileInfo(sFilePath)
	local oldInfo, index = self:GetPicInfo(sFilePath)
	if not oldInfo then
		LOG("UpdateFileInfo Can not find file: sFilePath: ", sFilePath)
		return
	end
	
	local newInfo = Helper.tipUtil:GetFileInfoByPath(sFilePath)
	self.tPictures[index] = newInfo
	--异步更新
	LOG("UpdateFileInfo index: ", index)
	self:QueryThumbByRange(index, index)
end

function ImagePool:RenameFileInfo(sOldPath, sNewPath)
	local info, index = self:GetPicInfo(sOldPath)
	if not info then
		LOG("Can not find file: sOldPath: ", sOldPath)
		return
	end
	info.FilePath = sNewPath
	info.FileName = string.match(info.FilePath, ".+[\\/]([^?]+)")
	self:DispatchEvent("OnRenameFile", info, index, sOldPath, sNewPath)
end

function ImagePool:OnDirChange(oldFilePath, newFilePath, eventType)
	LOG("OnDirChange: oldFilePath: ", oldFilePath, " newFilePath: ", newFilePath, "  eventType: ", eventType)
	
	--别的目录发生的事件，不予处理
	if not self:IsInCurFolder(oldFilePath) then
		LOG("Not curFolder! curFolder: ", self.curFolder, " oldFilePath: ", oldFilePath)
		return
	end
	
	if eventType == 1 then     -- 创建新的
		--要根据现有的排序方式将新创建的文件插入到合适的位置
		if Helper.tipUtil:IsCanHandleFileCheckByExt(oldFilePath) then
			self:AddFileInfo(oldFilePath)
		end
	elseif eventType == 2 then -- 删除
		LOG("OnDirChange DeleteFileInfo oldFilePath: ", oldFilePath)
		self:DeleteFileInfo(oldFilePath)
	elseif eventType == 3 then -- 文件被旋转、编辑等
		-- self:UpdateFileInfo(oldFilePath)
	elseif eventType == 5 then -- 重命名(当前尚未对重命名的文件进行重新排序)
		self:RenameFileInfo(oldFilePath, newFilePath)
	end
end

function ImagePool:OnThumbComplete(filePath, bitmap, width, height)
	LOG("OnThumbComplete: filePath: ", filePath)
	
	--别的目录发生的事件，不予处理
	if not self:IsInCurFolder(filePath) then
		LOG("Not curFolder! curFolder: ", self.curFolder, " filePath: ", filePath)
		return
	end
	local info, index = self:GetPicInfo(filePath)
	if not info then
		LOG("ERROR: Can not get PicInfo")
		return
	end
	
	info.xlhThumb = bitmap
	info.ThumbWidth = width
	info.ThumbHeight = height
	info.bThumbLoading = nil
	
	self:DispatchEvent("OnPicUpdate", info, index)
end

function ImagePool:Init()
	Helper.APIproxy.FolderMonitorManager.AttachListener(
		function(oldFilePath, newFilePath, eventType)
		self:OnDirChange(oldFilePath, newFilePath, eventType)
	end)
		
	self.thumbnailsLoader:AttatchLoadCompleteEvent(function(bitmap, width, height, filePath)
		self:OnThumbComplete(filePath, bitmap, width, height)
	end)
end

Helper.tipUtil:InitFolderMonitor()
ImagePool:Init()

