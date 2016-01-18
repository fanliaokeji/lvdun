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
	local desc = "良心软件啊，竟然能过滤所有视频网站的缓冲广告。免费，亲测有效。看视频再也不用等广告简直爽到哭..."
	local shareUrl = tFunctionHelper.GetShareUrl("qq", nil, desc)
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
	-- local result = tipUtil:SetClipboardText(qqGroupNum)
	-- Helper.Tip:SetTips("已复制QQ群号")
	local shareUrl = "tencent://groupwpa/?subcmd=all&param=7B2267726F757055696E223A36373534323234322C2274696D655374616D70223A313435333130313939357D0A"
	tipUtil:ShellExecute(0, "open", shareUrl, 0, 0, "SW_SHOWNORMAL")
end

function OnMouseEnterQQGroup(self)
	self:SetCursorID("IDC_HAND")
	Helper.Tip:SetTips("点击即可加入QQ群")
end

function OnMouseLeaveQQGroup(self)
	self:SetCursorID("IDC_ARROW")
	Helper.Tip:DestoryTipWnd()
end

function OnMouseEnterWeibo(self)
	Helper.Tip:SetTips("分享到新浪微博")
end
function OnMouseEnterQQ(self)
	Helper.Tip:SetTips("分享给QQ好友和QQ群")
end
function OnMouseEnterDouban(self)
	Helper.Tip:SetTips("分享到豆瓣")
end

function OnMouseEnterQQZone(self)
	Helper.Tip:SetTips("分享到QQ空间")
end

function OnMouseLeave(self)
	Helper.Tip:DestoryTipWnd()
end