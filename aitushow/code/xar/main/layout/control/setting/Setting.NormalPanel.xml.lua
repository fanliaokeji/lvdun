local Helper = XLGetGlobal("Helper")
local SettingHelper = Helper.Setting

function Update(self)
	local ckbox_sysboot = self:GetObject("chebox3")
	local ckbox_delremind = self:GetObject("chebox4")
	local radio_rensave = self:GetObject("radio7")
	local radio_covold = self:GetObject("radio8")
	local radio_nosave = self:GetObject("radio9")
	local radio_askme = self:GetObject("radio10")
	local radio_closetray = self:GetObject("radio13")
	local radio_mustclose = self:GetObject("radio14")
	
	ckbox_sysboot:SetCheck(SettingHelper.IsSysBoot())
	ckbox_delremind:SetCheck(SettingHelper.IsDelRemind())
	local strRotate = SettingHelper.GetRotateType()
	if strRotate == "rensave" then
		radio_rensave:SetCheck(true)
	elseif strRotate == "coverold" then
		radio_covold:SetCheck(true)
	elseif strRotate == "nosave" then
		radio_nosave:SetCheck(true)
	else
	end
	if SettingHelper.GetExitType() then
		radio_closetray:SetCheck(true)
	else
		radio_mustclose:SetCheck(true)
	end
end

function Apply(self)
	local attr = self:GetAttribute()
	if not attr.AllowCallApply then
		return
	end
	attr.AllowCallApply = false
	local attr_sysboot = self:GetObject("chebox3"):GetAttribute()
	local attr_delremind = self:GetObject("chebox4"):GetAttribute()
	local attr_rensave = self:GetObject("radio7"):GetAttribute()
	local attr_covold = self:GetObject("radio8"):GetAttribute()
	local attr_nosave = self:GetObject("radio9"):GetAttribute()
	local attr_askme = self:GetObject("radio10"):GetAttribute()
	local attr_closetray = self:GetObject("radio13"):GetAttribute()
	local attr_mustclose = self:GetObject("radio14"):GetAttribute()
	SettingHelper.SetSysBoot(attr_sysboot.Select)
	SettingHelper.SetDelRemind(attr_delremind.Select and 1 or 0)
	SettingHelper.SetRotateType(
		attr_rensave.Select and "rensave" or 
		attr_covold.Select and "coverold" or 
		attr_nosave.Select and "nosave" or 
		"askme"
	)
	SettingHelper.SetExitType(attr_closetray.Select and 1 or 0)
end

function OnSelectSaveOp(self, event, ischeck)
	local ower = self:GetOwnerControl()
	ower:GetAttribute().AllowCallApply  = true
	local id = self:GetID()
	for i = 7, 10 do
		if id ~= "radio"..i then
			local obj = ower:GetControlObject("radio"..i)
			obj:SetCheck(false, true)
		end
	end
end

function OnSelectExitOp(self, event, ischeck)
	local ower = self:GetOwnerControl()
	ower:GetAttribute().AllowCallApply  = true
	local id = self:GetID()
	for i = 13, 14 do
		if id ~= "radio"..i then
			local obj = ower:GetControlObject("radio"..i)
			obj:SetCheck(false, true)
		end
	end
end

function OnSelectBase(self, event, ischeck)
	local ower = self:GetOwnerControl()
	local attr = ower:GetAttribute()
	attr.AllowCallApply  = true
end
