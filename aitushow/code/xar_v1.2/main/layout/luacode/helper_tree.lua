local PathHelper = {}
local Helper = XLGetGlobal("Helper")
Helper.PathHelper = PathHelper

local tipUtil = XLGetObject("API.Util")
PathHelper.SpecialName = {
	["C:"] = "本地磁盘(C:)",
	["D:"] = "本地磁盘(D:)",
	["E:"] = "本地磁盘(E:)",
	["F:"] = "本地磁盘(F:)",
	["G:"] = "本地磁盘(G:)",
	["H:"] = "本地磁盘(H:)",
	["I:"] = "本地磁盘(I:)",
	["J:"] = "本地磁盘(J:)",
	["K:"] = "本地磁盘(K:)",
	["L:"] = "本地磁盘(L:)",
	["M:"] = "本地磁盘(M:)",
	["N:"] = "本地磁盘(N:)",
}

function PathHelper.GetRealPath(srcdir)
	local strRealPath = string.match(tostring(srcdir), "@([^@]*)$") or srcdir
	if tipUtil:QueryFileExists(strRealPath) then
		return strRealPath
	elseif strRealPath == "我的文档" then
		return tipUtil:GetSpecialFolderPathEx(5)
	elseif strRealPath == "我的图片" then
		return tipUtil:GetSpecialFolderPathEx(0x27)
	elseif strRealPath == "桌面" then
		return tipUtil:GetSpecialFolderPathEx(0)
	else
		return nil
	end
end

local VrPaths = {
	[tipUtil:GetSpecialFolderPathEx(0)] = "桌面", 
	[tipUtil:GetSpecialFolderPathEx(0x27)] = "我的图片", 
	[tipUtil:GetSpecialFolderPathEx(5)] = "我的文档", 
}
function PathHelper.GetVrPath(srcdir)
	for path, vpath in pairs(VrPaths) do
		if string.find(srcdir, string.upper(path), 1, true) or srcdir == vpath then
			local isfull = false
			if string.upper(srcdir) == string.upper(path) then
				isfull = true
			end
			return vpath, isfull
		end
	end
	return "计算机"
end

local asynUtil = XLGetObject("API.AsynUtil")
local IncGetDirList = 0
local DirListMap = {}
local function dec()
	IncGetDirList = IncGetDirList - 1
	if PathHelper.CanCallBack and IncGetDirList <= 0 and type(PathHelper.fncallbak) == "function" then
		PathHelper.CanCallBack = false
		PathHelper.fncallbak()
	end
end

function PathHelper.GetDeskTopPath()
	if DirListMap["桌面"] then return end
	local nCSIDL_DESKTOP = 0
	local bALnkExist = true
	local bCLnkExist = true
	local strPath = tipUtil:GetSpecialFolderPathEx(nCSIDL_DESKTOP)
	IncGetDirList = IncGetDirList + 1
	asynUtil:AsynGetFolders(strPath, 
		function(nRet, tList)
			DirListMap["桌面"] = tList
			dec()
		end)
	return true
end

function PathHelper.GetDocumentPath()
	if DirListMap["我的文档"] then return end
	local nCSIDL_PERSONAL = 5
	local bALnkExist = true
	local bCLnkExist = true
	local strPath = tipUtil:GetSpecialFolderPathEx(nCSIDL_PERSONAL)
	IncGetDirList = IncGetDirList + 1
	asynUtil:AsynGetFolders(strPath, 
		function(nRet, tList)
			DirListMap["我的文档"] = tList
			dec()
		end)
	return true
end

function PathHelper.GetPicturePath()
	if DirListMap["我的图片"] then return end
	local nCSIDL_MYPICTURES = 0x27
	local bALnkExist = true
	local bCLnkExist = true
	local strPath = tipUtil:GetSpecialFolderPathEx(nCSIDL_MYPICTURES)
	IncGetDirList = IncGetDirList + 1
	asynUtil:AsynGetFolders(strPath, 
		function(nRet, tList)
			DirListMap["我的图片"] = tList
			dec()
		end)
	return true
end

function PathHelper.GetDiskList()
	if DirListMap["计算机"] then return end
	local tAll = tipUtil:GetLogicalDrive()
	LOG("PathHelper.GetDiskList type(tAll) = "..type(tAll))
	local tDisk = {}
	for _, v in ipairs(tAll) do
		LOG("PathHelper.GetDiskList v = "..tostring(v))
		v = string.gsub(tostring(v), "[/\\]*$", "")
		LOG("PathHelper.GetDiskList v2 = "..tostring(v))
		if tipUtil:QueryFileExists(v) then
			tDisk[#tDisk+1] = {strFilePath=v, bHaveSubFolder=true}
		end
	end
	DirListMap["计算机"] = tDisk
end

function PathHelper.RequestDirList(dir)
	if not dir or not tipUtil:QueryFileExists(dir) then
		return
	end
	local key = string.lower(string.gsub(dir, "[/\\]*$", ""))
	if DirListMap[key] then 
		return
	end
	IncGetDirList = IncGetDirList + 1
	asynUtil:AsynGetFolders(dir, 
		function(nRet, tList)
			DirListMap[key] = tList
			dec()
		end)
	return true
end

function PathHelper.GetDirList(dir)
	local key = string.lower(string.gsub(dir, "[/\\]*$", ""))
	return DirListMap[key] or {}
end

function PathHelper.CanReBuild()
	return IncGetDirList <= 0
end