local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local TipLvdunWndUserData = nil
local Helper = XLGetGlobal("Helper")
function OnCreate( self )
	TipLvdunWndUserData = self:GetUserData()
	PopupInDeskRight(self)
end

function OnShowWindow(self, bshow)
	Helper:LOG("OnShowWindow  bshow: ", bshow)
	local tree = self:GetBindUIObjectTree()
	if not tree then
		XMP.LOG("[AlienTipsStyle] OnShowWindow: tree is nil")
		return
	end
	local container = tree:GetUIObject("TipLvdun.GifBkg")
	
	local aLeft, aTop, aRight,aBottom = container:GetObjPos()
	local height = aBottom - aTop
	
	local function PlayGif()
		local objGif = tree:GetUIObject("TipLvdun.GifBkg")
		if objGif then
			objGif:Play()
		end
	end

	if bshow then
		Helper.Ani:RunPosChangeAni(container, aLeft, aTop - height, aRight, aBottom - height, 0, 0, aRight, aBottom, PlayGif, 400)
	else
		self:SetEnable(false)
		self:Destroy()
	end
end

function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	
	Helper:DestoryModelessWnd("TipLvdunWnd")
end

function OnClickOKBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
end

function PopupInDeskRight(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
    local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move( workright - nLayoutWidth, workbottom - nLayoutHeight, nLayoutWidth, nLayoutHeight)
	return true
end

