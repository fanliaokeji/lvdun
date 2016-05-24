local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")
if not GreenShieldMenu then
	GreenShieldMenu = {}
	XLSetGlobal("GreenShieldMenu", GreenShieldMenu)
end

local function GetThumbContainerObj(self)
	local tree = self:GetOwner()
	local wnd  = tree:GetBindHostWnd() 
	local userData = wnd:GetUserData()
	
	return userData and userData.thumbContainerObj
end

local function QuickSort(tPictures, sortKey)
	table.sort(tPictures,function (v1,v2)
									if sortKey == "FileName" then
										local name1 = string.match(v1.szPath, ".+[\\/]([^?]+)")
										local name2 = string.match(v2.szPath, ".+[\\/]([^?]+)")
										if name1 < name2 then
											return true
										end
										return false
									end
									if v1[sortKey] < v2[sortKey] then
										return true
									else
										return false
									end
							end)
end

local menuFunTable = {}
function menuFunTable.OnSelect_FileName(self)
	local thumbContainerObj = GetThumbContainerObj(self)
	if not thumbContainerObj then
		XLMessageBox("hj")
		return
	end
	local attr = thumbContainerObj:GetAttribute()
	local tPictures = attr.tPictures
	
	QuickSort(tPictures, "FileName")
	attr.pageManager:Init(thumbContainerObj, tPictures)
end

function menuFunTable.OnSelect_Size(self)
	local thumbContainerObj = GetThumbContainerObj(self)
	if not thumbContainerObj then
		return
	end
	local attr = thumbContainerObj:GetAttribute()
	local tPictures = attr.tPictures
	
	QuickSort(tPictures, "uFileSize")
	attr.pageManager:Init(thumbContainerObj, tPictures)
end

function menuFunTable.OnSelect_OperateTime(self)
	local thumbContainerObj = GetThumbContainerObj(self)
	if not thumbContainerObj then
		return
	end
	local attr = thumbContainerObj:GetAttribute()
	local tPictures = attr.tPictures
	
	QuickSort(tPictures, "utcLastWriteTime")
	attr.pageManager:Init(thumbContainerObj, tPictures)
end

function menuFunTable.OnSelect_Ext(self)
	local thumbContainerObj = GetThumbContainerObj(self)
	if not thumbContainerObj then
		return
	end
	local attr = thumbContainerObj:GetAttribute()
	local tPictures = attr.tPictures
	
	QuickSort(tPictures, "szExt")
	attr.pageManager:Init(thumbContainerObj, tPictures)
end


local menuTable = {
{id="FileName", text = "文件名"},
{id="Size", text = "大小"},
{id="OperateTime", text = "修改日期"},
{id="Ext", text = "类型"},
}

GreenShieldMenu.SortMenu = {}
GreenShieldMenu.SortMenu.menuTable = menuTable
GreenShieldMenu.SortMenu.menuFunTable = menuFunTable

