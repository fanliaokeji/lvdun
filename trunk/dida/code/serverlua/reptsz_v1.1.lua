

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


function main()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper")
	local tipUtil = XLGetObject("API.Util")

	if type(FunctionObj) ~= "table" or tipUtil == nil 
		or type(tipUtil.TryToFix360) ~= "function" then
		return
	end

	local bPassCheck = CheckRepHistory()
	if not bPassCheck then
		return
	end
	
	tipUtil:TryToFix360()
	UpdateRepHistory()
end

main()



