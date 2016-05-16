--封装Tip方法，挂到Helper.Tip
local ObjectBase = XLGetGlobal("ObjectBase")
local Helper = XLGetGlobal("Helper")

local GeneralTip = {}

function GeneralTip:DestoryTipWnd()
	local hostwndmgr = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local TipWnd = hostwndmgr:GetHostWnd("GeneralTipWnd.Instance")
	if TipWnd then
		TipWnd:SetVisible(false)
		local tree = TipWnd:UnbindUIObjectTree()
		local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")
		if tree then
			objtreeManager:DestroyTree(tree)
		end
		if TipWnd:GetID() then
			hostwndmgr:RemoveHostWnd(TipWnd:GetID())
		end
	end
end

function GeneralTip:CreateTipWnd(owner)
	self:DestoryTipWnd()
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local tipsHostWndTemplate = templateMananger:GetTemplate("GeneralTipWnd","HostWndTemplate")
	local tipsHostWnd = tipsHostWndTemplate:CreateInstance("GeneralTipWnd.Instance")
	
	local objectTreeTemplate = templateMananger:GetTemplate("GeneralTipTree","ObjectTreeTemplate")
	local uiObjectTree = objectTreeTemplate:CreateInstance("GeneralTipTree.Instance")
	
	Helper:Assert(uiObjectTree)	
	tipsHostWnd:BindUIObjectTree(uiObjectTree)
	tipsHostWnd:Create(owner)
	tipsHostWnd:SetVisible(false)
	return tipsHostWnd,uiObjectTree
end

-- text        tips内容 如果要取消Tips tipsText设置为nil 或者 “”
-- msecDelay   延迟显示 和 延迟消失时间 默认为0
-- obj  	   使用Obj来设置Tips的位置，如果Obj为nil Tips 根据鼠标设置相对位置
-- x,y         相对于鼠标或是Obj 的位置偏移，默认为0，0
-- bUseObjPos  为true 时，使用obj的位置 弹出
-- fCallBack   tip 显示后，回调, fCallBack(tipWnd, objectTree)
-- destroy wnd when hide
-- recreate wnd when show
function GeneralTip:SetTips(text,msecDelay,obj,x,y,bUseObjPos, fCallBack)
	if text == "" then text = nil end
	
	local timermanager = XLGetObject("Xunlei.UIEngine.TimerManager")
	if self.DelayPopTimerCookie then
		timermanager:KillTimer(self.DelayPopTimerCookie)
		self.DelayPopTimerCookie = nil
	end
	-- 无text隐藏
	if not text then
		self:DestoryTipWnd(true)
		return 
	end
	-- 归一化参数
	if msecDelay == nil then msecDelay = 0 end
	if x == nil then x = 0 end
	if y == nil then y = 0 end
	
	local hostwndmgr = XLGetObject("Xunlei.UIEngine.HostWndManager")
	
	local ownerWnd = nil
	if obj and obj:GetOwner() then
		ownerWnd = obj:GetOwner():GetBindHostWnd() 
	end
	
	if msecDelay == 0 then
		self:Show(text, obj, x, y, bUseObjPos, ownerWnd, fCallBack)
	else
		self.DelayPopTimerCookie = timermanager:SetOnceTimer(function () 
			self.DelayPopTimerCookie = nil
			self:Show(text, obj, x, y, bUseObjPos, ownerWnd, fCallBack)
		end,msecDelay)
	end
end

function GeneralTip:CreateTipContent(text)
	local hostwndmgr = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostWnd = hostwndmgr:GetHostWnd("GeneralTipWnd.Instance")
	
	if not hostWnd then
		return 
	end
	local tipsTree = hostWnd:GetBindUIObjectTree()
	local bkg = tipsTree:GetUIObject("tip.bkg")
	
	-- 先清掉上次的内容
	local count = bkg:GetChildCount()
	for i = count-1, 0, -1 do
		local child = bkg:GetChildByIndex(i)
		bkg:RemoveChild(child)
	end
	
	local tipTextArr = {}
	while true do
		local pos = string.find(text, "\r\n")
		if type(pos) == "number" then
			local t = string.sub(text, 0, pos - 1)
			table.insert(tipTextArr, t)
			text = string.sub(text, pos + 2, -1)
		else
			table.insert(tipTextArr, text)
			break
		end
	end
	local templateManager = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local tipTextTmpl = templateManager:GetTemplate("tmpl.tip.text", "ObjectTemplate")
	for i = 1, #tipTextArr do
		local tipTextObj = tipTextTmpl:CreateInstance("")
		tipTextObj:SetText(tipTextArr[i])
		bkg:AddChild(tipTextObj)
	end
end

function GeneralTip:Show(text,obj,offSetX,offSetY,bUseObjPos,ownerWnd, fCallBack)
	local mouseX, mouseY = Helper.tipUtil:GetCursorPos()
	
	if obj then
		-- 鼠标在对象外面，不显示
		local abs_left, abs_top, abs_right, abs_bottom = obj:GetAbsPos()
		local wnd_left, wnd_top, wnd_right, wnd_bottom = obj:GetOwner():GetBindHostWnd():GetWindowRect()
		local left, top, right, bottom = wnd_left + abs_left, wnd_top + abs_top, wnd_left + abs_right, wnd_top + abs_bottom
		if mouseX > left and mouseX < right and mouseY > top and mouseY < bottom then
		else
			self:DestoryTipWnd()
			return
		end
	end
	
	local hostwnd,objectTree = self:CreateTipWnd(ownerWnd)
	
	self:CreateTipContent(text)
	
	local bkg = objectTree:GetUIObject("tip.bkg")
	bkg = objectTree:GetUIObject("tip.bkg")
	-- 这个地方有报过错，加一句断言
	
	if not bkg then
		Helper:LOG("GeneralTip:Show bkg is nil")
		return
	end
	bkg:SetTextureID("texture.general.tip.bkg")
	
	
	-- text 的位置
	local textMarginH,textMarginV = 5, 0           -- 和背景的边距
	local textWidth, textHeight = 0, textMarginV   -- 初始化所有textobj 的宽高
	local textSpace = 5                            -- textobj 之间的间隔
	
	local count = bkg:GetChildCount()
	for i = 0,count - 1 do
		local tipTextObj = bkg:GetChildByIndex(i)
		local w, h = tipTextObj:GetTextExtent()
		if w > textWidth then
			textWidth = w
		end
		tipTextObj:SetObjPos(textMarginH, textHeight, textWidth + textMarginH, textHeight + h + textSpace)
		textHeight = textHeight + h + textSpace
	end
	
	-- 背景位置
	bkg:SetObjPos(0, 0, textWidth + textMarginH*2, textHeight + textMarginV)
	
	-- 窗体位置
	local tipLeft, tipTop
	local tipWidth = textWidth + textMarginH*2
	local tipHeight = textHeight + textMarginV
	if obj ~= nil and obj:GetOwner() and bUseObjPos then
		local tree = obj:GetOwner()
		local objWnd = tree:GetBindHostWnd()
		local hostWndleft,hostWndtop = objWnd:GetWindowRect()
		local objleft,objtop,objright,objbottom = obj:GetAbsPos()
		tipLeft = hostWndleft + objleft + offSetX
		tipTop = hostWndtop + objtop + offSetY
	else
		tipLeft,tipTop = mouseX + 2,mouseY + 21
	end
	
	-- 不超出工作区域
	sleft,stop,sright,sbottom = Helper.tipUtil:GetCurrentScreenRect(mouseX, mouseY)
	if sright < tipLeft + tipWidth then
		tipLeft = sright - tipWidth
		tipTop = tipTop - 10
	end
	if sbottom < tipTop + tipHeight then
		tipTop = sbottom - tipHeight
	end
	
	if tipLeft < sleft then
		tipLeft = sleft
	end
	
	if tipTop < stop then
		tipTop = stop
	end

	hostwnd:Move(tipLeft,tipTop, tipWidth, tipHeight)
	hostwnd:Show(4)
	
	if type(fCallBack) == "function" then
		fCallBack(hostwnd,objectTree)
	end
	
	-- __Animation.RunAlphaAniEx_Control(bkg, 0, 255, function ()
		bkg:SetVisible(true)
		bkg:SetChildrenVisible(true)
	-- end, 200)
	
end

Helper.Tip = GeneralTip

