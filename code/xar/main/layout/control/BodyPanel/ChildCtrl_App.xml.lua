local tipUtil = XLGetObject("GS.Util")
local g_bHasInit = false
local g_nCurTopIndex = 1


function OnShowPanel(self, bShow)
	if not g_bHasInit then
		InitAppCtrl(self)
	end
end


function InitAppCtrl(self)
	local tAppList = LoadAppList()
	if type(tAppList) ~= "table" then
		TipLog("[InitAppCtrl] LoadAppList failed")
		return
	end
	
	local strImageBaseDir = GetImageBaseDir()
	if not IsRealString(strImageBaseDir) or not tipUtil:QueryFileExists(strImageBaseDir) then
		TipLog("[InitAppCtrl] GetImageBaseDir failed")
		return
	end

	local objAppContainer = self:GetControlObject("ChildCtrl_App.ItemList.Container")
	if not objAppContainer then
		TipLog("[InitAppCtrl] get objAppContainer failed")
		return
	end
	
	local nShowCount = 0
	--顺序创建
	for nIndex, tItem in ipairs(tAppList) do
		local tAppItem = tItem or {}
	
		local strAppName = tAppItem["strAppName"]
		local strImageName = tAppItem["strImageName"]
		local nOpenType = tAppItem["nOpenType"]
		local strOpenLink = tAppItem["strOpenLink"]
		local strImagePath = strImageBaseDir.."\\"..strImageName
	
		local objAppItemCtrl = CreateAppItemCtrl(nIndex)
		if objAppItemCtrl then		
			objAppContainer:AddChild(objAppItemCtrl)
			SetAppItemPos(objAppItemCtrl, nIndex)
			
			objAppItemCtrl:SetAppName(strAppName)
			objAppItemCtrl:SetAppImageByPath(strImagePath)
			objAppItemCtrl:SetAppOpenType(nOpenType)
			objAppItemCtrl:SetAppOpenLink(strOpenLink)
			objAppItemCtrl:SetRedirect("OnMouseWheel", "control:listbox.vscroll")

			nShowCount = nShowCount+1
		end
	end
	
	SetTotalLineCount(self, nShowCount)
	ResetScrollBar(self)
	g_bHasInit = true
end


function EventRouteToFather(self)
	self:RouteToFather()
end


function LoadAppList()
	local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
	local strCfgPath = FunctionObj.GetCfgPathWithName("AppList.dat")
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
	local nTotalLineCount = attr.nTotalLineCount
	local nLinePerPage = attr.nLinePerPage
	
	if nLinePerPage >= nTotalLineCount then
		objScrollBar:SetVisible(false)
		objScrollBar:SetChildrenVisible(false)
		return true
	end
		
	local nItemHeight = GetItemHeight(objRootCtrl)
	local nMaxHeight = nItemHeight * nTotalLineCount
	local nPageSize = nItemHeight * nLinePerPage
	
	objScrollBar:SetVisible(true)
	objScrollBar:SetChildrenVisible(true)
	objScrollBar:SetScrollRange( 0, nMaxHeight - nPageSize, true )
	objScrollBar:SetPageSize(nPageSize, true)	
	objScrollBar:SetScrollPos((g_nCurTopIndex-1) * nItemHeight, true )
	objScrollBar:Show(true)
	
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

function SetAppOpenLink(self, strLink)
	if not IsRealString(strLink) then
		return
	end
	
	local attr = self:GetAttribute()
	attr.strOpenLink = strLink
end

--1:浏览器打开;2:直接下载或运行
function SetAppOpenType(self, nType)
	if tonumber(nType) == nil then
		return
	end
	
	local attr = self:GetAttribute()
	attr.nOpenType = tonumber(nType)
end


function OnClickAppImage(objAppImage)
	OpenLinkAfterClick(objAppImage)
end

function OnClickAppText(objAppText)
	OpenLinkAfterClick(objAppText)
end


function OpenLinkAfterClick(objUIItem)
	local objRootCtrl = objUIItem:GetOwnerControl()
	local attr = objRootCtrl:GetAttribute()
	local nOpenType = attr.nOpenType
	local strOpenLink = attr.strOpenLink
	
	if not IsRealString(strOpenLink) then
		return
	end
	
	if nOpenType == 1 then
		tipUtil:OpenURL(strOpenLink)
	elseif nOpenType == 2 then
		OpenSoftware(strOpenLink)
	end	
end


function OpenSoftware(strOpenLink)

end


----------------------------------------------------
function IsRealString(str)
	return type(str) == "string" and str~=nil
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@GreenWall_Template ChildCtrl_App: " .. tostring(strLog))
	end
end


