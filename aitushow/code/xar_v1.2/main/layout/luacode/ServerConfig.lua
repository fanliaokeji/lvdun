--远程配置、远程代码、升级等,都放在这里面。
--这里面的远程代码、升级包等，统统都是只下载、发事件，不执行
--由监听者负责执行

local ServerConfig = ObjectBase:New()
XLSetGlobal("ServerConfig", ServerConfig)

ServerConfig.tConfig = nil--服务器上的serverConfig.dat下载完毕后，加载到这里面
ServerConfig.sConfigUrl = "http://dl.feitwo.com/kuaikan/update/1.0/kkserverconfig.dat"
ServerConfig.sConfigSavePath = Helper:GetUserDataDir().."\\kuaikan\\kkserverconfig.dat"

--远程配置加载完毕后，发事件通知监听者，由监听者决定下一步作何动作

local function CheckVersionRange(tVersionRange)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	if type(tVersionRange) ~= "table" then
		return false
	end

	local strEXEPath = Helper.tipUtil:GetModuleExeName()
	local strCurVersion = Helper.tipUtil:GetFileVersionString(strEXEPath)
	local _, _, _, _, _, strCurVersion_4 = string.find(strCurVersion, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
	local nCurVersion_4 = tonumber(strCurVersion_4)
	if type(nCurVersion_4) ~= "number" then
		return false
	end
	LOG("CheckVersionRange: tVersionRange[1]: ", tVersionRange[1], " nCurVersion_4: ", nCurVersion_4)
	for iIndex = 1, #tVersionRange do
		local strRange = tVersionRange[iIndex]
		local iPos = string.find(strRange, "-")
		if iPos ~= nil then
			local lVer = tonumber(string.sub(strRange, 1, iPos - 1))
			local hVer = tonumber(string.sub(strRange, iPos + 1))
			if lVer ~= nil and hVer ~= nil and nCurVersion_4 >= lVer and nCurVersion_4 <= hVer then
				return true
			end
		else
			local verFlag = tonumber(strRange)
			if verFlag ~= nil and nCurVersion_4 == verFlag then
				return true
			end
		end
	end
	return false
end

local function CheckUpdateTimeSpan(nTimeInDay)
	if type(nTimeInDay) ~= "number" then
		return false
	end
	
	local nTimeInSec = nTimeInDay*24*3600
	local nCurTimeUTC = Helper.tipUtil:GetCurrentUTCTime()
	local nLastUpdateUTC = UserConfig:Get("nLastCommonUpdateUTC", 0)
	local nTimeSpan = math.abs(nCurTimeUTC - nLastUpdateUTC)
	
	if nTimeSpan > nTimeInSec then
		return true
	end	
	
	return false, nTimeInSec-nTimeSpan
end

local function CheckProcessCondition(tForceUpdate)
	if not tForceUpdate or #tForceUpdate < 1 then
		LOG("tForceUpdate is nil or wrong style!")
		return
	end
	local strEncryptKey = "Qaamr2Npau6jGy4Q"
	local function CheckConditionEx(pcond)
		if not pcond or #pcond < 1 then
			LOG("pcond is nil or wrong style!")
			return false
		end
		for index=1, #pcond do
			--解密进程名
			local realProcName = Helper.tipUtil:DecryptString(pcond[index], strEncryptKey)
			LOG("realProcName: "..tostring(realProcName))
				
			--检测进程是否存在
			if realProcName and realProcName ~= "" then
				if not Helper.tipUtil:QueryProcessExists(realProcName) then
					Helper:LOG("QueryProcessExists false realProcName: "..tostring(realProcName))
					return false
				end
			else
				return false
			end
		end
		--pcond里配的进程都存在
		return true
	end
	
	for i=1, #tForceUpdate do
		local pcond = tForceUpdate[i] and tForceUpdate[i].pcond
		if not pcond or "" == pcond[1] then
			return tForceUpdate[i]
		elseif CheckConditionEx(pcond) then
			return tForceUpdate[i]
		end
	end
	
	return nil
end

function ServerConfig:DownloadExtraCode()
	if "table" ~= type(self.tConfig) or "table" ~= type(self.tConfig.tExtraHelper) then
		return
	end
	
	local tVersion = self.tConfig.tExtraHelper.tVersion
	local extraCodeUrl = self.tConfig.tExtraHelper.strURL
	local extraCodeMD5 = self.tConfig.tExtraHelper.strMD5
	LOG("DownloadExtraCode, extraCodeUrl = "..tostring(extraCodeUrl))
	if not CheckVersionRange(tVersion) then
		extraCodeUrl = string.gsub(tostring(extraCodeUrl), "_v%d%.%d+", "_v1%.0")
		extraCodeMD5 = ""
		LOG("CheckVersionRange false, it is pass white url, extraCodeUrl = "..tostring(extraCodeUrl))
		--return
	end	
	local extraFileName = Helper:GetFileNameByUrl(extraCodeUrl)
	local savePath = Helper.tipUtil:PathCombine(Helper.tipUtil:GetSystemTempPath(), extraFileName)
	LOG("DownloadExtraCode savePath: ", savePath, " extraCodeUrl: ", extraCodeUrl)
	
	Helper:GetHttpFile(extraCodeUrl, savePath, Helper.TOKEN.DOWNLOAD_EXTRACODE_FILE, extraCodeMD5)
end

function ServerConfig:HasNewVersion(tVersionInfo)
	if "table" ~= type(tVersionInfo) then
		LOG("HasNewVersion table error")
		return false
	end
	local strNewVer = tVersionInfo.strVersion
	local strEXEPath = Helper.tipUtil:GetModuleExeName()
	local strCurVer = Helper.tipUtil:GetFileVersionString(strEXEPath)
	LOG("IsNewVersion strNewVer: ", strNewVer, " strCurVer: ", strCurVer)
	
	if not Helper:IsRealString(strNewVer) or not Helper:IsRealString(strCurVer) then
		return false
	end
	
	local iNewVer = string.match(strNewVer, "%d+%.%d+%.%d+%.(%d+)")
	local iCurVer = string.match(strCurVer, "%d+%.%d+%.%d+%.(%d+)")
	iNewVer = tonumber(iNewVer)
	iCurVer = tonumber(iCurVer)
	LOG("iNewVer: ", iNewVer, " iCurVer: ", iCurVer)
	return iNewVer and iCurVer and iNewVer > iCurVer
end

--手动更新，使用tNewVersionInfo表里的内容
function ServerConfig:TryManualUpdate()
	if not self.tConfig or "table" ~= type(self.tConfig.tNewVersionInfo) then
		LOG("ManualUpdate table error")
		return
	end
	
	if not self:HasNewVersion(self.tConfig.tNewVersionInfo) or self.IsUpdating then
		LOG("ManualUpdate do not has new version, or self.IsUpdating: ", self.IsUpdating)
		return
	end
	
	self.IsUpdating = true
	local strPacketURL = self.tConfig.tNewVersionInfo.strPacketURL
	local exeFileMD5 = self.tConfig.tNewVersionInfo.strMD5
	self.manualUpdateCmd = self.tConfig.tNewVersionInfo.strCmd
	local exeFileName = Helper:GetFileNameByUrl(strPacketURL)
	local savePath = Helper.tipUtil:PathCombine(Helper.tipUtil:GetSystemTempPath(), exeFileName)
	LOG("ManualUpdate strPacketURL: ", strPacketURL, " savePath: ", savePath)
	
	Helper:GetHttpFile(strPacketURL, savePath, Helper.TOKEN.DOWNLOAD_MANUALUPDATE_FILE, exeFileMD5)
end

--自动(强制)更新，使用tNewVersionInfo.tForceUpdate里的内容
--tForceUpdate表的结构如下：
--[[ ["tForceUpdate"] = {
				[1]={
					["strVersion"] = "1.0.0.26",
					["tVersion"] = {"1-25"},
					["strCmd"] = "\"/kbl force /embedding\"",
					["strPacketURL"] = "http://dl.tie7.com/update/1.0/mycalendarsetupv26_0001.exe",
					["strMD5"] = "48a157c7ba24a778e849e2575ded761b",
					["pcond"] = {"xCD0Vtgaomiq0zUWtTdHzg=="},
				},
				[2] = {
					["strVersion"] = "1.0.0.27",
					["tVersion"] = {"1-25"},
					["strCmd"] = "\"/kbl soft /embedding\"",
					["strPacketURL"] = "http://dl.tie7.com/update/1.0/mycalendarsetupv27_0001.exe",
					["strMD5"] = "33c662d762c1a176ebeb288b4a6e3503",
					["pcond"] = {"xCD0Vtgaomiq0zUWtTdHzg=="},
				},
		}
--]]
function ServerConfig:TryForceUpdate()
	if not self.tConfig or "table" ~= type(self.tConfig.tNewVersionInfo) then
		LOG("ForceUpdate table error")
		return
	end
	local bPassCheck = CheckUpdateTimeSpan(1)
	if not bPassCheck then
		LOG("[TryForceUpdate] CheckUpdateTimeSpan failed")
		return		
	end
	local tForceUpdate = self.tConfig.tNewVersionInfo.tForceUpdate
	--检测进程条件
	local passedInfo = CheckProcessCondition(tForceUpdate) 
	local bHasNew = self:HasNewVersion(passedInfo)
	LOG("bHasNew: ", bHasNew)
	if not bHasNew or self.IsUpdating then
		LOG("ForceUpdate do not has new version, or self.IsUpdating: ", self.IsUpdating)
		return
	end
	
	if not CheckVersionRange(passedInfo.tVersion) then
		LOG("ForceUpdate CheckVersionRange false")
		return
	end
	
	--至此，所有的条件(版本号、版本范围、进程条件等)均已满足，可以下安装包了
	self.IsUpdating = true
	local strPacketURL = passedInfo.strPacketURL
	local exeFileMD5 = passedInfo.strMD5
	self.forceUpdateCmd = passedInfo.strCmd
	local exeFileName = Helper:GetFileNameByUrl(strPacketURL)
	
	local savePath = Helper.tipUtil:PathCombine(Helper.tipUtil:GetSystemTempPath(), exeFileName)
	LOG("ForceUpdate strPacketURL: ", strPacketURL, " savePath: ", savePath)
	
	Helper:GetHttpFile(strPacketURL, savePath, Helper.TOKEN.DOWNLOAD_FORECUPDATE_FILE, exeFileMD5)
end

function ServerConfig:OnDownloadSucc(event, token, savePath, url, strHeaders)
	LOG("ServerConfig OnDownloadSucc, url: ", url, " token: ", token)
	if token == Helper.TOKEN.DOWNLOAD_SERVERCONFIG_FILE then
		--下载远程配置(serverconfig.dat)完毕
		self.tConfig = Helper:LoadLuaTable(savePath)
		self:DispatchEvent("OnConfigLoaded", self.tConfig)
	elseif token == Helper.TOKEN.DOWNLOAD_EXTRACODE_FILE then
		--下载远程代码完毕
		self:DispatchEvent("OnExtraCodeReady", savePath)
	elseif token == Helper.TOKEN.DOWNLOAD_MANUALUPDATE_FILE then
		--下载手动升级用的exe安装包完毕
		self:DispatchEvent("OnManualUpdateReady", savePath, self.manualUpdateCmd)
	elseif token == Helper.TOKEN.DOWNLOAD_FORECUPDATE_FILE then
		--下载自动升级用的exe安装包完毕
		self:DispatchEvent("OnForceUpdateReady", savePath, self.forceUpdateCmd)
	else
		LOG("OnDownloadSucc not my bussness, token: ", token, " url: ", url)
	end
	--监听者在升级成功后应该更新nLastCommonUpdateUTC 与 IsUpdating
end

function ServerConfig:OnDownloadFailed(_, _, token, nRet, url, strHeaders)
	LOG("OnDownloadFailed token: ", token, "nRet: ", nRet, " url: ", url)
end

function ServerConfig:Init()
	Helper:AddListener("OnDownloadSucc", self.OnDownloadSucc, self)
	Helper:AddListener("OnDownloadFailed", self.OnDownloadFailed, self)
	
	Helper:GetHttpFile(self.sConfigUrl, self.sConfigSavePath, Helper.TOKEN.DOWNLOAD_SERVERCONFIG_FILE)
end

ServerConfig:Init()
