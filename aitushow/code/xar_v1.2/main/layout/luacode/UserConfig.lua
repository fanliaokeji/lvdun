
--设置项
local UserConfig = ObjectBase:New()
XLSetGlobal("UserConfig", UserConfig)
UserConfig.path = Helper:GetUserDataDir().."\\kuaikan\\UserConfig.lua"

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

function UserConfig:Load()
	--从配置文件里读取上次的配置
	self.configData = Helper:LoadLuaTable(self.path) or {}
	--删除失效的配置项
	delinvalid(self.configData)
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



