local Helper = XLGetGlobal("Helper")
local tipUtil = XLGetObject("API.Util")

function OnClose(self)
	self:Show(0)
	return 0, true
end

function OnShowWindow(self, bShow)
	if bShow then
		self:SetFocus(true)
	end
end

function InputFilter(self, msg, wpram, lparam)
	-- (UINT) WM_DROPFILES,      // message ID    0x0233
	-- (WPARAM) wParam,      // = (WPARAM) (HDROP) hDrop;    
	-- (LPARAM) lParam      // = 0; not used, must be zero );  
	if msg == 0x0233 then 
		local filePath = tipUtil:DragQueryFile(wpram)
		Helper:DispatchEvent("OnNotePadDrop",filePath)
	end
end

function OnCreate(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root")
	local notepadview = objtree:GetUIObject("mainwnd.frame:notepadview")
    local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local userData = self:GetUserData()
	-- local aniT = templateMananger:GetTemplate("tip.pos.animation","AnimationTemplate")
	-- local ani = aniT:CreateInstance()
	-- ani:BindObj(objRootLayout)
	if userData and userData.filePath 
		and tipUtil:QueryFileExists(userData.filePath) 
		and ".txt" == Helper:GetFileExt(userData.filePath) then
		--打开文件，初始化notepadCtrl
			local createTime = tipUtil:GetFileCreateTime(userData.filePath)
			local fileName = string.match(userData.filePath, ".*\\(.*)")
			local data = {["txtFilePath"]=userData.filePath, ["createtime"]=createTime, ["title"]=fileName, ["bIndependentNotePad"]=userData.bIndependentNotePad}
			notepadview:SetData(data)
	else
		local data = {["createtime"]=os.time(), ["title"]="新建文本文档.txt", ["bIndependentNotePad"]=userData.bIndependentNotePad}
		notepadview:SetData(data)
	end
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move( math.floor((workright - nLayoutWidth)/2), math.floor((workbottom - nLayoutHeight)/2), nLayoutWidth, nLayoutHeight)
	-- ani:SetKeyFramePos(0, nLayoutHeight, 0, 0) 
	-- objtree:AddAnimation(ani)
	-- ani:Resume()
	tipUtil:DragAcceptFiles(self:GetWndHandle(), true)
	self:AddInputFilter(false,InputFilter)
	
end

function OnInitNotePad(self)
	--若命令行中有传入文件路径，则打开文件，并以之初始化notepad
end

function OnDestroy(self)
	
end

function OnClickMin(self)
	local wndTree = self:GetOwner()
	local wnd = wndTree:GetBindHostWnd()
	wnd:Min()
end

function OnClickMax(self)
	local wndTree = self:GetOwner()
	local wnd = wndTree:GetBindHostWnd()
	wnd:Max()
end

function OnClickRestore(self)
	local wndTree = self:GetOwner()
	local wnd = wndTree:GetBindHostWnd()
	wnd:Restore()
end

function OnClickClose(self)
	--发事件，让控件检查一下是否需要保存
	
	
	--保存完毕，退出
	-- Helper:DestoryModelessWnd("NotePadWnd")
	tipUtil:Exit("Exit")
end

function OnSize(self, _type, width, height)
	local tree = self:GetBindUIObjectTree()
	if not tree then
		return
	end
	
	local rootObject = tree:GetRootObject()
	rootObject:SetObjPos(0, 0, width, height)
		
	local maxbtn = tree:GetUIObject("mainwnd.frame:mainwnd.max.btn")
	local restorebtn = tree:GetUIObject("mainwnd.frame:mainwnd.restore.btn")
	if "max" == _type then			
		maxbtn:Show(false)
		rootObject:SetObjPos(-5, -5, width+13, height+10)
		restorebtn:Show(true)
	elseif "restored" == _type then
		maxbtn:Show(true)
		restorebtn:Show(false)
	end
end

