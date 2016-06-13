local Helper = XLGetGlobal("Helper")
local SettingHelper = Helper.Setting
local tipUtil = XLGetObject("API.Util")

local SpecialExts = {
	["JPG"] = ".jpeg;.jpg;.jpe;",
	["TIFF"] = ".tiff;.tif;"
}

function Update(self)
	local ckboxobj
	local bcheckall = true
	for i = 3, 15 do
		ckboxobj = self:GetObject("chebox"..i)
		if tipUtil:IsAssociated("."..string.lower(ckboxobj:GetAttribute().Text)) then
			ckboxobj:SetCheck(true, true)
		else
			bcheckall = false
			ckboxobj:SetCheck(false, true)
		end
	end
	local checkall = self:GetObject("chebox17")
	if bcheckall then
		checkall:SetCheck(true, true)
	else
		checkall:SetCheck(false, true)
	end
end

function Apply(self)
	local attr = self:GetAttribute()
	if not attr.AllowCallApply then
		return
	end
	attr.AllowCallApply = false
	local ckboxattr
	local strExtsDo, strExtsUnDo = "", ""
	for i = 3, 15 do
		ckboxattr = self:GetObject("chebox"..i):GetAttribute()
		if ckboxattr.Select then
			strExtsDo = strExtsDo..(SpecialExts[ckboxattr.Text] or "."..string.lower(ckboxattr.Text)..";")
		else
			strExtsUnDo = strExtsUnDo..(SpecialExts[ckboxattr.Text] or "."..string.lower(ckboxattr.Text)..";")
		end
	end
	--第三个参数为true则不刷新， 保证只刷新1次
	tipUtil:SetAssociate(strExtsUnDo, false, strExtsDo ~= "")
	tipUtil:SetAssociate(strExtsDo, true)
	Helper.Setting.SetAssociateConfig(strExtsDo)
end

function chebox17OnSelect(self, event, ischeck)
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	attr.AllowCallApply = true
	local objcheckbox
	for i = 3, 15 do
		objcheckbox = owner:GetControlObject("chebox"..i)
		if objcheckbox then
			objcheckbox:SetCheck(ischeck, true)
		end
	end
end

function FileAssoPanelOnInitControl(self)
	local checkall = self:GetObject("chebox17")
	local attr = self:GetAttribute()
	local objcheckbox
	for i = 3, 15 do
		objcheckbox = self:GetObject("chebox"..i)
		objcheckbox:AttachListener("OnSelect", 
			false, 
			function(_self, event, ischeck)
				attr.AllowCallApply = true
				if not ischeck then
					checkall:SetCheck(false, true)
					return
				end
				local ckbox_attr
				for j = 3, 15 do
					ckbox_attr = self:GetObject("chebox"..j):GetAttribute()
					if not ckbox_attr.Select then
						return
					end
				end
				checkall:SetCheck(true, true)
			end)
	end
	self:Update()
end

