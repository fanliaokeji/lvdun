local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")
local tipUtil = XLGetObject("API.Util")

if not GreenShieldMenu then
	GreenShieldMenu = {}
	XLSetGlobal("GreenShieldMenu", GreenShieldMenu)
end

local menuFunTable = {}
function menuFunTable.OnSelect_Open(self)
	local fileFilter = "图片格式(*.bmp;*.cut;*.dds;*.exr;*.fax;*.gif;*.ico;*.iff;*.j2k;*.jng;*.jp2;*.jpeg;*.jpg;*.jxr;*.koala;*.lbm;*.mng;*.pbm;*.pcd;*.pcx;*.pfm;*.pgm;*.pict;*.png;*.ppm;*.ppmraw;*.psd;*.ras;*.sgi;*.tga;*.tiff;*.wbm;*.web;*.xbm;*.xpm)|*.bmp;*.cut;*.dds;*.exr;*.fax;*.gif;*.ico;*.iff;*.j2k;*.jng;*.jp2;*.jpeg;*.jpg;*.jxr;*.koala;*.lbm;*.mng;*.pbm;*.pcd;*.pcx;*.pfm;*.pgm;*.pict;*.png;*.ppm;*.ppmraw;*.psd;*.ras;*.sgi;*.tga;*.tiff;*.wbm;*.web;*.xbm;*.xpm|All Files(*.*)|*.*||"
	local strLocalFiles = tipUtil:FileDialog(true, fileFilter, "", "")
	if strLocalFiles == nil or #strLocalFiles == 0 then
		return
	end
	local clientobj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	local function FnCallBack()
		clientobj:UpdateFileList()
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
	local fileFilter = "图片格式(*.bmp;*.cut;*.dds;*.exr;*.fax;*.gif;*.ico;*.iff;*.j2k;*.jng;*.jp2;*.jpeg;*.jpg;*.jxr;*.koala;*.lbm;*.mng;*.pbm;*.pcd;*.pcx;*.pfm;*.pgm;*.pict;*.png;*.ppm;*.ppmraw;*.psd;*.ras;*.sgi;*.tga;*.tiff;*.wbm;*.web;*.xbm;*.xpm)|*.bmp;*.cut;*.dds;*.exr;*.fax;*.gif;*.ico;*.iff;*.j2k;*.jng;*.jp2;*.jpeg;*.jpg;*.jxr;*.koala;*.lbm;*.mng;*.pbm;*.pcd;*.pcx;*.pfm;*.pgm;*.pict;*.png;*.ppm;*.ppmraw;*.psd;*.ras;*.sgi;*.tga;*.tiff;*.wbm;*.web;*.xbm;*.xpm|All Files(*.*)|*.*||"
	local strLocalFiles = tipUtil:FileDialog(true, fileFilter, "", filename)
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
	clientobj:UpdateFileList()
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
		{id="SortByName", text = "名称", OnSelectFun = function(self) SortBy("name") end},
		{id="SortBySize", text = "大小", OnSelectFun = function(self) SortBy("size") end},
		{id="SortByType", text = "类型", OnSelectFun = function(self) SortBy("type") end},
		{id="SortByTime", text = "时间", OnSelectFun = function(self) SortBy("time") end},
	},
	OnInitFun = function(self)
		local attr = self:GetAttribute()
		local sortby = Helper.Setting.GetSortConfig()
		local function CheckOne(idx)
			for i, v in ipairs(attr.itemData["SubMenuTable"]) do
				if i == idx then
					v.iconNormalID = "setting_radio.icon"
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
			CheckOne(4)
		elseif sortby == "type" then
			CheckOne(3)
		end
	end,
},
{id="Spliter5", bSplitter = true},
{id="Delete", text = "删除", iconNormalID = "bitmap.menu.icon.del.normal", iconDisableID = "bitmap.menu.icon.del.disable"},
{id="Spliter6", bSplitter = true},
}

GreenShieldMenu.ImageRClickMenu = {}
GreenShieldMenu.ImageRClickMenu.menuTable = menuTable
GreenShieldMenu.ImageRClickMenu.menuFunTable = menuFunTable

