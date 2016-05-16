local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")
if not GreenShieldMenu then
	GreenShieldMenu = {}
	XLSetGlobal("GreenShieldMenu", GreenShieldMenu)
end

local menuFunTable = {}
function menuFunTable.OnSelect_PlayAsPPT(self)
	XLMessageBox("刷新")
end

function menuFunTable.OnSelect_Cut(self)
	XLMessageBox("取消")
end

function menuFunTable.OnSelect_Copy(self)
	XLMessageBox("继续刷新")
end

function menuFunTable.OnSelect_SaveAs(self)
	XLMessageBox("还是刷新")
end

function menuFunTable.OnSelect_SetWallpaper(self)

end

function menuFunTable.OnSelect_Delete(self)

end

function menuFunTable.OnSelect_PicInfo(self)

end
function menuFunTable.OnSelect_SetWallpaper_sub1(self)

end



local menuTable = {
{id="PlayAsPPT", text = "播放幻灯片", iconNormalID = "bitmap.menu.icon.play.normal", iconDisableID = "bitmap.menu.icon.play.disable"},
{id="Spliter1", bSplitter = true},
{id="Cut", text = "剪切"},
{id="Spliter2", bSplitter = true},
{id="Copy", text = "复制"},
{id="Spliter3", bSplitter = true},
{id="SaveAs", text = "另存为..."},
{id="Spliter4", bSplitter = true},
{id="SetWallpaper", text = "设为桌面壁纸"},
{id="Spliter5", bSplitter = true},
{id="Delete", text = "删除", iconNormalID = "bitmap.menu.icon.del.normal", iconDisableID = "bitmap.menu.icon.del.disable"},
{id="Spliter6", bSplitter = true},
{id="PicInfo", text = "图片信息"},
}

-- menuTable[9].SubMenuTable = {
	-- {id="SetWallpaper.sub1", text = "继续刷新"},
-- }
-- menuTable[9].SubMenuFunTable = menuFunTable


GreenShieldMenu.ImageRClickMenu = {}
GreenShieldMenu.ImageRClickMenu.menuTable = menuTable
GreenShieldMenu.ImageRClickMenu.menuFunTable = menuFunTable

