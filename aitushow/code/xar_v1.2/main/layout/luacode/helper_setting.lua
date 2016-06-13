local Helper = XLGetGlobal("Helper")
Helper.Setting = {}
local Setting = Helper.Setting
UserConfig = UserConfig or {Get = function() end, Set = function() end}

local tipUtil = XLGetObject("API.Util")
function IsRealString(str) return type(str) == "string" and str ~= "" end

function RegSetValue(sPath, value)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			tipUtil:CreateRegKey(sRegRoot, sRegPath)
			return tipUtil:SetRegValue(sRegRoot, sRegPath, sRegKey or "", value or "")
		end
	end
	return false
end

function Setting.IsSysBoot()
	local bRet = false
	for _, hkey in ipairs({"HKEY_CURRENT_USER", "HKEY_LOCAL_MACHINE"}) do
		local strRet = tipUtil:QueryRegValue(hkey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", "kuaikan")
		if IsRealString(strRet) then
			local strRealPath = string.match(strRet, "[\"']([^\"']*)[\"']") or strRet
			if IsRealString(strRealPath) and tipUtil:QueryFileExists(strRealPath) then
				bRet = true
				break
			end
		end
	end
	return bRet
end

function Setting.IsDelRemind()
	local tRead = UserConfig:Get("setting") or {}
	local bRet = false
	if type(tRead) == "table" and tonumber(tRead["delremind"]) ~= 0 then
		bRet = true
	end
	return bRet
end

function Setting.GetRotateType()
	local tRead = UserConfig:Get("setting") or {}
	strRet = string.lower(tostring(tRead["rotate"]))
	return (strRet ~= "rensave" and strRet ~= "coverold" and strRet ~= "nosave" and strRet ~= "askme") and "askme" or strRet
end

function Setting.GetExitType()
	local tRead = UserConfig:Get("setting") or {}
	return tonumber(tRead["exittray"]) ~= 0
end

function Setting.SetSysBoot(isDo)
	local tRead = UserConfig:Get("setting") or {}
	tRead["sysboot"] = isDo
	UserConfig:Set("setting", tRead)
	if not isDo then
		tipUtil:DeleteRegValue("HKEY_CURRENT_USER", "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\kuaikan")
		return
	end
	local strExePath = tipUtil:QueryRegValue("HKEY_LOCAL_MACHINE", "Software\\kuaikan", "Path")
	if not IsRealString(strExePath) or not tipUtil:QueryFileExists(strExePath) then
		return
	end
	tipUtil:SetRegValue("HKEY_CURRENT_USER", "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", "kuaikan", "\""..strExePath.."\" /sstartfrom sysboot /embedding")
end

function Setting.SetDelRemind(nValue)
	local tRead = UserConfig:Get("setting") or {}
	tRead["delremind"] = tonumber(nValue) or 0
	UserConfig:Set("setting", tRead)
end

function Setting.SetRotateType(strValue)
	local tRead = UserConfig:Get("setting") or {}
	tRead["rotate"] = strValue
	UserConfig:Set("setting", tRead)
end

function Setting.SetExitType(nValue)
	nValue = tonumber(nValue) or 0
	local tRead = UserConfig:Get("setting") or {}
	tRead["exittray"] = nValue
	UserConfig:Set("setting", tRead)
end

--这里只记录用户的选择，界面的状态由实际关联注册表项控制
function Setting.SetAssociateConfig(strValue)
	strValue = tostring(strValue)
	local tRead = UserConfig:Get("setting") or {}
	tRead["associate"] = strValue
	UserConfig:Set("setting", tRead)
end

function Setting.GetAssociateConfig()
	strValue = tostring(strValue)
	local tRead = UserConfig:Get("setting") or {}
	return tRead["associate"] or ""
end

function Setting.GetSortConfig()
	local tRead = UserConfig:Get("setting") or {}
	local tSort = tRead["sort"] or {}
	local sortby = tSort["sortby"] or "time"
	local sorttype = tSort["sorttype"] or "positive"
	return sortby, sorttype
end

function Setting.SaveSortConfig(sortby, sorttype)
	sortby = sortby or "time"
	sorttype = sorttype or "positive"
	local tRead = UserConfig:Get("setting") or {}
	tRead["sort"] = tRead["sort"] or {}
	tRead["sort"]["sortby"] = sortby
	tRead["sort"]["sorttype"] = sorttype
	UserConfig:Set("setting", tRead)
end

function Setting.GetImageWindowConfig()
	local tRead = UserConfig:Get("setting") or {}
	local tWindow = tRead["imagewindow"] or {}
	return tWindow["state"], tWindow["width"], tWindow["height"]
end

function Setting.SetImageWindowConfig(state, width, height)
	local tRead = UserConfig:Get("setting") or {}
	tRead["imagewindow"] = tRead["imagewindow"] or {}
	tRead["imagewindow"]["state"] = state
	tRead["imagewindow"]["width"] = width
	tRead["imagewindow"]["height"] = height
	UserConfig:Set("setting", tRead)
	
end