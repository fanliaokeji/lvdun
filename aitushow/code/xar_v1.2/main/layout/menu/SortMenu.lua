local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")
if not GreenShieldMenu then
	GreenShieldMenu = {}
	XLSetGlobal("GreenShieldMenu", GreenShieldMenu)
end


local bReverse = true
local menuFunTable = {}
function menuFunTable.OnSelect_FileName(self)
	-- bReverse = not bReverse
	ImagePool:Sort("FileName")
end

function menuFunTable.OnSelect_Size(self)
	Helper:SaveLuaTable(ImagePool.tPictures, "E:\\before.lua")
	ImagePool:Sort("FileSize")
	Helper:SaveLuaTable(ImagePool.tPictures, "E:\\after.lua")
end

function menuFunTable.OnSelect_OperateTime(self)
	ImagePool:Sort("LastWriteTime")
end

function menuFunTable.OnSelect_Ext(self)
	ImagePool:Sort("ExtName")
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

