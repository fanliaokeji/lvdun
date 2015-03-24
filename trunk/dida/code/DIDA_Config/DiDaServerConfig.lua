function GetSubTable()
	local t = {
		["tNewVersionInfo"] = {
			["strVersion"] = "1.0.0.4",
			["strPacketURL"] = "http://down.didarili.com/client/DIDASetup_0001.exe",
			["strMD5"] = "F7EECA53C509B95D57A86C8F064F77E3",
			["tInroduce"] = {
				"1.修复部分bug",
				"",
				"",
			},
			
			["tForceUpdate"] = {
				["strVersion"] = "1.0.0.4",
				["tVersion"] = {"1-3"},
				["strPacketURL"] = "http://down.didarili.com/client/DIDASetup_0001.exe",
				["strMD5"] = "F7EECA53C509B95D57A86C8F064F77E3",
			},
		},
		
		["tExtraHelper"] = {
			["strURL"] = "http://dl.didarili.com/update/1.0/ddextra_v1.1.dat",
			["strMD5"] = "70D75D0EA1823CD7112F70499381B9D0",
			["param"] = {
				["nAISpanTimeInSec"] = 3*24*3600,
			},
		},
	}
	return t
end

