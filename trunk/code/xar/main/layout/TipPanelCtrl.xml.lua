local tipUtil = XLGetObject("GS.Util")
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
			end	
		end
	)
	
	function OnFilterResult(bFilterSucc, strDomain)
		if not bFilterSucc then
			return
		end
		
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
		local strName = p1
		local strDomain = p2
		if not IsRealString(strDomain) then
			return
		end
		
		local bCheckSucc = CheckPopupCond(strDomain)
		if bCheckSucc then
			ShowPopupWindow(strName, strDomain)
		end
	end
end

function ShowPopupWindow(strName, strDomain)
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
	objTextName:SetText(tostring(strName))
	objTextDomain:SetText(tostring(strDomain))
	
	if not IsUserFullScreen() then
		frameHostWnd:SetTopMost(true)
	elseif type(tipUtil.GetForegroundProcessInfo) == "function" then
		local hFrontHandle, strPath = tipUtil:GetForegroundProcessInfo()
		if hFrontHandle ~= nil then
			frameHostWnd:BringWindowToBack(hFrontHandle)
		end
	end
	
	frameHostWnd:Show(4)
end


function CheckPopupCond(strDomain)
	local nCheckSpanInSec = 24*3600
	local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
	if type(tFunctionHelper.GetSpecifyFilterTableFromMem) ~= "function" then
		return false
	end
	
	local tRefuseList = tFunctionHelper.GetSpecifyFilterTableFromMem("tRefuseList") or {}
	if type(tRefuseList[strDomain]) ~= "table" then
		return true
	end	
	
	local nLastUTC = tonumber(tRefuseList[strDomain]["nLastUTC"])
	local nCurUTC = tipUtil:GetCurrentUTCTime()
	if nLastUTC == nil or nCurUTC == nil then
		return false
	end
	
	TipLog("[CheckPopupCond] nCurUTC: "..tostring(nCurUTC).."  nLastUTC:"..tostring(nLastUTC))
	local nDiff = nCurUTC-nLastUTC
	if nDiff < nCheckSpanInSec then
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



