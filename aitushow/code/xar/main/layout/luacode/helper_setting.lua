local Helper = XLGetGlobal("Helper")
Helper.Setting = {}
local Setting = Helper.Setting

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
	local nRet = tipUtil:QueryRegValue("HKEY_CURRENT_USER", "Software\\kuaikan\\setting", "delremind")
	return tonumber(nRet) ~= 0
end

function Setting.GetRotateType()
	local strRet = tipUtil:QueryRegValue("HKEY_CURRENT_USER", "Software\\kuaikan\\setting", "rotate")
	strRet = string.lower(tostring(strRet))
	return (strRet ~= "rensave" and strRet ~= "coverold" and strRet ~= "nosave" and strRet ~= "askme") and "askme" or strRet
end

function Setting.GetExitType()
	local nRet = tipUtil:QueryRegValue("HKEY_CURRENT_USER", "Software\\kuaikan\\setting", "exittray")
	return tonumber(nRet) ~= 0
end

function Setting.SetSysBoot(isDo)
	if not isDo then
		tipUtil:DeleteRegValue("HKEY_CURRENT_USER", "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", "kuaikan")
		return
	end
	local strExePath = tipUtil:QueryRegValue("HKEY_LOCAL_MACHINE", "Software\\kuaikan", "Path")
	if not IsRealString(strExePath) or tipUtil:QueryFileExists(strExePath) then
		return
	end
	tipUtil:SetRegValue("HKEY_CURRENT_USER", "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", "kuaikan", "\""..strExePath.."\" /sstartfrom sysboot /embedding")
end

function Setting.SetDelRemind(nValue)
	nValue = tonumber(nValue) or 0
	RegSetValue("HKEY_CURRENT_USER\\Software\\kuaikan\\setting\\delremind", nValue)
end

function Setting.SetRotateType(strValue)
	RegSetValue("HKEY_CURRENT_USER\\Software\\kuaikan\\setting\\rotate", strValue)
end

function Setting.SetExitType(nValue)
	nValue = tonumber(nValue) or 0
	RegSetValue("HKEY_CURRENT_USER\\Software\\kuaikan\\setting\\exittray", nValue)
end