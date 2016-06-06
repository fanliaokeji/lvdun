
function SetObjVisible(obj, bVisible)
	if not obj then return end
	
	if bVisible then
		obj:SetVisible(true)
		obj:SetChildrenVisible(true)
	else
		obj:SetVisible(false)
		obj:SetChildrenVisible(false)
	end
end


function OnInitControl(self)
	local preFileBtn = self:GetControlObject("btn.preFile")
	preFileBtn:AddTipText("上一张( <- )")
	local nextFileBtn = self:GetControlObject("btn.nextFile")
	nextFileBtn:AddTipText("下一张( -> 或空格)")
end

function OnBtnClick(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	local id = self:GetID()
	owner:FireExtEvent("OnBtnClick", id)
end

function ShowContent(self, bshow)
	local attr = self:GetAttribute()
	local bkgObj = self:GetControlObject("client.bkg")
	SetObjVisible(bkgObj, bshow and true or false)
end

function EnableToolbarItem(self, itemIds, bEnable)
	if type(itemIds) == "table" then
		for i=1, #itemIds do
			local btnObj = self:GetControlObject("btn." .. itemIds[i])
			if btnObj then
				btnObj:SetEnable(bEnable)
			end
		end
	else
		local btnObj = self:GetControlObject("btn." .. itemIds)
		if btnObj then
			btnObj:SetEnable(bEnable)
		end
	end
end


function GetToolbarItem(self, id)
	return self:GetControlObject("btn." .. id)
end