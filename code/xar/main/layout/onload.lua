local gTipInfoTab = {}
local gFilterConfigInfo = {}
local gRuleInfo = {}

local gbLoadCfgSucc = false
local tipUtil = XLGetObject("GS.Util")
-----------------

function RegisterFunctionObject(self)
	local function FailExitTipWnd(self, iExitCode)
		ExitTipWnd()
	end
	local function ShowTipWnd(self,iShow)
		local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
		local frameHostWnd = hostwndManager:GetHostWnd("GreenWallTipWnd.MainFrame")
	
		frameHostWnd:Show(4)
	end
	
	local function TipConvStatistic(tStatInfo)
		ExitTipWnd()
	end
	
	local obj = {}
	obj.FailExitTipWnd = FailExitTipWnd
	obj.GetFailExitCode = GetFailExitCode
	obj.ShowTipWnd = ShowTipWnd
	obj.TipConvStatistic = TipConvStatistic
	obj.SaveUserConfigToFile = SaveUserConfigToFile
	obj.GetUserConfigFromFile = GetUserConfigFromFile
	obj.SaveUserConfigToMem = SaveUserConfigToMem
	obj.GetUserConfigFromMem = GetUserConfigFromMem
	obj.GetSpecifyFilterTableFromMem = GetSpecifyFilterTableFromMem
	obj.SaveSpecifyFilterTableToMem = SaveSpecifyFilterTableToMem
	obj.GetRuleListFromMem = GetRuleListFromMem
	obj.SaveRuleListToMem = SaveRuleListToMem
	obj.UIAutoEnableDomain = UIAutoEnableDomain

	XLSetGlobal("GreenWallTip.FunctionHelper", obj)
end


function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@greenwall_Template MainTipLog: " .. tostring(strLog))
	end
end


function QueryAllUsersDir()	--»ñÈ¡AllUserÂ·¾¶
	local bRet = false
	local strPublicEnv = "%PUBLIC%"
	local strRet = tipUtil:ExpandEnvironmentStrings(strPublicEnv)
	if strRet == nil or strRet == "" or strRet == strPublicEnv then
		local nCSIDL_COMMON_APPDATA = 35 --CSIDL_COMMON_APPDATA(0x0023)
		strRet = tipUtil:GetSpecialFolderPathEx(nCSIDL_COMMON_APPDATA)
	end
	if not IsNilString(strRet) and tipUtil:QueryFileExists(strRet) then
		bRet = true
	end
	return bRet, strRet
end


function SaveAllConfig()
	if gbLoadCfgSucc then
		SaveUserConfigToFile()
		SaveFilterConfigToFile()
		SaveRuleListToFile()
	end
end


function ExitTipWnd(statInfo)
	SaveAllConfig()

	TipLog("************ Exit ************")
	tipUtil:Exit("Exit")
end

function LoadTableFromFile(strDatFilePath)
	local tResult = nil

	if IsRealString(strDatFilePath) and tipUtil:QueryFileExists(strDatFilePath) then
		local tMod = XLLoadModule(strDatFilePath)
		if type(tMod) == "table" and type(tMod.GetSubTable) == "function" then
			local tDat = tMod.GetSubTable()
			if type(tDat) == "table" then
				tResult = tDat
			end
		end
	end
	
	return tResult
end

function PopTipWnd(OnCreateFunc)
	local bSuccess = false
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate("TipMainWnd", "HostWndTemplate" )
	if frameHostWndTemplate then
		local frameHostWnd = frameHostWndTemplate:CreateInstance("GreenWallTipWnd.MainFrame")
		if frameHostWnd then
			local objectTreeTemplate = nil
			objectTreeTemplate = templateMananger:GetTemplate("TipPanelTree", "ObjectTreeTemplate")
			if objectTreeTemplate then
				local uiObjectTree = objectTreeTemplate:CreateInstance("GreenWallTipWnd.MainObjectTree")
				if uiObjectTree then
					frameHostWnd:BindUIObjectTree(uiObjectTree)
					local ret = OnCreateFunc(uiObjectTree)
	
					if ret then
						local iRet = frameHostWnd:Create()
						if iRet ~= nil and iRet ~= 0 then
							bSuccess = true
							local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
							FunctionObj:ShowTipWnd(1)
						end
					end
				end
			end
		end
	end
	if not bSuccess then
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd(7)
	end
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


function SaveUserConfigToMem(tNewConfig)
	gTipInfoTab = tNewConfig
end

function GetUserConfigFromMem()
	return gTipInfoTab
end

function GetRuleListFromMem()
	return gRuleInfo
end

function SaveRuleListToMem(tNewRuleList)
	gRuleInfo = tNewRuleList
end


function GetCfgPathWithName(strCfgName)
	local bOk, strBaseDir = QueryAllUsersDir()
	if not bOk then
		return ""
	end
	
	local strCfgFilePath = tipUtil:PathCombine(strBaseDir, "GreenShield\\"..tostring(strCfgName))
	return strCfgFilePath or ""
end


function GetUserCfgPath()
	return GetCfgPathWithName("UserConfig.dat")
end

function GetFilterCfgPath()
	return GetCfgPathWithName("FilterConfig.dat")
end

function GetRuleListCfgPath()
	return GetCfgPathWithName("RuleList.dat")
end


function GetSpecifyFilterTableFromMem(strTableName)
	if not IsRealString(strTableName) then
		return gFilterConfigInfo
	end

	return gFilterConfigInfo[strTableName]
end

function SaveSpecifyFilterTableToMem(tNewTable, strTableName)
	if not IsRealString(strTableName) then
		gFilterConfigInfo = tNewTable
	else
		gFilterConfigInfo[strTableName] = tNewTable
	end
end

function SaveUserConfigToFile()
	local strUserCfgPath = GetUserCfgPath()
	if IsRealString(strUserCfgPath) then
		tipUtil:SaveLuaTableToLuaFile(gTipInfoTab, strUserCfgPath)
	end
end

function GetUserConfigFromFile()
	local strUserCfgPath = GetUserCfgPath()
	local infoTable = LoadTableFromFile(strUserCfgPath)
	return infoTable
end


function GetFilterConfigFromFile()
	local strFilterCfgPath = GetFilterCfgPath()
	local infoTable = LoadTableFromFile(strFilterCfgPath)
	return infoTable
end

function SaveFilterConfigToFile()
	local strFilterCfgPath = GetFilterCfgPath()
	if IsRealString(strFilterCfgPath) then
		tipUtil:SaveLuaTableToLuaFile(gFilterConfigInfo, strFilterCfgPath)
	end
end

function GetRuleListFromFile()
	local strRuleListPath = GetRuleListCfgPath()
	local tRuleList = LoadTableFromFile(strRuleListPath)
	return tRuleList
end

function SaveRuleListToFile()
	local strRuleListPath = GetRuleListCfgPath()
	if IsRealString(strRuleListPath) then
		tipUtil:SaveLuaTableToLuaFile(gRuleInfo, strRuleListPath)
	end
end


function ReadAllConfigInfo()	
	local infoTable = GetUserConfigFromFile()
	if type(infoTable) ~= "table" then
		TipLog("[ReadAllConfigInfo] GetUserConfigFromFile failed! ")
		return false
	end
		
	local tFilterTable = GetFilterConfigFromFile()
	if type(tFilterTable) ~= "table" then
		TipLog("[ReadAllConfigInfo] GetFilterConfigFromFile failed! ")
		return false
	end
	
	local tRuleListTable = GetRuleListFromFile()
	if type(tFilterTable) ~= "table" then
		TipLog("[ReadAllConfigInfo] GetRuleListFromFile failed! ")
		return false
	end
			
	gTipInfoTab = infoTable
	gFilterConfigInfo = tFilterTable
	gRuleInfo = tRuleListTable
	
	gbLoadCfgSucc = true	
	TipLog("[ReadAllConfigInfo] success!")
	return true
end


function SendFileDateToFilterThread()
	local bSucc = SendRuleListToFilterThread()
	if not bSucc then
		return false
	end
	
	local bSucc = SendEnableListToFilterThread()
	if not bSucc then
		return false
	end
	
	
	return true
end


function SendRuleListToFilterThread()
	local tRuleList = gRuleInfo
	
	for key, tRuleElem in pairs(tRuleList) do
		if type(tRuleElem) == "table" then
			local strDomain = tRuleElem["strDomain"]
			local tExtraPath = tRuleElem["tExtraPath"]
			if IsRealString(strDomain) and type(tExtraPath) == "table" then
				tipUtil:AddDomain(strDomain, tExtraPath)
			end
		end
	end
	
	return true
end


function SendEnableListToFilterThread()
	local tRuleList = gRuleInfo
	
	for key, tRuleElem in pairs(tRuleList) do 
		if type(tRuleElem) == "table" then
			local strDomain = tRuleElem["strDomain"]
			UIAutoEnableDomain(strDomain)
		end
	end

	return true
end

function UIAutoEnableDomain(strDomain)
	if IsRealString(strDomain) then
		if IsDomainInWhiteList(strDomain) then
			--tipUtil:EnableDomain(false, strDomain)
			tipUtil:EnableDomain(strDomain,false)
		elseif IsDomainInBlackList(strDomain) then
			--tipUtil:EnableDomain(true, strDomain)
			tipUtil:EnableDomain(strDomain, true)
		end
	end
end


function IsDomainInWhiteList(strDomain)
	local tWhiteList = GetSpecifyFilterTableFromMem("tWhiteList")
	for key, tWhiteElem in pairs(tWhiteList) do
		local strWhiteDomain = tWhiteElem["strDomain"]
		local bStateOpen = tWhiteElem["bState"]
		if IsRealString(strWhiteDomain) and bStateOpen
			and string.find(strWhiteDomain, strDomain) then
			
			return true
		end
		
	end
	return false
end


function IsDomainInBlackList(strDomain)
	local tBlackList = GetSpecifyFilterTableFromMem("tBlackList")
	for key, tBlackElem in pairs(tBlackList) do
		local strBlackDomain = tBlackElem["strDomain"]
		local bStateOpen = tBlackElem["bState"]
		if IsRealString(strBlackDomain) and bStateOpen
			and string.find(strBlackDomain, strDomain) then
			
			return true
		end
		
	end
	return false
end


function CreateMainTipWnd()
	local function OnCreateFuncF(treectrl)
		local rootctrl = treectrl:GetUIObject("root.layout:root.ctrl")
		local bRet = rootctrl:SetTipData(gTipInfoTab)			
		if not bRet then
			return false
		end
	
		return true
	end
	
	PopTipWnd(OnCreateFuncF)	
end


function CreatePopupTipWnd()
	local bSuccess = false
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate("TipPopupWnd", "HostWndTemplate" )
	if frameHostWndTemplate then
		local frameHostWnd = frameHostWndTemplate:CreateInstance("GreenWallPopupWnd.MainFrame")
		if frameHostWnd then
			local objectTreeTemplate = nil
			objectTreeTemplate = templateMananger:GetTemplate("TipPopupWndTree", "ObjectTreeTemplate")
			if objectTreeTemplate then
				local uiObjectTree = objectTreeTemplate:CreateInstance("GreenWallPopupWnd.MainObjectTree")
				if uiObjectTree then
					frameHostWnd:BindUIObjectTree(uiObjectTree)
					local iRet = frameHostWnd:Create()
					if iRet ~= nil and iRet ~= 0 then
						bSuccess = true
					end
				end
			end
		end
	end
	if not bSuccess then
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd()
	end
end

function StartTimer()
	local nTimeSpanInMs = 3600 * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		SaveAllConfig()
	end, nTimeSpanInMs)
end


function TipMain() 
	RegisterFunctionObject()
	local bSuccess = ReadAllConfigInfo()	
	if not bSuccess then
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd()
		return
	end

	local bSucc = SendFileDateToFilterThread()
	if not bSucc then
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd()
		return
	end
	
	CreateMainTipWnd()
	CreatePopupTipWnd()
	StartTimer()
		--tipUtil:GetPeerId();
end


TipMain()
