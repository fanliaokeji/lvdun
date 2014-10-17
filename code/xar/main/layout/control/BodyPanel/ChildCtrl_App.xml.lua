local tipUtil = XLGetObject("GS.Util")
local tipAsynUtil = XLGetObject("GS.AsynUtil")
local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")

local g_bHasInit = false
local g_bAppListDownLoading = false

local g_nShowedCount = 0
local g_nDownLoagFlag = 0
local g_tDownLoadList = {}

function OnInitControl(self)
	InitAppCtrl(self)
end

function OnShowPanel(self, bShow)
	if not g_bHasInit and not g_bAppListDownLoading then
		InitAppCtrl(self)
	end
end


function InitAppCtrl(self)
	local objScroolBar = self:GetControlObject("listbox.vscroll")
	if objScroolBar then
		objScroolBar:SetVisible(false)
		objScroolBar:SetChildrenVisible(false)
	end

	local function ShowAppPanel(strAppListName)
		local tAppList = LoadAppList(strAppListName)
		if type(tAppList) ~= "table" then
			TipLog("[InitAppCtrl] LoadDefaultAppList failed")
			return
		end
		
		local bSucc = ShowItemList(self, tAppList, true)
		if bSucc then
			g_bHasInit = true
		end
	end
	
	DownLoadAppList(ShowAppPanel)
end


function DownLoadAppList(fnCallBack)
	local tUserConfig = FunctionObj.GetUserConfigFromMem() or {}
	
	local strAppListURL = tUserConfig["strServerAppListURL"]
	if not IsRealString(strAppListURL) then
		fnCallBack("AppList.dat")
		return
	end
	
	local strSavePath = FunctionObj.GetCfgPathWithName("ServerAppList.dat")
	if not IsRealString(strSavePath) then
		fnCallBack("AppList.dat")
		return
	end
	
	local nTimeInMs = 10*1000
	g_bAppListDownLoading = true
	
	FunctionObj.NewAsynGetHttpFile(strAppListURL, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[InitAppCtrl] bRet:"..tostring(bRet)
				.." strRealPath:"..tostring(strRealPath))
		
		if 0 == bRet then
			fnCallBack("ServerAppList.dat")
		else
			fnCallBack("AppList.dat")
		end
		
		g_bAppListDownLoading = false
	end, nTimeInMs)
end


function ShowItemList(objRootCtrl, tAppList, bDownLoad)
	local strImageBaseDir = GetImageBaseDir()
	if not IsRealString(strImageBaseDir) or not tipUtil:QueryFileExists(strImageBaseDir) then
		TipLog("[ShowItemList] GetImageBaseDir failed")
		return false
	end

	local objAppContainer = objRootCtrl:GetControlObject("ChildCtrl_App.ItemList.Container")
	if not objAppContainer then
		TipLog("[ShowItemList] get objAppContainer failed")
		return false
	end
	
	if bDownLoad then
		g_tDownLoadList = {}
		IncreaseDownLoadFlag()
	end
	
	--顺序创建
	for nIndex, tItem in ipairs(tAppList) do
		if type(tItem) == "table" then
			local strImageName = tostring(tItem["strKeyName"])..".png"
			local strImagePath = strImageBaseDir.."\\"..tostring(strImageName)
	
			if tipUtil:QueryFileExists(strImagePath) then
				tItem["strImagePath"] = strImagePath
				local bSucc = TryCreateAppItem(objAppContainer, tItem, g_nShowedCount+1)
				if bSucc then
					g_nShowedCount = g_nShowedCount+1
				end
			elseif bDownLoad then
				DownLoadImage(objRootCtrl, tItem)
				g_tDownLoadList[#g_tDownLoadList+1] = tItem
			end		
		end
	end
	
	if bDownLoad then
		DecreaseDownLoadFlag(objRootCtrl)
	end

	SetTotalLineCount(objRootCtrl, g_nShowedCount)
	ResetScrollBar(objRootCtrl)
	return true
end


function TryCreateAppItem(objAppContainer, tAppItem, nIndex)
	local strAppName = tAppItem["strAppName"]
	local strImagePath = tAppItem["strImagePath"]
	
	if IsRealString(strImagePath) and tipUtil:QueryFileExists(strImagePath) then
		local objAppItemCtrl = CreateAppItemCtrl(nIndex)
		if objAppItemCtrl then				
			objAppContainer:AddChild(objAppItemCtrl)
			SetAppItemPos(objAppItemCtrl, nIndex)
			
			objAppItemCtrl:SetAppName(strAppName)
			objAppItemCtrl:SetAppImageByPath(strImagePath)
			objAppItemCtrl:SetAppInfo(tAppItem)
			
			objAppItemCtrl:SetRedirect("OnMouseWheel", "control:listbox.vscroll")
			
			return true
		end
	end
	
	return false
end


function DownLoadImage(objRootCtrl, tItem)
	local strImageURL = tItem["strImageURL"] 
	if not IsRealString(strImageURL) then
		return
	end

	local strImageBaseDir = GetImageBaseDir()
	if not IsRealString(strImageBaseDir) or not tipUtil:QueryFileExists(strImageBaseDir) then
		TipLog("[DownLoadImage] GetImageBaseDir failed")
		return
	end
	
	local strImageName = tostring(tItem["strKeyName"])..".png"
	local strSavePath = strImageBaseDir.."\\"..tostring(strImageName)
	
	IncreaseDownLoadFlag()
	FunctionObj.NewAsynGetHttpFile(strImageURL, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[DownLoadImage] bRet:"..tostring(bRet).." strRealPath:"..tostring(strRealPath))
		DecreaseDownLoadFlag(objRootCtrl)
	end)	
end


function IncreaseDownLoadFlag()
	g_nDownLoagFlag = g_nDownLoagFlag+1
end


function DecreaseDownLoadFlag(objRootCtrl)
	g_nDownLoagFlag = g_nDownLoagFlag - 1
	if g_nDownLoagFlag <= 0 and objRootCtrl then
		ShowItemList(objRootCtrl, g_tDownLoadList, false)
	end	
end


function EventRouteToFather(self)
	self:RouteToFather()
end


function LoadAppList(strAppListName)
	local strCfgPath = FunctionObj.GetCfgPathWithName(strAppListName)
	local infoTable = FunctionObj.LoadTableFromFile(strCfgPath)
	return infoTable
end


function GetImageBaseDir()
	local strImageDir = __document .. "\\..\\..\\..\\..\\..\\..\\appimage"
	return strImageDir
end


function CreateAppItemCtrl(nIndex)
	local strKey = "AppItemCtrl.Instance_"..tostring(nIndex)

	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local objAppItemCtrl = objFactory:CreateUIObject(strKey, "AppItemCtrl")
	return objAppItemCtrl
end


function SetAppItemPos(objAppItemCtrl, nIndex)
	local objRootCtrl = objAppItemCtrl:GetOwnerControl()
	if not objRootCtrl then
		return
	end

	local attr = objRootCtrl:GetAttribute()
	local nColumPerLine = attr.nColumPerLine
	
	local nIndexWithFix = nIndex - 1
	local nLine = math.floor(nIndexWithFix/nColumPerLine) + 1
	local nColum = math.mod(nIndexWithFix, nColumPerLine) + 1
	
	local attr = objAppItemCtrl:GetAttribute()
	local nItemHeight = attr.nItemHeight
	local nItemWidth = attr.nItemWidth
	local nItemSpaceH = attr.nItemSpaceH
	local nItemSpaceW = attr.nItemSpaceW
	
	local nItemLeft = (nColum-1)*(nItemWidth+nItemSpaceW)
	local nItemTop = (nLine-1)*(nItemHeight+nItemSpaceH)
	
	objAppItemCtrl:SetObjPos(nItemLeft, nItemTop, nItemLeft+nItemWidth, nItemTop+nItemHeight)
end


function SetTotalLineCount(objRootCtrl, nShowCount)
	local nShowCountWithFix = nShowCount - 1
	if nShowCountWithFix < 0 then
		attr.nTotalLineCount = 0
		return
	end
	
	local attr = objRootCtrl:GetAttribute()
	local nColumPerLine = attr.nColumPerLine
	attr.nTotalLineCount = math.floor(nShowCountWithFix/nColumPerLine) + 1
end

function GetItemHeight(objRootCtrl)
	local objContainer = objRootCtrl:GetControlObject("ChildCtrl_App.ItemList.Container")
	if not objContainer then
		return 0
	end
	
	local objItem = objContainer:GetChildByIndex(1)
	if not objItem then
		return 0
	end

	local attr = objItem:GetAttribute()
	local nItemHeight = attr.nItemHeight
	local nItemSpaceH = attr.nItemSpaceH
	
	return nItemHeight+nItemSpaceH
end


--滚动条
function ResetScrollBar(objRootCtrl)
	if objRootCtrl == nil then
		return false
	end
	local objScrollBar = objRootCtrl:GetControlObject("listbox.vscroll")
	if objScrollBar == nil then
		return false
	end
	
	local attr = objRootCtrl:GetAttribute()
	local nLinePerPage = attr.nLinePerPage
	local nTotalLineCount = attr.nTotalLineCount
	
	local nItemHeight = GetItemHeight(objRootCtrl)
	local nMaxHeight = nItemHeight * nTotalLineCount
	local nPageSize = nItemHeight * nLinePerPage

	objScrollBar:SetScrollRange( 0, nMaxHeight - nPageSize, true )
	objScrollBar:SetPageSize(nPageSize, true)	
	-- objScrollBar:SetScrollPos((g_nCurTopIndex-1) * nItemHeight, true )

	if nLinePerPage >= nTotalLineCount then
		objScrollBar:SetVisible(false)
		objScrollBar:SetChildrenVisible(false)
		return true
	else
		objScrollBar:SetVisible(true)
		objScrollBar:SetChildrenVisible(true)
		objScrollBar:Show(true)
	end
	
	return true
end


function CLB__OnScrollBarMouseWheel(self, name, x, y, distance)
	local objRootCtrl = self:GetOwnerControl()
	local nScrollPos = self:GetScrollPos()

	local nItemHeight = GetItemHeight(objRootCtrl)
		
    if distance > 0 then
		self:SetScrollPos( nScrollPos - nItemHeight, true )
    else		
		self:SetScrollPos( nScrollPos + nItemHeight, true )
    end

	local nNewScrollPos = self:GetScrollPos()
	MoveItemListPanel(objRootCtrl, nNewScrollPos)
	return true	
end


function CLB__OnScrollMousePosEvent(self, name, x, y)
	local objRootCtrl = self:GetOwnerControl()
	local nScrollPos = self:GetScrollPos()
	
	MoveItemListPanel(objRootCtrl, nScrollPos)
end


function MoveItemListPanel(objRootCtrl, nScrollPos)
	if not objRootCtrl then
		return
	end
	
	local objContainer = objRootCtrl:GetControlObject("ChildCtrl_App.ItemList.Container")
	if not objContainer then
		return
	end
	
	local nL, nT, nR, nB = objContainer:GetObjPos()
	local nHeight = nB-nT
	local nNewT = 0-nScrollPos
	
	objContainer:SetObjPos(nL, nNewT, nR, nNewT+nHeight)
end

----

--------AppItemCtrl----
function SetAppName(self, strText)
	if not IsRealString(strText) then
		return
	end

	local objText = self:GetControlObject("AppItemCtrl.Text")
	if not objText then
		return
	end	
	
	objText:SetText(strText)
end

function SetAppImageByPath(self, strImagePath, strColorType)
	if not IsRealString(strImagePath) or not tipUtil:QueryFileExists(strImagePath) then
		TipLog("[SetAppImageByPath] para error:"..tostring(strImagePath))
		return
	end
	
	local strType = strColorType or "ARGB32"
	
	local objGraphicFac = XLGetObject("Xunlei.XLGraphic.Factory.Object")
	if not objGraphicFac then
		TipLog("[SetAppImageByPath] get Graphic Factory failed")
		return
	end
	
	local objBitmap = objGraphicFac:CreateBitmap(strImagePath, strType)
	if not objBitmap  then
		TipLog("[SetAppImageByPath] CreateBitmap failed:"..tostring(strImagePath).." ,"..tostring(strType))
		return
	end
	
	local objImage = self:GetControlObject("AppItemCtrl.Image")
	if not objImage then
		TipLog("[SetAppImageByPath] get objImage failed")
		return
	end
	objImage:SetBitmap(objBitmap)
end

function SetAppInfo(self, tItemInfo)
	if type(tItemInfo)~="table" then
		return
	end
	
	local attr = self:GetAttribute()
	attr.tItemInfo = tItemInfo
end


function OnClickAppImage(objAppImage)
	OpenLinkAfterClick(objAppImage)
end

function OnClickAppText(objAppText)
	OpenLinkAfterClick(objAppText)
end


--1:浏览器打开;2:直接下载或运行
function OpenLinkAfterClick(objUIItem)
	local objRootCtrl = objUIItem:GetOwnerControl()
	local attr = objRootCtrl:GetAttribute()
	local tItemInfo = attr.tItemInfo
	local nOpenType = tItemInfo.nOpenType
	local strOpenLink = tItemInfo.strOpenLink
	local strKeyName = tItemInfo.strKeyName or ""
	
	if not IsRealString(strOpenLink) then
		return
	end
	
	if nOpenType == 1 then
		tipUtil:OpenURL(strOpenLink)
		SendAppReport(strKeyName, 1)
	elseif nOpenType == 2 then
		OpenSoftware(objUIItem, tItemInfo)
	end	
end


function OpenSoftware(objUIItem, tItemInfo)
	local strRegPath = tItemInfo.strRegPath or ""
	local strExeName = tItemInfo.strExeName or ""
	local strKeyName = tItemInfo.strKeyName or ""
	
	local strInstallDir = RegQueryValue(strRegPath)
	if string.lower(strExeName) == "baidusd.exe" then
		local strVersion = RegQueryValue(tItemInfo.strRegVersion)
		strInstallDir = tipUtil:PathCombine(strInstallDir, strVersion)
	end
	local strInstallPath = tipUtil:PathCombine(strInstallDir, strExeName)
	if IsRealString(strInstallDir) and tipUtil:QueryFileExists(strInstallPath) then
		tipUtil:ShellExecute(0, "open", strInstallPath, 0, 0, "SW_SHOWNORMAL")
		SendAppReport(strKeyName, 2)
	else
		DownLoadSoftware(objUIItem, tItemInfo)
		SendAppReport(strKeyName, 3)
	end
end


function DownLoadSoftware(objUIItem, tItemInfo)
	local objRootCtrl = objUIItem:GetOwnerControl()
	local attr = objRootCtrl:GetAttribute()
	if attr.bIsDownLoading then
		return
	end
	attr.bIsDownLoading = true
	
	local strOpenLink = tItemInfo.strOpenLink
	local strCommand = tItemInfo.strCommand or ""
	if not IsRealString(strOpenLink) then
		return
	end
	
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strFileName = GetFileSaveNameFromUrl(strOpenLink)	
	if not string.find(strFileName, "%.exe$") then
		strFileName = strFileName..".exe"
	end
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)
	
	FunctionObj.NewAsynGetHttpFile(strOpenLink, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[DownLoadSoftware] strOpenLink:"..tostring(strOpenLink)
		        .."  bRet:"..tostring(bRet).."  strRealPath:"..tostring(strRealPath))
				
		attr.bIsDownLoading = false
		if 0 ~= bRet then
			return
		end
		
		tipUtil:ShellExecute(0, "open", strRealPath, strCommand, 0, "SW_HIDE")
	end)	
end

-- 1 浏览器打开链接
-- 2 激活软件
-- 3 下载软件 
function SendAppReport(strKeyName, nType)
	local tStatInfo = {}
	tStatInfo.strEC = "AppPanel"
	tStatInfo.strEA = strKeyName
	tStatInfo.strEL = tostring(nType)
	
	if type(FunctionObj.TipConvStatistic) == "function" then
		FunctionObj.TipConvStatistic(tStatInfo)
	end
end


----------------------------------------------------
function IsRealString(str)
	return type(str) == "string" and str~=""
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@GreenWall_Template ChildCtrl_App: " .. tostring(strLog))
	end
end

function RegQueryValue(sPath)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			return tipUtil:QueryRegValue(sRegRoot, sRegPath, sRegKey or "") or ""
		end
	end
	return ""
end

function GetFileSaveNameFromUrl(url)
	local _, _, strFileName = string.find(tostring(url), ".*/(.*)$")
	local npos = string.find(strFileName, "?", 1, true)
	if npos ~= nil then
		strFileName = string.sub(strFileName, 1, npos-1)
	end
	return strFileName
end

