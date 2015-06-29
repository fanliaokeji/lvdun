local FunctionObj = XLGetGlobal("Project.FunctionHelper")
local tipUtil = XLGetObject("API.Util")
function Log(str)
	tipUtil:Log(tostring(str))
end

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end


function FetchValueByPath(obj, path)
	local cursor = obj
	for i = 1, #path do
		cursor = cursor[path[i]]
		if cursor == nil then
			return nil
		end
	end
	return cursor
end


	
--拉服务项的业务
function DoLaunchAI()	
	if not CheckAiSvcsHist() then
		Log("[DoLaunchAI] CheckAiSvcsHist failed")
		return
	end
	
	local bret = tipUtil:LaunchUpdate()
	Log("[DoLaunchAI] LaunchUpdate bret:"..tostring(bret))
	WriteAiSvcsHistory()
end


function CheckAiSvcsHist()
	local tServerParam = LoadServerConfig() or {}
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	local nLaunchAiSvcTime = tUserConfig["nLaunchAiSvcTime"] or 0
	local nSpanTimeInSec = tServerParam["nAISpanTimeInSec"] or 3*24*3600
	local nCurrentTime = tipUtil:GetCurrentUTCTime()
	
	if math.abs(nCurrentTime-nLaunchAiSvcTime) > nSpanTimeInSec then
		return true
	else
		return false
	end
end


function LoadServerConfig()
	local strCfgPath = FunctionObj.GetCfgPathWithName("RabbitServerConfig.dat")
	local infoTable = FunctionObj.LoadTableFromFile(strCfgPath) or {}
	local tParam = FetchValueByPath(infoTable, {"tExtraHelper", "param"})
	return tParam
end


function WriteAiSvcsHistory()
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nLaunchAiSvcTime"] = tipUtil:GetCurrentUTCTime()
	
	FunctionObj.SaveConfigToFileByKey("tUserConfig")
end
	
	
function DoAiSvcsBussiness()
	if type(tipUtil.LaunchUpdate) ~= "function" then
		return
	end
	
	local strInstallMethod = FunctionObj.RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\FlyRabbit\\InstallMethod")
	if not IsRealString(strInstallMethod) or strInstallMethod~="silent" then
		return 
	end

	DoLaunchAI()
end

------------------------------------	

function main()
	if type(FunctionObj) ~= "table" or tipUtil == nil then
		return
	end
	DoAiSvcsBussiness()
end

main()


