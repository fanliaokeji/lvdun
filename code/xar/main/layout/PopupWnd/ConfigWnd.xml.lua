local tipUtil = XLGetObject( "GS.Util" )
local Helper = XLGetGlobal("Helper")

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
	--[[
	tBind = self:GetUserData()
	if #tBindSoftware > 0 and not tBind.bHide then
		
	elseif #tBindSoftware > 0 and tBind.bHide then
		--销毁窗口
		Helper:DestoryModelessWnd("TipBindWnd")
		
		--直接无界面下载,拉起安装
		for key, value in ipairs(tBindSoftware) do
			DownloadAndInstall(value.url, value.cmd, value.name)
		end
	elseif tBind.bHide then
		--销毁窗口
		Helper:DestoryModelessWnd("TipBindWnd")
		Helper:LOG("tBind.bHide is true")
	end
	--]]
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