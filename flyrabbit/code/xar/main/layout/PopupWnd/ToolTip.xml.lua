function OnInitControl(self)
	local objText = self:GetControlObject("ToolTipText")
	objText:SetMultilineTextLimitWidth(390)
end


function SetToolTipText(self, strText)
	local objText = self:GetControlObject("ToolTipText")
	objText:SetText(strText)	
	
	AdjustTextPos(self)
end


function GetToolTipWidth(self)
	local objRootCtrl = self
	local nRootLeft, nRootTop, nRootRight, nRootBottom = objRootCtrl:GetObjPos()
	return nRootRight - nRootLeft
end


function AdjustTextPos(self)
	local objRootCtrl = self
	local objText = objRootCtrl:GetControlObject("ToolTipText")
	local nSuitWidth, nSuitHeight = objText:GetTextExtent()

	local nRootLeft, nRootTop, nRootRight, nRootBottom = objRootCtrl:GetObjPos()
	local nRootWidth = nRootRight - nRootLeft
	local nRootHeight = nRootBottom - nRootTop
	local nShadowW = 15
	local nShadowH = 10
	
	objRootCtrl:SetObjPos(nRootLeft, nRootTop, nRootLeft + nSuitWidth+nShadowW, nRootTop+nSuitHeight+nShadowH)
end


function OnMouseEnter_Root(self)
	local objTree = self:GetOwner()
	local objMainWnd = objTree:GetBindHostWnd()
	objMainWnd:SetVisible(false)
end
