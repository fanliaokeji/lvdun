local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local tipAsynUtil = tFunHelper.tipAsynUtil
local g_tNewVersionInfo = {}


function OnCreate( self )
	PopupInDeskRight(self)
end


function OnShowWindow(self, bVisible)
	if not bVisible then
		return
	end
	
	local objTree = self:GetBindUIObjectTree()
	local objHostWnd = objTree:GetBindHostWnd()
	if objHostWnd then
		objHostWnd:SetTitle("嘀嗒在线升级")
	end
	
	local objRootCtrl = objTree:GetUIObject("root.layout")
	if not objRootCtrl then
		return
	end	
	
	local attr = objRootCtrl:GetAttribute()
	if attr.bInstall and tipUtil:QueryFileExists(attr.strInstallPath) then
		return
	end
	
	local function InitMainWnd(nRet, strCfgPath)	
		HideCheckingImage(objRootCtrl)
		
		if 0 ~= nRet then
			ShowNoUpdate(objRootCtrl)
			return
		end	

		local tServerConfig = tFunHelper.LoadTableFromFile(strCfgPath) or {}
		local tNewVersionInfo = tServerConfig["tNewVersionInfo"] or {}
		local strPacketURL = tNewVersionInfo["strPacketURL"]
		if not IsRealString(strPacketURL) then
			ShowNoUpdate(objRootCtrl)
			return 
		end
		
		g_tNewVersionInfo = tNewVersionInfo
		
		local strCurVersion = tFunHelper.GetProjectVersion()
		local strNewVersion = tNewVersionInfo.strVersion
		if not IsRealString(strCurVersion) or not IsRealString(strNewVersion)
			or not tFunHelper.CheckIsNewVersion(strNewVersion, strCurVersion) then
			ShowNoUpdate(objRootCtrl)
			return
		end
		
		local strSavePath = GetPacketSavePath(strPacketURL)
		if IsRealString(tNewVersionInfo.strMD5) 
			and tFunHelper.CheckMD5(strSavePath, tNewVersionInfo.strMD5) then
			objRootCtrl:ShowInstallPanel(strSavePath)
			return
		end
		
		ShowReadyUpdate(objRootCtrl, tNewVersionInfo)
	end
	
	ShowCheckingImage(objRootCtrl)
	tFunHelper.DownLoadServerConfig(InitMainWnd)
end


function ShowInstallPanel(self, strInstallPath)
	local objRootCtrl = self
	SetVersionText(objRootCtrl)
	
	ShowLayout(objRootCtrl, "TipUpdate.CheckUpdate.Layout", false)
	ShowLayout(objRootCtrl, "TipUpdate.Update.Layout", true)
	
	local objEnterBtn = objRootCtrl:GetControlObject("TipUpdate.EnterBtn")
	local objTitle = objRootCtrl:GetControlObject("TipUpdate.DownLoading.Text")
	local objGif = objRootCtrl:GetControlObject("TipUpdate.DownLoading.Loading")
	
	objGif:SetVisible(false)
	objEnterBtn:SetVisible(true)
	objEnterBtn:SetChildrenVisible(true)
	objTitle:SetVisible(true)
	objTitle:SetChildrenVisible(true)
	
	strText = "已经下载完毕"
	objTitle:SetText(strText)
	objEnterBtn:SetText("立即安装")
	
	local attr = objRootCtrl:GetAttribute()
	attr.bInstall = true
	attr.strInstallPath = strInstallPath
end

----------------------------------------------------------------

function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Show(0)
end


function OnClickMinBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Min()
end


function OnClickEnterBtn(self)
	local objRootCtrl = self:GetOwnerControl()
	local attr = objRootCtrl:GetAttribute()
	if attr.bInstall and tipUtil:QueryFileExists(attr.strInstallPath) then
		local strInstallPath = attr.strInstallPath 
		tipUtil:ShellExecute(0, "open", strInstallPath, 0, 0, "SW_SHOWNORMAL")
		HideCurrentWnd(self)
		SendUpdateReport(1)
	else
		ShowDownLoading(objRootCtrl)
		self:SetVisible(false)
		self:SetChildrenVisible(false)	
		SendUpdateReport(2)
		
		tFunHelper.DownLoadNewVersion(g_tNewVersionInfo, function(strPacketPath)
			if not IsRealString(strPacketPath) then
				ShowErrorPanel(objRootCtrl)
				return
			end
			
			objRootCtrl:ShowInstallPanel(strPacketPath)
		end)
	end
end


-----------
function ShowLayout(objRootCtrl, strLayoutKey, bShow)
	local objLayout = objRootCtrl:GetControlObject(strLayoutKey)
	if objLayout then
		objLayout:SetVisible(bShow)
		objLayout:SetChildrenVisible(bShow)
	end
end


function ShowCheckingImage(objRootCtrl)
	ShowLayout(objRootCtrl, "TipUpdate.CheckUpdate.Layout", true)
	ShowLayout(objRootCtrl, "TipUpdate.Update.Layout", false)
	
	local objUpdateGif = objRootCtrl:GetControlObject("TipUpdate.CheckUpdate.Loading")
	objUpdateGif:Play()
end


function HideCheckingImage(objRootCtrl)
	ShowLayout(objRootCtrl, "TipUpdate.CheckUpdate.Layout", false)
	ShowLayout(objRootCtrl, "TipUpdate.Update.Layout", true)
	ShowLayout(objRootCtrl, "TipUpdate.DownLoading.Layout", false)
	
	local objUpdateGif = objRootCtrl:GetControlObject("TipUpdate.CheckUpdate.Loading")
	objUpdateGif:Stop()
end


function ShowNoUpdate(objRootCtrl)
	ShowLayout(objRootCtrl, "TipUpdate.CheckUpdate.Layout", true)
	ShowLayout(objRootCtrl, "TipUpdate.Update.Layout", false)

	local objText = objRootCtrl:GetControlObject("TipUpdate.CheckUpdate.Text")
	local objGif = objRootCtrl:GetControlObject("TipUpdate.CheckUpdate.Loading")

	objGif:SetVisible(false)
	objText:SetText("已经是最新版本")
end


function ShowReadyUpdate(objRootCtrl)	
	local objEnterBtn = objRootCtrl:GetControlObject("TipUpdate.EnterBtn") 
	objEnterBtn:SetVisible(true)
	objEnterBtn:SetChildrenVisible(true)
	objEnterBtn:SetText("立即下载")

	ShowLayout(objRootCtrl, "TipUpdate.CheckUpdate.Layout", false)
	ShowLayout(objRootCtrl, "TipUpdate.Update.Layout", true)
	ShowLayout(objRootCtrl, "TipUpdate.DownLoading.Layout", false)
	
	SetVersionText(objRootCtrl)
end


function ShowDownLoading(objRootCtrl, tNewVersionInfo)	
	ShowLayout(objRootCtrl, "TipUpdate.CheckUpdate.Layout", false)
	ShowLayout(objRootCtrl, "TipUpdate.Update.Layout", true)
	ShowLayout(objRootCtrl, "TipUpdate.DownLoading.Layout", true)
	
	local objUpdateGif = objRootCtrl:GetControlObject("TipUpdate.DownLoading.Loading")
	objUpdateGif:SetVisible(true)
	objUpdateGif:Play()
	
	local objText = objRootCtrl:GetControlObject("TipUpdate.DownLoading.Text")
	objText:SetVisible(true)
	objText:SetText("下载中，请稍候")
end


function ShowErrorPanel(objRootCtrl)
	ShowLayout(objRootCtrl, "TipUpdate.CheckUpdate.Layout", false)
	ShowLayout(objRootCtrl, "TipUpdate.Update.Layout", true)
	ShowLayout(objRootCtrl, "TipUpdate.DownLoading.Layout", true)
	
	local objGif = objRootCtrl:GetControlObject("TipUpdate.DownLoading.Loading")
	local objEnterBtn = objRootCtrl:GetControlObject("TipUpdate.EnterBtn")
	local objTitle = objRootCtrl:GetControlObject("TipUpdate.DownLoading.Text")
	
	objGif:SetVisible(false)
	objEnterBtn:SetVisible(true)
	objEnterBtn:SetChildrenVisible(true)
	objTitle:SetVisible(true)
	objTitle:SetChildrenVisible(true)
	
	objTitle:SetText("新版本下载失败")
	objEnterBtn:SetText("重新下载")
end


function CheckPacketMD5(strPacketPath)
	local strServerMD5 = g_tNewVersionInfo["strMD5"]
	return tFunHelper.CheckMD5(strPacketPath, strServerMD5)
end


function GetPacketSavePath(strURL)
	local strFileName = tFunHelper.GetFileSaveNameFromUrl(strURL)
	if not string.find(strFileName, "%.exe$") then
		strFileName = strFileName..".exe"
	end
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)

	return strSavePath
end


function HideCurrentWnd(objUIItem)
	local objTree = objUIItem:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	if objHostWnd then
		objHostWnd:Show(0)
	end
end


function SetVersionText(objRootCtrl)
	local tNewVersionInfo = g_tNewVersionInfo
	if type(tNewVersionInfo) ~= "table" then
		return
	end

	local objVersion = objRootCtrl:GetControlObject("TipUpdate.Title.Text")
	local objContent = objRootCtrl:GetControlObject("TipUpdate.Content.Layout")
	if not objVersion or not objContent then
		return
	end
	
	objVersion:SetVisible(true)
	objVersion:SetChildrenVisible(true)
	objContent:SetVisible(true)
	objContent:SetChildrenVisible(true)
	
	local strVersion = tNewVersionInfo.strVersion
	
	local strText = "发现新版本嘀嗒日历V"..tostring(strVersion)
	objVersion:SetText(strText)
	
	local objText1 = objContent:GetObject("TipUpdate.Content.Text1")
	local objText2 = objContent:GetObject("TipUpdate.Content.Text2")
	local objText3 = objContent:GetObject("TipUpdate.Content.Text3")
	local tInroduce = tNewVersionInfo.tInroduce

	if IsRealString(tInroduce[1]) then
		objText1:SetText(tostring(tInroduce[1]))
	end
	
	if IsRealString(tInroduce[2]) then
		objText2:SetText(tostring(tInroduce[2]))
	end
	
	if IsRealString(tInroduce[3]) then
		objText3:SetText(tostring(tInroduce[3]))
	end
end



--1 表示立即安装， 2表示立即更新
function SendUpdateReport(nState)
	local tStatInfo = {}
	tStatInfo.strEC = "UpdateWnd"
	tStatInfo.strEA = tostring(nState)
	
	if type(tFunHelper.TipConvStatistic) == "function" then
		tFunHelper.TipConvStatistic(tStatInfo)
	end
end


function PopupInDeskRight(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
    local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move( workright - nLayoutWidth - 7, workbottom - nLayoutHeight-5, nLayoutWidth, nLayoutHeight)
	return true
end


--------------------------
function IsRealString(str)
	return type(str) == "string" and str~=""
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@TipUpdateWnd: " .. tostring(strLog))
	end
end

