local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
local tipUtil = XLGetObject( "GS.Util" )
local g_bAppListDownLoading = false


function OnCreate( self )
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	local selfleft, selftop, selfright, selfbottom = self:GetWindowRect()
	local wndwidth, wndheight = selfright - selfleft, selfbottom - selftop
	local objtree = self:GetBindUIObjectTree()
	local objRootCtrl = objtree:GetUIObject("root.layout")
	local webleft, webtop, webright, webbottom = objRootCtrl:GetAbsPos()
	local webwidth, webheight = webright - webleft, webbottom - webtop
	local wndleft = ((workright-workleft)-webwidth)/2-webleft
	local wndtop = ((workbottom-worktop)-webheight)/2-webtop
	self:Move(wndleft, wndtop, wndwidth, wndheight)
end


function OnShowWindow(self, bVisible)
	if not bVisible then
		return
	end
	
	local objTree = self:GetBindUIObjectTree()
	local objRootCtrl = objTree:GetUIObject("root.layout")
	if not objRootCtrl then
		return
	end	
	
	local function InitMainWnd(nRet, strCfgPath)	
		HideCheckingImage(objRootCtrl)
		
		if 0 ~= nRet then
			ShowNoUpdate(objRootCtrl)
			return
		end	
		
		local tServerConfig = FunctionObj.LoadTableFromFile(strCfgPath) or {}
		local tNewVersionInfo = tServerConfig["tNewVersionInfo"]
		if(type(tNewVersionInfo)) ~= "table" then
			ShowNoUpdate(objRootCtrl)
			return 
		end
		
		local strCurVersion = FunctionObj.GetGSVersion()
		local strNewVersion = tNewVersionInfo.strVersion
		if not IsRealString(strCurVersion) or not IsRealString(strNewVersion)
			or string.lower(strCurVersion) == string.lower(strNewVersion) then
			ShowNoUpdate(objRootCtrl)
			return
		end
		
		ShowReadyUpdate(objRootCtrl, tNewVersionInfo)
	end
	
	ShowCheckingImage(objRootCtrl)
	DownLoadServerConfig(InitMainWnd)
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
	self:SetVisible(false)
	self:SetChildrenVisible(false)

	local objTree = self:GetOwner()
	local objRootCtrl = objTree:GetUIObject("root.layout")
	local objProgBarLayout = objRootCtrl:GetObject("TipUpdate.Bkg:TipUpdate.ProgressBar.Layout")
	objProgBarLayout:SetVisible(true)
	objProgBarLayout:SetChildrenVisible(true)
	
	SetProgBar(objProgBarLayout)
end


-----------
function DownLoadServerConfig(fnCallBack)
	local tUserConfig = FunctionObj.GetUserConfigFromMem() or {}
	
	local strConfigURL = tUserConfig["strServerConfigURL"]
	if not IsRealString(strConfigURL) then
		fnCallBack(-1)
		return
	end
	
	local strSavePath = FunctionObj.GetCfgPathWithName("ServerConfig.dat")
	if not IsRealString(strSavePath) then
		fnCallBack(-1)
		return
	end
	
	local nTimeInMs = 30*1000
	g_bAppListDownLoading = true
	
	FunctionObj.NewAsynGetHttpFile(strConfigURL, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[DownLoadServerConfig] bRet:"..tostring(bRet)
				.." strRealPath:"..tostring(strRealPath))
		
		g_bAppListDownLoading = false
		
		if 0 == bRet then
			fnCallBack(0, strSavePath)
		else
			fnCallBack(bRet)
		end		
	end, nTimeInMs)

end


function ShowCheckingImage(objRootCtrl)
	local objBkg = objRootCtrl:GetObject("TipUpdate.Bkg")
	if not objBkg then
		return
	end
	
	local objTitle = objBkg:GetObject("TipUpdate.Title.Text")
	local objContent = objBkg:GetObject("TipUpdate.Content.Layout")
	local objCheckingImage = objBkg:GetObject("TipUpdate.CheckingImage")
	local objEnterBtn = objBkg:GetObject("TipUpdate.EnterBtn")
	local objProgressBar = objBkg:GetObject("TipUpdate.ProgressBar.Layout")
	
	if not objTitle or not objContent or not objCheckingImage
		or not objEnterBtn or not objProgressBar then
		return
	end

	objContent:SetVisible(false)
	objContent:SetChildrenVisible(false)
	objEnterBtn:SetVisible(false)
	objEnterBtn:SetChildrenVisible(false)
	objProgressBar:SetVisible(false)
	objProgressBar:SetChildrenVisible(false)
	
	objTitle:SetText("正在为您检查更新......")
	
	local timeMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
	local nIndex = 0
	gCheckTimerId = timeMgr:SetTimer(function(Itm, id)
		local nTempIdx = math.mod(nIndex, 4)+1
		local strImageID = "GreenWall.UpdateWnd.Checking_"..tostring(nTempIdx)
		objCheckingImage:SetResID(strImageID)
		
		nIndex = nIndex+1
		if nIndex >= 100 then
			nIndex = 0
		end		
	end, 200)
	
end


function HideCheckingImage(objRootCtrl)
	local timeMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
	if gCheckTimerId then
		timeMgr:KillTimer(gCheckTimerId)
	end

	local objCheckingImage = objRootCtrl:GetObject("TipUpdate.Bkg:TipUpdate.CheckingImage")
	if not objCheckingImage then
		return
	end
	
	objCheckingImage:SetVisible(false)	
end


function ShowNoUpdate(objRootCtrl)
	local objTitle = objRootCtrl:GetObject("TipUpdate.Bkg:TipUpdate.Title.Text")
	if not objTitle then
		return
	end
	
	objTitle:SetText("已经是最新版本")
end


function ShowReadyUpdate(objRootCtrl, tNewVersionInfo)	
	local objProgBarLayout = objRootCtrl:GetObject("TipUpdate.Bkg:TipUpdate.ProgressBar.Layout")
	if not objProgBarLayout then
		return
	end
	
	local objEnterBtn = objRootCtrl:GetObject("TipUpdate.Bkg:TipUpdate.EnterBtn")
	if not objEnterBtn then
		return
	end
	
	objProgBarLayout:SetVisible(false)
	objProgBarLayout:SetChildrenVisible(false)
	objEnterBtn:SetVisible(true)
	objEnterBtn:SetChildrenVisible(true)
	
	SetVersionText(objRootCtrl, tNewVersionInfo)
end

function SetProgBar(objProgBarLayout)
	local objProgBar = objProgBarLayout:GetObject("TipUpdate.ProgressBar")
	if not objProgBar then
		return
	end
	local l_nProgress = 0
	

	function DownLoadNewVersion()

		objProgBar:SetProgress(l_nProgress)
	end

	DownLoadNewVersion()
end


function SetVersionText(objRootCtrl, tNewVersionInfo)
	local objVersion = objRootCtrl:GetObject("TipUpdate.Bkg:TipUpdate.Title.Text")
	local objContent = objRootCtrl:GetObject("TipUpdate.Bkg:TipUpdate.Content.Layout")
	if not objVersion or not objContent then
		return
	end
	
	objVersion:SetVisible(true)
	objVersion:SetChildrenVisible(true)
	objContent:SetVisible(true)
	objContent:SetChildrenVisible(true)
	
	local strVersion = tNewVersionInfo.strVersion
	local strText = "发现新版本绿盾广告管家V"..tostring(strVersion)
	objVersion:SetText(strText)
	
	local objText1 = objContent:GetObject("TipUpdate.Content.Text1")
	local objText2 = objContent:GetObject("TipUpdate.Content.Text2")
	local objText3 = objContent:GetObject("TipUpdate.Content.Text3")
	local tInroduce = tNewVersionInfo.tInroduce

	objText1:SetText(tostring(tInroduce[1]))
	objText2:SetText(tostring(tInroduce[2]))
	objText3:SetText(tostring(tInroduce[3]))	
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

