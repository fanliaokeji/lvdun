local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local TipLvdunWndUserData = nil
local Helper = XLGetGlobal("Helper")

function ReportGoogle(strStat)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local tStatInfo = {}
	
	tStatInfo.strEC = "tipstat"
	tStatInfo.strEA = TipLvdunWndUserData["tipInfo"] and TipLvdunWndUserData["tipInfo"].id
	tStatInfo.strEL = tostring(strStat)
	tStatInfo.strEV = 1
	
	FunctionObj.TipConvStatistic(tStatInfo)
end

local gHostWnd = nil
local gHoldTime = 0
local gHoldTimer = nil
function EndTimer()
	Helper:LOG("EndTimer Enter")
	if gHoldTimer then
		KillTimer(gHoldTimer)
		gHoldTimer = nil
	end
end

function StartTimer()
	EndTimer()
	local nHoldMins = TipLvdunWndUserData["tipInfo"] and TipLvdunWndUserData["tipInfo"]["nHoldMins"]
	nHoldMins = tonumber(nHoldMins) or 0.5
	Helper:LOG("StartTimer Enter  nHoldMins: "..tostring(nHoldMins))
	
	gHoldTimer = SetOnceTimer(function(item, id)
					Helper:LOG("StartTimer SetOnceTimer Enter")
					ReportGoogle("autoclose")
					Helper:DestoryModelessWndEx("TipLvdunWnd", "TipCommon.Instance")
					gHoldTimer = nil
	end, 1000*60*nHoldMins)
end

function OnCreate( self )
	Helper:LOG("OnCreate Enter")
	TipLvdunWndUserData = self:GetUserData()
	PopupInDeskRight(self)
end

function OnShowWindow(self, bshow)
	Helper:LOG("OnShowWindow  bshow: ", bshow)
	local tree = self:GetBindUIObjectTree()
	if not tree then
		Helper:LOG("[TipLvdunWnd] OnShowWindow: tree is nil")
		return
	end
	local container = tree:GetUIObject("TipLvdun.GifBkg")
	
	local aLeft, aTop, aRight,aBottom = container:GetObjPos()
	local height = aBottom - aTop
	
	local function PlayGif()
		-- container:Play()
		Helper.Ani:RunSeqFrameAni(container, "DiDa.lvdun_seq_ani", nil, 2200)
	end

	if bshow then
		Helper:LOG("OnShowWindow do ani")
		StartTimer()
		Helper.Ani:RunPosChangeAni(container, aLeft, aTop + height, aRight, aBottom + height, 0, 0, aRight, aBottom, PlayGif, 400)
		ReportGoogle("show")
	else
		self:SetEnable(false)
	end
end

function OnClickCloseBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	
	-- Helper:DestoryModelessWnd("TipLvdunWnd")
	EndTimer()
	ReportGoogle("close")
	Helper:DestoryModelessWndEx("TipLvdunWnd", "TipCommon.Instance")
end

function OnClickOKBtn(self)
	if TipLvdunWndUserData then
		local callback = TipLvdunWndUserData["tip_callback_ok"]
		if callback then
			callback()
		end
	end
	
	EndTimer()
	ReportGoogle("click")
	-- Helper:DestoryModelessWnd("TipLvdunWnd")
	Helper:DestoryModelessWndEx("TipLvdunWnd", "TipCommon.Instance")
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

