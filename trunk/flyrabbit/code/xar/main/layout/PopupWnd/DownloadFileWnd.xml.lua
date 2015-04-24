local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local tipAsynUtil = tFunHelper.tipAsynUtil

function OnCreate( self )
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move((workright + workleft-nLayoutWidth)/2, (worktop + workbottom-nLayoutHeight)/2, nLayoutWidth, nLayoutHeight)
end

function SetData(self, tData)
	local attr = self:GetAttribute()
	attr.okfunc = tData["okfunc"]
	attr.selectfunc = tData["selectfunc"]
	local name = self:GetControlObject("DownloadFile.FileDesc.Name")
	local size = self:GetControlObject("DownloadFile.FileDesc.Size")
	if name and IsRealString(tData["name"]) then
		name:SetText(tData["name"])
	end
	if size and IsRealString(tData["size"]) then
		size:SetText(tData["size"])
	end
end

function OnClose(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end

function OnSelect(self)
	local attr = self:GetAttribute()
	if attr.ForegroundResID == "DeleteTask.Select.Texture" then
		attr.ForegroundResID = "DeleteTask.UnSelect.Texture"
	else
		attr.ForegroundResID = "DeleteTask.Select.Texture"
	end
	local foreground = self:GetControlObject("Foreground")
	foreground:SetTextureID(attr.ForegroundResID)
end

function OnClickOK(self)
	local ctrl = self:GetOwnerControl()
	local atrlattr = ctrl:GetAttribute()
	if type(atrlattr.okfunc) == "function" then
		atrlattr.okfunc()
	end
	local selectctrl = ctrl:GetControlObject("DownloadFile.SelectBtn")
	local selattr = selectctrl:GetAttribute()
	if selattr.ForegroundResID == "DeleteTask.Select.Texture" then
		if type(atrlattr.selectfunc) == "function" then
			atrlattr.selectfunc()
		end
	end
	OnClose(self)
end

function OnEditFocusChange(self, isfocus)
	local bkg = self:GetParent()
	if isfocus then
		if bkg then
			bkg:SetTextureID("edit.long.select")
		end
	else
		if bkg then
			bkg:SetTextureID("edit.long.normal")
		end
	end
end

function OnPopDirSelectDialog(self)
end

function OnShowWindow(self, bVisible)
end

--------------------------
function IsRealString(str)
	return type(str) == "string" and str~=""
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@TipUpdateWnd: " .. tostring(strLog))
	end
end

