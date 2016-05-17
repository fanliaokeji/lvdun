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

function SetPath(self, sPath, bNoNotify)
	if not Helper.tipUtil:QueryFileExists(sPath) then
		return false
	end
	local attr = self:GetAttribute()
	attr.pathTable = GetPathTable(sPath)
	if #attr.pathTable <= 0 then
		return false
	end
	local editObj = self:GetControlObject("EditObj")
	editObj:SetEnable(false)
	editObj:SetVisible(false)
	editObj:SetChildrenVisible(false)
	local buttonListContainer = self:GetControlObject("ButtonListContainer")
	buttonListContainer:RemoveAllChild()
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
	
	attr.curPath = sPath
	if not attr.pathHistoryQueue then
		attr.pathHistoryQueue = Helper.FixedLengthQueue:New()
	end
	attr.pathHistoryQueue:Insert(sPath)
	if not bNoNotify then
		self:FireExtEvent("OnPathChanged", attr.curPath)
	end
end

function GetPath(self)

end

function OnClickLeftArrow(self)
	local ownerCtrl = self:GetOwnerControl()
	SetPath(ownerCtrl, "E:\\project\\gitHub\\COM_A\\trunk\\aitushow\\code")
end

function OnClickRightArrow(self)

end

function OnClickUpArrow(self)

end

function OnEditInputChar(self, charCode, counts, flag)
	if 0x0D == charCode then
		self:SetFocus(false)
	end
end

function OnEditFocusChange(self, bFocus)
	if bFocus then
		return
	end
	local ownerCtrl = self:GetOwnerControl()
	local text = self:GetText()
	self:SetEnable(false)
	self:SetVisible(false)
	self:SetZorder(0)
	self:SetText("")
	
	
	local buttonListContainer = ownerCtrl:GetControlObject("ButtonListContainer")
	buttonListContainer:SetVisible(true)
	buttonListContainer:SetChildrenVisible(true)
	buttonListContainer:SetZorder(9999)
	
	SetPath(ownerCtrl, text)
end

function OnEditLButtonUp(self)

end

function OnButtonListContainerLButtonUp(self)
	self:RemoveAllChild()
	self:SetVisible(false)
	self:SetChildrenVisible(false)
	
	local ownerCtrl = self:GetOwnerControl()
	local editObj = ownerCtrl:GetControlObject("EditObj")
	editObj:SetEnable(true)
	editObj:SetZorder(9999)
	editObj:SetFocus(true)
	editObj:SetVisible(true)
	editObj:SetChildrenVisible(true)
	
	local attr = ownerCtrl:GetAttribute()
	editObj:SetText(attr.curPath)
end