local Helper = XLGetGlobal("Helper")
local SettingHelper = Helper.Setting
local tipUtil = XLGetObject("API.Util")

local SpecialExts = {
	["JPG"] = ".jpeg;.jpg;.jpe;",
	["TIFF"] = ".tiff;.tif;"
}

local SpecialExtsCount = {
	["JPG"] = 3,
	["TIFF"] = 2,
}

function Update(self)
	local ckboxobj
	local bcheckall = true
	for i = 3, 21 do
		ckboxobj = self:GetObject("chebox"..i)
		if tipUtil:IsAssociated("."..string.lower(ckboxobj:GetAttribute().Text)) then
			ckboxobj:SetCheck(true, true)
		else
			bcheckall = false
			ckboxobj:SetCheck(false, true)
		end
	end
	local checkall = self:GetObject("chebox23")
	if bcheckall then
		checkall:SetCheck(true, true)
	else
		checkall:SetCheck(false, true)
	end
end

function IsUACOS()
	local bRet = true
	local iMax, iMin = tipUtil:GetOSVersion()
	if type(iMax) == "number" and iMax <= 5 then
		bRet = false
	end
	return bRet
end

function IsUserAdmin()
	local bRet = false
	if type(tipUtil.GetProcessElevation) == "function" then
		local bResult, iElevation, bAdmin = tipUtil:GetProcessElevation()
		local iMax, iMin = tipUtil:GetOSVersion()
		if (bResult and iElevation == 2 and bAdmin) or not bResult then
			bRet = true
		elseif type(iMax) == "number" and iMax >= 6 then
			if bResult and bAdmin and iElevation == 1 then
				bRet = true
			end
		end
	elseif not IsUACOS() then
		bRet = true
	end
	return bRet
end
XLSetGlobal("IsUserAdmin", IsUserAdmin)

function ChangeApplyButtonImg(self)
	if IsUserAdmin() then
		return
	end
	local apply = self:GetObject("control:apply")
	if apply then
		local attr = apply:GetAttribute()
		attr.NormalBkgID = "setting_apply_admin.normal"
		attr.HoverBkgID = "setting_apply_admin.hover"
		attr.DownBkgID = "setting_apply_admin.down"
		attr.DisableBkgID = "setting_apply_admin.normal"
		apply:Updata()
	end
end

function Apply(self)
	local attr = self:GetAttribute()
	if not attr.AllowCallApply then
		return
	end
	attr.AllowCallApply = false
	local ckboxattr
	local strExtsDo, strExtsUnDo, NeedReport, count = "", "", false, 0
	for i = 3, 21 do
		ckboxattr = self:GetObject("chebox"..i):GetAttribute()
		if ckboxattr.Select then
			if not NeedReport and not tipUtil:IsAssociated("."..string.lower(ckboxattr.Text)) then
				NeedReport = true--只要有新增的就上报
			end
			count = count + (SpecialExtsCount[ckboxattr.Text] or 1)
			strExtsDo = strExtsDo..(SpecialExts[ckboxattr.Text] or "."..string.lower(ckboxattr.Text)..";")
		else
			strExtsUnDo = strExtsUnDo..(SpecialExts[ckboxattr.Text] or "."..string.lower(ckboxattr.Text)..";")
		end
	end
	tipUtil:SetAssociate(strExtsDo, strExtsUnDo, false, IsUserAdmin())
	Helper.AssociateUpdateFlag = false
	Helper.Setting.SetAssociateConfig(strExtsDo)
	if NeedReport then
		--抢关联上报
		StatUtil.SendStat({
			strEC = "associate",
			strEA = "manual",
			strEL = tostring(count),
			strEV = 1,
		}) 
	end
end

function chebox23OnSelect(self, event, ischeck)
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	attr.AllowCallApply = true
	ChangeApplyButtonImg(owner)
	local objcheckbox
	for i = 3, 21 do
		objcheckbox = owner:GetControlObject("chebox"..i)
		if objcheckbox then
			objcheckbox:SetCheck(ischeck, true)
		end
	end
end

function FileAssoPanelOnInitControl(self)
	local checkall = self:GetObject("chebox23")
	local attr = self:GetAttribute()
	local objcheckbox
	for i = 3, 21 do
		objcheckbox = self:GetObject("chebox"..i)
		objcheckbox:AttachListener("OnSelect", 
			false, 
			function(_self, event, ischeck)
				attr.AllowCallApply = true
				ChangeApplyButtonImg(self)
				if not ischeck then
					checkall:SetCheck(false, true)
					return
				end
				local ckbox_attr
				for j = 3, 21 do
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

