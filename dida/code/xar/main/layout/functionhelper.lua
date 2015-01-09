local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")

local gStatCount = 0
local gForceExit = nil

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@DiDa_Log: " .. tostring(strLog))
	end
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

function GetCommandStrValue(strKey)
	local bRet, strValue = false, nil
	local cmdString = tipUtil:GetCommandLine()
	
	if string.find(cmdString, strKey .. " ") then
		local cmdList = tipUtil:CommandLineToList(cmdString)
		if cmdList ~= nil then	
			for i = 1, #cmdList, 2 do
				local strTmp = tostring(cmdList[i])
				if strTmp == strKey 
					and not string.find(tostring(cmdList[i + 1]), "^/") then		
					bRet = true
					strValue = tostring(cmdList[i + 1])
					break
				end
			end
		end
	end
	return bRet, strValue
end


function ExitProcess()
	SaveAllConfig()

	TipLog("************ Exit ************")
	tipUtil:Exit("Exit")
end

function HideMainWindow()
	local objMainWnd = GetMainWndInst()
	if objMainWnd then
		objMainWnd:Show(0)
	end
end

function ReportAndExit()
	local tStatInfo = {}
	HideMainWindow()		
	SendRunTimeReport(0, true)
	
	tStatInfo.strEC = "exit"	
	tStatInfo.strEA = GetInstallSrc() or ""
	tStatInfo.Exit = true
			
	TipConvStatistic(tStatInfo)
end


function SendRunTimeReport(nTimeSpanInSec, bExit)
	local tStatInfo = {}
	tStatInfo.strEC = "runtime"
	tStatInfo.strEA = GetInstallSrc() or ""
	
	local nRunTime = 0
	local nLastReportRunTmUTC = XLGetGlobal("DiDa.LastReportRunTime") 
	if bExit and nLastReportRunTmUTC ~= 0 then
		nRunTime = math.abs(tipUtil:GetCurrentUTCTime() - nLastReportRunTmUTC)
	else
		nRunTime = nTimeSpanInSec
	end
	tStatInfo.strEV = nRunTime
	
	TipConvStatistic(tStatInfo)
end


function IsUserFullScreen()
	local bRet = false
	if type(tipUtil.IsNowFullScreen) == "function" then
		bRet = tipUtil:IsNowFullScreen()
	end
	return bRet
end


function CheckIsNewVersion(strNewVer, strCurVer)
	if not IsRealString(strNewVer) or not IsRealString(strCurVer) then
		return false
	end

	local a,b,c,d = string.match(strNewVer, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
	local A,B,C,D = string.match(strCurVer, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
	return a>A or (a==A and (b>B or (b==B and (c>C or (c==C and d>D)))))
end


function GetPeerID()
	local strPeerID = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\DiDa\\PeerId")
	if IsRealString(strPeerID) then
		return strPeerID
	end

	local strRandPeerID = tipUtil:GetPeerId()
	if not IsRealString(strRandPeerID) then
		return ""
	end
	
	RegSetValue("HKEY_LOCAL_MACHINE\\Software\\DiDa\\PeerId", strRandPeerID)
	return strRandPeerID
end

--渠道
function GetInstallSrc()
	local strInstallSrc = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\DiDa\\InstallSource")
	if not IsNilString(strInstallSrc) then
		return tostring(strInstallSrc)
	end
	
	return ""
end


function FailExitTipWnd(self, iExitCode)
	local tStatInfo = {}
	tStatInfo.Exit = true
	TipConvStatistic(tStatInfo)
end


function CheckTimeIsAnotherDay(LastTime)
	local bRet = false
	local LYear, LMonth, LDay, LHour, LMinute, LSecond = tipUtil:FormatCrtTime(LastTime)
	local curTime = tipUtil:GetCurrentUTCTime()
	local CYear, CMonth, CDay, CHour, CMinute, CSecond = tipUtil:FormatCrtTime(curTime)
	if LYear ~= CYear or LMonth ~= CMonth or LDay ~= CDay then
		bRet = true
	end
	return bRet
end


function TipConvStatistic(tStat)
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
	
	local strUrl = ""
	
	TipLog("TipConvStatistic: " .. tostring(strUrl))
	
	gStatCount = gStatCount + 1
	if not gForceExit and tStat.Exit then
		gForceExit = true
	end
	tipAsynUtil:AsynSendHttpStat(strUrl, function()
		gStatCount = gStatCount - 1
		if gStatCount == 0 and gForceExit then
			ExitProcess()
		end
	end)
	
	local iStatCount = gStatCount
	if gForceExit and iStatCount > 0 and gTimeoutTimerId == nil then	--开启定时退出定时器
		local timeMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
		gTimeoutTimerId = timeMgr:SetTimer(function(Itm, id)
			Itm:KillTimer(id)
			ExitProcess()
		end, 15000 * iStatCount)
	end
end


function NewAsynGetHttpFile(strUrl, strSavePath, bDelete, funCallback, nTimeoutInMS)
	local bHasAlreadyCallback = false
	local timerID = nil
	
	tipAsynUtil:AsynGetHttpFile(strUrl, strSavePath, bDelete, 
		function (nRet, strTargetFilePath, strHeaders)
			if timerID ~= nil then
				tipAsynUtil:KillTimer(timerID)
			end
			if not bHasAlreadyCallback then
				bHasAlreadyCallback = true
				funCallback(nRet, strTargetFilePath, strHeaders)
			end
		end)
	
	timerID = tipAsynUtil:SetTimer(nTimeoutInMS or 2 * 60 * 1000,
		function (nTimerId)
			tipAsynUtil:KillTimer(nTimerId)
			timerID = nil
			if not bHasAlreadyCallback then
				bHasAlreadyCallback = true
				funCallback(-2)
			end
		end)
end



function CheckMD5(strFilePath, strExpectedMD5) 
	local bPassCheck = false
	
	if not IsNilString(strFilePath) then
		local strMD5 = tipUtil:GetMD5Value(strFilePath)
		TipLog("[CheckMD5] strFilePath = " .. tostring(strFilePath) .. ", strMD5 = " .. tostring(strMD5))
		if not IsRealString(strExpectedMD5) 
			or (not IsNilString(strMD5) and not IsNilString(strExpectedMD5) and string.lower(strMD5) == string.lower(strExpectedMD5))
			then
			bPassCheck = true
		end
	end
	
	TipLog("[CheckMD5] strFilePath = " .. tostring(strFilePath) .. ", strExpectedMD5 = " .. tostring(strExpectedMD5) .. ". bPassCheck = " .. tostring(bPassCheck))
	return bPassCheck
end


function DownLoadFileWithCheck(strURL, strSavePath, strCheckMD5, fnCallBack)
	if type(fnCallBack) ~= "function"  then
		return
	end

	if IsRealString(strCheckMD5) and CheckMD5(strSavePath, strCheckMD5) then
		TipLog("[DownLoadFileWithCheck]File Already existed")
		fnCallBack(1)
		return
	end
	
	NewAsynGetHttpFile(strURL, strSavePath, false, function(bRet, strDownLoadPath)
		TipLog("[DownLoadFileWithCheck] NewAsynGetHttpFile:bret = " .. tostring(bRet) 
				.. ", strURL = " .. tostring(strURL) .. ", strDownLoadPath = " .. tostring(strDownLoadPath))
		if 0 == bRet then
			strSavePath = strDownLoadPath
            if CheckMD5(strSavePath, strCheckMD5) then
				fnCallBack(bRet, strSavePath)
			else
				TipLog("[DownLoadFileWithCheck]Did Not Pass MD5 Check")
				fnCallBack(-2)
			end	
		else
			TipLog("[DownLoadFileWithCheck] DownLoad failed")
			fnCallBack(-3)
		end
	end)
end


function GetProgramTempDir(strSubDir)
	local strSysTempDir = tipUtil:GetSystemTempPath()
	local strProgramTempDir = tipUtil:PathCombine(strSysTempDir, strSubDir)
	if not tipUtil:QueryFileExists(strProgramTempDir) then
		tipUtil:CreateDir(strProgramTempDir)
	end
	
	return strProgramTempDir
end


function GetFileSaveNameFromUrl(url)
	local _, _, strFileName = string.find(tostring(url), ".*/(.*)$")
	local npos = string.find(strFileName, "?", 1, true)
	if npos ~= nil then
		strFileName = string.sub(strFileName, 1, npos-1)
	end
	return strFileName
end


function QueryAllUsersDir()	
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

function GetExePath()
	return tipUtil:GetModuleExeName()
end


function GetDBPath(strDBName)
	local strExePath = GetExePath()
	local _, _, strExeDir = string.find(strExePath, "(.*)\\.*$")
	
	local strDBPath = tipUtil:PathCombine(strExeDir, strDBName)
	if IsRealString(strDBPath) and tipUtil:QueryFileExists(strDBPath) then
		return strDBPath
	end
	
	XLMessageBox(tostring("数据缺失， 请重新安装"))
	ExitProcess()
	return nil
end



function GetDiDaVersion()
	local strEXEPath = GetExePath()
	if not IsRealString(strEXEPath) or not tipUtil:QueryFileExists(strEXEPath) then
		return ""
	end

	return tipUtil:GetFileVersionString(strEXEPath)
end


function GetMinorVer()
	local strVersion = GetDiDaVersion()
	if not IsRealString(strVersion) then
		return ""
	end
	
	local _, _, strMinorVer = string.find(strVersion, "%d+%.%d+%.%d+%.(%d+)")
	return strMinorVer
end


function RegQueryValue(sPath)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			return tipUtil:QueryRegValue(sRegRoot, sRegPath, sRegKey or "") or ""
		end
	end
	return ""
end


function RegDeleteValue(sPath)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath = string.match(sPath, "^(.-)[\\/](.*)")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			return tipUtil:DeleteRegValue(sRegRoot, sRegPath)
		end
	end
	return false
end


function RegSetValue(sPath, value)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			return tipUtil:SetRegValue(sRegRoot, sRegPath, sRegKey or "", value or "")
		end
	end
	return false
end


----UI相关---

function GetMainWndInst()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local objMainWnd = hostwndManager:GetHostWnd("DiDaTipWnd.MainFrame")
	return objMainWnd
end


function GetMainCtrlChildObj(strObjName)
	local objMainWnd = GetMainWndInst()
	if not objMainWnd then
		return nil
	end
	
	local objTree = objMainWnd:GetBindUIObjectTree()
	
	if not objMainWnd or not objTree then
		TipLog("[GetMainCtrlChildObj] get main wnd or tree failed")
		return nil
	end
	
	local objRootCtrl = objTree:GetUIObject("root.layout:root.ctrl")
	if not objRootCtrl then
		TipLog("[GetMainCtrlChildObj] get objRootCtrl failed")
		return nil
	end 

	return objRootCtrl:GetControlObject(tostring(strObjName))
end


function GetYearMonthFromUI()
	local objDateSelectCtrl = GetMainCtrlChildObj("DiDa.DateSelectCtrl")
	local strYear = objDateSelectCtrl:GetYearText()
	local strMonth = objDateSelectCtrl:GetMonthText()
	
	if not IsRealString(strYear) or not IsRealString(strMonth) then
		return nil
	end
	
	local nMonth = tonumber(strMonth)
	strMonth = string.format("%02d", nMonth)
	
	local strYearMonth = strYear..strMonth
	return strYearMonth
end


--strDateInfo：格式： 20150101 , nDay可为空
function GetClndrContent(strDateInfo, fnCallBack)
	local strCalendarDB = GetDBPath("lunar.db")
	local strYear = string.sub(strDateInfo, 1, 4)
	local strMonth = string.sub(strDateInfo, 5, 6)
	local strDay = string.sub(strDateInfo, 7, 8)
	
	local nYear = tonumber(strYear)
	local nMonth = tonumber(strMonth)
	local nDay = tonumber(strDay)
	
	tipAsynUtil:AsynGetCalendarData(strCalendarDB, nYear, nMonth, nDay, 
		function(ret, tData)
		if ret == 0 then
			fnCallBack(tData)
		else
			fnCallBack(nil)
		end
	end)
end

--默认返回每月第一天的索引
--如果当前日期在这个月内， 返回当前日的索引
function GetFocusDayIdxInMonth(tClndrContent, strYearMonth)
	if type(tClndrContent) ~= "table" then
		return 0
	end
	
	local strDateToFind = strYearMonth.."01"
	local strCurYearMonth = os.date("%Y%m")
	if strYearMonth == strCurYearMonth then
		strDateToFind = os.date("%Y%m%d")
	end
	
	for nIndex, tContent in ipairs(tClndrContent) do
		if type(tContent) == "table" and
		   tContent.solarcalendar == strDateToFind then
		   
		   return nIndex
		end
	end
		
	return 0
end


function CheckIsVacation(strDate)
	return CheckIsDateInVacList(strDate, "tVacDay")
end


function CheckIsWorkDay(strDate)
	return CheckIsDateInVacList(strDate, "tWorkDay")
end


function CheckIsDateInVacList(strDate, strType)
	local tVacationList = ReadConfigFromMemByKey("tVacationList") or {}
	local _, _, strYear, strMonth, strDay = string.find(strDate, "(%d%d%d%d)(%d%d)(%d%d)")
	local nMonth = tonumber(strMonth)
	strMonth = string.format("%1d", nMonth)
	
	local nDay = tonumber(strDay)
	strDay = string.format("%1d", nDay)

	local tDateScale = FetchValueByPath(tVacationList, {strYear, strMonth, strType})
	if type(tDateScale) ~= "table" then
		return false
	end
	
	return IsDayInScale(strDay, tDateScale)
end

function IsDayInScale(strDay, tDateScale)
	for _, strScale in pairs(tDateScale) do
		if string.find(strScale, "-") then
			local _, _, strBegin, strEnd = string.find(strScale, "(%d*)-(%d*)")
			local nDay = tonumber(strDay)
			local nBegin = tonumber(strBegin)
			local nEnd = tonumber(strEnd)
			
			if nBegin<=nDay and nDay<=nEnd then
				return true
			end
		else
			if strScale == strDay then
				return true
			end
		end	
	end
	
	return false
end


function UpdateCalendarContent()
	local strYearMonth = GetYearMonthFromUI()
	local objCalendarCtrl = GetMainCtrlChildObj("DiDa.CalendarCtrl")
	objCalendarCtrl:ShowClndrContent(strYearMonth)
end


------------UI--


----菜单--
function TryDestroyOldMenu(objUIElem, strMenuKey)
	local uHostWndMgr = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local uObjTreeMgr = XLGetObject("Xunlei.UIEngine.TreeManager")
	local strHostWndName = strMenuKey..".HostWnd.Instance" 
	local strObjTreeName = strMenuKey..".Tree.Instance"

	if uHostWndMgr:GetHostWnd(strHostWndName) and uObjTreeMgr:GetUIObjectTree(strObjTreeName) then
		uHostWndMgr:RemoveHostWnd(strHostWndName)
		uObjTreeMgr:DestroyTree(strObjTreeName)
		return true
	end
	
	return false
end


--nTopSpan: 离弹出控件的高度差
--bRBtnPopup：右键弹出菜单
function CreateAndShowMenu(objUIElem, strMenuKey, nTopSpan, bRBtnPopup)
	local uTempltMgr = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local uHostWndMgr = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local uObjTreeMgr = XLGetObject("Xunlei.UIEngine.TreeManager")

	if uTempltMgr and uHostWndMgr and uObjTreeMgr then
		local uHostWnd = nil
		local strHostWndName = strMenuKey..".HostWnd.Instance"
		local strHostWndTempltName = "MenuHostWnd"
		local strHostWndTempltClass = "HostWndTemplate"
		local uHostWndTemplt = uTempltMgr:GetTemplate(strHostWndTempltName, strHostWndTempltClass)
		if uHostWndTemplt then
			uHostWnd = uHostWndTemplt:CreateInstance(strHostWndName)
		end

		local uObjTree = nil
		local strObjTreeTempltName = strMenuKey.."Tree"
		local strObjTreeTempltClass = "ObjectTreeTemplate"
		local strObjTreeName = strMenuKey..".Tree.Instance"
		local uObjTreeTemplt = uTempltMgr:GetTemplate(strObjTreeTempltName, strObjTreeTempltClass)
		if uObjTreeTemplt then
			uObjTree = uObjTreeTemplt:CreateInstance(strObjTreeName)
		end

		if uHostWnd and uObjTree then
			--函数会阻塞
			local bSucc = ShowMenuHostWnd(objUIElem, uHostWnd, uObjTree, nTopSpan, bRBtnPopup)
			
			if bSucc and uHostWnd:GetMenuMode() == "manual" then
				uObjTreeMgr:DestroyTree(strObjTreeName)
				uHostWndMgr:RemoveHostWnd(strHostWndName)
			end
		end
	end
end


function ShowMenuHostWnd(objUIElem, uHostWnd, uObjTree, nTopSpan, bRBtnPopup)
	uHostWnd:BindUIObjectTree(uObjTree)
					
	local objMainLayout = uObjTree:GetUIObject("Menu.MainLayout")
	if not objMainLayout then
		TipLog("[ShowMenuHostWnd] find Menu.MainLayout obj failed")
	    return false
	end	
	
	local objNormalMenu = uObjTree:GetUIObject("Menu.Context")
	if not objNormalMenu then
		TipLog("[ShowMenuHostWnd] find normalmenu obj failed")
	    return false
	end	
	objNormalMenu:BindRelateObject(objUIElem)
	
	local nL, nT, nR, nB 
	local objMenuFrame = objNormalMenu:GetControlObject("menu.frame")
	if objMenuFrame then
		nL, nT, nR, nB = objMenuFrame:GetObjPos()				
	else
		nL, nT, nR, nB = objNormalMenu:GetObjPos()	
	end	
	
	local nMenuContainerWidth = nR - nL
	local nMenuContainerHeight = nB - nT

	local nMenuLeft, nMenuTop = 0, 0
	if bRBtnPopup then
		nMenuLeft, nMenuTop = tipUtil:GetCursorPos() 	
		nTopSpan = 0 
	else
		nMenuLeft, nMenuTop = GetScreenAbsPos(objUIElem)
	end
	
	local nMenuL, nMenuT, nMenuR, nMenuB = objUIElem:GetAbsPos()
	local nMenuHeight = nMenuB - nMenuT
	if tonumber(nTopSpan) == nil then
		nTopSpan = nMenuHeight
	end
	
	local nMenuLeft, nMenuTop = AdjustScreenEdge(nMenuLeft, nMenuTop, nMenuContainerWidth, nMenuContainerHeight, nTopSpan)
	
	--函数会阻塞
	local bOk = uHostWnd:TrackPopupMenu(objHostWnd, nMenuLeft, nMenuTop, nMenuContainerWidth, nMenuContainerHeight)
	return bOk
end


function GetScreenAbsPos(objUIElem)
	local objTree = objUIElem:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	local nL, nT, nR, nB = objUIElem:GetAbsPos()
	return objHostWnd:HostWndPtToScreenPt(nL, nT)
end


function AdjustScreenEdge(nMenuLeft, nMenuTop, nMenuContainerWidth, nMenuContainerHeight, nTopSpan)
	local nScrnLeft, nScrnTop, nScrnRight, nScrnBottom = tipUtil:GetWorkArea()
	
	if nMenuLeft < nScrnLeft then
		nMenuLeft = nScrnLeft
	end
	
	if nMenuLeft+nMenuContainerWidth > nScrnRight then
		nMenuLeft = nScrnRight - nMenuContainerWidth
	end
	
	if nMenuTop < nScrnTop then
		nMenuTop = nTopSpan+nScrnTop
	elseif nMenuTop+nMenuContainerHeight+nTopSpan > nScrnBottom then
		nMenuTop = nMenuTop - nMenuContainerHeight
	else
		nMenuTop = nTopSpan+nMenuTop
	end	
		
	return nMenuLeft, nMenuTop
end

-----


-------文件操作---
local g_bLoadCfgSucc = false
local g_tConfigFileStruct = {
	["tUserConfig"] = {
		["strFileName"] = "UserConfig.dat",
		["tContent"] = {}, 
	},
	["tVacationList"] = {
		["strFileName"] = "VacationList.dat",
		["tContent"] = {}, 
	},
}


function ReadAllConfigInfo()
	for strKey, tConfig in pairs(g_tConfigFileStruct) do
		local strFileName = tConfig["strFileName"]
		local strCfgPath = GetCfgPathWithName(strFileName)
		local infoTable = LoadTableFromFile(strCfgPath) or {}
		
		if type(infoTable) ~= "table" then
			TipLog("[ReadAllConfigInfo] no content in file: "..tostring(strFileName))
		end
		
		local tContent = infoTable
		local bMerge = false
		local fnMergeOldFile = tConfig["fnMergeOldFile"]
		if type(fnMergeOldFile) == "function" then
			bMerge, tContent = fnMergeOldFile(infoTable, strFileName)
		end
		
		tConfig["tContent"] = tContent
		if bMerge then
			SaveConfigToFileByKey(strKey)
		end
	end
	
	g_bLoadCfgSucc = true
	TipLog("[ReadAllConfigInfo] success!")
	return true
end



function GetCfgPathWithName(strCfgName)
	local bOk, strBaseDir = QueryAllUsersDir()
	if not bOk then
		return ""
	end
	
	local strCfgFilePath = tipUtil:PathCombine(strBaseDir, "DiDa\\"..tostring(strCfgName))
	return strCfgFilePath or ""
end

function ReadConfigFromMemByKey(strKey)
	if not IsRealString(strKey) or type(g_tConfigFileStruct[strKey])~="table" then
		return nil
	end

	local tContent = g_tConfigFileStruct[strKey]["tContent"]
	return tContent
end


function SaveConfigToFileByKey(strKey)
	if not IsRealString(strKey) or type(g_tConfigFileStruct[strKey])~="table" then
		return
	end

	local strFileName = g_tConfigFileStruct[strKey]["strFileName"]
	local tContent = g_tConfigFileStruct[strKey]["tContent"] or {}
	local strConfigPath = GetCfgPathWithName(strFileName)
	if IsRealString(strConfigPath) then
		tipUtil:SaveLuaTableToLuaFile(tContent, strConfigPath)
	end
end


function SaveAllConfig()
	if g_bLoadCfgSucc then
		for strKey, tContent in pairs(g_tConfigFileStruct) do
			SaveConfigToFileByKey(strKey)
		end
	end
end


---升级--
local g_bIsUpdating = false

function DownLoadNewVersion(tNewVersionInfo, fnCallBack)
	local strPacketURL = tNewVersionInfo.strPacketURL
	local strMD5 = tNewVersionInfo.strMD5
	if not IsRealString(strPacketURL) then
		return
	end
	
	local strFileName = GetFileSaveNameFromUrl(strPacketURL)
	if not string.find(strFileName, "%.exe$") then
		strFileName = strFileName..".exe"
	end
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)

	DownLoadFileWithCheck(strPacketURL, strSavePath, strMD5
	, function(bRet, strRealPath)
		TipLog("[DownLoadNewVersion] strOpenLink:"..tostring(strPacketURL)
		        .."  bRet:"..tostring(bRet).."  strRealPath:"..tostring(strRealPath))
				
		if 0 == bRet then
			fnCallBack(strRealPath, tNewVersionInfo)
			return
		end
		
		if 1 == bRet then	--安装包已经存在
			fnCallBack(strSavePath, tNewVersionInfo)
			return
		end
		
		fnCallBack(nil)
	end)	
end


function CheckCommonUpdateTime(nTimeInDay)
	return CheckUpdateTimeSpan(nTimeInDay, "nLastCommonUpdateUTC")
end

function CheckAutoUpdateTime(nTimeInDay)
	return CheckUpdateTimeSpan(nTimeInDay, "nLastAutoUpdateUTC")
end

function CheckUpdateTimeSpan(nTimeInDay, strUpdateType)
	if type(nTimeInDay) ~= "number" then
		return false
	end
	
	local nTimeInSec = nTimeInDay*24*3600
	local nCurTimeUTC = tipUtil:GetCurrentUTCTime()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local nLastUpdateUTC = tUserConfig[strUpdateType] or 0
	local nTimeSpan = math.abs(nCurTimeUTC - nLastUpdateUTC)
	
	if nTimeSpan > nTimeInSec then
		return true
	end	
	
	return false
end


function SaveCommonUpdateUTC()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nLastCommonUpdateUTC"] = tipUtil:GetCurrentUTCTime()
	SaveConfigToFileByKey("tUserConfig")
end


function SaveAutoUpdateUTC()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nLastAutoUpdateUTC"] = tipUtil:GetCurrentUTCTime()
	SaveConfigToFileByKey("tUserConfig")
end


function CheckIsUpdating()
	return g_bIsUpdating
end

function SetIsUpdating(bIsUpdating)
	if type(bIsUpdating) == "boolean" then
		g_bIsUpdating = bIsUpdating
	end
end


function DownLoadServerConfig(fnCallBack, nTimeInMs)
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	
	local strConfigURL = tUserConfig["strServerConfigURL"]
	if not IsRealString(strConfigURL) then
		fnCallBack(-1)
		return
	end
	
	local strSavePath = GetCfgPathWithName("ServerConfig.dat")
	if not IsRealString(strSavePath) then
		fnCallBack(-1)
		return
	end
	
	local nTime = tonumber(nTimeInMs) or 5*1000
		
	NewAsynGetHttpFile(strConfigURL, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[DownLoadServerConfig] bRet:"..tostring(bRet)
				.." strRealPath:"..tostring(strRealPath))
				
		if 0 == bRet then
			fnCallBack(0, strSavePath)
		else
			fnCallBack(bRet)
		end		
	end, nTime)
end

--

------------------文件--

local obj = {}
obj.tipUtil = tipUtil
obj.tipAsynUtil = tipAsynUtil

--通用
obj.TipLog = TipLog
obj.FailExitTipWnd = FailExitTipWnd
obj.TipConvStatistic = TipConvStatistic
obj.ExitProcess = ExitProcess
obj.ReportAndExit = ReportAndExit
obj.GetCommandStrValue = GetCommandStrValue
obj.GetExePath = GetExePath
obj.LoadTableFromFile = LoadTableFromFile
obj.CheckIsNewVersion = CheckIsNewVersion

obj.NewAsynGetHttpFile = NewAsynGetHttpFile
obj.GetProgramTempDir = GetProgramTempDir
obj.GetDiDaVersion = GetDiDaVersion
obj.GetInstallSrc = GetInstallSrc
obj.GetMinorVer = GetMinorVer

--UI
obj.GetMainWndInst = GetMainWndInst
obj.GetMainCtrlChildObj = GetMainCtrlChildObj
obj.GetYearMonthFromUI = GetYearMonthFromUI
obj.GetClndrContent = GetClndrContent
obj.GetFocusDayIdxInMonth = GetFocusDayIdxInMonth
obj.CheckIsVacation = CheckIsVacation
obj.CheckIsWorkDay = CheckIsWorkDay
obj.UpdateCalendarContent = UpdateCalendarContent

--文件
obj.GetCfgPathWithName = GetCfgPathWithName
obj.ReadConfigFromMemByKey = ReadConfigFromMemByKey
obj.SaveConfigToFileByKey = SaveConfigToFileByKey
obj.ReadAllConfigInfo = ReadAllConfigInfo

--菜单
obj.TryDestroyOldMenu = TryDestroyOldMenu
obj.CreateAndShowMenu = CreateAndShowMenu


--升级
obj.DownLoadServerConfig = DownLoadServerConfig
obj.DownLoadNewVersion = DownLoadNewVersion
obj.CheckIsUpdating = CheckIsUpdating
obj.SetIsUpdating = SetIsUpdating
obj.CheckCommonUpdateTime = CheckCommonUpdateTime
obj.SaveCommonUpdateUTC = SaveCommonUpdateUTC
obj.SaveAutoUpdateUTC = SaveAutoUpdateUTC

--注册表
obj.RegQueryValue = RegQueryValue
obj.RegDeleteValue = RegDeleteValue
obj.RegSetValue = RegSetValue


XLSetGlobal("DiDa.FunctionHelper", obj)

