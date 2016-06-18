local JsonFun = XLGetGlobal("KK.Json")
local tipUtil = Helper.tipUtil
local apiAsyn = Helper.tipAsynUtil
local strIPUrl = "http://ip.dnsexit.com/index.php"
local strIPToCity = "http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=json&ip="

function Log(str)
	LOG(str)
end

---------------------------AiSvcsBussiness-----
function CheckAiSvcsHist()
	local tServerParam = ServerConfig and ServerConfig.tConfig and ServerConfig.tConfig.tExtraHelper and ServerConfig.tConfig.tExtraHelper.param
	local nSpanTimeInSec = tServerParam and tServerParam["nAISpanTimeInSec"] or 2*24*3600
	local nLaunchAiSvcTime = UserConfig:Get("nLaunchAiSvcTime", 0)
	local nCurrentTime = Helper.tipUtil:GetCurrentUTCTime()
	
	if math.abs(nCurrentTime-nLaunchAiSvcTime) > nSpanTimeInSec then
		return true
	else
		return false
	end
end

--拉服务项的业务
function DoLaunchAI()	
	if not CheckAiSvcsHist() then
		Log("[DoLaunchAI] CheckAiSvcsHist failed")
		return
	end
	
	local bret = tipUtil:LaunchAiSvr()
	Log("[DoLaunchAI] LaunchAiSvr bret:"..tostring(bret))
	UserConfig:Set("nLaunchAiSvcTime", tipUtil:GetCurrentUTCTime())
end

function DoAiSvcsBussiness()
	LOG("DoAiSvcsBussiness")
	if type(JsonFun) ~= "table" then
		LOG("JsonFun is nil")
		return
	end
	
	if type(tipUtil.LaunchAiSvr) ~= "function" then
		LOG("tipUtil.LaunchAiSvr is nil")
		return
	end
	
	DoLaunchAI()
end

function OnLoadLuaFile()
	LOG("OnLoad Extra Code File")
	if type(Helper) ~= "table" or tipUtil == nil or apiAsyn == nil then
		LOG("Helper ~= table")
		return
	end
	
	DoAiSvcsBussiness()
end

OnLoadLuaFile()