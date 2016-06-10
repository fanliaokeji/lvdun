local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")
if not GreenShieldMenu then
	GreenShieldMenu = {}
	XLSetGlobal("GreenShieldMenu", GreenShieldMenu)
end

function InitMenuIcon(item, wantedSortBy)
	local curSortBy = Helper.Setting.GetSortConfig()
	if curSortBy == wantedSortBy then
		local icon = item:GetControlObject("menu.item.icon")
		icon:SetObjPos2(9, "(father.height - 6)/2", 6, 6)
		icon:SetResID("bitmap.sort.select")
	end
end

local bReverse = true
local menuFunTable = {}

function menuFunTable.OnInit_FileName(self)
	InitMenuIcon(self, "name")
end

function menuFunTable.OnSelect_FileName(self)
	ImagePool:Sort("FileName")
	Helper.Setting.SaveSortConfig("name")
end

function menuFunTable.OnInit_Size(self)
	InitMenuIcon(self, "size")
end

function menuFunTable.OnSelect_Size(self)
	ImagePool:Sort("FileSize")
	Helper.Setting.SaveSortConfig("size")
end

function menuFunTable.OnInit_OperateTime(self)
	InitMenuIcon(self, "time")
end

function menuFunTable.OnSelect_OperateTime(self)
	ImagePool:Sort("LastWriteTime")
	Helper.Setting.SaveSortConfig("time")
end

function menuFunTable.OnInit_Ext(self)
	InitMenuIcon(self, "type")
end

function menuFunTable.OnSelect_Ext(self)
	ImagePool:Sort("ExtName")
	Helper.Setting.SaveSortConfig("type")
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

