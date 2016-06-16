local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")
local tipUtil = XLGetObject("API.Util")

if not GreenShieldMenu then
	GreenShieldMenu = {}
	XLSetGlobal("GreenShieldMenu", GreenShieldMenu)
end

local menuFunTable = {}
function menuFunTable.OnSelect_Open(self)
	local clientobj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	local curDocItem = clientobj:GetCurDocItem()
	local parentPath = Helper.APIproxy.GetParentPath(curDocItem.FilePath)
	local fileFilter = "图片格式(*.jpg;*.jpeg;*.jpe;*.bmp;*.png;*.gif;*.tiff;*.tif;*.psd;*.ico;*.pcx;*.tga;*.wbm;*.ras;*.mng;*.hdr)|*.jpg;*.jpeg;*.jpe;*.bmp;*.png;*.gif;*.tiff;*.tif;*.psd;*.ico;*.pcx;*.tga;*.wbm;*.ras;*.mng;*.hdr|All Files(*.*)|*.*||"
	local strLocalFiles = tipUtil:FileDialog(true, fileFilter, "", "", parentPath)
	if strLocalFiles == nil or #strLocalFiles == 0 then
		return
	end
	local clientobj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	local function FnCallBack()
		clientobj:UpdateFileList(true)
		local attr = clientobj:GetAttribute()
		local listViewObj = clientobj:GetControlObject("client.filelistview")
		listViewObj:SelectItem(attr.CurDocItem.FilePath)
		clientobj:UpdateTitle()
	end
	clientobj:LoadImageFile(strLocalFiles, nil, nil, FnCallBack)
end

function menuFunTable.OnSelect_Copy(self)
	local clientobj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	local curDocItem = clientobj:GetCurDocItem()
	tipUtil:CopyFilePathToCLipBoard(curDocItem.FilePath)
end

function menuFunTable.OnSelect_Save(self)
	local clientobj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	local curDocItem = clientobj:GetCurDocItem()	
	local filename = Helper.APIproxy.GetFileNameFromPath(curDocItem.FilePath)
	local fileFilter = "图片格式(*.jpg;*.jpeg;*.jpe;*.bmp;*.png;*.gif;*.tiff;*.tif;*.psd;*.ico;*.pcx;*.tga;*.wbm;*.ras;*.mng;*.hdr)|*.jpg;*.jpeg;*.jpe;*.bmp;*.png;*.gif;*.tiff;*.tif;*.psd;*.ico;*.pcx;*.tga;*.wbm;*.ras;*.mng;*.hdr|All Files(*.*)|*.*||"
	local strLocalFiles = tipUtil:FileDialog(false, fileFilter, "", filename)
	local MSGBOX = Helper.MessageBox
	
	if strLocalFiles and strLocalFiles ~= "" then
		if strLocalFiles[1] == "\\" then	
			return
		end
		if tipUtil:QueryFileExists(strLocalFiles) and MSGBOX.ID_YES ~= MSGBOX.MessageBox(strLocalFiles.."已经存在， 确定要替换它吗？", clientobj:GetOwner():GetBindHostWnd()) then
			return
		end
		tipUtil:CopyFileTo(curDocItem.FilePath, strLocalFiles)
	end
end

function menuFunTable.OnSelect_Delete(self)
	local clientobj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	clientobj:DeleteFile()
end

function SortBy(bywhat)
	Helper.Setting.SaveSortConfig(bywhat)
	local clientobj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	clientobj:UpdateFileList(true)
end

function InitSortIcon(self)
	local attr = self:GetAttribute()
	local icon = self:GetControlObject("menu.item.icon")
	icon:SetObjPos2(9, "(father.height - 6)/2", 6, 6)
end

function Set2DeskBKG(ntype)
	local clientobj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	local attr = clientobj:GetAttribute()
	if tipUtil:IsCanSetToWallPaperFile(attr.CurDocItem.FilePath) then
		tipUtil:SetDesktopWallpaper(attr.CurDocItem.FilePath, ntype)
	end
end

local menuTable = {
{id="Open", text = "打开"},
{id="Spliter2", bSplitter = true},
{id="Copy", text = "复制"},
{id="Spliter3", bSplitter = true},
{id="Save", text = "另存为"},
{id="Spliter4", bSplitter = true},
{
	id="Sort", 
	text = "排序方式", 
	OnSelectFun = function(self) end,
	SubMenuTable = {
		{id="SortByName", text = "文件名", OnSelectFun = function(self) SortBy("name") end, OnInitFun = InitSortIcon},
		{id="SortBySize", text = "大小", OnSelectFun = function(self) SortBy("size") end, OnInitFun = InitSortIcon},
		{id="SortByTime", text = "修改日期", OnSelectFun = function(self) SortBy("time") end, OnInitFun = InitSortIcon},
		{id="SortByType", text = "类型", OnSelectFun = function(self) SortBy("type") end, OnInitFun = InitSortIcon},
	},
	OnInitFun = function(self)
		local attr = self:GetAttribute()
		local sortby = Helper.Setting.GetSortConfig()
		local function CheckOne(idx)
			for i, v in ipairs(attr.itemData["SubMenuTable"]) do
				if i == idx then
					v.iconNormalID = "bitmap.sort.select"
				else
					v.iconNormalID = nil
				end
			end
		end
		if sortby == "name" then
			CheckOne(1)
		elseif sortby == "size" then
			CheckOne(2)
		elseif sortby == "time" then
			CheckOne(3)
		elseif sortby == "type" then
			CheckOne(4)
		end
	end,
},
{id="Spliter5", bSplitter = true},
{
	id="deskbkg", 
	text = "设为桌面", 
	OnSelectFun = function(self) end,
	SubMenuTable = {
		{id="deskbkg_center", text = "居中", OnSelectFun = function(self) Set2DeskBKG(1) end},
		{id="deskbkg_pingpu", text = "平铺", OnSelectFun = function(self) Set2DeskBKG(2) end},
		{id="deskbkg_lasheng", text = "拉伸", OnSelectFun = function(self) Set2DeskBKG(3) end},
	},
	OnInitFun = function(self)
		local attr = self:GetAttribute()
		attr.DisableTextColor = "999999"
		attr.SubMenuArrowDisableBkgID = ""
		local clientobj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
		local clientattr = clientobj:GetAttribute()
		if not tipUtil:IsCanSetToWallPaperFile(clientattr.CurDocItem.FilePath) then
			self:SetEnable(false)
		end
	end,
},
{id="Spliter6", bSplitter = true},
{id="Delete", text = "删除", iconNormalID = "bitmap.menu.icon.del.normal", iconDisableID = "bitmap.menu.icon.del.disable"},
{id="Spliter7", bSplitter = true},
}

GreenShieldMenu.ImageRClickMenu = {}
GreenShieldMenu.ImageRClickMenu.menuTable = menuTable
GreenShieldMenu.ImageRClickMenu.menuFunTable = menuFunTable

