local Helper = XLGetGlobal("Helper")
local SettingHelper = Helper.Setting
local tipUtil = XLGetObject("API.Util")

local SpecialExts = {
	["JPG"] = ".jpeg;.jpg;",
	["PPM"] = ".ppm;.ppmraw;"
}

function Update(self)
	local ckboxobj
	for i = 3, 37 do
		ckboxobj = self:GetObject("chebox"..i)
		if tipUtil:IsAssociated("."..string.lower(ckboxobj:GetAttribute().Text)) then
			ckboxobj:SetCheck(true)
		else
			ckboxobj:SetCheck(false)
		end
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
	for i = 3, 37 do
		ckboxattr = self:GetObject("chebox"..i):GetAttribute()
		if ckboxattr.Select then
			strExtsDo = strExtsDo..(SpecialExts[ckboxattr.Text] or "."..string.lower(ckboxattr.Text)..";")
		else
			strExtsUnDo = strExtsUnDo..(SpecialExts[ckboxattr.Text] or "."..string.lower(ckboxattr.Text)..";")
		end
	end
	tipUtil:SetAssociate(strExtsUnDo, false, strExtsDo ~= "")
	tipUtil:SetAssociate(strExtsDo, true)
end

function chebox39OnSelect(self, event, ischeck)
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	attr.AllowCallApply = true
	local objcheckbox
	for i = 3, 37 do
		objcheckbox = owner:GetControlObject("chebox"..i)
		if objcheckbox then
			objcheckbox:SetCheck(ischeck, true)
		end
	end
end

function FileAssoPanelOnInitControl(self)
	local checkall = self:GetObject("chebox39")
	local attr = self:GetAttribute()
	local objcheckbox
	for i = 3, 37 do
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
				for j = 3, 37 do
					ckbox_attr = self:GetObject("chebox"..j):GetAttribute()
					if not ckbox_attr.Select then
						return
					end
				end
				checkall:SetCheck(true, true)
			end)
	end
end

