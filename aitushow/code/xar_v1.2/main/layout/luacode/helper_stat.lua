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

function StatUtil.AsynSend(strUrl)
	gStatCount = gStatCount + 1
	local bHasCallBack = false
	local function fn_callbak()
		if bHasCallBack then return end
		bHasCallBack = true
		gStatCount = gStatCount - 1
		if gStatCount <= 0 and StatUtil.ExitFlag then
			StatUtil.ExitProcess()
		end
	end
	tipAsynUtil:AsynSendHttpStat(strUrl, fn_callbak)
	if StatUtil.ExitFlag then
		SetOnceTimer(fn_callbak, 15000)--设定15秒超时
	end
end

--谷歌上报
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
	
	StatUtil.AsynSend(strUrl)
end

--上报到快看
function StatUtil.SendKKStat(nOPeration)
	local strCID = GetPeerID()
	local strChannelID = StatUtil.GetInstallSrc()
	local strVer = StatUtil.GetMainVer()
	local strRandom = tipUtil:GetCurrentUTCTime()
	
	local strPort = "8082"
	if nOPeration == 10 then   --心跳上报的端口为8083
		strPort = "8083"
	end
	
	local strUrl = "http://stat.feitwo.com:"..tostring(strPort).."/c?appid=1001&peerid=".. tostring(strCID)
					.."&proid=15&op="..tostring(nOPeration).."&cid="..(strChannelID)
					.."&ver="..tostring(strVer).."&rd="..tostring(strRandom)
	
	LOG("SendKKStat: " .. tostring(strUrl))
	StatUtil.AsynSend(strUrl)
end

--获取当前时间距离第二天0点的秒数
function StatUtil.Get2DayNeedTime()
	local nCur = tonumber(os.time())
	if type(nCur) == "number" then
		local tDate = os.date("*t", nCur)
		local nDay2 = os.time({year=tonumber(tDate.year), month=tonumber(tDate.month), day=tonumber(tDate.day), hour=23, min=59, sec=59})
		nDay2 = tonumber(nDay2)
		if type(nDay2) == "number" and nDay2 > nCur then
			return nDay2 - nCur + 1
		end
	end
end

--启动上报
function StatUtil.SendStartupStat()
	--记录启动时间
	StatUtil.StartTime = tipUtil:GetCurrentUTCTime()
	--启动时间写入注册表
	Helper:SetRegValue("HKEY_CURRENT_USER\\Software\\kuaikan\\laststartuputc", tipUtil:GetCurrentUTCTime())
	--快看心跳上报
	SetTimer(function(item, id)
		StatUtil.SendKKStat(10)
	end, 2*60*1000)
	local function fn_report()
		--启动上报
		StatUtil.SendStat({
			strEC = "startup",
			strEA = StatUtil.GetMainVer(),
			strEL = StatUtil.GetInstallSrc(),
			strEV = 1,
		}) 
		--启动来源
		StatUtil.SendStat({
			strEC = "launch",
			strEA = tostring(GetSStartFrom()),
			strEL = StatUtil.GetInstallSrc(),
			strEV = 1,
		}) 
		--快看启动上报
		StatUtil.SendKKStat(2)
		local needUTC = StatUtil.Get2DayNeedTime()
		LOG("SendStartupStat, needUTC = "..tostring(needUTC))
		if type(needUTC) == "number" then
			LOG("SendStartupStat, will run timer")
			SetOnceTimer(fn_report, needUTC*1000)
		end
	end
	fn_report()
end

function StatUtil.HideAllWindow()
	local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local str = ""
	local hostWnd
	if hostWndManager:BeginEnumHostWnd() then
		hostWnd = hostWndManager:GetNextHostWnd()
		while hostWnd do
			LOG("StatUtil.HideAllWindow hostWnd:GetID() = "..tostring(hostWnd:GetID()))
			if type(hostWnd.Show) == "function" then
				hostWnd:Show(0)
			end
			hostWnd = hostWndManager:GetNextHostWnd()
		end
	end
end

function StatUtil.ExitProcess()
	--保存配置
	UserConfig:SaveToFile()
	LOG("************ Exit ************")
	tipUtil:Exit("Exit")
end

function GetSStartFrom()
	local cmdString = tostring(tipUtil:GetCommandLine())
	return string.match(string.lower(cmdString), "/sstartfrom%s*(%S+)") or "rawexefile"
end

function StatUtil.Exit(bForce)
	--退出时检查1次关联
	Helper.CheckAssociate(true)
	--隐藏所有窗口
	StatUtil.HideAllWindow()
	--先把托盘干掉
	Helper.Tray.Hide()
	--关闭单例互斥量
	tipUtil:CloseSingletonMutex()
	--设置退出标记
	StatUtil.ExitFlag = true
	--退出上报
	StatUtil.SendStat({
		strEC = "exit",
		strEA = Helper.Setting.IsSysBoot() and "1" or "0",
		strEL = Helper.Setting.GetExitType() and "1" or "0",
		strEV = StatUtil.GetUsedTime(),
	}) 
	--退出时上报1条心跳
	StatUtil.SendKKStat(10)
	if bForce then
		StatUtil.ExitProcess()
	end
end