local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

----方法----
function SetTipData(self, infoTab) 
	CreateFilterListener(self)
	return true
end


---事件--
function OnCloseWnd(self)
	tipUtil:Exit("test")
end



function OnLButtonUp(self, bFocus)

end



--监听事件
function CreateFilterListener(objRootCtrl)
	local objFactory = XLGetObject("APIListen.Factory")
	if not objFactory then
		tFunHelper.TipLog("[CreateFilterListener] not support APIListen.Factory")
		return
	end
	
	local objListen = objFactory:CreateInstance()	
	objListen:AttachListener(
		function(key,...)	

			tFunHelper.TipLog("[CreateFilterListener] key: " .. tostring(key))
			
			local tParam = {...}	
			if tostring(key) == "OnCommandLine" then
				OnCommandLine(tParam)
			elseif tostring(key) == "OnExplorerNotify" then
				OnExplorerNotify(tParam)
			end
		end)
end


function ShowHostWnd()
	local objHostWnd = tFunHelper.GetMainWndInst()
	if objHostWnd then
		local hWnd = objHostWnd:GetWndHandle()
		if hWnd then
			tipUtil:SetForegroundWindow(hWnd)
		else
			objHostWnd:Show(5)
		end
	end
end


function OnCommandLine(tParam)
	ShowHostWnd()
end


function OnExplorerNotify(tParam)
	local nMessage = tonumber(tParam[1]) or -1
	local nShow = 0
	local nUpdate = 1
	local nAbout = 2
	local nQUIT = 3
	
	if nMessage == nShow then
		if tFunHelper.CheckIsNeedShow() then
			ShowHostWnd()
		end
	elseif nMessage == nUpdate then
		tFunHelper.ShowPopupWndByName("TipUpdateWnd.Instance", true)
	elseif nMessage == nAbout then
		tFunHelper.ShowPopupWndByName("TipAboutWnd.Instance", true)
	elseif nMessage == nQUIT then
		tFunHelper.ShowPopupWndByName("TipExitRemindWnd.Instance", true)
	end
end


function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end


function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
end

