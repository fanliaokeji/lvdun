-- NormalMenu 包住MenuObject，里面很多方法是直接调MenuObject的方法
function OnInitControl(self, objMenuContext)
	local attr = self:GetAttribute()
	if attr.bHasInit then
		return
	end
	
	attr.v_status = 1
	attr.h_status = 1

	--注意objMenuContext的id必须是context_menu
	if objMenuContext ~= nil then
		local bkn = self:GetControlObject("menu.bkn")
		local attr = objMenuContext:GetAttribute()
		local nRightSpan = attr.ItemRight or 0
	
		objMenuContext:SetObjPos("father.left", "father.top", "father.right-"..tostring(nRightSpan), "father.bottom")	
		bkn:AddChild(objMenuContext)
		bkn:SetLimitChild(true)
	else
	
		local contextid = attr.ContextID
		if contextid ~= nil then
			local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")	
			local contextTemplate = templateMananger:GetTemplate(contextid, "ObjectTemplate")
			local contextObj = contextTemplate:CreateInstance("context_menu")
			if contextObj ~= nil then
				local bkn = self:GetControlObject("menu.bkn")
				contextObj:SetObjPos("father.left", "father.top", "father.right", "father.bottom")
				bkn:AddChild(contextObj)
			end
		end
	end
	
	local shadeID = attr.ShadeID
	local bknRes = attr.BknID
	
	if shadeID ~= nil then
		local shade = self:GetControlObject("menu.shade")
		shade:SetResID(shadeID)
	end
	
	if bknRes ~= nil then
		local bkn = self:GetControlObject("menu.bkn")
		if attr.v_status == nil or attr.v_status == 1 then
			bkn:SetResID(bknRes)
		else
			bkn:SetResID(attr.UpBknID)
		end
	end
	
	UpdateSize( self )
	attr.bHasInit = true
	return true
end

function UpdateSize( self )
	local attr = self:GetAttribute()
	if attr.ani then
		attr.ani:ForceStop()
	end
	
	local menu = self:GetControlObject( "context_menu" )
	if menu ~= nil then
		local attr = self:GetAttribute()
		local nTotalCount = menu:GetItemCount()
		if nTotalCount <= 0 then
			self:SetVisible(false)
			self:SetChildrenVisible(false)
			return
		end
	
		local left, top, right, bottom = menu:GetObjPos()
		local self_left, self_top, self_right, self_bottom = self:GetObjPos()
		local menu_bkn = self:GetControlObject("menu.bkn")
		local menu_frame = self:GetControlObject("menu.frame")
		
		local nMenuH = bottom - top
		local nSelfH = self_bottom - self_top
		if nSelfH > nMenuH then
			nSelfH = nMenuH
		end	
		
		menu_frame:SetObjPos(0,0,self_right - self_left, nSelfH)
		
		--注 图片有阴影
		menu_bkn:SetObjPos(0,0,"father.width","father.height")
		
		UpdateScrollBar(self, menu)
	end
end

function EndMenu( self )
	local menu = self:GetControlObject( "context_menu" )
	if menu ~= nil then
		menu:EndMenu()
	end
end

--v_status垂直方向状态1表示往下弹，2表示往上弹
--h_status水平方向状态1表示往右弹，2表示往左弹
function SetPopStatus( self, v_status, h_status )
	local attr = self:GetAttribute()
	attr.v_status = v_status
	if h_status ~= nil then
		attr.h_status = h_status
	end
	
	local shadeID = attr.ShadeID
	local bknRes = attr.BknID
	
	if shadeID ~= nil then
		local shade = self:GetControlObject("menu.shade")
		shade:SetResID(shadeID)
	end
	
	if bknRes ~= nil then
		local bkn = self:GetControlObject("menu.bkn")
		if attr.v_status == nil or attr.v_status == 1 or attr.UpBknID == nil then
			bkn:SetResID(bknRes)
		else
			bkn:SetResID(attr.UpBknID)
		end
	end
	
	-- local shadingID = attr.ShadingID
	-- if shadingID ~= nil then
		-- local shading = self:GetControlObject("menu.shading")
		-- if attr.v_status == nil or attr.v_status == 1 then
			-- shading:SetResID(shadingID)
			-- shading:SetVisible( true )
		-- else
			-- shading:SetVisible( false )
		-- end
	-- end
end

function AnimateShow(self)
	local attr = self:GetAttribute()
	local menuTree = self:GetOwner()
	
	local frame = self:GetControlObject("menu.frame")
	local bkn = self:GetControlObject("menu.bkn")
	local shading = self:GetControlObject("menu.shading")
	local shade = self:GetControlObject("menu.shade")
	
    local function onAniFinish(self,old,new)
        if new == 4 then
			attr.ani = nil
			frame:SetLimitChild( false )
			if attr.UserData then
				if not g_UserData then
					g_UserData = "11" 
				end
				if attr.UserData:GetAttribute().NowState ~= 3 then
					attr.UserData:SetState(1)
				end
			end
        end
    end
	
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local menuAniT = templateMananger:GetTemplate("menu.showani","AnimationTemplate")
	
	--local xarManager = XLGetObject("Xunlei.UIEngine.XARManager")
	--local xarfactory = xarManager:GetXARFactory()
	
	--local aniTime = 200
	--local curveID = "menu.show"
	
	local posChange = menuAniT:CreateInstance()
	--local posChange = xarfactory:CreateAnimation("PosChangeAnimation")
	posChange:BindLayoutObj(frame)
	--posChange:SetTotalTime(aniTime)
	--posChange:BindCurveID(curveID)
	
	local left,top,right,bottom = self:GetObjPos()
	local srcLeft, srcTop, srcRight, srcBottom
	if attr.v_status == nil or attr.v_status == 1 then
		srcTop = 0
		srcBottom = 0
	else
		srcTop = bottom - top
		srcBottom = bottom - top
	end
	
	if attr.h_status == nil or attr.h_status == 1 then
		srcLeft = 0
		srcRight = 0
	else
		srcLeft = right - left
		srcRight = right - left
	end
	--posChange:SetKeyFrameRect(0,0,right-left,bottom-top, 0,0,right-left,bottom-top)
	
	posChange:SetKeyFrameRect(srcLeft, srcTop, srcRight, srcBottom, 0,0,right-left,bottom-top)
	--shade:SetObjPos(-1,-1,right-left + 1,bottom-top + 3)
	
	-- shading:SetObjPos( ((right-left)*0.1/18)*19,bottom - top - 31- 2,right-left-((right-left)*0.1/18)*19,bottom - top -2 )
	-- local shading_left,shading_top,shading_right,shading_bottom = shading:GetObjPos()

	posChange:AttachListener(true,onAniFinish)
	
	--menuTree:AddAnimation(aniChange)
	menuTree:AddAnimation(posChange)
	attr.ani = posChange
	--aniChange:Resume()
	posChange:Resume()	
end

function AnimateHide(self)
	local attr = self:GetAttribute()
	if attr.ani then
		attr.ani:ForceStop()
	end
	local menuTree = self:GetOwner()
	local context = menuTree:GetRootObject()
	
	local frame = self:GetControlObject("menu.frame")
	local bkn = self:GetControlObject("menu.bkn")
	local shading = self:GetControlObject("menu.shading")
	
	frame:SetLimitChild(true)
	
    local function onAniFinish(self,old,new)
        if new == 4 then
			local tree = frame:GetOwner()
			local hostWnd = tree:GetBindHostWnd()
			hostWnd:FinalClear()
			if attr.UserData then
				if not g_UserData then
					g_UserData = attr.UserData 
				end
				if g_UserData ~= attr.UserData then
					g_UserData = attr.UserData 
					
					if attr.UserData:IsMouseInRect() then
						attr.UserData:SetState(2)
					else
						attr.UserData:SetState(0)
					end
				else
					attr.UserData:SetState(2)
				end
				if EscPress then
					attr.UserData:SetState(2)
					EscPress = nil
				end
			end
			--hostWnd:UnbindUIObjectTree()
			--local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
			--hostwndManager:RemoveHostWnd(hostWnd:GetID())
			
			--local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
			--objtreeManager:DestroyTree(tree:GetID())
        end
    end
	
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local menuAniT = templateMananger:GetTemplate("menu.showani","AnimationTemplate")
	
	local posChange = menuAniT:CreateInstance()
	--local xarManager = XLGetObject("Xunlei.UIEngine.XARManager")
	--local xarfactory = xarManager:GetXARFactory()
			
	--local aniTime = 150
	--local curveID = "menu.hide"
	
	--local posChange = xarfactory:CreateAnimation("PosChangeAnimation")
	posChange:BindLayoutObj(frame)
	--posChange:SetTotalTime(aniTime)
	--posChange:BindCurveID(curveID)
		
	local left,top,right,bottom = self:GetObjPos()
	local destLeft, destTop, destRight, destBottom
	if attr.v_status == nil or attr.v_status == 1 then
		destTop = 0
		destBottom = 0
	else
		destTop = bottom - top
		destBottom = bottom - top
	end
	
	if attr.h_status == nil or attr.h_status == 1 then
		destLeft = 0
		destRight = 0
	else
		destLeft = right - left
		destRight = right - left
	end
		
	posChange:SetKeyFrameRect(0,0,right-left,bottom-top,destLeft, destTop, destRight, destBottom)

	posChange:AttachListener(true,onAniFinish)
	
	menuTree:AddAnimation(posChange)

	posChange:Resume()	
end

function InsertItem( self, index, item )
	local menu = self:GetControlObject("context_menu")
	if menu ~= nil then
		menu:InsertItem( index, item )
	end
	UpdateSize( self )
end

function RemoveItem( self, index )
	local menu = self:GetControlObject("context_menu")
	if menu ~= nil then
		menu:RemoveItem( index )
	end
	UpdateSize( self )
end

function GetItemCount( self )
	local menu = self:GetControlObject("context_menu")
	if menu ~= nil then
		return menu:GetItemCount()
	end
end

function GetItem( self, index )
	local menu = self:GetControlObject("context_menu")
	if menu ~= nil then
		return menu:GetItem( index )
	end
end

function GetItembyID(self,ID)
	local menu = self:GetControlObject("context_menu")
	if menu ~= nil then
		return menu:GetItembyID( ID )
	end
end

function AddItem( self, item )
	local menu = self:GetControlObject("context_menu")
	if menu ~= nil then
		menu:AddItem( item )
	end
	UpdateSize( self )
end

-- 得到菜单条，xmp暂时没用上
function GetMenuBar( self )
	local father = self:GetFather()
	local temp = self
	while true do
		if temp:GetClass() == "NormalMenu" then
			if father == nil or father:GetClass() ~= "MenuItemObject" then
				local attr = temp:GetAttribute()
				return attr.menu_bar
			else
                temp = father
                father = father:GetFather()
			end
		else
			if father == nil then
				return
			end
			temp = father
			father = father:GetFather()
		end
	end
end

function OnKeyDown( self, char )
	local menu = self:GetControlObject("context_menu")
	if menu == nil then
		return
	end
	
	if char == 38 then --up
		menu:MoveToPrevItem()
	elseif char == 40 then --down
		menu:MoveToNextItem()
	elseif char == 39 then --right
		local item = menu:GetHoverItem()
		if item ~= nil then
			if item:HasSubMenu() then
				item:ShowSubMenu( true )
				local submenu = item:GetSubMenu()
				submenu:SetFocus( true )
				return
			end
		end
		local menu_bar = GetMenuBar( self )
		if menu_bar ~= nil then
			menu_bar:MoveToNext( true )
		end
	elseif char == 37 then --left
		local father = self:GetFather()
		if father ~= nil and father:GetClass() == "MenuItemObject" then
			father:SetFocus( true )
			father:EndSubMenu()
		else
			local menu_bar = GetMenuBar( self )
			if menu_bar ~= nil then
				menu_bar:MoveToPrev( true )
			end
		end
	elseif char == 27 then--ESC
		local father = self:GetFather()
		EscPress =  true
		if father ~= nil and father:GetClass() == "MenuItemObject" then
			father:SetFocus( true )
			father:EndSubMenu()
		else
			local attr = self:GetAttribute()
			if attr.menu_bar ~= nil then
				attr.esc_close_menu = true
				attr.menu_bar:EndLastPopMenu()
			elseif father == nil then
				self:EndMenu()
			end
		end
	elseif char == 18 then--ALT
		local attr = self:GetAttribute()
		local menu_bar = GetMenuBar( self )
		if menu_bar ~= nil then
			attr.alt_close_menu = true
			menu_bar:EndLastPopMenu()
		end
	elseif char == 13 then--ENTER
		local item = menu:GetHoverItem()
		if item ~= nil then
			if item:HasSubMenu() then
				item:ShowSubMenu( true )
			else
				item:SelectItem()
			end
		end
	else
		local key_item = menu:GetKeyItem( char )
		if #key_item == 1 then
			if key_item[1]:HasSubMenu() then
				menu:SetHoverItem( key_item[1], false )				
				key_item[1]:ShowSubMenu( true )
			else
				key_item[1]:SelectItem()
			end
		elseif #key_item > 1 then
			local hover_item = menu:GetHoverItem()
			if hover_item == nil then
				menu:SetHoverItem( key_item[ 1 ], false )
			else
				for i = 1, #key_item do
					if key_item[i]:GetID() == hover_item:GetID() then
						if i == #key_item then
							menu:SetHoverItem( key_item[ 1 ], false )
							break
						else
							menu:SetHoverItem( key_item[ i + 1 ], false )
							break
						end
					else
						if i == #key_item then
							menu:SetHoverItem( key_item[ 1 ], false )
							break
						end
					end
				end
			end
		end
				
		--[[
		local list_id = "global.keylist"
		local key_list = XLGetGlobal( list_id )
		if key_list == nil then
			return
		end
		local shell = XLGetObject( "Xunlei.UIEngine.OSShell" )
		local id = ""
		--VK_CONTROL
		if shell:GetKeyState( 17 ) < 0 then
			id=id.."17."
		end
		--VK_MENU ALT
		if shell:GetKeyState( 18 ) < 0 then
			id=id.."18."
		end
		--VK_SHIFT
		if shell:GetKeyState( 16 ) < 0 then
			id=id.."16."
		end
		id=id..char
		if key_list[ id ] ~= nil then
			local menu_bar = GetMenuBar( self )
			if menu_bar ~= nil then
				menu_bar:EndLastPopMenu()
			end
			key_list[ id ]()
		end
		]]
	end
end

function OnFocusChange( self, is_focus )
	
end

function MoveNextItem( self )
	local menu = self:GetControlObject("context_menu")
	if menu == nil then
		return
	end
	menu:MoveToNextItem()
end

function MovePrevItem( self )
	local menu = self:GetControlObject("context_menu")
	if menu == nil then
		return
	end
	menu:MoveToPrevItem()
end

function GetCurItem( self )
	local menu = self:GetControlObject("context_menu")
	if menu == nil then
		return
	end
	return menu:GetHoverItem()
end

function SetMenuBar( self, menu_bar )
	local attr = self:GetAttribute()
	attr.menu_bar = menu_bar
end

function GetParentItem(self)
	local menu = self:GetControlObject("context_menu")
	return menu:GetParentItem()
end

function AdjustItemPos(self)
	local menu = self:GetControlObject("context_menu")
	menu:AdjustItemPos()
	UpdateSize( self )
end

function InitMenuWithRelateObject(self)
	local objRelate = self:GetRelateObject()
	if not objRelate then
		return
	end
	
	local nCount = self:GetItemCount()
	for nIndex = 1, nCount do 
		local objMenuItem = self:GetItem(nIndex)
		if objMenuItem then
			objMenuItem:FireExtEvent( "OnInitWithRelateObj" )
		end	
	end
end

--滚动条
function UpdateScrollBar(objRootCtrl, menu)
	local attr = objRootCtrl:GetAttribute()
	local nTotalCount = menu:GetItemCount()
	attr.nTotalLineCount = nTotalCount
		
	ResetScrollBar(objRootCtrl)
end

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
	
	local nItemHeight = objRootCtrl:GetItemHeight()
	local nMaxHeight = nItemHeight * nTotalLineCount
	local nPageSize = nItemHeight * nLinePerPage
	
	objScrollBar:SetScrollRange( 0, nMaxHeight - nPageSize, true )
	objScrollBar:SetPageSize(nPageSize, true)	
	objScrollBar:SetScrollPos(0, true)	
		
	if nLinePerPage == 0 or nLinePerPage >= nTotalLineCount then
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


function SetScrollPos(self, nScrollPos)
	local objRootCtrl = self
	local objScrollBar = objRootCtrl:GetControlObject("listbox.vscroll")
	if objScrollBar == nil then
		return
	end
	
	objScrollBar:SetScrollPos(nScrollPos, true)
end


function ProcessScrollWhenWheel(self, x, y, distance)
	local objRootCtrl = self
	local objScrollBar = objRootCtrl:GetControlObject("listbox.vscroll")
	
	local nScrollPos = objScrollBar:GetScrollPos()
	local nItemHeight = objRootCtrl:GetItemHeight()
		
    if distance > 0 then
		objScrollBar:SetScrollPos( nScrollPos - nItemHeight, true )
    else		
		objScrollBar:SetScrollPos( nScrollPos + nItemHeight, true )
    end

	local nNewScrollPos = objScrollBar:GetScrollPos()
	objRootCtrl:MoveItemListPanel(nNewScrollPos)
end



function CLB__OnScrollBarMouseWheel(self, name, x, y, distance)
	local objRootCtrl = self:GetOwnerControl()
	objRootCtrl:ProcessScrollWhenWheel(x, y, distance)
end


function CLB__OnScrollMousePosEvent(self)
	local objRootCtrl = self:GetOwnerControl()
	local nScrollPos = self:GetScrollPos()
	
	objRootCtrl:MoveItemListPanel(nScrollPos)
end


function MoveItemListPanel(self, nScrollPos)
	local objRootCtrl = self
	if not objRootCtrl then
		return
	end
	
	local objContainer = objRootCtrl:GetControlObject("context_menu")
	if not objContainer then
		return
	end
	
	local objScrollBar = objRootCtrl:GetControlObject("listbox.vscroll")
	local nBegin, nEnd = objScrollBar:GetScrollRange()
	if nScrollPos>nEnd then
		nScrollPos = nEnd
	end
	
	local nL, nT, nR, nB = objContainer:GetObjPos()
	local nHeight = nB-nT
	local nNewT = 0-nScrollPos
	
	objContainer:SetObjPos(nL, nNewT, nR, nNewT+nHeight)
end


function GetItemHeight(self)
	local objRootCtrl = self
	local objContainer = objRootCtrl:GetControlObject("context_menu")
	if not objContainer then
		return 0
	end
	
	local attr = objContainer:GetAttribute()
	local objItem = attr.ItemList[1]
	if not objItem then
		return 0
	end

	local nL, nT, nR, nB = objItem:GetObjPos()
	local nItemHeight = nB-nT
	return nItemHeight
end

function BindRelateObject(self, objRelate)
	local attr = self:GetAttribute()
	attr.RelateObject = objRelate
end


function GetRelateObject(self)
	local attr = self:GetAttribute()
	return attr.RelateObject
end


function RouteToFather(self)
	self:RouteToFather()
end




