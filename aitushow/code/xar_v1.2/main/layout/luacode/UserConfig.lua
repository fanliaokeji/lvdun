
--设置项
local UserConfig = ObjectBase:New()
XLSetGlobal("UserConfig", UserConfig)
UserConfig.path = Helper:GetUserDataDir().."\\kuaikan\\UserConfig.lua"

function UserConfig:Load()
	--从配置文件里读取上次的配置
	self.configData = Helper:LoadLuaTable(self.path) or {}
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



