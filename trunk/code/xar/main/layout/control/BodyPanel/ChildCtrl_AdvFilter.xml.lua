local tipUtil = XLGetObject("GS.Util")
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")

local g_bHasLoadCfg = false
local g_selfRootCtrl = nil
local g_bHasInit = false
local g_bActionAdd = true -- true 表示添加, false删除
local g_nPageSize = 4

local g_tObjLineList = {}
local g_nCurTopIndex = 1
local g_nCurrentLineIndex = 0
local g_nLineCtrlHeight = 27
local g_nLayoutGap = 0
local g_nIDToDelete = 0

local g_tBlackList = {}

local g_nReportELEnable = 1
local g_nReportELDisable = 2
local g_nReportELAdd = 3
local g_nReportELDelete = 4


function OnShowPanel(self, bShow)
	if not g_bHasInit then
		InitAdvFilter(self)
	end
	
	if bShow then
		ShowPopupPanel(self, false)
	end
end

--主面板添加删除按钮
function OnClickActionBtn(self)
	local objRootCtrl = self:GetOwnerControl()

	if g_bActionAdd then  --添加
		ShowPopupPanel(objRootCtrl, true)
	else
		if g_nIDToDelete == 0 then
			return 
		end
		
		ReportDomainStateByID(g_nIDToDelete, g_nReportELDelete)
		
		RemoveBlackList(g_nIDToDelete)
		UpdateBlackListPanel()
		ResetScrollBar(objRootCtrl)
		g_nIDToDelete = 0
		SetAddBtnStyle(self, true)		
	end
end


function OnClickCancleBtn(self)
	local objRootCtrl = self:GetOwnerControl()
	ShowPopupPanel(objRootCtrl, false)
end

--弹出面版添加
function OnClickAddBtn(self)
	local objRootCtrl = self:GetOwnerControl()
	local objEditName = objRootCtrl:GetControlObject("ChildCtrl_AdvFilter.PopupPanel.Name.Input")
	local objEditURL = objRootCtrl:GetControlObject("ChildCtrl_AdvFilter.PopupPanel.URL.Input")
	if nil == objEditName or nil == objEditURL then
		ShowPopupPanel(objRootCtrl, false)
		return
	end
	
	local strName = objEditName:GetText()
	local strDomain = objEditURL:GetText()
	if not IsRealString(strName) or not IsRealString(strDomain) then
		ShowPopupPanel(objRootCtrl, false)
		return
	end
	
	PushBlackList(strName, strDomain, true)
	ReportDomainState(strDomain, g_nReportELAdd)
	
	UpdateBlackListPanel()
	ResetScrollBar(objRootCtrl)
	ShowPopupPanel(objRootCtrl, false)	
end


function OnPopupPanelKeyDown(self, nKeyCode)
	if nKeyCode ~= 13 then --处理回车
		return
	end

	local objRootCtrl = self:GetOwnerControl()
	local objPopupAddBtn = objRootCtrl:GetControlObject("ChildCtrl_AdvFilter.PopupPanel.AddBtn")
	if not objPopupAddBtn then
		return
	end
	
	OnClickAddBtn(objPopupAddBtn)	
end

-------blacklist 事件----
function EventRouteToFather(self)
	self:RouteToFather()
end

function OnClick_StateButton(self)
	local objFather = self:GetParent()
	local nUrlID = GetUrlIDByControl(objFather)

	if IsNilString(nUrlID) then
		return
	end

	local bState = GetStateByID(nUrlID)
	local bNewState = not bState
	
	SetStateButtonStyle(self, bNewState)	
	SetStateByID(nUrlID, bNewState)
	SetFilterLayoutStyle(objFather, true)
	
	local strDomain = FetchValueByPath(g_tBlackList, {nUrlID, "strDomain"})
	AutoEnableDomain(strDomain)
	SaveConfigToFile()
	
	if bNewState then
		ReportDomainState(strDomain, g_nReportELEnable)
	else
		ReportDomainState(strDomain, g_nReportELDisable)
	end
end


--切换删除\添加
function OnClick_Layout(self)
	local objRootCtrl = self:GetOwnerControl()
	local objAddBtn = objRootCtrl:GetControlObject("ChildCtrl_AdvFilter.MainWnd.ActionBtn")
	if nil == objAddBtn then
		return
	end
	
	local nID = GetUrlIDByControl(self)
	g_nIDToDelete = nID
	
	local bAdd = false
	SetAddBtnStyle(objAddBtn, bAdd)
	
	local bHover = true
	SetFilterLayoutStyle(self, bHover)
	self:SetZorder(2000)
end

function OnFocus_Layout(self, bFocus)
	if not bFocus then
		local objRootCtrl = self:GetOwnerControl()
		local objTree = self:GetOwner()
		local objHostWnd = objTree:GetBindHostWnd()
		local mouseX, mouseY = tipUtil:GetCursorPos()
		local nWndX, nWndY = objHostWnd:ScreenPtToHostWndPt(mouseX, mouseY)
		local nTreeX, nTreeY = objHostWnd:HostWndPtToTreePt(nWndX, nWndY)

		local objActionBtn = objRootCtrl:GetControlObject("ChildCtrl_AdvFilter.MainWnd.ActionBtn")
		if nil == objActionBtn then
			return
		end
		local nLeft, nTop, nRight, nBottom = objActionBtn:GetAbsPos()

		if nTreeX > nLeft and nTreeX < nRight and nTreeY < nBottom and nTreeY > nTop then
			--点击删除按钮时先不改变状态
			return
		end
		
		local bAdd = true
		SetAddBtnStyle(objActionBtn, bAdd)
		
		local bHover = false
		SetFilterLayoutStyle(self, bHover)
		self:SetZorder(0)
		g_nIDToDelete = 0
	end
end


----滚动条----
function CLB__OnVScroll(self, fun, type_, pos)
	local nScrollPos = self:GetScrollPos()
    local nItemHeight = g_nLineCtrlHeight + g_nLayoutGap
		
	--点击向上按钮或上方空白
    if type_==1 then
        self:SetScrollPos( nScrollPos - nItemHeight, true )
		
		g_nCurTopIndex = g_nCurTopIndex - 1
		if g_nCurTopIndex < 1 then
			g_nCurTopIndex = 1
		end
	end
	
	--点击向下按钮或下方空白
    if type_==2 then
		self:SetScrollPos( nScrollPos + nItemHeight, true )
		
		g_nCurTopIndex = g_nCurTopIndex + 1
		if g_nCurTopIndex > #g_tBlackList - g_nPageSize + 1 then
			g_nCurTopIndex = #g_tBlackList - g_nPageSize + 1
		end		
    end

	UpdateBlackListPanel()
	return true
end


function CLB__OnScrollBarMouseWheel(self, name, x, y, distance)
	local pos = self:GetScrollPos()
    local nItemHeight = g_nLineCtrlHeight + g_nLayoutGap
		
    if distance > 0 then
		g_nCurTopIndex = g_nCurTopIndex - 1
		if g_nCurTopIndex < 1 then
			g_nCurTopIndex = 1
		end

		self:SetScrollPos( pos - nItemHeight, true )
    else		
		g_nCurTopIndex = g_nCurTopIndex + 1
		if g_nCurTopIndex > #g_tBlackList - g_nPageSize + 1 then
			g_nCurTopIndex = #g_tBlackList - g_nPageSize + 1
			return
		end
		
		self:SetScrollPos( pos + nItemHeight, true )
    end
    
	if pos == 0 and g_nCurTopIndex == 1 then
		return
	end

	UpdateBlackListPanel()
	return true	
end

function CLB__OnScrollMousePosEvent(self, name, x, y)
	local nScrollPos = self:GetScrollPos()
	local nItemHeight = g_nLineCtrlHeight + g_nLayoutGap 
	local nLineIndex = math.floor(nScrollPos/nItemHeight) + 1
	
	if g_nCurTopIndex ~= nLineIndex then
		g_nCurTopIndex = nLineIndex
		
		if g_nCurTopIndex < 1 then
			g_nCurTopIndex = 1
		end
		
		if g_nCurTopIndex > #g_tBlackList - g_nPageSize + 1 then
			g_nCurTopIndex = #g_tBlackList - g_nPageSize + 1
		end	

		UpdateBlackListPanel()
	end
end

--------------


--------------------------
function OnDestroy(self)

end


--监听后的弹出窗口
function OnClickPopupCancle(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	local objRootLayout = objTree:GetUIObject("root.layout")
	if objRootLayout == nil then
		TipLog("[OnClickPopupCancle] get objRootLayout failed")
		return
	end
	
	local objTextName = objRootLayout:GetObject("FilterRemind.Record.Name")
	local objTextDomain = objRootLayout:GetObject("FilterRemind.Record.Domain")
	if objTextName == nil or objTextDomain == nil then
		TipLog("[OnClickPopupCancle] get objTextName failed")
		return
	end  
	
	local strName = objTextName:GetText()
	local strDomain = objTextDomain:GetText()

	TipLog("[OnClickPopupCancle] user cancle video filter")
	tFunctionHelper.EnableVideoDomain(strDomain, 2)
	objHostWnd:Show(0)
	ReportPopupState(strDomain, 2)
end

function OnClickPopupEnter(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	local objRootLayout = objTree:GetUIObject("root.layout")
	if objRootLayout == nil then
		TipLog("[OnClickPopupCancle] get objRootLayout failed")
		return
	end
	
	local objTextName = objRootLayout:GetObject("FilterRemind.Record.Name")
	local objTextDomain = objRootLayout:GetObject("FilterRemind.Record.Domain")
	if objTextName == nil or objTextDomain == nil then
		TipLog("[OnClickPopupCancle] get objTextName failed")
		return
	end  
	
	local strName = objTextName:GetText() or ""
	local strDomain = objTextDomain:GetText()
	
	if IsRealString(strDomain) then
		AddBlackList(strName, strDomain)
		ReportPopupState(strDomain, 1)
	end
	
	objHostWnd:Show(0)
end

--nState: 1 免费过滤， 2 取消
function ReportPopupState(strDomain, nState)
	local tStatInfo = {}
	tStatInfo.strEA = strDomain
	tStatInfo.strEL = tostring(nState)
	tStatInfo.strEC = "FilterRemindWnd"
	
	SendReport(tStatInfo)
end


function AddBlackList(strName, strDomain)
	LoadConfig()
	PushBlackList(strName, strDomain, true)
		
	if g_bHasInit then
		UpdateBlackListPanel()
		ResetScrollBar(g_selfRootCtrl)
	end
end

----------------------------------------
function InitAdvFilter(self)
	local bSucc = LoadConfig()
	if not bSucc then
		return false
	end
	
	local objBlackList = self:GetControlObject("ChildCtrl_AdvFilter.MainWnd.List.Container")
	if nil == objBlackList then
		return false
	end 
	
	for i=1,g_nPageSize do
		g_tObjLineList[#g_tObjLineList + 1] = CreateLine(objBlackList)
	end

	UpdateBlackListPanel()
	ResetScrollBar(self)
	
	g_selfRootCtrl = self
	g_bHasInit = true
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
	
	if g_nPageSize >= #g_tBlackList then
		objScrollBar:SetVisible(false)
		objScrollBar:SetChildrenVisible(false)
		return true
	end
	
	local nItemHeight = g_nLineCtrlHeight
	local nMaxHeight = nItemHeight * #g_tBlackList
	local nPageSize = nItemHeight * g_nPageSize
	
	objScrollBar:SetVisible(true)
	objScrollBar:SetChildrenVisible(true)
	objScrollBar:SetScrollRange( 0, nMaxHeight - nPageSize, true )
	objScrollBar:SetPageSize(nPageSize, true)	
	objScrollBar:SetScrollPos((g_nCurTopIndex-1) * nItemHeight, true )
	objScrollBar:Show(true)
	
	return true
end


function LoadConfig()
	if g_bHasLoadCfg then
		return true
	end
	
	---黑名单
	g_tBlackList = tFunctionHelper.GetSpecifyFilterTableFromMem("tBlackList") or {}
	
	g_bHasLoadCfg = true
	return true
end


function GetUrlIDByControl(objBlackListElem)
	local objTextID = objBlackListElem:GetChildByIndex(3)
	if objTextID == nil then
		return nil
	end
	
	local strID = objTextID:GetText()
	return tonumber(strID)
end

function GetStateByID(nUrlID)
	if type(g_tBlackList[nUrlID]) ~= "table" then
		return nil
	end

	return g_tBlackList[nUrlID]["bState"]
end

function SetStateByID(nUrlID, bNewState)
	if type(bNewState) ~= "boolean" or type(g_tBlackList[nUrlID]) ~= "table" then
		return
	end
	
	g_tBlackList[nUrlID]["bState"] = bNewState
end


function UpdateBlackListPanel()
	if g_nCurTopIndex + g_nPageSize > #g_tBlackList+1 then
		g_nCurTopIndex = #g_tBlackList - g_nPageSize + 1
	end
	
	if #g_tBlackList < g_nPageSize+1 or g_nCurTopIndex < 1 then
		g_nCurTopIndex = 1
	end
	
	local nCurMovieIndex = g_nCurTopIndex
	
	for nIndex, objLine in ipairs(g_tObjLineList) do
		objLine:SetVisible(false)
		objLine:SetChildrenVisible(false)
		
		local tBlackListItem = FetchValueByPath(g_tBlackList, {nCurMovieIndex})
		if type(tBlackListItem) == "table" then
			local tItemWithID = tBlackListItem
			tItemWithID["nID"] = nCurMovieIndex
			ShowFilterInfo(tItemWithID, nIndex)
			tItemWithID["nID"] = nil
		end	
		nCurMovieIndex = nCurMovieIndex + 1
	end	
end


function ShowFilterInfo(tBlackItemWithID, nIndex)
	local objLayout = g_tObjLineList[nIndex]
	local objTextName = objLayout:GetChildByIndex(0)
	local objTextURL = objLayout:GetChildByIndex(1)
	local objStateBtn = objLayout:GetChildByIndex(2)
	local objTextID = objLayout:GetChildByIndex(3)
	
	if objTextName == nil or objTextURL == nil 
		or objStateBtn == nil or objTextID == nil then
		return false
	end
	
	local strName = tBlackItemWithID["strName"]
	local strDomain = tBlackItemWithID["strDomain"]
	objTextName:SetText(strName)
	objTextURL:SetText(strDomain)
	
	local nID = tBlackItemWithID["nID"]
	objTextID:SetText(tostring(nID))
	
	local bState = GetStateByID(nID)
	SetStateButtonStyle(objStateBtn, bState)
	
	local bHover = false
	SetFilterLayoutStyle(objLayout, bHover)
	
	objLayout:SetFocus(false)
	objLayout:SetVisible(true)
	objLayout:SetChildrenVisible(true)
end


function CreateLine(objBlackList)
	local objFather = objBlackList
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	
	local objLayout = objFactory:CreateUIObject("", "TextureObject")
	local objTextName = objFactory:CreateUIObject("", "TextObject")
	local objTextURL = objFactory:CreateUIObject("", "TextObject")
	local objStateBtn = objFactory:CreateUIObject("", "TextureObject")
	local objTextID = objFactory:CreateUIObject("", "TextObject")
	
	if nil == objLayout or nil == objTextName 
		or nil == objTextURL or nil == objStateBtn then
		return nil
	end

	objFather:AddChild(objLayout)
	local nFatherLeft, nFatherTop, nFatherRight, nFatherBottom = objFather:GetObjPos()
	local nObjTop = g_nCurrentLineIndex * (g_nLineCtrlHeight - 1) - 1
	local nLayoutWidth = nFatherRight - nFatherLeft
	objLayout:SetObjPos(0, nObjTop, 0+nLayoutWidth, nObjTop + g_nLineCtrlHeight)
	objLayout:SetTextureID("Greenwall.AdvFilter.ElemBkg.Normal")
	objLayout:AddChild(objTextName)
	objLayout:AddChild(objTextURL)
	objLayout:AddChild(objStateBtn)
	objLayout:AddChild(objTextID)
	
	objLayout:SetCursorID("IDC_HAND")
	objLayout:SetFocus(false)
	
	objTextName:SetObjPos(8, 0, 8+68-15, "father.height")	
	objTextName:SetTextColorResID("4D4D4D")
	objTextName:SetTextFontResID("font.yahei11")
	objTextName:SetVAlign("center")
	objTextName:SetHAlign("left")
	
	objTextURL:SetObjPos(8+68-5, 0, 8+68-15+157, "father.height")	
	objTextURL:SetTextColorResID("4D4D4D")
	objTextURL:SetTextFontResID("font.yahei11")
	objTextURL:SetVAlign("center")
	objTextURL:SetHAlign("left")
	
	objStateBtn:SetObjPos("father.width-8-54", 3, "father.width-8", 20+3)
	objStateBtn:SetTextureID("GreenWall.Common.SwitchButton.Open")
	objStateBtn:SetCursorID("IDC_HAND")
	
	objStateBtn:AttachListener("OnLButtonUp", false, OnClick_StateButton)
	objStateBtn:AttachListener("OnMouseWheel", false, EventRouteToFather)
	objStateBtn:AttachListener("OnFocusChange", false, EventRouteToFather)

	objLayout:AttachListener("OnLButtonUp", false, OnClick_Layout)
	objLayout:AttachListener("OnMouseWheel", false, EventRouteToFather)
	objLayout:AttachListener("OnFocusChange", false, OnFocus_Layout)
	
	g_nCurrentLineIndex = g_nCurrentLineIndex + 1
	objLayout:SetVisible(false)
	objLayout:SetChildrenVisible(false)
	
	return objLayout
end

function SetStateButtonStyle(objStateButton, bState)
	if bState then
		objStateButton:SetTextureID("GreenWall.Common.SwitchButton.Open")
	else
		objStateButton:SetTextureID("GreenWall.Common.SwitchButton.Close")
	end
end

function SetFilterLayoutStyle(objLayout, bHover)
	local nUrlID = GetUrlIDByControl(objLayout)
	if IsNilString(nUrlID) then
		return
	end

	local bState = GetStateByID(nUrlID)

	if bState then
		if bHover then
			objLayout:SetTextureID("Greenwall.AdvFilter.ElemBkg.Hover")
		else
			objLayout:SetTextureID("Greenwall.AdvFilter.ElemBkg.Normal")	
		end
	else
		if bHover then
			objLayout:SetTextureID("Greenwall.AdvFilter.ElemBkg.Disable.Hover")
		else
			objLayout:SetTextureID("Greenwall.AdvFilter.ElemBkg.Disable.Normal")	
		end	
	end
end


function SetAddBtnStyle(objAddBtn, bAdd)
	if bAdd then
		objAddBtn:SetText("添加")
		objAddBtn:SetTextColor("4D4D4D")
	else
		objAddBtn:SetText("删除")
		objAddBtn:SetTextColor("system.red")
	end
	
	g_bActionAdd = bAdd
end

function ShowPopupPanel(objRootCtrl, bShowPopup)
	local objPopUpPanel = objRootCtrl:GetControlObject("ChildCtrl_AdvFilter.PopupPanel.Bkg")
	if objPopUpPanel == nil then
		return
	end
	
	local objMainPanel = objRootCtrl:GetControlObject("ChildCtrl_AdvFilter.MainPanel")
	if objMainPanel == nil then
		return
	end
	
	local objEditName = objRootCtrl:GetControlObject("ChildCtrl_AdvFilter.PopupPanel.Name.Input")
	local objEditURL = objRootCtrl:GetControlObject("ChildCtrl_AdvFilter.PopupPanel.URL.Input")
	if nil ~= objEditName and nil ~= objEditURL then
		objEditName:SetText("")
		objEditURL:SetText("")
	end
	
	objPopUpPanel:SetVisible(bShowPopup)
	objPopUpPanel:SetChildrenVisible(bShowPopup)
	
	local bShowMain = not bShowPopup	
	objMainPanel:SetVisible(bShowMain)
	objMainPanel:SetChildrenVisible(bShowMain)
	
	objEditName:SetFocus(true)
end


function GetIDByDomain(strDomain)
	if not IsRealString(strDomain) then
		return nil
	end

	for nID, tBlackElem in ipairs(g_tBlackList) do
		if type(tBlackElem) == "table" then
			local strBlackDomain = tBlackElem["strDomain"] or ""
			if strDomain == strBlackDomain then
				return nID
			end		
		end	
	end
	
	return nil
end


function PushBlackList(strName, strDomain, bState)
	if not IsRealString(strDomain) then
		return
	end

	if type(bState) ~= "boolean" then
		bState = true
	end
	
	local nTopIndex = #g_tBlackList+1
	local nIndex = GetIDByDomain(strDomain) or nTopIndex
	
	g_tBlackList[nIndex] = {}
	g_tBlackList[nIndex]["strName"] = strName or ""
	g_tBlackList[nIndex]["strDomain"] = strDomain
	g_tBlackList[nIndex]["bState"] = bState
	
	AutoEnableDomain(strDomain)
	SaveConfigToFile()
end


function RemoveBlackList(nIDToDelete)
	if type(g_tBlackList[nIDToDelete]) == "table" then
		local strDomain = FetchValueByPath(g_tBlackList, {nIDToDelete, "strDomain"})
		local bIsVideo = tFunctionHelper.IsVideoDomain(strDomain)
		table.remove(g_tBlackList, nIDToDelete)
		
		AutoEnableDomain(strDomain, true, bIsVideo)
		SaveConfigToFile()
	end	
end


function AutoEnableDomain(strDomain, bDelete, bIsVideo)
	if not IsRealString(strDomain) then
		return
	end
	
	--删除时，不是视频则设置为过滤
	if bDelete then
		if bIsVideo then
			tFunctionHelper.EnableVideoDomain(strDomain, 0)
		else
			tFunctionHelper.EnableVideoDomain(strDomain, 1)
		end
		return
	end
	
	local nState = tFunctionHelper.GetVideoDomainState(strDomain)
	if nState == 1 then
		tFunctionHelper.EnableVideoDomain(strDomain, 1)
	elseif nState == 2 then
		tFunctionHelper.EnableVideoDomain(strDomain, 2)
	end
end


function SaveConfigToFile()
	local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
	FunctionObj.SaveFilterConfigToFile()
end


function ReportDomainStateByID(nDomianID, nEL)
	local strDomain = ""
	if type(g_tBlackList[nDomianID]) == "table" then
		strDomain = FetchValueByPath(g_tBlackList, {nDomianID, "strDomain"})
	end
	
	ReportDomainState(strDomain, nEL)
end

function ReportDomainState(strDomain, nEL)
	local tStatInfo = {}
	tStatInfo.strEA = strDomain
	tStatInfo.strEL = tostring(nEL)
	tStatInfo.strEC = "AdvFilterPanel"
	
	SendReport(tStatInfo)
end

function SendReport(tStatInfo)
	local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
	if type(FunctionObj.TipConvStatistic) == "function" then
		FunctionObj.TipConvStatistic(tStatInfo)
	end
end


function IsRealString(str)
	return type(str) == "string" and str ~= ""	
end

function IsNilString(str)
	return str == "" or str == nil	
end

function FetchValueByPath(obj, path)
	local cursor = obj
	for i = 1, #path do
		cursor = cursor[path[i]]
		if cursor == nil then
			return nil
		end
	end
	return cursor
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@ChildCtrl_AdvFilter: " .. tostring(strLog))
	end
end