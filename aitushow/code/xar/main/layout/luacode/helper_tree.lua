local PathHelper = {}
local Helper = XLGetGlobal("Helper")
Helper.PathHelper = PathHelper

local tipUtil = XLGetObject("API.Util")
function PathHelper.GetDeskTopPath()
	local nCSIDL_DESKTOP = 0
	local bALnkExist = true
	local bCLnkExist = true
	local strPath = tipUtil:GetSpecialFolderPathEx(nCSIDL_DESKTOP)
	return tipUtil:FindDirList(strPath)
end

function PathHelper.GetDocumentPath()
	local nCSIDL_PERSONAL = 5
	local bALnkExist = true
	local bCLnkExist = true
	local strPath = tipUtil:GetSpecialFolderPathEx(nCSIDL_PERSONAL)
	return tipUtil:FindDirList(strPath)
end

function PathHelper.GetPicturePath()
	local nCSIDL_MYPICTURES = 0x27
	local bALnkExist = true
	local bCLnkExist = true
	local strPath = tipUtil:GetSpecialFolderPathEx(nCSIDL_MYPICTURES)
	return tipUtil:FindDirList(strPath)
end

function PathHelper.GetDiskList()
	local tAll = {"C:", "D:", "E:", "F:", "G", "H:", "I:", "J:", "K:", "L:", "M:", "N:"}
	local tDisk = {}
	for _, v in ipairs(tAll) do
		if tipUtil:QueryFileExists(v) then
			tDisk[#tDisk+1] = v
		else
			break
		end
	end
	return tDisk
end

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
		return srcdir
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