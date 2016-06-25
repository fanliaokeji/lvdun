local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")
if not GreenShieldMenu then
	GreenShieldMenu = {}
	XLSetGlobal("GreenShieldMenu", GreenShieldMenu)
end
local tipUtil = XLGetObject("API.Util")

function GetEditObj(self)
	local tree = self:GetOwner()
	local menuWnd = tree:GetBindHostWnd()
	local userData = menuWnd:GetUserData()
	return userData.EditObj
end

local menuFunTable = {}
function menuFunTable.OnInit_undo(self)
	local edit = GetEditObj(self)
	if edit:CanUndo() then
		self:SetEnable(true)
	else
		self:SetEnable(false)
	end
end

function menuFunTable.OnSelect_undo(self)
	local edit = GetEditObj(self)
	if edit:CanUndo() then
		edit:Undo()
	end
end

function menuFunTable.OnInit_cut(self)
	local edit = GetEditObj(self)
	local text = edit:GetSelText()
	if type(text) == "string" and text ~= "" then
		self:SetEnable(true)
	else
		self:SetEnable(false)
	end
end

function menuFunTable.OnSelect_cut(self)
	local edit = GetEditObj(self)
	local text = edit:GetSelText()
	if type(text) == "string" and text ~= "" then
		edit:Cut()
	end
end

function menuFunTable.OnInit_copy(self)
	local edit = GetEditObj(self)
	local text = edit:GetSelText()
	if type(text) == "string" and text ~= "" then
		self:SetEnable(true)
	else
		self:SetEnable(false)
	end
end

function menuFunTable.OnSelect_copy(self)
	local edit = GetEditObj(self)
	local text = edit:GetSelText()
	if type(text) == "string" and text ~= "" then
		tipUtil:CopyTextToClipboard(text)
	end
end

function menuFunTable.OnInit_paste(self)
	if tipUtil:IsClipboardTextFormatAvailable() then
		self:SetEnable(true)
	else
		self:SetEnable(false)
	end
end

function menuFunTable.OnSelect_paste(self)
	local edit = GetEditObj(self)
	edit:Paste()
end

function menuFunTable.OnInit_delete(self)
	local edit = GetEditObj(self)
	local text = edit:GetSelText()
	if type(text) == "string" and text ~= "" then
		self:SetEnable(true)
	else
		self:SetEnable(false)
	end
end

function menuFunTable.OnSelect_delete(self)
	local edit = GetEditObj(self)
	edit:Clear()
end

function menuFunTable.OnInit_selall(self)
	local edit = GetEditObj(self)
	local text = edit:GetText()
	if type(text) == "string" and text ~= "" then
		self:SetEnable(true)
	else
		self:SetEnable(false)
	end
end

function menuFunTable.OnSelect_selall(self)
	local edit = GetEditObj(self)
	edit:SetSelAll()
end

local menuTable = {
{id="undo", text = "撤销"},
{id="Spliter2", bSplitter = true},
{id="cut", text = "剪切"},
{id="Spliter3", bSplitter = true},
{id="copy", text = "复制"},
{id="Spliter4", bSplitter = true},
{id="paste", text = "粘贴"},
{id="Spliter5", bSplitter = true},
{id="delete", text = "删除"},
{id="Spliter6", bSplitter = true},
{id="selall", text = "全选"},
}

GreenShieldMenu.AddressEditMenu = {}
GreenShieldMenu.AddressEditMenu.menuTable = menuTable
GreenShieldMenu.AddressEditMenu.menuFunTable = menuFunTable

