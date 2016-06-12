local StatUtil = {}
XLSetGlobal("StatUtil", StatUtil)
local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")
local defaultStatPeerid = "0123456789ABCDEF"
local gStatCount = 0

function IsRealString(str) return type(str) == "string" and str ~= "" end
function IsNilString(str) return not str or str == "" end

function RegQueryValue(sPath)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			return tipUtil:QueryRegValue(sRegRoot, sRegPath, sRegKey or "") or ""
		end
	end
	return ""
end

function IsCurPeerIDStatAllowed()
	local statPeerID = RegQueryValue("HKEY_CURRENT_USER\\Software\\kuaikan\\statpeerid")
	LOG("IsCurPeerIDStatAllowed statPeerID : "..tostring(statPeerID))
	if not IsRealString(statPeerID) then
		statPeerID = defaultStatPeerid
	end
	local machinePeerID = GetPeerID()
	local lastChar = tostring(string.sub(machinePeerID, 12, 12))
	LOG("IsCurPeerIDStatAllowed machinePeerID : "..tostring(machinePeerID).." lastChar: "..lastChar..", statPeerID = "..tostring(statPeerID)..", defaultStatPeerid = "..tostring(defaultStatPeerid).."....end")
	local begin, _ = string.find(string.lower(statPeerID), string.lower(lastChar))
	if begin then
		LOG("IsCurPeerIDStatAllowed stat is allowed!")
		return true
	end
	
	LOG("IsCurPeerIDStatAllowed stat not allowed!")
	return false
end

function GetPeerID()
	local strPeerID = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\kuaikan\\PeerId")
	if IsRealString(strPeerID) then
		return strPeerID
	end

	local strRandPeerID = tipUtil:GetPeerId()
	if not IsRealString(strRandPeerID) then
		return ""
	end
	
	RegSetValue("HKEY_LOCAL_MACHINE\\Software\\kuaikan\\PeerId", strRandPeerID)
	return strRandPeerID
end

function StatUtil.GetMainVer()
	local strExePath = tipUtil:GetModuleExeName()
	if not IsRealString(strExePath) or not tipUtil:QueryFileExists(strExePath) then
		return ""
	end
	local strVersion = tipUtil:GetFileVersionString(strExePath)
	if not IsRealString(strVersion) then
		return ""
	end
	local _, _, strMinorVer = string.find(strVersion, "%d+%.%d+%.%d+%.(%d+)")
	return strMinorVer
end

--渠道
function StatUtil.GetInstallSrc()
	local strInstallSrc = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\kuaikan\\InstallSource")
	if not IsNilString(strInstallSrc) then
		return tostring(strInstallSrc)
	end
	
	return ""
end

--使用时间
function StatUtil.GetUsedTime()
	if type(StatUtil.StartTime) ~= "number" then
		return 0
	end
	local nNow = tipUtil:GetCurrentUTCTime() or 0
	return nNow - StatUtil.StartTime
end

function StatUtil.SendStat(tStat)
	--由于谷歌统计超标,只取四分之一peerid上报
	if not IsCurPeerIDStatAllowed() then 
		return 
	end
	
	local rdRandom = tipUtil:GetCurrentUTCTime()
	local tStatInfo = tStat or {}
	local strDefaultNil = "null"
	
	local strCID = GetPeerID()
	local strEC = tStatInfo.strEC 
	local strEA = tStatInfo.strEA 
	local strEL = tStatInfo.strEL
	local strEV = tStatInfo.strEV
	
	if IsNilString(strEC) then
		strEC = strDefaultNil
	end
	
	if IsNilString(strEA) then
		strEA = strDefaultNil
	end
	
	if IsNilString(strEL) then
		strEL = strDefaultNil
	end
	
	if tonumber(strEV) == nil then
		strEV = 1
	end
	
	local strUrl = "http://www.google-analytics.com/collect?v=1&tid=UA-77713162-1&cid="..tostring(strCID)
						.."&t=event&ec="..tostring(strEC).."&ea="..tostring(strEA)
						.."&el="..tostring(strEL).."&ev="..tostring(strEV)
	
	LOG("SendStat: " .. tostring(strUrl))
	
	gStatCount = gStatCount + 1
	tipAsynUtil:AsynSendHttpStat(strUrl, function()
		gStatCount = gStatCount - 1
	end)
end

function StatUtil.Exit(bForce)
	local function ExitProcess()
		--保存配置
		UserConfig:SaveToFile()
		LOG("************ Exit ************")
		tipUtil:Exit("Exit")
	end
	--先把托盘干掉
	Helper.Tray.Hide()
	if bForce then
		ExitProcess()
		return
	end
	if gStatCount <= 0 then
		ExitProcess()
	elseif gStatCount > 0 then	--开启定时退出定时器
		SetOnceTimer(function()
			ExitProcess()
		end, 15000 * gStatCount)
	end
end