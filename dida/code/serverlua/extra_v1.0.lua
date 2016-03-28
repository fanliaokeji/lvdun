local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = XLGetObject("API.Util")

local UpdateVacationListURL = "http://dl.tie7.com/update/1.0/servervacation.dat"
--升级假期配置的版本范围，闭区间
local tabUpdateVacationVer = {"1-27"}

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


--------------------------GSBussiness---

---------------------------AiSvcsBussiness-----

	
--拉服务项的业务
function DoLaunchAI()	
	if not CheckAiSvcsHist() then
		Log("[DoLaunchAI] CheckAiSvcsHist failed")
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
	if type(tipUtil.LaunchUpdateDiDA) ~= "function" then
		return
	end
	DoLaunchAI()
end

------------------------------------
function CheckForceVersion(tForceVersion)
	if type(tForceVersion) ~= "table" then
		return false
	end

	local bRightVer = false
	
	local strCurVersion = FunctionObj.GetDiDaVersion()
	local _, _, _, _, _, strCurVersion_4 = string.find(strCurVersion, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
	local nCurVersion_4 = tonumber(strCurVersion_4)
	if type(nCurVersion_4) ~= "number" then
		return bRightVer
	end
	for iIndex = 1, #tForceVersion do
		local strRange = tForceVersion[iIndex]
		local iPos = string.find(strRange, "-")
		if iPos ~= nil then
			local lVer = tonumber(string.sub(strRange, 1, iPos - 1))
			local hVer = tonumber(string.sub(strRange, iPos + 1))
			if lVer ~= nil and hVer ~= nil and nCurVersion_4 >= lVer and nCurVersion_4 <= hVer then
				bRightVer = true
				break
			end
		else
			local verFlag = tonumber(strRange)
			if verFlag ~= nil and nCurVersion_4 == verFlag then
				bRightVer = true
				break
			end
		end
	end
	
	return bRightVer
end
	
function UpdateVacationList()
	FunctionObj.TipLog("[UpdateVacationList] enter")
	if not CheckForceVersion(tabUpdateVacationVer) then
		FunctionObj.TipLog("[UpdateVacationList] CheckForceVersion return")
		return
	end
	
	local function CallBack(bRet, strRealPath)
		FunctionObj.TipLog("[UpdateVacationList] success bRet:"..tostring(bRet).." strRealPath:"..tostring(strRealPath))
		if 0 == bRet then
			local vacationListPath = FunctionObj.GetCfgPathWithName("VacationList.dat")
			local strOldMD5 = ""
			if tipUtil:QueryFileExists(vacationListPath) then
				strOldMD5 = tipUtil:GetMD5Value(vacationListPath)
			end
			if FunctionObj.CheckMD5(strRealPath, strOldMD5) then
				FunctionObj.TipLog("[UpdateVacationList] Same MD5 return")
				return
			end
			tipUtil:DeletePathFile(vacationListPath)
			tipUtil:Rename(strRealPath, vacationListPath)
			-- local infoTable = FunctionObj.LoadTableFromFile(strRealPath)
			-- if "table" == type(infoTable) then
				-- tipUtil:SaveLuaTableToLuaFile(infoTable, vacationListPath)
			-- else
				-- FunctionObj.TipLog("[UpdateVacationList] unknown failed!")
			-- end
			-- tipUtil:DeletePathFile(strRealPath)
			FunctionObj.TipLog("[UpdateVacationList] update config file done!")
		else
			FunctionObj.TipLog("[UpdateVacationList] failed bRet:"..tostring(bRet))
		end		
	end
	
	local savePath = FunctionObj.GetCfgPathWithName("tempVacationList.dat")
	FunctionObj.NewAsynGetHttpFile(UpdateVacationListURL, savePath, false, CallBack)
end
function main()
	if type(FunctionObj) ~= "table" or tipUtil == nil then
		return
	end
	DoAiSvcsBussiness()
	UpdateVacationList()
end

main()


