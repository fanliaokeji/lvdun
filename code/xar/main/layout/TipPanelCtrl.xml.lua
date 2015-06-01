local tipUtil = XLGetObject("GS.Util")
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")

local g_tPanelCtrlList = {
	"ChildCtrl_AdvCount",
	"ChildCtrl_Config",
}

----方法----
function SetTipData(self, infoTab) 
	if infoTab == nil or type(infoTab) ~= "table" then
		return false
	end
	
	local bSucc = InitMainBodyCtrl(self)
	if not bSucc then
		TipLog("[SetTipData] InitMainBodyCtrl failed")
		return false
	end
	
	CreateFilterListener(self)
	return true
end

function InitMainBodyCtrl(objRootCtrl)
	local objMainBodyCtrl = objRootCtrl:GetControlObject("TipCtrl.MainWnd.MainBody")
	if objMainBodyCtrl == nil then
		return false
	end
	
	local bSucc = objMainBodyCtrl:InitPanelList(g_tPanelCtrlList)
	if not bSucc then
		return false
	end
	
	local bSucc = objMainBodyCtrl:ChangePanel("ChildCtrl_AdvCount")
	if not bSucc then
		return false
	end
	
	return true
end

---------事件---------



--------辅助函数----
local g_bFilterASKFlag = true
function CheckFilterASKLock()
	return g_bFilterASKFlag
end

function FilterASKLock()
	g_bFilterASKFlag = false
end

function FilterASKUnLock()
	g_bFilterASKFlag = true
end


function CreateFilterListener(objRootCtrl)
	local gsFactory = XLGetObject("GSListen.Factory")
	if not gsFactory then
		TipLog("[CreateFilterListener] not support GSListen.Factory")
		return
	end
	
	local gsListen = gsFactory:CreateInstance()	
	gsListen:AttachListener(
		function(key,p1,p2)		
			TipLog("[CreateFilterListener] key: " .. tostring(key)
					.. "  p1: "..tostring(p1) .. " p2: "..tostring(p2))
			
			if tostring(key) == "OnFilterResult" then
				OnFilterResult(p1, p2)
			elseif tostring(key) == "OnFilterASK" then
				OnFilterASK(p1, p2)
			elseif tostring(key) == "OnCommandLine" then
				OnCommandLine(p1, p2)
			elseif tostring(key) == "OnFilterVideo" then
				OnFilterVideo(p1, p2)
			end		
		end
	)
	
	function OnFilterResult(bFilterSucc, strDomain)
		if not bFilterSucc then
			return
		end
		
		tFunctionHelper.PopupBubbleOneDay()
		
		local objMainBodyCtrl = objRootCtrl:GetControlObject("TipCtrl.MainWnd.MainBody")
		if objMainBodyCtrl == nil then
			TipLog("[OnFilterResult] get objMainBodyCtrl failed")
			return
		end
		
		local objAdvCount = objMainBodyCtrl:GetChildObjByCtrlName("ChildCtrl_AdvCount")
		if objMainBodyCtrl == nil then
			TipLog("[OnFilterResult] get ChildCtrl_AdvCount failed")
			return
		end

		objAdvCount:AddAdvCount()
	end

	function OnFilterASK(p1, p2)
		local strDomain = p1
		local strURL = p2
		TipLog("[OnFilterASK] strDomain: "..tostring(strDomain)
				.."  strURL: "..tostring(strURL))
		
		-- local strDomain = ExractRuleDomain(strURL)
		if not IsRealString(strDomain) then
			TipLog("[OnFilterASK] strDomain is nil")
			return
		end
		
		local bCheckSucc = CheckPopupCond(strDomain)
		if bCheckSucc then
			FilterASKLock()
			ShowPopupWindow(strDomain)
			FilterASKUnLock()
		end
	end
	
	function OnCommandLine(p1, p2)
		local strCommand = p1
		if not IsRealString(strCommand) then
			return
		end
		
		local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
		local strHostWndName = "GreenWallTipWnd.MainFrame"
		local objHostWnd = hostwndManager:GetHostWnd(strHostWndName)
		objHostWnd:Show(4)
		objHostWnd:BringWindowToTop(true)
	end	
end


local g_nFilterFlag = false
function OnFilterVideo(p1, p2)
	if g_nFilterFlag then
		return
	end
	
	local nVideoIncSec = GetVideoSec()
	tFunctionHelper.IncVideoFiltTime(nVideoIncSec)
	
	g_nFilterFlag = true
	SetFilterFlagDelay()
end

function SetFilterFlagDelay()
	local nTimeSpanInMs = 5 * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		item:KillTimer(id)
		g_nFilterFlag = false
	end, nTimeSpanInMs)
end

function GetVideoSec()
	local ost = os.time()
	math.randomseed(tostring(ost):reverse():sub(1, 6))
	math.random(100)  --第一个值跳过，随机性低
	local nRandom = math.random(100)

	local tSecondZone = {
		["20"] = {1,30},
		["10"] = {31,50},
		["15"] = {51,70},
		["30"] = {71,80},
		["50"] = {81,90},
		["60"] = {91,95},
		["70"] = {96,100},
	}

	local nVideoSec = 0
	for strVideoSec, tScale in pairs(tSecondZone) do
		local nBegin = tScale[1]
		local nEnd = tScale[2]
		if nRandom >= nBegin and nRandom <= nEnd then
			nVideoSec = tonumber(strVideoSec)
			break
		end	
	end
	
	TipLog("[GetVideoSec] nRandom: "..tostring(nRandom).."  nVideoSec: "..tostring(nVideoSec))
	return nVideoSec
end


function ExractRuleDomain(strURL)
	if not IsRealString(strURL) then
		return nil
	end

	local _, _, strHost = string.find(strURL, "^.*://([^/]*)/.*")
	if not IsRealString(strHost) then
		return nil
	end
	
	local tVideoList = tFunctionHelper.ReadConfigFromMemByKey("tVideoList") or {}
	for strDomain, tVideoElem in pairs(tVideoList) do
		if string.find(strHost, strDomain.."$") then
			return strDomain
		end
	end	
	
	return nil
end


function ShowPopupWindow(strDomain)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd("TipFilterRemindWnd.Instance")
	if frameHostWnd == nil then
		TipLog("[ShowPopupWindow] GetHostWnd failed")
		return
	end
	
	local objPopupTree = frameHostWnd:GetBindUIObjectTree()
	if objPopupTree == nil then
		TipLog("[ShowPopupWindow] GetBindUIObjectTree failed")
		return
	end
	
	local objRootLayout = objPopupTree:GetUIObject("root.layout")
	if objRootLayout == nil then
		TipLog("[ShowPopupWindow] get objRootLayout failed")
		return
	end
	
	local objTextName = objRootLayout:GetObject("FilterRemind.Record.Name")
	local objTextDomain = objRootLayout:GetObject("FilterRemind.Record.Domain")
	if objTextName == nil or objTextDomain == nil then
		TipLog("[ShowPopupWindow] get objTextName failed")
		return
	end 
	
	--记录名称和域名，准备传给过滤面版
	local strName = GetDomainDefName(strDomain)
	objTextName:SetText(tostring(strName))
	objTextDomain:SetText(tostring(strDomain))
	
	tFunctionHelper.SetWndForeGround(frameHostWnd)
	frameHostWnd:Show(5)
	RecordPopupTime(strDomain)
end


function GetDomainDefName(strDomain)
	local tVideoList = tFunctionHelper.ReadConfigFromMemByKey("tVideoList") or {}
	local tVideoElem = tVideoList[strDomain]
	if type(tVideoElem) ~= "table" then
		return ""
	end
	
	return tVideoElem["strTitle"] or ""
end


function RecordPopupTime(strDomain)
	local tVideoList = tFunctionHelper.ReadConfigFromMemByKey("tVideoList") or {}
	if type(tVideoList[strDomain]) ~= "table" then
		tVideoList[strDomain] = {}
	end
	
	tVideoList[strDomain]["nLastPopupUTC"] = tipUtil:GetCurrentUTCTime()
	tFunctionHelper.SaveConfigToFileByKey("tVideoList")
end 


function CheckPopupCond(strDomain)
	local tVideoList = tFunctionHelper.ReadConfigFromMemByKey("tVideoList") or {}
	local tVideoElem = tVideoList[strDomain]
	if type(tVideoElem) ~= "table" then
		TipLog("[CheckPopupCond] not video website: "..tostring(strDomain))
		return false
	end
	
	local nLastPopupUTC = tVideoElem["nLastPopupUTC"] 
	if not IsNilString(nLastPopupUTC) and not tFunctionHelper.CheckTimeIsAnotherDay(nLastPopupUTC) then
		TipLog("[CheckPopupCond] has popup today: "..tostring(strDomain))
		return false
	end
	
	local bState = tFunctionHelper.GetVideoDomainState(strDomain)
	if bState == 1 then
		TipLog("[CheckPopupCond] fiter has opened: "..tostring(strDomain))
		return false
	end
	
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd("TipFilterRemindWnd.Instance")
	if frameHostWnd ~= nil then
		local bVisible = frameHostWnd:GetVisible()
		if bVisible then
			TipLog("[CheckPopupCond] TipFilterRemindWnd is visible: "..tostring(strDomain))
			return false
		end
	end
	
	local bPassCheck = CheckFilterASKLock()
	if not bPassCheck then
		TipLog("[CheckPopupCond] CheckFilterASKLock failed: "..tostring(strDomain))
		return false
	end	
	
	return true
end



function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@GreenWall_Template TipPanelCtrl: " .. tostring(strLog))
	end
end


function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
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


