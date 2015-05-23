local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local g_hTimer = nil
local g_hRepBackupTimer = nil
local g_AllowRepair = true
local TimerTryShowRepairWnd = XLGetGlobal("TimerTryShowRepairWnd")

function OnCreate( self )
	PopupInDeskCenter(self)
end


function OnShowWindow(self, bShow)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
	
	if bShow then
		g_AllowRepair = true
		StartTimer(objRootLayout)
	else	
		StopTimer()
	end	
end

-------------

function OnClickCloseBtn(self)
	StopTimer()
	TryRepairStup(objRootLayout)
	HideWindow(self)
end


function OnClickCheckBox(self)
	g_AllowRepair = not g_AllowRepair
	
	if g_AllowRepair then
		self:SetTextureID("CheckBox.Green.Check")
	else
		self:SetTextureID("CheckBox.Green.UnCheck")
	end	
	
	local objtree = self:GetOwner()
	local objRootLayout = objtree:GetUIObject("root.layout")
	StopTimer()
	HideTimerText(objRootLayout)
end


-------------------------
function StopTimer()
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	if g_hTimer ~= nil then
		timerManager:KillTimer(g_hTimer)
		g_hTimer = nil
	end
end


function StartTimer(objRootLayout)
	local objTimerText = objRootLayout:GetObject("RepairRemind.TimerText")
	if not objTimerText then
		return
	end
	
	StopTimer()
	objTimerText:SetText("(5秒)")	
	local nTimeInSec = 5

	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	g_hTimer = timerManager:SetTimer(function(item, id)
		nTimeInSec = nTimeInSec-1
		
		if nTimeInSec < 0 then
			timerManager:KillTimer(g_hTimer)
			g_hTimer = nil
			
			TryRepairStup(objRootLayout)
			HideWindow(objRootLayout)
			return
		end
		
		local strTimeInSec = "("..tostring(nTimeInSec).."秒)"
		objTimerText:SetText(strTimeInSec)	
	end, 1*1000)
end


function HideTimerText(objRootLayout)
	local objTimerText = objRootLayout:GetObject("RepairRemind.TimerText")
	if not objTimerText then
		return
	end
	objTimerText:SetVisible(false)
end


function HideWindow(objElem)
	local objTree = objElem:GetOwner()
	if not objTree then
		return
	end
	
	local tUserConfig = tFunHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["bUserSetAutoStup"] = g_AllowRepair
	
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
	tFunHelper.SaveConfigToFileByKey("tUserConfig")
end


function PopupInDeskCenter(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move( (workright - nLayoutWidth)/2, (workbottom - nLayoutHeight)/2, nLayoutWidth, nLayoutHeight)
	return true
end


function TryRepairStup(objRootLayout)
	if not g_AllowRepair then
		tFunHelper.TipLog("[TryRepairStup] g_AllowRepair = false ")
		return
	end

	local bRepair = DoRepairStup("sysboot_repwnd")
	tFunHelper.TipLog("[TryRepairStup] bRepair = "..tostring(bRepair))
	if bRepair then
		SendRepairReport("repautostupbywnd")
		TimerTryShowRepairWnd()
	else
		StartRepairBackupTimer()
	end
end


function StopRepairBackupTimer()
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	if g_hRepBackupTimer ~= nil then
		timerManager:KillTimer(g_hRepBackupTimer)
		g_hRepBackupTimer = nil
	end
end


function StartRepairBackupTimer()
	tFunHelper.TipLog("[StartRepairBackupTimer] enter")
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	StopRepairBackupTimer()
	
	local tUserConfig = tFunHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nLastBkRepUTC"] = tipUtil:GetCurrentUTCTime()
	tFunHelper.SaveConfigToFileByKey("tUserConfig")
	local nRepAutoStupSpanInSec =  tUserConfig["nRepAutoStupSpanInSec"] or 3*60 --默认3分钟修复一次
	local nRepAutoStupCount =  tUserConfig["nRepAutoStupCount"] or 1 --默认修复一次
	
	g_hRepBackupTimer = timerManager:SetTimer(function(item, id)
		nRepAutoStupCount = nRepAutoStupCount - 1
		if nRepAutoStupCount < 0 then
			StopRepairBackupTimer()
			TimerTryShowRepairWnd()
			return
		end
	
		local bRepair = DoRepairStup("sysboot_repbk")
		if bRepair then
			StopRepairBackupTimer()
			SendRepairReport("repautostupbackground")
			TimerTryShowRepairWnd()
			return
		end		
	end, nRepAutoStupSpanInSec*1000)
end
XLSetGlobal("StartRepairBackupTimer", StartRepairBackupTimer)

function DoRepairStup(strRepSource)
	tFunHelper.TipLog("[DoRepairStup] strRepSource: "..tostring(strRepSource))
	local strExePath = tFunHelper.GetExePath()
	local strRegPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\WebEraser"
	
	local szCmdLine = tFunHelper.RegQueryValue(strRegPath) or ""
	if IsRealString(szCmdLine) 
		and string.find(string.lower(szCmdLine), string.lower(tostring(strExePath)), 1, true) then
		return false  -- 已经开机启动
	end
 
	if IsRealString(strExePath) and tipUtil:QueryFileExists(strExePath) then
		local strCommandline = "\""..strExePath.."\"".." /sstartfrom " ..tostring(strRepSource).. " /embedding"
		bRet = tFunHelper.RegSetValue(strRegPath, strCommandline)
		tFunHelper.TipLog("[DoRepairStup] RegSetValue bret: "..tostring(bRet))
		return bRet
	end
	
	return false
end


function SendRepairReport(strRepSource)
	local tStatInfo = {}
	tStatInfo.strEC = strRepSource	
	tStatInfo.strEA = tFunHelper.GetInstallSrc() or ""
	
	local bRet, strSource = tFunHelper.GetCommandStrValue("/sstartfrom")
	tStatInfo.strEL = strSource or ""
		
	tFunHelper.TipConvStatistic(tStatInfo)
end


function IsRealString(str)
	return type(str) == "string" and str ~= ""
end




