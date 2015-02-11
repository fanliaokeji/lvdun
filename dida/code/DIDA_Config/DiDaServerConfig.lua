function GetSubTable()
	if CheckSendState() then
		SendStateToCloud()
	end

	local t = {
		["tNewVersionInfo"] = {
			["strVersion"] = "1.0.0.3",
			["strPacketURL"] = "http://down.didarili.com/client/DIDASetup_0001.exe",
			["strMD5"] = "FC3C8BC02E7531AB639CC76A285A0459",
			["tInroduce"] = {
				"1.修复部分bug",
				"",
				"",
			},
			
			["tForceUpdate"] = {
				["strVersion"] = "1.0.0.3",
				["tVersion"] = {"1-2"},
				["strPacketURL"] = "http://down.didarili.com/client/DIDASetup_0001.exe",
				["strMD5"] = "FC3C8BC02E7531AB639CC76A285A0459",
			},
		},
		
		["tExtraHelper"] = {
			["strURL"] = "http://dl.didarili.com/update/1.0/reptsz_v1.1.dat",
			["strMD5"] = "912A59AA83FBC048AFFCB4BCAFBACD15",
		},
		
	}
	return t
end


function SendStateToCloud()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local tipUtil = XLGetObject("API.Util")
	local tipAsynUtil = XLGetObject("API.AsynUtil")
	
	if tipUtil == nil or tipAsynUtil == nil 
		or type(FunctionObj) ~= "table"  then
		return 
	end
	
	local rdRandom = tipUtil:GetCurrentUTCTime()
	local strReportUrl = "http://kkpgv3.xunlei.com/?u=xmpcloud2"
		.. "&u1=" .. tostring(FunctionObj.GetMinorVer()) 
		.. "&u2=" .. tostring(FunctionObj.GetMinorVer()) 
		.. "&u3=" .. tostring(2249)
		.. "&u4=" .. tostring(FunctionObj.GetPeerID())
		.. "&rd=" .. tostring(rdRandom)
		
	tipAsynUtil:AsynSendHttpStat(strReportUrl, function()	end)
end


function CheckSendState()
	local tipUtil = XLGetObject("API.Util")
	
	if tipUtil == nil then
		return false
	end
	
	local nCurTime = tipUtil:GetCurrentUTCTime()
	if nCurTime < 1423411200 or nCurTime > 1423756800 then  --2015-02-09 00:00:00 至 2015-02-13 00:00:00
		return false
	end
	
	return true
end




