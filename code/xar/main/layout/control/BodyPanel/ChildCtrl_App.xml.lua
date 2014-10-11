local tipUtil = XLGetObject("GS.Util")
local g_bHasInit = false
local g_nCountPerLine = 4


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

	local objAppContainer = self:GetControlObject("ChildCtrl_App.MainWnd.Container")
	if not objAppContainer then
		TipLog("[InitAppCtrl] get objAppContainer failed")
		return
	end
	
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
			objAppItemCtrl:SetAppName(strAppName)
			objAppItemCtrl:SetAppImageByPath(strImagePath)
			objAppItemCtrl:SetAppOpenType(nOpenType)
			objAppItemCtrl:SetAppOpenLink(strOpenLink)
		
			objAppContainer:AddChild(objAppItemCtrl)
			SetAppItemPos(objAppItemCtrl, nIndex)
		end
	end
	
	g_bHasInit = true
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
	local nIndexWithFix = nIndex - 1
	local nLine = math.floor(nIndexWithFix/g_nCountPerLine) + 1
	local nColum = math.mod(nIndexWithFix, g_nCountPerLine) + 1
	
	local attr = objAppItemCtrl:GetAttribute()
	local nItemHeight = attr.nItemHeight
	local nItemWidth = attr.nItemWidth
	local nItemSpaceH = attr.nItemSpaceH
	local nItemSpaceW = attr.nItemSpaceW
	
	local nItemLeft = (nColum-1)*(nItemWidth+nItemSpaceW)
	local nItemTop = (nLine-1)*(nItemHeight+nItemSpaceH)
	
	objAppItemCtrl:SetObjPos(nItemLeft, nItemTop, nItemLeft+nItemWidth, nItemTop+nItemHeight)
end





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
	local objRootCtrl = objAppImage:GetOwnerControl()
	local attr = objRootCtrl:GetAttribute()
end



function IsRealString(str)
	return type(str) == "string" and str~=nil
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@GreenWall_Template ChildCtrl_App: " .. tostring(strLog))
	end
end


