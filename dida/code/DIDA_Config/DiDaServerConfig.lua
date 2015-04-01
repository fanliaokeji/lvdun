function GetSubTable()
	local t = {
		["tNewVersionInfo"] = {
			["strVersion"] = "1.0.0.6",
			["strPacketURL"] = "http://down.didarili.com/client/DIDASetup_0001.exe",
			["strMD5"] = "1D536B51ED3B28889F79913D18673979",
			["tInroduce"] = {
				"1.修复部分bug",
				"",
				"",
			},
			
			["tForceUpdate"] = {
				["strVersion"] = "1.0.0.6",
				["tVersion"] = {"1-5"},
				["strPacketURL"] = "http://down.didarili.com/client/DIDASetup_0001.exe",
				["strMD5"] = "1D536B51ED3B28889F79913D18673979",
			},
		},
		
		["tExtraHelper"] = {
			["strURL"] = "http://dl.didarili.com/update/1.0/ddextra_v1.1.dat",
			["strMD5"] = "2B41499A9E2F537820ABD6BBDB174B5E",
			["param"] = {
				["nAISpanTimeInSec"] = 3*24*3600,
			},
		},
	}
	return t
end

