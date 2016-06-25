local Helper = XLGetGlobal("Helper")

-- pathTable:{[1]="E:", [2]="code"...}
local function GetPathTable(sPath)
	local pathTable = {}
	
	if sPath[#sPath] ~= '\\' then
		sPath = sPath .."\\"
	end
	for v in string.gmatch(sPath, "(%C-)\\") do
		table.insert(pathTable, v)
	end
	
	return pathTable
end

function OnClickButtonListBtn(self, btnSelf)
	local attr = self:GetAttribute()
	local id = btnSelf:GetID()
	local index = string.sub(id, 31, #id)
	index = tonumber(index)
	if "number" ~= type(index) then
		return
	end
	local buttonListContainer = self:GetControlObject("ButtonListContainer")
	for i=index+1, #attr.pathTable do
		
		local tmpBtnID = "AddressEdit.ButtonList.Button."..tostring(i)
		local tmpBtn = self:GetControlObject(tmpBtnID)
		if tmpBtn then
			buttonListContainer:RemoveChild(tmpBtn)
		else
			XLMessageBox(tostring(tmpBtnID))
		end
		local tmpArrowID = "AddressEdit.ButtonList.Arrow."..tostring(i - 1)
		local tmpArrow = self:GetControlObject(tmpArrowID)
		if tmpArrow then
			buttonListContainer:RemoveChild(tmpArrow)
		end
	end
	local curPath = ""
	for i=1, index do
		if i ~= index then
			curPath = curPath..attr.pathTable[i].."\\"
		else
			curPath = curPath..attr.pathTable[i]
		end
	end
	attr.curPath = curPath
	attr.pathTable = nil
	attr.pathTable = GetPathTable(curPath)
	self:FireExtEvent("OnPathChanged", attr.curPath)
end

function RemoveInvalid(t, pos)
	if type(t) ~= "table" or pos >= #t then
		return 
	end
	for i = #t, 1, -1 do
		if pos < i then
			table.remove(t, i)
		else
			break
		end
	end
end

--bNoNotify:不发通知事件
--isUndo是点击回退和向前按钮触发
function SetPath(self, sPath, bNoNotify, isUndo)
	local attr = self:GetAttribute()
	if not Helper.tipUtil:QueryFileExists(sPath) then
		sPath = attr.curPath
	end
	if not sPath or not Helper.tipUtil:QueryFileExists(sPath) then
		return false
	end
	
	attr.pathTable = GetPathTable(sPath)
	if #attr.pathTable <= 0 then
		return false
	end
	local editObj = self:GetControlObject("EditObj")
	editObj:SetEnable(false)
	editObj:SetVisible(false)
	editObj:SetChildrenVisible(false)
	editObj:SetText("")
	editObj:SetZorder(0)
	local buttonListContainer = self:GetControlObject("ButtonListContainer")
	buttonListContainer:RemoveAllChild()
	buttonListContainer:SetZorder(9999)
	buttonListContainer:SetVisible(true)
	buttonListContainer:SetChildrenVisible(true)
	local tmpText = Helper.objectFactory:CreateUIObject("tmpText", "TextObject")
	tmpText:SetTextFontResID("basecontrol.defaultfont.13")
	local curButtonListLength = 0
	for i=1, #attr.pathTable do
		tmpText:SetText(attr.pathTable[i])
		
		--获取文本所需宽度
		local textLength = tmpText:GetTextExtent()
		local buttonListBtnTempl = Helper.templateManager:GetTemplate("AddressEdit.ButtonList.Button.template", "ObjectTemplate")
		local buttonListArrowTempl = Helper.templateManager:GetTemplate("AddressEdit.ButtonList.Arrow.template", "ObjectTemplate")
		local newButton = buttonListBtnTempl:CreateInstance("AddressEdit.ButtonList.Button."..tostring(i))
		newButton:SetObjPos2(curButtonListLength, 0, textLength, "father.height")
		newButton:SetText(attr.pathTable[i])
		newButton:AttachListener("OnClick", false, function(btnSelf) OnClickButtonListBtn(self,btnSelf) end)
		buttonListContainer:AddChild(newButton)
		if i ~= #attr.pathTable then
			local newArrow  = buttonListArrowTempl:CreateInstance("AddressEdit.ButtonList.Arrow."..tostring(i))
			newArrow:SetObjPos2(curButtonListLength + textLength, "(father.height - 9) / 2", 9, 9)
			buttonListContainer:AddChild(newArrow)
			curButtonListLength = curButtonListLength + textLength + 9
		else
			curButtonListLength = curButtonListLength + textLength
		end
	end
	local NeedUndo = true
	if attr.curPath == sPath then
		bNoNotify = true
		NeedUndo = false
	end
	attr.curPath = sPath
	if not attr.pathHistoryQueue then
		attr.pathHistoryQueue = Helper.FixedLengthQueue:New()
	end
	attr.pathHistoryQueue:Insert(sPath)
	
	local function ProcessArrow()
		local uparrow = self:GetObject("UpArrow")
		--有父目录
		if string.match(sPath, "[^/\\]+[/\\][^/\\]+") then
			uparrow:Enable(true)
		else
			uparrow:Enable(false)
		end
		if not isUndo and NeedUndo then
			attr.UndoStack = attr.UndoStack or {}
			attr.UndoPos = attr.UndoPos or 0
			RemoveInvalid(attr.UndoStack, attr.UndoPos)
			table.insert(attr.UndoStack, sPath)
			attr.UndoPos = #attr.UndoStack
			local leftarrow = self:GetObject("LeftArrow")
			local rightarrow = self:GetObject("RightArrow")
			if #attr.UndoStack < 2 then
				leftarrow:Enable(false)
				rightarrow:Enable(false)
			else
				leftarrow:Enable(true)
				rightarrow:Enable(false)
			end
		end
	end
	ProcessArrow()
	if not bNoNotify then
		self:FireExtEvent("OnPathChanged", attr.curPath)
	end
end

function GetPath(self)

end

function OnClickLeftArrow(self)
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	if type(attr.UndoStack) == "table" and type(attr.UndoPos) == "number" and attr.UndoPos > 1 then
		attr.UndoPos = attr.UndoPos - 1
		local sPath = attr.UndoStack[attr.UndoPos]
		ownerCtrl:SetPath(sPath, false, true)
		if attr.UndoPos == 1 then
			self:Enable(false)
		end
		if attr.UndoPos < #attr.UndoStack then
			self:GetObject("control:RightArrow"):Enable(true)
		end
	end
end

function OnClickRightArrow(self)
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	if type(attr.UndoStack) == "table" and type(attr.UndoPos) == "number" and attr.UndoPos < #attr.UndoStack then
		attr.UndoPos = attr.UndoPos + 1
		local sPath = attr.UndoStack[attr.UndoPos]
		ownerCtrl:SetPath(sPath, false, true)
		if attr.UndoPos == #attr.UndoStack then
			self:Enable(false)
		end
		if attr.UndoPos > 1 then
			self:GetObject("control:LeftArrow"):Enable(true)
		end
	end
end

function OnClickUpArrow(self)
	local ownerCtrl = self:GetOwnerControl()
	local attr = ownerCtrl:GetAttribute()
	if not attr.curPath then return end
	local dir = string.gsub(attr.curPath, "[/\\]*$", "")
	local parent = string.match(dir, "^(.*)[/\\][^/\\]+$")
	if not parent then return end
	ownerCtrl:SetPath(parent, false)
end

function OnEditInputChar(self, charCode, counts, flag)
	if 0x0D == charCode then
		--self:SetFocus(false)
		self:GetOwnerControl():FocusChange()
	end
end

function FocusChange(self, bFocus)
	local edit = self:GetObject("EditObj")
	local text = edit:GetText()
	edit:SetEnable(false)
	edit:SetVisible(false)
	edit:SetZorder(0)
	edit:SetText("")
	
	
	local buttonListContainer = self:GetControlObject("ButtonListContainer")
	buttonListContainer:SetVisible(true)
	buttonListContainer:SetChildrenVisible(true)
	buttonListContainer:SetZorder(9999)
	
	SetPath(self, text)
end

function OnEditFocusChange(self, bFocus)
	--if bFocus then
	--	return
	--end
end

function OnEditLButtonUp(self)

end

function OnEditOnRButtonUp(self, x, y)
	if self:GetVisible() and self:GetEnable() then
		local curX, curY = Helper.tipUtil:GetCursorPos()
		local tree = self:GetOwner()
		local wnd = tree:GetBindHostWnd()
		local menuTable = GreenShieldMenu.AddressEditMenu.menuTable
		local menuFunTable = GreenShieldMenu.AddressEditMenu.menuFunTable
		local userData = {}
		userData.EditObj = self
		local Begin, End= self:GetSel()
		userData.OnPopupMenu = 
			function()
				if Begin ~= End then
					self:SetFocus(true)
					self:SetSel(Begin, End)
				end
			end
		Helper:CreateMenu(curX, curY, wnd:GetWndHandle(), menuTable, menuFunTable, userData)
	end
end

function OnButtonListContainerLButtonDown(self)
	self:RemoveAllChild()
	self:SetVisible(false)
	self:SetChildrenVisible(false)
	
	local ownerCtrl = self:GetOwnerControl()
	local editObj = ownerCtrl:GetControlObject("EditObj")
	editObj:SetEnable(true)
	editObj:SetZorder(9999)
	editObj:SetVisible(true)
	editObj:SetChildrenVisible(true)
	editObj:SetFocus(true)
	
	local attr = ownerCtrl:GetAttribute()
	editObj:SetText(attr.curPath)
end