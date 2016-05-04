local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")

local gStatCount = 0
local gForceExit = nil
local defaultStatPeerid = "0123"

function IsCurPeerIDStatAllowed()
	local statPeerID = RegQueryValue("HKEY_CURRENT_USER\\Software\\mycalendar\\statpeerid")
	TipLog("IsCurPeerIDStatAllowed statPeerID : "..tostring(statPeerID))
	statPeerID = statPeerID or defaultStatPeerid
	
	local machinePeerID = GetPeerID()
	local lastChar = tostring(string.sub(machinePeerID, 12, 12))
	TipLog("IsCurPeerIDStatAllowed machinePeerID : "..tostring(machinePeerID).." lastChar: "..lastChar)
	local begin, _ = string.find(statPeerID, lastChar)
	if begin then
		TipLog("IsCurPeerIDStatAllowed stat is allowed!")
		return true
	end
	
	TipLog("IsCurPeerIDStatAllowed stat not allowed!")
	return false
end

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
		local traceInfo = debug.traceback("", 2)
		local s = string.gsub(traceInfo, "\n", " < ") 
		s =string.sub(s,1, 512)
		tipUtil:Log("@@DiDa_Log: " .. tostring(strLog)..tostring(s))
	end
end


function GetTimeStamp()
	local strPeerId = GetPeerID()
	local iFlag = tonumber(string.sub(strPeerId, 12, 12), 16) or 0
	local iTime = tipUtil:GetCurrentUTCTime()
	local ss = math.floor((iTime + 8 * 3600  - (iFlag + 1) * 3600)/(24*3600))
	local strStamp = "?stamp=" .. tostring(ss)
	return strStamp 
end


function LoadTableFromFile(strDatFilePath)
	local tResult = nil

	if IsRealString(strDatFilePath) and tipUtil:QueryFileExists(strDatFilePath) then
		if XLModuleExists(strDatFilePath) then
			XLUnloadModule(strDatFilePath)
		end
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
			for i = 1, #cmdList, 1 do
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
	DestroyPopupWnd()	
	SendRunTimeReport(0, true)
	SendDiDaReport(10)
	
	tStatInfo.strEC = "exit"	
	tStatInfo.strEA = GetInstallSrc() or ""
	tStatInfo.Exit = true
			
	TipConvStatistic(tStatInfo)
end


function SendRunTimeReport(nTimeSpanInSec, bExit)
	local tStatInfo = {}
	tStatInfo.strEC = "runtime"
	tStatInfo.strEA = GetInstallSrc() or ""
	
	--由于谷歌统计超标，去掉心跳上报
	-- local nRunTime = 0
	-- local nLastReportRunTmUTC = XLGetGlobal("DiDa.LastReportRunTime") 
	-- if bExit and nLastReportRunTmUTC ~= 0 then
		-- nRunTime = math.abs(tipUtil:GetCurrentUTCTime() - nLastReportRunTmUTC)
	-- else
		-- nRunTime = nTimeSpanInSec
	-- end
	-- tStatInfo.strEV = nRunTime
	
	-- TipConvStatistic(tStatInfo)
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
	
	a = tonumber(a)
	b = tonumber(b)
	c = tonumber(c)
	d = tonumber(d)
	
	A = tonumber(A)
	B = tonumber(B)
	C = tonumber(C)
	D = tonumber(D)
	
	return a>A or (a==A and (b>B or (b==B and (c>C or (c==C and d>D)))))
end


function GetPeerID()
	local strPeerID = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\mycalendar\\PeerId")
	if IsRealString(strPeerID) then
		return strPeerID
	end

	local strRandPeerID = tipUtil:GetPeerId()
	if not IsRealString(strRandPeerID) then
		return ""
	end
	
	RegSetValue("HKEY_LOCAL_MACHINE\\Software\\mycalendar\\PeerId", strRandPeerID)
	return strRandPeerID
end

--渠道
function GetInstallSrc()
	local strInstallSrc = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\mycalendar\\InstallSource")
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
	--由于谷歌统计超标,只取四分之一peerid上报
	if not IsCurPeerIDStatAllowed() then end
	
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
	
	local strUrl = "http://www.google-analytics.com/collect?v=1&tid=UA-58424540-1&cid="..tostring(strCID)
						.."&t=event&ec="..tostring(strEC).."&ea="..tostring(strEA)
						.."&el="..tostring(strEL).."&ev="..tostring(strEV)
	
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


function SendDiDaReport(nOPeration)
	local strCID = GetPeerID()
	local strChannelID = GetInstallSrc()
	local strVer = GetMinorVer()
	local strRandom = tipUtil:GetCurrentUTCTime()
	
	local strPort = "8082"
	if nOPeration == 10 then   --心跳上报的端口为8083
		strPort = "8083"
	end
	
	local strUrl = "http://stat.tie7.com:"..tostring(strPort).."/c?appid=1001&peerid=".. tostring(strCID)
					.."&proid=12&op="..tostring(nOPeration).."&cid="..(strChannelID)
					.."&ver="..tostring(strVer).."&rd="..tostring(strRandom)
	
	TipLog("SendDiDaReport: " .. tostring(strUrl))
	tipAsynUtil:AsynSendHttpStat(strUrl, function() end)
end

function IsUACOS()
	local bRet = true
	local iMax, iMin = tipUtil:GetOSVersion()
	if type(iMax) == "number" and iMax <= 5 then
		bRet = false
	end
	return bRet
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
	
	timerID = tipAsynUtil:SetTimer(nTimeoutInMS or 5 * 60 * 1000,
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

	if not IsNilString(strCheckMD5) and CheckMD5(strSavePath, strCheckMD5) then
		TipLog("[DownLoadFileWithCheck]File Already existed")
		fnCallBack(1, strSavePath)
		return
	end
	
	NewAsynGetHttpFile(strURL, strSavePath, false, function(bRet, strDownLoadPath)
		TipLog("[DownLoadFileWithCheck] NewAsynGetHttpFile:bret = " .. tostring(bRet) 
				.. ", strURL = " .. tostring(strURL) .. ", strDownLoadPath = " .. tostring(strDownLoadPath))
		if 0 == bRet then
			strSavePath = strDownLoadPath
            if IsNilString(strCheckMD5) or CheckMD5(strSavePath, strCheckMD5) then
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


function MessageBox(str)
	if not IsRealString(str) then
		return
	end
	
	tipUtil:MsgBox(str, "错误", 0x10)
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


function GetProgramDir()
	local strExePath = GetExePath()
	local _, _, strProgramDir = string.find(strExePath, "(.*)\\.*$")
	return strProgramDir
end


function GetDBPath(strDBName)
	local strProgramDir = GetProgramDir() or ""
	
	local strDBPath = tipUtil:PathCombine(strProgramDir, strDBName)
	if IsRealString(strDBPath) and tipUtil:QueryFileExists(strDBPath) then
		return strDBPath
	end
	
	MessageBox(tostring("关键数据丢失，请重新安装".." "..tostring(strDBPath)))
	ExitProcess()
	return nil
end


function GetDllPath(strDllName)
	local strProgramDir = GetProgramDir() or ""
	local strDllPath = tipUtil:PathCombine(strProgramDir, strDllName)
	if IsRealString(strDllPath) and tipUtil:QueryFileExists(strDllPath) then
		return strDllPath
	end
	
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
			tipUtil:CreateRegKey(sRegRoot, sRegPath)
			return tipUtil:SetRegValue(sRegRoot, sRegPath, sRegKey or "", value or "")
		end
	end
	return false
end


function FindClockWindow()
	local nCurProcessID = tipUtil:GetCurrentProcessId()
	if not nCurProcessID or nCurProcessID == 0 then
		return nil
	end

	local strWndClass = "Shell_TrayWnd"
	local uWndHandle = tipUtil:FindWindow(strWndClass, nil)
	
	if not uWndHandle then
		return nil
	end

	uWndHandle = tipUtil:FindWindowEx(uWndHandle, 0, "TrayNotifyWnd", nil)
	if not uWndHandle then
		return nil
	end
	
	return tipUtil:FindWindowEx(uWndHandle, 0, "TrayClockWClass", NULL)
end


function KillClockWindow()
	local g_InjectDLLTimer = XLGetGlobal("DiDa.InjectDLLTimer") 
	if g_InjectDLLTimer then
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
		timerManager:KillTimer(g_InjectDLLTimer)
	end

	local hClockWnd = FindClockWindow()
	if not hClockWnd then
		return
	end
	
	local uMsg = 1024 + 320
	local wParam = 100
	tipUtil:PostWndMessageByHandle(hClockWnd, uMsg, wParam,0)
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
	
	local objCtrl = objTree:GetUIObject("root.layout:root.ctrl:"..strObjName)
	
	return objCtrl
	
end


function ShowPopupWndByName(strWndName, bSetTop)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd(tostring(strWndName))
	if frameHostWnd == nil then
		TipLog("[ShowPopupWindow] GetHostWnd failed: "..tostring(strWndName))
		return
	end

	if not IsUserFullScreen() then
		if type(tipUtil.SetWndPos) == "function" then
			local hWnd = frameHostWnd:GetWndHandle()
			if hWnd ~= nil then
				TipLog("[ShowPopupWndByName] success")
				if bSetTop then
					frameHostWnd:SetTopMost(true)
					tipUtil:SetWndPos(hWnd, 0, 0, 0, 0, 0, 0x0043)
				else
					tipUtil:SetWndPos(hWnd, -2, 0, 0, 0, 0, 0x0043)
				end
			end
		end
	elseif type(tipUtil.GetForegroundProcessInfo) == "function" then
		local hFrontHandle, strPath = tipUtil:GetForegroundProcessInfo()
		if hFrontHandle ~= nil then
			frameHostWnd:BringWindowToBack(hFrontHandle)
		end
	end
	
	frameHostWnd:Show(5)
end

function ShowDeleteNotepadRemindWnd(strType, callback)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd("TipExitRemindWnd.Instance")
	if frameHostWnd == nil then
		TipLog("[ShowDeleteNotepadRemindWnd] GetHostWnd failed: TipExitRemindWnd.Instance")
		return
	end
	local objTree = frameHostWnd:GetBindUIObjectTree()
	local objRootCtrl = objTree:GetUIObject("root.layout")
	local title = objRootCtrl:GetObject("ExitRemind.Title")
	local ExitRemindText1 = objRootCtrl:GetObject("ExitRemind.Text1")
	local ExitRemindText2 = objRootCtrl:GetObject("ExitRemind.Text2")
	local CenterImg = objRootCtrl:GetObject("ExitRemind.Center.Obj")
	local Exitbtn = objRootCtrl:GetObject("ExitRemind.ExitBtn")
	local strCenterTextureID, strRemindText, strRemindText1, strTitle, nStartPos, tCenterPos
	if strType == "notepad" then
		strCenterTextureID = "delete.notepad.center"
		strRemindText = "确定要删除所选记事本？"
		strTitle = "记事本删除提示："
		nStartPos = 105
		tCenterPos={141, 26, 110, 110}
	elseif strType == "querybind" then
		strCenterTextureID = "DiDa.ExitRemind.Exit.Center"
		strRemindText = "是否需要为您新增广告过滤功能？"
		strRemindText1 = "您的电脑日历已升级成功"
		strTitle = "我的-任务栏日历"
		nStartPos = 42
		tCenterPos={141, 2, 177, 135}
	else
		strCenterTextureID = "delete.remind.center"
		strRemindText = "确定要删除所选提醒文本？"
		strTitle = "提醒删除提示："
		nStartPos = 96
		tCenterPos={141, 26, 110, 110}
	end
	title:SetText(strTitle)
	CenterImg:SetTextureID(strCenterTextureID)
	CenterImg:SetObjPos2(tCenterPos[1], tCenterPos[2], tCenterPos[3], tCenterPos[4])
	if strRemindText1 ~= nil then
		ExitRemindText1:SetText(strRemindText1)
		ExitRemindText2:SetText(strRemindText)
		Exitbtn:SetText("确定")
	else
		Exitbtn:SetText("删除")
		ExitRemindText1:SetVisible(false)
		ExitRemindText2:SetText(strRemindText)
		ExitRemindText2:SetObjPos2(nStartPos, 151, 200, 20)
	end
	local tUserData = {
		["callback"] = callback,
		["restore"] = function()
						Exitbtn:SetText("退出")
						title:SetText("我的-任务栏日历")
						CenterImg:SetTextureID("DiDa.ExitRemind.Exit.Center")
						CenterImg:SetObjPos2(141, 2, 177, 135)
						ExitRemindText1:SetVisible(true)
						ExitRemindText1:SetText("退出后，")
						ExitRemindText2:SetText("就不能随时查看日历信息哦～")
						ExitRemindText2:SetObjPos2(42, 168, "father.width-75", 20)
						frameHostWnd:SetUserData(nil)
						--恢复状态标记：失去焦点时隐藏
						local SetLoseFocusNoHideFlag = XLGetGlobal("SetLoseFocusNoHideFlag") 
						SetLoseFocusNoHideFlag(false)
					end,
	}
	frameHostWnd:SetUserData(tUserData)
	ShowPopupWndByName("TipExitRemindWnd.Instance", true)
end

function ShowRemindBubble(data)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd("TipRemindBubbleWnd.Instance")
	if frameHostWnd == nil then
		TipLog("[ShowRemindBubble] GetHostWnd failed: TipRemindBubbleWnd.Instance")
		return
	end
	local objTree = frameHostWnd:GetBindUIObjectTree()
	local objRootCtrl = objTree:GetUIObject("root.layout")
	local ExitRemindText1 = objRootCtrl:GetObject("ExitRemind.Text1")
	local ExitRemindText2 = objRootCtrl:GetObject("ExitRemind.Text2")
	local ExitRemindText3 = objRootCtrl:GetObject("ExitRemind.Text3")
	local ExitRemindText4 = objRootCtrl:GetObject("ExitRemind.Text4")
	local LYear, LMonth, LDay, LHour, LMinute, LSecond = tipUtil:FormatCrtTime(data["createtime"])
	local strData = string.format("记事时间：%04d-%02d-%02d     %02d:%02d:%02d", LYear, LMonth, LDay, LHour, LMinute, LSecond)
	ExitRemindText1:SetText(strData)
	LYear, LMonth, LDay, LHour, LMinute, LSecond = tipUtil:FormatCrtTime(data["remindtime"])
	strData = string.format("提醒时间：%04d-%02d-%02d     %02d:%02d:%02d", LYear, LMonth, LDay, LHour, LMinute, LSecond)
	ExitRemindText2:SetText(strData)
	ExitRemindText3:SetText("提醒标题："..tostring(data["title"]))
	ExitRemindText4:SetText("提醒内容："..tostring(data["content"]))
	ShowPopupWndByName("TipRemindBubbleWnd.Instance", true)
end

function GetYearScale()
	return 1900, 2100
end


function GetYearMonthFromUI()
	local objDateSelectCtrl = GetMainCtrlChildObj("DiDa.CalendarView:DiDa.DateSelectCtrl")
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
	TipLog("strDateInfo: "..tostring(strDateInfo))
	local strCalendarDB = GetDBPath("lunar.db")
	local strYear = string.sub(strDateInfo, 1, 4)
	local strMonth = string.sub(strDateInfo, 5, 6)
	local strDay = string.sub(strDateInfo, 7, 8)
	
	local nYear = tonumber(strYear)
	local nMonth = tonumber(strMonth)
	local nDay = tonumber(strDay)
	TipLog("[GetClndrContent] strYear:"..tostring(strYear).."  strMonth:"..tostring(strMonth)
				.."  strDay:".. tostring(strDay))
	
	tipAsynUtil:AsynGetCalendarData(strCalendarDB, nYear, nMonth, nDay, 
		function(ret, tData)
		if ret == 0 then
			fnCallBack(tData)
		else
			fnCallBack(nil)
		end
	end)
end

--返回当前天的索引
function GetFocusDayIdxInMonth(tClndrContent, strYearMonth)
	if type(tClndrContent) ~= "table" then
		return 0
	end
	
	local strCurYearMonth = os.date("%Y%m")
	if strYearMonth ~= strCurYearMonth then
		return 0
	end
	
	local strDateToFind = os.date("%Y%m%d")
	for nIndex, tContent in ipairs(tClndrContent) do
		if type(tContent) == "table" and
		   tContent.solarcalendar == strDateToFind then
		   
		   return nIndex
		end
	end
		
	return 0
end


function UpdateBackTodayStyle(nFocusDayIdxInMonth)
	local objCalendarCtrl = GetMainCtrlChildObj("DiDa.CalendarView:DiDa.DateSelectCtrl")
	if nFocusDayIdxInMonth == 0 then
		objCalendarCtrl:SetBackTodayVisible(true)
	else	
		objCalendarCtrl:SetBackTodayVisible(false)
	end
end


function CheckIsInMonth(tClndrItem, strQryYearMonth)		
	local strSolarcalendar = tClndrItem.solarcalendar
	local strYearMonth = string.sub(strSolarcalendar, 1, 6)
	if strYearMonth == strQryYearMonth then
		return true
	end
	
	return false	
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


function CheckIsYearInVacList(strYear) 
	local tVacationList = ReadConfigFromMemByKey("tVacationList") or {}
	local tYearInfo = FetchValueByPath(tVacationList, {strYear})
	if type(tYearInfo) ~= "table" then
		return false
	end
	
	return true
end


function UpdateCalendarContent()
	local strYearMonth = GetYearMonthFromUI()
	local objCalendarDisp = GetMainCtrlChildObj("DiDa.CalendarView:DiDa.CalendarDispatch")
	objCalendarDisp:ShowCalendarInfo(strYearMonth)
end


function UpdateLeftBarContent()
	local objLeftBarCtrl = GetMainCtrlChildObj("DiDa.CalendarPreView:DiDa.LeftBarCtrl")
	local strCurDate = os.date("%Y%m%d")
	
	GetClndrContent(strCurDate, 
		function (tClndrContentList)
			if type(tClndrContentList) ~= "table" then
				return
			end
		
			tClndrContent = tClndrContentList[1]
			objLeftBarCtrl:SetClndrInfo(tClndrContent)
		end)
end


function UpdateUIPanel()
	UpdateLeftBarContent()
	UpdateCalendarContent()
end


function RunSeqFrameAni(object, seqResID, fun, total_time, loop)	
	if not object then
		return
	end
	if loop == nil then
		loop = true
	end
	if total_time == nil then
		total_time = 800
	end
	
	object:SetVisible(true)
	local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")
	local seqFramAni = aniFactory:CreateAnimation("SeqFrameAnimation")
	seqFramAni:SetLoop(loop)
	seqFramAni:SetResID(seqResID)
	seqFramAni:SetTotalTime(total_time)
	seqFramAni:BindImageObj(object)
	local tree = object:GetOwner()
	tree:AddAnimation(seqFramAni)
	local handle = object:GetHandle()
	local function OnAniFinish(self, old, new)
		if new == 4 then
			if fun then fun(bkg) end
		end
		return true
	end
	
	seqFramAni:AttachListener(true, OnAniFinish)
	seqFramAni:Resume()
	return seqFramAni
end

--天气
local strIPUrl = "http://ip.dnsexit.com/index.php"
local strIPToCity = "http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=json&ip="
local strWeatherPrefix = "http://weather.51wnl.com/weatherinfo/GetMoreWeather?cityCode="

function GetWeatherInfo(fnSuccess, fnFail)
	local JsonFun = XLGetGlobal("DiDa.Json")
	GetCityCode(
		function(nRet, tData)
			if nRet ~= 0 or type(tData) ~= "table" or type(tData[1]) ~= "table" 
			or not IsRealString(tData[1]["code"]) then
				fnFail()
				return
			end
			
			local strWeatherUrl = strWeatherPrefix .. tostring(tData[1]["code"]) .. "&weatherType=0"
			
			tipAsynUtil:AjaxGetHttpContent(strWeatherUrl, 
				function(iRet, strContent, respHeaders)
					TipLog("[GetWeather] strWeatherUrl = " .. tostring(strWeatherUrl) .. ", iRet = " .. tostring(iRet))
					if iRet ~= 0 or not IsRealString(strContent) or strContent == "{}" then
						TipLog("[GetWeather] Get weather info failed.")
						fnFail()
						return
					end
					local tabWeather = JsonFun:decode(strContent)
					if type(tabWeather) ~= "table" or type(tabWeather["weatherinfo"]) ~= "table" then
						TipLog("[GetWeather] Parse weather info failed.")
						fnFail()
						return
					end
					
					tabDetail = tabWeather["weatherinfo"]
					local strCity = tabDetail["city"]
					local strTemp1 = tabDetail["temp1"]
					local strWeather1 = tabDetail["weather1"]
					fnSuccess(strCity,strTemp1,strWeather1)
				end)
	end)
end


function SaveCityCode(strCityCode)
	if IsNilString(strCityCode) then
		return
	end

	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	
	if type(tUserConfig["tCityCode"]) ~= "table" then
		tUserConfig["tCityCode"] = {}
	end
	
	local tCityCode = tUserConfig["tCityCode"]
	tCityCode["nLastUTC"] = tipUtil:GetCurrentUTCTime()
	tCityCode["strCityCode"] = strCityCode
	SaveConfigToFileByKey("tUserConfig")
end


function CheckExistCityCode()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local tCityCode = tUserConfig["tCityCode"]
	if type(tCityCode)~="table" or not IsRealString(tCityCode.strCityCode) then
		return false, nil
	end

	local nLastUTC = tCityCode["nLastUTC"]
	if CheckTimeIsAnotherDay(nLastUTC) then
		return false, nil
	end
	
	return true, tCityCode.strCityCode
end



function GetCityCode(fnCallBack)
	local bExist, strCityCode = CheckExistCityCode()
	if bExist then
		local tData = {}
		tData[1] = {}
		tData[1]["code"] = strCityCode
		
		fnCallBack(0, tData)
		return
	end
	
	local JsonFun = XLGetGlobal("DiDa.Json")
	tipAsynUtil:AjaxGetHttpContent(strIPUrl, 
		function(iRet, strContent, respHeaders)
			TipLog("[GetCityCode] strIPUrl = " .. tostring(strIPUrl) .. ", iRet = " .. tostring(iRet))
			if iRet ~= 0 then
				fnCallBack(-1)
				return
			end
			
			local strIP =  string.match(strContent,"(%d+.%d+.%d+.%d+)")
			if not IsRealString(strIP) then
				fnCallBack(-1)
				return
			end
			
			local strQueryIPToCityUrl = strIPToCity .. tostring(strIP)
			tipAsynUtil:AjaxGetHttpContent(strQueryIPToCityUrl, 
				function(iRet, strContent, respHeaders)
					TipLog("[GetCityCode] strQueryIPToCityUrl = " .. tostring(strQueryIPToCityUrl) .. ", iRet = " .. tostring(iRet))
					if iRet ~= 0 or not IsRealString(strContent) then
						fnCallBack(-1)
						return
					end
					local tabCityInfo = JsonFun:decode(strContent)
					if type(tabCityInfo) ~= "table" or not IsRealString(tabCityInfo["city"]) then
						fnCallBack(-1)
						return
					end
					
					local strCityName = tabCityInfo["city"]
					local strCityDB = GetDBPath("citycode.db")
					local strSQL = "select * from cityinfo where city='" .. strCityName .. "'"
					TipLog("[GetCityCode] strSQL = " .. tostring(strSQL))
					tipAsynUtil:AsynExecuteSqlite3DML(strCityDB, strSQL, nil,nil,function(iRet, tData)
					
						if iRet == 0 and type(tData) == "table" and type(tData[1]) == "table" 
							and not IsNilString(tData[1]["code"]) then
								SaveCityCode(tData[1]["code"])
						end
						
						fnCallBack(iRet, tData)
					end)
				end)
		end)
end



--弹出窗口--
local g_tPopupWndList = {
	[1] = {"TipAboutWnd", "TipAboutTree"},
	[2] = {"TipExitRemindWnd", "TipExitRemindTree"},
	[3] = {"TipUpdateWnd", "TipUpdateTree"},
	[4] = {"TipRemindBubbleWnd", "TipRemindBubbleTree"},
	--[5] = {"TipCommon", "TipCommonTree"},
}

function CreatePopupTipWnd()
	for key, tItem in pairs(g_tPopupWndList) do
		local strHostWndName = tItem[1]
		local strTreeName = tItem[2]
		local bSucc = CreateWndByName(strHostWndName, strTreeName)
	end
	
	return true
end

function CreateWndByName(strHostWndName, strTreeName)
	local bSuccess = false
	local strInstWndName = strHostWndName..".Instance"
	local strInstTreeName = strTreeName..".Instance"
	
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate(strHostWndName, "HostWndTemplate" )
	if frameHostWndTemplate then
		local frameHostWnd = frameHostWndTemplate:CreateInstance(strInstWndName)
		if frameHostWnd then
			local objectTreeTemplate = nil
			objectTreeTemplate = templateMananger:GetTemplate(strTreeName, "ObjectTreeTemplate")
			if objectTreeTemplate then
				local uiObjectTree = objectTreeTemplate:CreateInstance(strInstTreeName)
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

	return bSuccess
end

function DestroyPopupWnd()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")

	for key, tItem in pairs(g_tPopupWndList) do
		local strPopupWndName = tItem[1]
		local strPopupInst = strPopupWndName..".Instance"
		
		local objPopupWnd = hostwndManager:GetHostWnd(strPopupInst)
		if objPopupWnd then
			hostwndManager:RemoveHostWnd(strPopupInst)
		end
	end
end

------------UI--



-------文件操作---
local g_bLoadCfgSucc = false
local g_tConfigFileStruct = {
	["tUserConfig"] = {
		["strFileName"] = "UserConfig.dat",
		["tContent"] = {}, 
		["fnMergeOldFile"] = function(infoTable, strFileName) return MergeOldUserCfg(infoTable, strFileName) end,
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


function MergeOldUserCfg(tCurrentCfg, strFileName)
	local tOldCfg, strOldCfgPath = GetOldCfgContent(strFileName)
	if type(tOldCfg) ~= "table" then
		return false, tCurrentCfg
	end
	
	if type(tCurrentCfg["tExtraCodeInfo"]) ~= "table" then
		tCurrentCfg["tExtraCodeInfo"] = {}
	end
	
	local tOldStateConfig = tOldCfg["tExtraCodeInfo"] or {}
	for strKey, tStateInfo in pairs(tOldStateConfig) do
		tCurrentCfg["tExtraCodeInfo"][strKey] = tStateInfo
	end	
	
	tCurrentCfg["nLaunchAiSvcTime"] = tOldCfg["nLaunchAiSvcTime"] or 0
	
	tipUtil:DeletePathFile(strOldCfgPath)
	return true, tCurrentCfg
end


function GetOldCfgContent(strCurFileName)
	local strOldFileName = strCurFileName..".bak"
	local strOldCfgPath = GetCfgPathWithName(strOldFileName)
	if not IsRealString(strOldCfgPath) or not tipUtil:QueryFileExists(strOldCfgPath) then
		return nil
	end
	
	local tOldCfg = LoadTableFromFile(strOldCfgPath)
	return tOldCfg, strOldCfgPath
end


function GetCfgPathWithName(strCfgName)
	local bOk, strBaseDir = QueryAllUsersDir()
	if not bOk then
		return ""
	end
	
	local strCfgFilePath = tipUtil:PathCombine(strBaseDir, "mycalendar\\"..tostring(strCfgName))
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
			if strKey ~= "tVacationList" then
				SaveConfigToFileByKey(strKey)
			end
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

	local strStamp = GetTimeStamp()
	local strURLFix = strPacketURL..strStamp
	
	DownLoadFileWithCheck(strURLFix, strSavePath, strMD5
	, function(bRet, strRealPath)
		TipLog("[DownLoadNewVersion] strOpenLink:"..tostring(strURLFix)
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
	
	return false, nTimeInSec-nTimeSpan
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
	
	-- local strConfigURL = tUserConfig["strServerConfigURL"]
	local strConfigURL = tUserConfig["strServerConfigURL"]--25及以后的版本
	if not IsRealString(strConfigURL) then
		fnCallBack(-1)
		return
	end
	
	local strConfigName = GetFileSaveNameFromUrl(strConfigURL)
	local strSavePath = GetCfgPathWithName(strConfigName)
	if not IsRealString(strSavePath) then
		fnCallBack(-1)
		return
	end
	
	local strStamp = GetTimeStamp()
	local strURLFix = strConfigURL..strStamp	
	
	NewAsynGetHttpFile(strURLFix, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[DownLoadServerConfig] bRet:"..tostring(bRet)
				.." strRealPath:"..tostring(strRealPath))
				
		if 0 == bRet then
			fnCallBack(0, strSavePath)
		else
			fnCallBack(bRet)
		end		
	end)
end

--

------------------文件--

local obj = {}
obj.tipUtil = tipUtil
obj.tipAsynUtil = tipAsynUtil

--通用
obj.TipLog = TipLog
obj.GetTimeStamp = GetTimeStamp
obj.MessageBox = MessageBox
obj.GetPeerID = GetPeerID
obj.FailExitTipWnd = FailExitTipWnd
obj.TipConvStatistic = TipConvStatistic
obj.ExitProcess = ExitProcess
obj.ReportAndExit = ReportAndExit
obj.GetCommandStrValue = GetCommandStrValue
obj.GetExePath = GetExePath
obj.GetProgramDir = GetProgramDir
obj.LoadTableFromFile = LoadTableFromFile
obj.CheckIsNewVersion = CheckIsNewVersion
obj.GetFileSaveNameFromUrl = GetFileSaveNameFromUrl
obj.CheckMD5 = CheckMD5
obj.SendRunTimeReport = SendRunTimeReport

obj.NewAsynGetHttpFile = NewAsynGetHttpFile
obj.DownLoadFileWithCheck = DownLoadFileWithCheck
obj.GetProgramTempDir = GetProgramTempDir
obj.GetDiDaVersion = GetDiDaVersion
obj.GetInstallSrc = GetInstallSrc
obj.GetMinorVer = GetMinorVer
obj.GetDllPath = GetDllPath
obj.KillClockWindow = KillClockWindow
obj.CheckTimeIsAnotherDay = CheckTimeIsAnotherDay
obj.SendDiDaReport = SendDiDaReport
obj.IsUACOS = IsUACOS

obj.RunSeqFrameAni = RunSeqFrameAni


--UI
obj.GetMainWndInst = GetMainWndInst
obj.HideMainWindow = HideMainWindow
obj.GetMainCtrlChildObj = GetMainCtrlChildObj
obj.ShowPopupWndByName = ShowPopupWndByName
obj.CreatePopupTipWnd = CreatePopupTipWnd
obj.GetYearMonthFromUI = GetYearMonthFromUI
obj.GetYearScale = GetYearScale
obj.GetClndrContent = GetClndrContent
obj.GetFocusDayIdxInMonth = GetFocusDayIdxInMonth
obj.CheckIsVacation = CheckIsVacation
obj.CheckIsInMonth = CheckIsInMonth
obj.CheckIsWorkDay = CheckIsWorkDay
obj.UpdateCalendarContent = UpdateCalendarContent
obj.UpdateUIPanel = UpdateUIPanel
obj.GetWeatherInfo = GetWeatherInfo
obj.CheckIsYearInVacList = CheckIsYearInVacList
obj.UpdateBackTodayStyle = UpdateBackTodayStyle
obj.ShowDeleteNotepadRemindWnd = ShowDeleteNotepadRemindWnd
obj.ShowRemindBubble = ShowRemindBubble
obj.CreateWndByName = CreateWndByName

--文件
obj.GetCfgPathWithName = GetCfgPathWithName
obj.ReadConfigFromMemByKey = ReadConfigFromMemByKey
obj.SaveConfigToFileByKey = SaveConfigToFileByKey
obj.ReadAllConfigInfo = ReadAllConfigInfo


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

