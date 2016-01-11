local tipUtil = XLGetObject( "GS.Util" )
local Helper = XLGetGlobal("Helper")
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")

local tBind = nil
local tBindSoftware = {}

local function DownloadAndInstall(url, cmd, name)
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strFileName = string.match(url, "/([^/]+exe)")
	if not IsRealString(strFileName) then
		strFileName = name
	end
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)
	local strStamp = tFunHelper.GetTimeStamp()
	local strURLFix = url..strStamp
	
	tFunHelper.NewAsynGetHttpFile(strURLFix, strSavePath, false
	, function(bRet, strRealPath)
			tFunHelper.TipLog("[DownLoadGS] bRet:"..tostring(bRet)
					.." strRealPath:"..tostring(strRealPath).." exeName: "..tostring(name))
					
			if 0 ~= bRet then return end
			
			tipUtil:ShellExecute(0, "open", strRealPath, cmd, 0, "SW_HIDE")
	end)
end

function OnCreate(self)
	local userData = self:GetUserData()
	if userData and userData.parentWnd then
		local objtree = self:GetBindUIObjectTree()
		local objRootLayout = objtree:GetUIObject("root")
		local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
		local nLayoutWidth  = nLayoutR - nLayoutL
		local nLayoutHeight = nLayoutB - nLayoutT
	
		local parentLeft, parentTop, parentRight, parentBottom = userData.parentWnd:GetWindowRect()
		local parentWidth  = parentRight - parentLeft
		local parentHeight = parentBottom - parentTop
		self:Move( parentLeft + (parentWidth - nLayoutWidth)/2, parentTop + (parentHeight - nLayoutHeight)/2, nLayoutWidth, nLayoutHeight)
	end
	
	local tree = self:GetBindUIObjectTree()
	local title = tree:GetUIObject("ShareWnd.Caption.Title")
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local nFilterCount = tonumber(tUserConfig["nFilterCountOneDay"]) or 0
	local strText = "今天为您过滤广告"..tostring(nFilterCount).."次"
	title:SetText(strText)
end

function OnShowWindow(self, bVisible)
	if not bVisible then
		return
	end
end

function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	
	local userData = objHostWnd:GetUserData()
	if userData and userData["maskWndParent"] then
		Helper:DestoryTransparentMask(userData["maskWndParent"])
	end
	
	objHostWnd:EndDialog(0)
end

function OnClickWeibo(self)
	local shareUrl = tFunctionHelper.GetShareUrl("weibo")
	if type(shareUrl) == "string" and shareUrl ~= "" then
		tipUtil:ShellExecute(0, "open", shareUrl, 0, 0, "SW_SHOWNORMAL")
	end
end

function OnClickQQ(self)
	local shareUrl = tFunctionHelper.GetShareUrl("qq")
	if type(shareUrl) == "string" and shareUrl ~= "" then
		tipUtil:ShellExecute(0, "open", shareUrl, 0, 0, "SW_SHOWNORMAL")
	end
end

function OnClickQQZone(self)
	local shareUrl = tFunctionHelper.GetShareUrl("qqzone")
	if type(shareUrl) == "string" and shareUrl ~= "" then
		tipUtil:ShellExecute(0, "open", shareUrl, 0, 0, "SW_SHOWNORMAL")
	end
end

function OnClickDouban(self)
	local shareUrl = tFunctionHelper.GetShareUrl("douban")
	if type(shareUrl) == "string" and shareUrl ~= "" then
		tipUtil:ShellExecute(0, "open", shareUrl, 0, 0, "SW_SHOWNORMAL")
	end
end

function OnClickQQGroup(self)
	--将QQ群号复制到剪切板，并给出提示
	local qqGroupNum = "67542242"
	local result = tipUtil:SetClipboardText(qqGroupNum)
	--XLMessageBox("SetClipboardText result: "..tostring(result))
end

function OnMouseEnterQQGroup(self)
	self:SetCursorID("IDC_HAND")
	Helper.Tip:SetTips("点击复制群号到剪切板")
end

function OnMouseLeaveQQGroup(self)
	self:SetCursorID("IDC_ARROW")
	Helper.Tip:DestoryTipWnd()
end
