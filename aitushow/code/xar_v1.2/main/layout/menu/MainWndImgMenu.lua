local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")
if not GreenShieldMenu then
	GreenShieldMenu = {}
	XLSetGlobal("GreenShieldMenu", GreenShieldMenu)
end
local tipUtil = XLGetObject("API.Util")

function GetSelectObj(self)
	local tree = self:GetOwner()
	local menuWnd = tree:GetBindHostWnd()
	local userData = menuWnd:GetUserData()
	return userData.SelectObj
end

local menuFunTable = {}
function menuFunTable.OnInit_open(self)
	local sel = GetSelectObj(self)
	if sel then
		self:SetEnable(true)
	else
		self:SetEnable(false)
	end
end

function menuFunTable.OnSelect_open(self)
	local sel = GetSelectObj(self)
	if sel then
		sel:LButtonDbClick()
	end
end

function menuFunTable.OnInit_copy(self)
	local sel = GetSelectObj(self)
	if sel then
		local attr = sel:GetAttribute()
		if attr and type(attr.data) == "table" and attr.data.FilePath then
			self:SetEnable(true)
			return
		end
	end
	self:SetEnable(false)
end

function menuFunTable.OnSelect_copy(self)
	local sel = GetSelectObj(self)
	if sel then
		local attr = sel:GetAttribute()
		if attr and type(attr.data) == "table" and attr.data.FilePath then
			Helper.tipUtil:CopyFilePathToCLipBoard(attr.data.FilePath)
		end
	end
end


function menuFunTable.OnInit_leftrrotate(self)
	local sel = GetSelectObj(self)
	if sel then
		local attr = sel:GetAttribute()
		if attr and type(attr.data) == "table" and string.lower(attr.data.ExtName) ~= ".gif" then
			self:SetEnable(true)
			return
		end
	end
	self:SetEnable(false)
end

function menuFunTable.OnSelect_leftrrotate(self)
	local sel = GetSelectObj(self)
	if sel then
		sel:Rotate(true)
	end
end

function menuFunTable.OnInit_rightrotate(self)
	menuFunTable.OnInit_leftrrotate(self)
end

function menuFunTable.OnSelect_rightrotate(self)
	local sel = GetSelectObj(self)
	if sel then
		sel:Rotate(false)
	end
end

function menuFunTable.OnInit_delete(self)
	menuFunTable.OnInit_open(self)
end

function menuFunTable.OnSelect_delete(self)
	local sel = GetSelectObj(self)
	if sel then
		local ThumbnailContainerObj = sel:GetOwner():GetUIObject("ThumbnailContainerObj")
		local attr = ThumbnailContainerObj:GetAttribute()
		attr.pageManager = attr.pageManager or {}
		local oldsel = attr.pageManager.selectedObj
		attr.pageManager.selectedObj = sel
		local tree = sel:GetOwner()
		local deletebtn = tree:GetUIObject("MainWnd.DeleteButton")
		if deletebtn then
			local delattr = deletebtn:GetAttribute()
			if delattr.Enable then
				deletebtn:FireExtEvent("OnClick", 0, 0)
			end
		end
		attr.pageManager.selectedObj = oldsel
	end
end

local menuTable = {
{id="open", text = "打开"},
{id="Spliter2", bSplitter = true},
{id="copy", text = "复制"},
{id="Spliter3", bSplitter = true},
{id="leftrrotate", text = "左旋"},
{id="Spliter4", bSplitter = true},
{id="rightrotate", text = "右旋"},
{id="Spliter5", bSplitter = true},
{id="delete", text = "删除"},
}

GreenShieldMenu.MainWndImgMenu = {}
GreenShieldMenu.MainWndImgMenu.menuTable = menuTable
GreenShieldMenu.MainWndImgMenu.menuFunTable = menuFunTable

