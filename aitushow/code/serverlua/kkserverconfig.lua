function GetSubTable()
	local t = {
		["tNewVersionInfo"] = {
			["strVersion"] = "1.0.0.29",
			["tVersion"] = {"1-28"},
			["strCmd"] = "\"/kbl force /embedding\"",--手动升级这个命令行没有用
			["strPacketURL"] = "http://dl.feitwo.com/kuaikan/client/kuaikansetupv8_0001.exe",
			["strMD5"] = "077a8b1469b788915d31cf997b59eeb4",
			["tInroduce"] = {
				"1.修复部分bug",
				"",
				"",
			},
			["tForceUpdate"] = {
					[1]={
						["strVersion"] = "1.0.0.29",
						["tVersion"] = {"1-28"},
						["strCmd"] = "\"/kbl force /embedding\"",
						["strPacketURL"] = "http://dl.feitwo.com/kuaikan/client/kuaikansetupv8_0001.exe",
						["strMD5"] = "077a8b1469b788915d31cf997b59eeb4",
					},
				},
		},
		["tExtraHelper"] = {
			["tVersion"] = {"1-29"},
			["strURL"] = "http://dl.feitwo.com/kuaikan/update/1.0/kkextra_v1.2.dat",
			["strMD5"] = "1a6a491357aebfe4d10240a69b52def8",
		},
	}
	return t
end

