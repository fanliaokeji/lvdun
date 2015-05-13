local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

-------事件---
function OnSelect_Copy(self)
	local objEdit = GetEditObject(self)
	if objEdit then
		objEdit:Copy()
	end
end


function OnSelect_Cut(self)
	local objEdit = GetEditObject(self)
	if objEdit then
		objEdit:Cut()
	end
end


function OnInit_Cut(self)
	local objEdit = GetEditObject(self)
	if not objEdit then
		return
	end
	
	local strSelText = objEdit:GetSelText()
	if not IsRealString(strSelText) then
		self:SetEnable(false)
	end		
end


function OnInit_Paste(self)
	local CF_TEXT = 1
	local bIsPasteAvlbl = tipUtil:IsClipboardFormatAvailable(CF_TEXT)
	if not bIsPasteAvlbl then
		self:SetEnable(false)
	end	
end



function OnSelect_Paste(self)
	local objEdit = GetEditObject(self)
	if objEdit then
		objEdit:Paste()
	end
end


function OnSelect_PasteAndDownLoad(self)
	local objEdit = GetEditObject(self)
	if objEdit then
		objEdit:Paste()
	end
	
	local objNewTaskCtrl = objEdit:GetOwnerControl()
	if not objNewTaskCtrl then
		return
	end
	objNewTaskCtrl:BeginDownLoad()	
end


function OnSelect_SelectAll(self)
	local objEdit = GetEditObject(self)
	if objEdit then
		objEdit:SetSelAll()
	end
end


function GetEditObject(objMenuItem)
	local objTree = objMenuItem:GetOwner()
	local objMainLayout = objTree:GetUIObject("Menu.MainLayout")
	local objNormalCtrl = objMainLayout:GetObject("Menu.Context")

	local objEdit = objNormalCtrl:GetRelateObject()
	return objEdit
end


function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

