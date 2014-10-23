local tipUtil = XLGetObject("GS.Util")
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
local g_tPanelCtrlList = {
	"ChildCtrl_AdvCount",
	"ChildCtrl_Config",
	"ChildCtrl_App",
	"ChildCtrl_FilterConfig",
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
function OnClickCloseBtn(self)
	HideWndToTray(self)
end

function OnClickMinBtn(self)
	local objTree = self:GetOwner()
	if nil == objTree then
		return
	end
	
	local objHostWnd = objTree:GetBindHostWnd()
	if nil == objHostWnd then
		return
	end
	
	objHostWnd:Min()	
end

function OnClickConfigBtn(self)
	OpenPanel(self, "ChildCtrl_Config")
	SetCaptionText(self, "设置")
end


function OnClickMainPage(self)
	OpenPanel(self, "ChildCtrl_AdvCount")
	SetCaptionText(self, "绿盾广告管家")
end

function OnClickApp(self)
	OpenPanel(self, "ChildCtrl_App")
	SetCaptionText(self, "应用")
end

function OnClickFilterConfig(self)
	OpenPanel(self, "ChildCtrl_FilterConfig")
	SetCaptionText(self, "过滤设置")
end


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
				--show
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
end


function ExractRuleDomain(strURL)
	if not IsRealString(strURL) then
		return nil
	end

	local _, _, strHost = string.find(strURL, "^.*://([^/]*)/.*")
	if not IsRealString(strHost) then
		return nil
	end
	
	local tVideoList = tFunctionHelper.GetVideoListFromMem() or {}
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
	
	if not IsUserFullScreen() then
		frameHostWnd:SetTopMost(true)
		if type(tipUtil.SetWndPos) == "function" then
			local hWnd = frameHostWnd:GetWndHandle()
			if hWnd ~= nil then
				tipUtil:SetWndPos(hWnd, 0, 0, 0, 0, 0,0x0053)
			end
		end
	elseif type(tipUtil.GetForegroundProcessInfo) == "function" then
		local hFrontHandle, strPath = tipUtil:GetForegroundProcessInfo()
		if hFrontHandle ~= nil then
			frameHostWnd:BringWindowToBack(hFrontHandle)
		end
	end
	
	frameHostWnd:Show(4)
	RecordPopupTime(strDomain)
end


function GetDomainDefName(strDomain)
	local tVideoList = tFunctionHelper.GetVideoListFromMem() or {}
	local tVideoElem = tVideoList[strDomain]
	if type(tVideoElem) ~= "table" then
		return ""
	end
	
	return tVideoElem["strTitle"] or ""
end


function RecordPopupTime(strDomain)
	local tVideoList = tFunctionHelper.GetVideoListFromMem() or {}
	if type(tVideoList[strDomain]) ~= "table" then
		tVideoList[strDomain] = {}
	end
	
	tVideoList[strDomain]["nLastPopupUTC"] = tipUtil:GetCurrentUTCTime()
	tFunctionHelper.SaveVideoListToFile()
end 


function CheckPopupCond(strDomain)
	local tVideoList = tFunctionHelper.GetVideoListFromMem() or {}
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


function OpenPanel(objButton, strNewCtrlName)
	if objButton == nil then
		return
	end

	local objRootCtrl = objButton:GetOwnerControl()
	if objRootCtrl == nil then
		return
	end

	local objMainBodyCtrl = objRootCtrl:GetControlObject("TipCtrl.MainWnd.MainBody")
	if objMainBodyCtrl == nil then
		return
	end
	
	local strCurCtrlName = objMainBodyCtrl:GetCurrentCtrlName()
	if strCurCtrlName ~= strNewCtrlName then
		objMainBodyCtrl:ChangePanel(strNewCtrlName)
	end
end

function SetCaptionText(objUIItem, strTitle)
	if not IsRealString(strTitle) then
		return
	end

	local objRootCtrl = objUIItem:GetOwnerControl()
	local objCaptionText = objRootCtrl:GetControlObject("TipCtrl.Caption.Text")
	if not objCaptionText then
		return
	end

	objCaptionText:SetText(strTitle)
end


function IsUserFullScreen()
	local bRet = false
	if type(tipUtil.IsNowFullScreen) == "function" then
		bRet = tipUtil:IsNowFullScreen()
	end
	return bRet
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

function HideWndToTray(objUIElement)
	local objTree = objUIElement:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end



