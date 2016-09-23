
--设置项
local UserConfig = ObjectBase:New()
XLSetGlobal("UserConfig", UserConfig)
UserConfig.path = Helper:GetUserDataDir().."\\kuaikantu\\UserConfig.lua"
local tipUtil = XLGetObject("API.Util")

function delinvalid(tcfg)
	local keylist = {}
	for k, v in pairs(tcfg) do
		if string.find(string.lower(tostring(k)), "_del$") then
			keylist[#keylist+1] = k
		end
		if type(v) == "table" then
			delinvalid(v)
		end
	end
	for _, v in ipairs(keylist) do
		tcfg[v] = nil
	end
end

function MergeUserConfig()
	local strConfigPath2 = Helper:GetUserDataDir().."\\kuaikantu\\UserConfig2.lua"
	if not Helper:IsRealString(strConfigPath2) or not tipUtil:QueryFileExists(strConfigPath2) then
		return
	end
	local tConfig2 =  Helper:LoadLuaTable(strConfigPath2) or {}
	tipUtil:DeletePathFile(strConfigPath2)
	local nInstallType = Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\kuaikantu\\InstallType")
	local InstallMethod = Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\kuaikantu\\InstallMethod")
	if nInstallType == 0 or InstallMethod == "nosilent" then--是非静默，或初次安装 才合并
		local tRead = UserConfig:Get("setting") or {}
		tRead["sysboot"] =  tConfig2["setting"]["sysboot"]
		tRead["associate"] = tConfig2["setting"]["associate"]
		UserConfig:Set("setting", tRead)
	end
end

function UserConfig:Load()
	--从配置文件里读取上次的配置
	self.configData = Helper:LoadLuaTable(self.path) or {}
	--删除失效的配置项
	delinvalid(self.configData)
	--合并配置
	MergeUserConfig()
end

--带#的设置项只对当前进程有效
function UserConfig:SaveToFile()
	Helper:SaveLuaTable(self.configData, self.path)
end

function UserConfig:Set(key, value)
	self.configData[key] = value
	self:SaveToFile()
end

function UserConfig:Get(key, defaultValue)
	return self.configData[key] or defaultValue
end

UserConfig:Load()



