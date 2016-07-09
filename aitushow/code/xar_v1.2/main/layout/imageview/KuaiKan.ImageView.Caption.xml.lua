function OnSettingBtnClick(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	Helper:CreateModalWnd("SettingWnd","SettingWndTree", objHostWnd)
end

function OnSysBtnInitControl(self)
	local hostWnd = Helper.Selector.select("", "", "Kuaikan.MainWnd.Instance")
    local state = hostWnd:GetWindowState()
    if state == "max" then
        self:SetMaxState(false)
    else
        self:SetMaxState(true)
    end
end

function OnLButtonDbClickLogo(self)
	local closebtn = self:GetObject("control:client.caption.sysbtn")
	OnCloseBtnClick(closebtn)
end

function OnMinisizeBtnClick(self)
	local objTree = self:GetOwner()
	local hostWnd = objTree:GetBindHostWnd()
    hostWnd:Min()
end


function OnMaxBtnClick(self)
	local objTree = self:GetOwner()
	local hostWnd = objTree:GetBindHostWnd()
	local state = hostWnd:GetWindowState()
	if state == "max" then
		hostWnd:Restore()
		self:SetMaxState(true)
	else
		hostWnd:Max()
		self:SetMaxState(false)
	end
end


function OnCloseBtnClick(self)
	local clientobj = self:GetOwnerControl():GetOwnerControl()
	local function fn_exit()
		local hostwnd = self:GetOwner():GetBindHostWnd()
		hostwnd:Show(0)
		local lastwnd = Helper.Listener.LastShowWnd
		if lastwnd and lastwnd:GetID() ~= "Kuaikan.MainWnd.Instance" then
			lastwnd:BringWindowToTop(true)
		else
			if not Helper.Setting.GetExitType() then
				--退出
				StatUtil.Exit()
			else
				Helper.Tray.Show()
			end
		end
	end
	clientobj:HandleRotateExit(fn_exit)
end

function GetSystemBtn(self)
	return self:GetControlObject("client.caption.sysbtn")
end

function UpdateTitleShow(self)
	local attr = self:GetAttribute() 
	local extendText = ""
	if	attr.Size ~= "" then
		extendText = attr.Size  .. " - " .. attr.FileIndex .. "," .. attr.Ration
	end
	local pathTextBakObj = self:GetControlObject("client.caption.title.path.bak")
	LOG("KuaikanLog extendText:" .. extendText)
	pathTextBakObj:SetText(extendText)
	local textWidth, textHeight = pathTextBakObj:GetTextExtent()
	
	local pathTextObj = self:GetControlObject("client.caption.title.path")
	local l,t,r,b = pathTextObj:GetObjPos()
	local width = r-l
	local validWidth = width - textWidth
	
	LOG("KuaikanLog width:" .. width .. " textWidth:" .. textWidth)
	
	local text = Helper.APIproxy.GetSpecialPathbyWidth(pathTextBakObj, attr.Path, validWidth) .. " " .. extendText	
	pathTextObj:SetText(text)
end

function SetFilePath(self, path, size, ration, index)
	local attr = self:GetAttribute()
	attr.Path = path
	attr.Size = size
	attr.Ration = ration
	attr.FileIndex = index
	UpdateTitleShow(self)
end

function ShowContent(self, bShow)
	local bkgObj = self:GetControlObject("client.bkg")
	bkgObj:SetVisible(bShow)
	bkgObj:SetChildrenVisible(bShow)
end

function OnFullScreenBtnClick(self)
	local clientObj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	clientObj:FullScreenSwitch()
end