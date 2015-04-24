local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil


---方法---
function SetTitleText(objRootCtrl, strText)	
	local objTitle = objRootCtrl:GetControlObject("BrowserTitleCtrl.Title")
	if not objTitle then
		return
	end
	
	objTitle:SetText(strText)
end



----事件--
function OnInitControl(self)
	
end


function OnClickCpationClose(self)
	local objMainWnd = tFunHelper.GetMainWndInst()
	objMainWnd:Show(0)
end


function OnClickCpationMin(self)
	local objHostWnd = GetHostWndByUIElem(self)
	if objHostWnd then
		objHostWnd:Min()
	end
end


------辅助函数---
function GetHostWndByUIElem(objUIElem)
	local objTree = objUIElem:GetOwner()
	if objTree then
		return objTree:GetBindHostWnd()
	end
end


function IsRealString(str)
	return type(str) == "string" and str ~= ""
end


