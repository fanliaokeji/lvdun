local CBState ={Normal = 1,Hover = 2,Disable = 3}
function RouteToFather_OnChar(self, char)
    if char == 9 or char == 13 or char == 27 then
        self:RouteToFather()
    end
end

function RouteToFather_OnKeyDown( self, char )
	if char == 0x0D then	-- Enter键
		self:RouteToFather()
	end
end


function CB_GetListCtr(self)
	return self:GetControlObject("listctrl")
end

function CB_SetState(self, state)
    local attr = self:GetAttribute()
    if attr.NowState ~= state then
        local bkg = self:GetControlObject("Board")
        attr.NowState = state
        if attr.NowState == CBState.Normal then
			attr.HeaderNormalBkgID = (not attr.HeaderNormalBkgID and "" or attr.HeaderNormalBkgID)
            bkg:SetTextureID(attr.HeaderNormalBkgID)
        elseif attr.NowState == CBState.Hover then
			attr.HeaderHoverBkgID = (not attr.HeaderHoverBkgID and "" or attr.HeaderHoverBkgID)
            bkg:SetTextureID(attr.HeaderHoverBkgID)
        elseif attr.NowState == CBState.Disable then
			attr.HeaderDisableBkgID = (not attr.HeaderDisableBkgID and "" or attr.HeaderDisableBkgID)
            bkg:SetTextureID(attr.HeaderDisableBkgID)
        end
    end
end

function CB_Edit_OnKeyDown(self, char)
	if char==0x0D then	-- Enter
		local combobox = self:GetOwnerControl()
		local cur_id = combobox:GetCurSel()
		if combobox:IsListExpand() and cur_id then
			CB_Item_OnClick(combobox,cur_id)
		else
			local edit = combobox:GetControlObject( "combo.edit" )
			combobox:FireExtEvent("OnEnterContent", edit:GetText())
		end
	elseif char==0x1B then	-- Esc键
	
	elseif char==0x26 or char==0x28 then -- 上、下箭头键
		local combobox = self:GetOwnerControl()
		local item_num = combobox:GetCount()
		if item_num == 0 then return end
		
		local bExpand = combobox:IsListExpand()
		local attr = combobox:GetAttribute()
		local cur_id = combobox:GetCurSel()
		
		if not bExpand and attr.EnableEdit then
			combobox:ExpandList(true)
		end
		if not cur_id then
			CB_SetItemActive(combobox, 1)
			return
		end
		if char==0x26 and cur_id~=1 then
			CB_SetItemActive(combobox,cur_id-1)
		elseif char==0x28 and cur_id~=item_num then
			CB_SetItemActive(combobox,cur_id+1)
		end
	end
end

function CB_ChildObject_OnFocusChange(self, focus, dest)
	local control = self:GetOwnerControl()
	--XMP.LOG("OpenURL:"..(focus and "1" or "0"))
	
	CB_OnFocusChange(control,focus,dest)
end

function CB_OnFocusChange(self, focus, dest)
	
	local attr = self:GetAttribute()
	if focus or (not attr.Enable) then
		if focus then 
			attr.Focus = focus 
			local texthintobj = self:GetControlObject("text.hint")
			texthintobj:SetVisible(false)
		end	
	else
		if dest then
			local dest_parent = dest:GetOwnerControl()
			local test1 = self:IsChild(dest)
			local test2 = self:IsChild(dest_parent)
			local test3 = (self == dest)
			if test1 or test2 or test3 then
				
				return 
			end
		end
		attr.Focus = focus
		if self:IsListExpand() then
			self:ExpandList(false)
		end
		if attr.EditTextHint then
			local texthintobj = self:GetControlObject("text.hint")
			local edit = self:GetControlObject("combo.edit")
			local text = edit:GetText()
			if not text or "" == text then
				texthintobj:SetVisible(true)
			end
		end
	end
	self:FireExtEvent("CBFocusChange",focus,dest)
	
end

function CB_SetEditFocus(self, focus)
    local edit = self:GetControlObject("combo.edit")
	edit:SetFocus(focus)
end

function CB_SetItemActive(self, id)
	local listctr = CB_GetListCtr(self)
	listctr:SetItemActive(id)
end

function CB_SelectItem(self, id)
	local attr = self:GetAttribute()
	local listctr = CB_GetListCtr(self)
	attr.selectItemID = listctr:SelectItem(id)
end
function CB_GetSelectItem(self)
	local attr = self:GetAttribute()
	return attr.selectItemID
end

function CB_Edit_OnMouseWheel(self, x, y, distance)
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	if owner:IsListExpand() then
		local listctr = CB_GetListCtr(owner)
		listctr:MouseWheel(distance)
	else
		if not attr.EnableEdit then
			
		end
	end
end

function CB_Undo(self)
    local edit = self:GetControlObject("combo.edit")
    edit:Undo()
end

function CB_GetText(self)
    local edit = self:GetControlObject("combo.edit")
    return edit:GetText()
end

function CB_SetText(self, text)
    local edit = self:GetControlObject("combo.edit")
    edit:SetText(text)
	
	if not text or "" == text then
		local attr = self:GetAttribute()
		if attr.EditTextHint and not attr.Focus then
			self:GetControlObject("text.hint"):SetVisible(true)
		end
	else
		self:GetControlObject("text.hint"):SetVisible(false)
	end
end

function CB_GetCount(self)
	local listctr = CB_GetListCtr(self)
	return listctr:GetItemCount()
end

function CB_GetCurSel(self)
	local listCtr = CB_GetListCtr(self)
	return listCtr:GetSelectedItem()
end

function CB_SetCurSel(self, itemid)
	return self:SelectItem(itemid)
end

function CB_GetItemText(self, itemid)
	local listctr = CB_GetListCtr(self)
	local item = listctr:GetItem(itemid)
	return item:GetText()
end

function CB_SetItemText(self, itemid, text)
	local listctr = CB_GetListCtr(self)
	local item = listctr:GetItem(itemid)
	return item:SetText(text)
end

function CB_GetItemData(self, itemid)
	local listctr = CB_GetListCtr(self)
	local item = listctr:GetItem(itemid)
	if item == nil then return end
	local attr = item:GetAttribute()
	return attr.Data
end

function CB_SetItemData(self, itemid, data)
	local listctr = CB_GetListCtr(self)
	local item = listctr:GetItem(itemid)
	local attr = item:GetAttribute()
	attr.Data = data
end

function CB_Clear(self)
	self:RemoveAllItem()
	self:SetText("")
end
-- attrTable 为 item listitem 对象的属性。Data 挂在 attrTable 下面
function CB_AppendItem(self, Text, attrTable,enable)
	self:InsertItem(-1, Text, attrTable, enable)
end

function CB_InsertItem(self, index, Text, attrTable, enable)
    local listctr = CB_GetListCtr(self)
	listctr:InsertItem(index, Text, attrTable, enable)
end

function CB_RemoveItem(self, nindex)
	local listctr = CB_GetListCtr(self)
    listctr:DeleteItem(nindex)
end

function CB_RemoveAllItem(self)
    local listctr = CB_GetListCtr(self)
    listctr:DeleteAllItem()
end

function CB_Item_OnClick(self,id)
	self:SelectItem(id)
	self:ExpandList(false)
end

function CB_Item_OnSelect(self,id)
	local attr = self:GetAttribute()
	self:ExpandList(false)
	attr.bfireEditChange = false
	self:SetText(self:GetItemText(id))
	attr.selectItemID = id
	self:FireExtEvent("OnSelChange", tonumber(id))
end


function CB_SetEnable(self, enable)
    local attr = self:GetAttribute()
    attr.Enable = enable
    local edit = self:GetControlObject("combo.edit")
    if attr.EnableEdit then
        edit:SetReadOnly(not attr.Enable)
        edit:SetNoCaret(not attr.Enable)
		edit:SetCursorID("IDC_HAND")
		if attr.EditDisableColor and not attr.Enable then
			edit:SetTextColorID(attr.EditDisableColor)
		end
    end
	if not enable then
		if  attr.EditDisableColor then
			edit:SetTextColorID(attr.EditDisableColor)
		else
			edit:SetTextColorID("")
		end
	else
		if attr.EditColor then
			edit:SetTextColorID(attr.EditColor)
		else
			edit:SetTextColorID("")
		end
	end
    local btn = self:GetControlObject("combo.btn")
    btn:Enable(enable)
    local bkg = self:GetControlObject("Board")
    if attr.Enable then
        self:SetState(CBState.Normal)
    else
        self:SetState(CBState.Disable)
    end
end

function CB_GetEnable(self)
    local attr = self:GetAttribute()
    return attr.Enable
end

function CB_OnInitControl(self)
    local attr = self:GetAttribute()
	attr.NowState = CBState.Normal
    local left, top, right, bottom = self:GetObjPos()
    local width = right - left
    local height = bottom - top
	
	--设置头部的背景
    local bkg = self:GetControlObject("Board")
    bkg:SetTextureID(attr.HeaderNormalBkgID)
	
	--设置头部对象整体与头部边框的位置
	local layout = self:GetControlObject("Background")
	local layout_left = attr.LeftMargin or 0
	local layout_top = attr.TopMargin or 0
	layout:SetObjPos(layout_left, layout_top, "father.width-left", "father.height-top")
	
    local nowLeft = 0
	
	--Icon相关属性初始化
	local icon = self:GetControlObject("combo.icon")
    if attr.IconResID then
        icon:SetTextureID(attr.IconResID)
        icon:SetVisible(true)
        icon:SetObjPos(2, "father.height/2-" .. attr.IconHeight/2, attr.IconWidth+4, "father.height/2+" .. attr.IconHeight/2)
        nowLeft = nowLeft + attr.IconWidth + 4
    end
	
	--btn相关属性初始化
	local btn = self:GetControlObject("combo.btn")
	if attr.BtnNormalBkgID then
		local btnattr = btn:GetAttribute()
        btnattr.NormalBkgID = attr.BtnNormalBkgID
		btnattr.HoverBkgID = attr.BtnHoverBkgID
		btnattr.DownBkgID = attr.BtnDownBkgID
		btnattr.DisableBkgID = attr.BtnDisableBkgID
        btn:SetVisible(true)
		btn:SetEnable(true)
		attr.BtnHeight = attr.BtnHeight or attr.BtnWidth
		local off = math.floor(attr.BtnHeight/2)
		btn:SetObjPos("father.width - "..attr.BtnWidth.. "-" ..attr.BtnRightMargin, 
						"(father.height -" .. attr.BtnHeight .. ")/2",
						"father.width -".. attr.BtnRightMargin, 
						"(father.height -" .. attr.BtnHeight .. ")/2 + " .. attr.BtnHeight)
		btn:UpdateUI()
    end
	
	--edit相关属性初始化
	local edit = self:GetControlObject("combo.edit")
	local edit_left = nowLeft+attr.EditLeftSpace
	local edit_right
	edit_right = "father.width-"..(attr.BtnWidth+attr.EditRightSpace + attr.BtnRightMargin)
	edit:SetReadOnly(not attr.EnableEdit)
    edit:SetNoCaret(not attr.EnableEdit)
    edit:SetObjPos( edit_left, 1, edit_right, "father.height" )
	if attr.EditFont ~= nil then
		edit:SetFontID(attr.EditFont)
	end
	if attr.EditColor ~= nil then
		edit:SetTextColorID(attr.EditColor)
	end
	
	--设置ListCtrl的位置
    local lblayout = self:GetControlObject("listbox.layout")
	if attr.UpwardList then
		lblayout:SetObjPos(0, 3-attr.ListCtrlHeight, "father.width", 3)
	else
		lblayout:SetObjPos(0, "father.height-1", "father.width", "top+"..attr.ListCtrlHeight)
	end	-- 默认不显示List
    lblayout:SetVisible(false)
	lblayout:SetChildrenVisible(false)
    self:SetEnable(attr.Enable)
	
	-- TextHint
	local textobjhint = self:GetControlObject("text.hint")
	if attr.EditTextHint then
		textobjhint:SetText(attr.EditTextHint)
	end
	if attr.EditTextHintFont then
		textobjhint:SetTextFontResID(attr.EditTextHintFont)
	end
	if attr.EditTextHintColor then
		textobjhint:SetTextColorResID(attr.EditTextHintColor)
	end
	
	--设置ListCtrl的属性
	local listCtr = CB_GetListCtr(self)
	
	function OnMouseMoveItem(source,event,id)
		local item = source:GetItem(id)
		self:FireExtEvent("OnMouseMoveItem", tonumber(id),item)
	end
		
	function OnMouseLeaveItem(source,event,id)
		local item = source:GetItem(id)
		self:FireExtEvent("OnMouseLeaveItem", tonumber(id),item)
	end
	function OnFoucusChange(source,event,focus,dest)
		--XMP.LOG("focus:"..event)
		CB_OnFocusChange(self,focus,dest)
	end
		
	function OnSelectItem(source,event,id)
		CB_Item_OnSelect(self,id)
	end
	listCtr:AttachListener("OnMouseMoveItem",true,OnMouseMoveItem)
	listCtr:AttachListener("OnMouseLeaveItem",true,OnMouseLeaveItem)
--	listCtr:AttachListener("OnClickItem",true,OnClickItem)
	listCtr:AttachListener("FoucusChange",true,OnFoucusChange)
	listCtr:AttachListener("OnSelectItem",true,OnSelectItem)
	
	local listCtrattr = listCtr:GetAttribute()
	if attr.ListCtrlBkgID then
		listCtrattr.BkgID = attr.ListCtrlBkgID
	end
	if attr.ItemNormalBkgID ~= nil then
		listCtrattr.ItemNormalBkgID = attr.ItemNormalBkgID
	end
	if attr.ItemTextHoverColor ~= nil then
		listCtrattr.ItemTextHoverColor = attr.ItemTextHoverColor
	end
	if attr.ItemTextSelectColor ~= nil then
		listCtrattr.ItemTextSelectColor = attr.ItemTextSelectColor
	end
	if attr.ItemHoverBkgID ~= nil then
		listCtrattr.ItemHoverBkgID = attr.ItemHoverBkgID
	end
	if attr.EnableSelectedState ~= nil then
		listCtrattr.EnableSelectedState = attr.EnableSelectedState
	end
	if attr.ItemSelectedBkgID ~= nil then
		listCtrattr.ItemSelectedBkgID = attr.ItemSelectedBkgID
	end
	if attr.ItemLeftMargin ~= nil then
		listCtrattr.ItemLeftMargin = attr.ItemLeftMargin
	end
	if attr.ItemTopMargin ~= nil then
		listCtrattr.ItemTopMargin = attr.ItemTopMargin
	end
	if attr.ItemTextColor ~= nil then
		listCtrattr.ItemTextColor = attr.ItemTextColor
	end
	if attr.ItemTextFont ~= nil then
		listCtrattr.ItemTextFont = attr.ItemTextFont
	end
	if attr.ItemTextHoverFont ~= nil then
		listCtrattr.ItemTextHoverFont = attr.ItemTextHoverFont
	end
	if attr.IconSelectResID ~= nil then
		listCtrattr.IconSelectResID = attr.IconSelectResID
	end
	if attr.ListCtrlHeight ~= nil then
		listCtrattr.ListCtrlHeight = attr.ListCtrlHeight
	end
	if attr.ListItemHeight ~= nil then
		listCtrattr.ListItemHeight = attr.ListItemHeight
	end
	if attr.EnableItemTip ~= nil then
		listCtrattr.EnableItemTip = attr.EnableItemTip
	end 
	if attr.ItemTextDisableColor ~= nil then
		listCtrattr.ItemTextDisableColor = attr.ItemTextDisableColor
	end 
	if attr.bRepeat ~= nil then
		listCtrattr.bRepeat = attr.bRepeat
	end
	if attr.ScrollBarTemplate ~= nil then
		listCtrattr.ScrollBarTemplate = attr.ScrollBarTemplate
	end
	listCtr:Init()
end


function CB_SetEditColorFont(self,color,font)
	local edit = self:GetControlObject("combo.edit")
	local attr = self:GetAttribute()
	if color ~= nil then
		attr.EditColor = color
		edit:SetTextColorID(attr.EditColor)
	end
	if font ~= nil then
		attr.EditFont = font
		edit:SetFontID(attr.EditFont)
	end
end

function CB_Edit_OnChange(self)
    local control = self:GetOwnerControl()
	local controlattr = control:GetAttribute()
    local text = self:GetText()
	if controlattr.bFireSelectEditChange then
		control:FireExtEvent("OnEditChange", text)
	else
		if (controlattr.bfireEditChange or  controlattr.bfireEditChange == nil) then          --controlattr.bfireEditChange 是否发生OnEditChange消息
			control:FireExtEvent("OnEditChange", text)
		end
	end
	controlattr.bfireEditChange = true
	return true
end

-- 鼠标左键点击编辑框，看是否需要弹出List
function CB_Edit_OnLButtonDown(self)
	local combobox = self:GetOwnerControl()
	local attr = combobox:GetAttribute()
	if not attr.EnableEdit then
		--XLMessageBox("her")
		CB_Btn_OnClick(self)
	end
end

-- ListBox显示和消失
function CB_Btn_OnClick(self)
	local owner = self:GetOwnerControl()
	local bExpand = not owner:IsListExpand()
	owner:ExpandList(bExpand)
end

function CB_IsListExpand(self)
	local lblayout = self:GetControlObject("listbox.layout")
	return lblayout:GetVisible()
end


local posAni
-- 根据Item的高度调整listbox的大小
function CB_ExpandList(self, bExpand)
    local attr = self:GetAttribute()
    if not attr.Enable then	return	end
	if self:GetCount() <= 0 then return end
	
    local left, top, right, bottom = self:GetObjPos()
    local width, height = right - left, bottom - top
    local lblayout = self:GetControlObject("listbox.layout")
	local listCtr = self:GetControlObject("listctrl")
	
	if bExpand then
		local needed_height = listCtr:GetNeededHeight()
		if needed_height<attr.ListCtrlHeight then			
			if attr.UpwardList then
				listCtr:SetObjPos(0, "father.height-"..needed_height, "father.width", "father.height")
			else
				listCtr:SetObjPos(0, 0, "father.width", needed_height)
			end
		else
			listCtr:SetObjPos(0, 0, "father.width", attr.ListCtrlHeight)
		end
		lblayout:SetVisible(true)
		lblayout:SetChildrenVisible(true)
		
		local function onAniFinish(listCtr)
			-- XMP.LOG("CLB_ScrollActiveItem  1111",listCtr:GetID())
			listCtr:ScrollActiveItem(self:GetSelectItem())
        end
		local lbleft, lbtop, lbright, lbbottom = listCtr:GetObjPos()
		if attr.UpwardList then
			__Animation.RunPosAni_Control(listCtr, lbleft+2, lbbottom, lbright-2, lbbottom+4, lbleft, lbtop, lbright, lbbottom, onAniFinish, 300)
		else
			__Animation.RunPosAni_Control(listCtr, lbleft+2, lbtop-4, lbright-2, lbtop, lbleft, lbtop, lbright, lbbottom, onAniFinish, 300)
		end
	else
	    local function onAniFinish(self)
			lblayout:SetVisible(false)
			lblayout:SetChildrenVisible(false)
        end
		
        local lbleft, lbtop, lbright, lbbottom = listCtr:GetObjPos()
		if attr.UpwardList then
			__Animation.RunPosAni_Control(listCtr, lbleft, lbtop, lbright, lbbottom, lbleft+2, lbbottom, lbright-2, lbbottom+4, onAniFinish, 300)
		else
			__Animation.RunPosAni_Control(listCtr, lbleft, lbtop, lbright, lbbottom, lbleft+2, lbtop-4, lbright-2, lbtop, onAniFinish, 300)
		end
	end
end

function CB_Edit_OnMouseMove(self)
	local combobox = self:GetOwnerControl()
	local attr = combobox:GetAttribute()
	if not attr.EnableEdit then
		self:SetCursorID("IDC_HAND")
	end
    if attr.Enable then 
		combobox:SetState(CBState.Hover)
	end
	
end
function  CB_Edit_OnMouseLeave(self)
	local combobox = self:GetOwnerControl()
	local attr = combobox:GetAttribute()
	if attr.Enable then
		combobox:SetState(CBState.Normal)
	else
		combobox:SetState(CBState.Disable)
	end
end



function CB_Edit_OnRButtonUp(self)
	local attr = self:GetOwnerControl():GetAttribute()
	if attr.ShowMenu then
		XLSetGlobal( "xmp.cur.combobox_edit", self )
		local x, y = XMP.XmpPre.self:GetCursorPos()
		local wnd = self:GetOwner():GetBindHostWnd()
		XMP.WndFactory.CreateMenuEx(x, y, "Singleton.MenuHostWnd", "comboBox.menu.context", "context_menu", wnd:GetWndHandle())
	end
end

function CB_Cut_OnInit(self)
	local edit = XLGetGlobal( "xmp.cur.combobox_edit" )
	if edit ~= nil then
		local start, end_ = edit:GetSel()
		if start == end_ or edit:GetReadOnly() then
			self:SetEnable( 0 )
		end
	end
end
function CB_Cut_OnSelect(self)
	local edit = XLGetGlobal( "xmp.cur.combobox_edit" )
	if edit ~= nil then
		edit:Cut()
	end
end

function CB_Copy_OnInit(self)
	local edit = XLGetGlobal( "xmp.cur.combobox_edit" )
	if edit ~= nil then
		local start, end_ = edit:GetSel()
		if start == end_ then
			self:SetEnable( 0 )
		end
	end
end
function CB_Copy_OnSelect(self)
	local edit = XLGetGlobal( "xmp.cur.combobox_edit" )
	if edit ~= nil then
		edit:Copy()
	end
end

function CB_Paste_OnInit(self)
	local edit = XLGetGlobal( "xmp.cur.combobox_edit" )
	if edit ~= nil then
		if edit:GetReadOnly() then
			self:SetEnable( 0 )
			return
		end
	end
	local clipbrdText = XMP.XmpPre.self:GetClipbrdData()
	if not clipbrdText or clipbrdText == "" then
		self:SetEnable( 0 )
	end
end
function CB_Paste_OnSelect(self)
	local edit = XLGetGlobal( "xmp.cur.combobox_edit" )
	if edit ~= nil then
		edit:Paste()
	end
end
