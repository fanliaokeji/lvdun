function OnInitControl(self)endfunction OnDestroy(self)endfunction OnPosChange(self)endfunction OnBind(self)endfunction OnLButtonDbClick(self)endfunction OnCreate(self)	endfunction OnClose(self)endfunction OnMainWndSize(self, type_, width, height)	if type_ == "min" then		return	elseif type_ == "max" then		local tree = self:GetBindUIObjectTree()		if tree ~= nil then			local button = tree:GetUIObject( "mainwnd.sysbtn" )			if button ~= nil then				button:SetMaxState( false )			end		end	elseif type_ == "restored" then		local tree = self:GetBindUIObjectTree()		if tree ~= nil then			local button = tree:GetUIObject( "mainwnd.sysbtn" )			if button ~= nil then				button:SetMaxState( true )			end		end	end		local objectTree = self:GetBindUIObjectTree()	local rootObject = objectTree:GetRootObject()		rootObject:SetObjPos(0, 0, width, height)endfunction OnMaxBtnClick(self)	-- local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")	-- local hostwnd = hostwndManager:GetHostWnd("IDE.MainFrame")	-- local state = hostwnd:GetWindowState()	-- if state == "max" then		-- hostwnd:Restore()		-- self:SetMaxState( true )	-- else		-- hostwnd:Max()		-- self:SetMaxState( false )	-- endend
