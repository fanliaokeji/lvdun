local Helper = XLGetGlobal("Helper")
local tipUtil = XLGetObject("API.Util")
local NotePad = XLGetGlobal("NotePad", NotePad)

local iWindowPosXReg = "HKEY_CURRENT_USER\\Software\\ddnotepad\\iWindowPosX"
local iWindowPosYReg = "HKEY_CURRENT_USER\\Software\\ddnotepad\\iWindowPosY"
local iWindowPosDXReg = "HKEY_CURRENT_USER\\Software\\ddnotepad\\iWindowPosDX"
local iWindowPosDYReg = "HKEY_CURRENT_USER\\Software\\ddnotepad\\iWindowPosDY"

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
	
	if userData and userData.filePath 
		and tipUtil:QueryFileExists(userData.filePath) 
		and ".txt" == Helper:GetFileExt(userData.filePath) then
		--打开文件，初始化notepadCtrl
			local year, month, day, hour, minute, second = tipUtil:GetFileCreateTime(userData.filePath)
			local createtime = os.time({year = year, month = month, day = day, hour = hour, min = minute, sec = second})
			
			local fileName = string.match(userData.filePath, ".*\\(.*)")
			local data = {["txtFilePath"]=userData.filePath, ["createtime"]=createTime, ["title"]=fileName, ["bIndependentNotePad"]=userData.bIndependentNotePad}
			notepadview:SetData(data)
	else
		local data = {["createtime"]=os.time(), ["title"]="新建文本文档.txt", ["bIndependentNotePad"]=userData.bIndependentNotePad}
		notepadview:SetData(data)
	end
	
	local iWindowPosX = Helper:QueryRegValue(iWindowPosXReg)
	local iWindowPosY = Helper:QueryRegValue(iWindowPosYReg)
	local iWindowPosDX = Helper:QueryRegValue(iWindowPosDXReg)
	local iWindowPosDY = Helper:QueryRegValue(iWindowPosDYReg)
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
		
	if tonumber(iWindowPosX) and tonumber(iWindowPosY) and tonumber(iWindowPosDX) and tonumber(iWindowPosDY) 
		and iWindowPosX < workright and iWindowPosY < workbottom and iWindowPosDX > workleft and iWindowPosDY > worktop then
		self:Move(iWindowPosX, iWindowPosY, iWindowPosDX-iWindowPosX, iWindowPosDY-iWindowPosY)
	else
		local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
		local nLayoutWidth = nLayoutR - nLayoutL
		local nLayoutHeight = nLayoutB - nLayoutT
		self:Move( math.floor((workright - nLayoutWidth)/2), math.floor((workbottom - nLayoutHeight)/2), nLayoutWidth, nLayoutHeight)
	end
	
	tipUtil:DragAcceptFiles(self:GetWndHandle(), true)
	self:AddInputFilter(false,InputFilter)
	
	local tStatInfo = {}
	tStatInfo.strEC = "runtime"
	tStatInfo.strEA = Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\InstallSource")
	
	Helper:SendConvStatistic(tStatInfo)
	NotePad:DownloadRemoteConfig()
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
	
	local tStatInfo = {}
	tStatInfo.strEC = "exit"
	tStatInfo.strEA = Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\InstallSource")
	
	Helper:SendConvStatistic(tStatInfo)
	--保存完毕，退出
	-- Helper:DestoryModelessWnd("NotePadWnd")
	NotePad:Exit()
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
		return
	elseif "restored" == _type then
		maxbtn:Show(true)
		restorebtn:Show(false)
		
		local x, y = self:HostWndPtToScreenPt(self:TreePtToHostWndPt(0, 0))
		Helper:SetRegValue(iWindowPosXReg, x)
		Helper:SetRegValue(iWindowPosYReg, y)
		Helper:SetRegValue(iWindowPosDXReg, x+width)
		Helper:SetRegValue(iWindowPosDYReg, y+height)
	end
end

function OnMove(self)
	local state = self:GetWindowState()
	if "max" == state or "min" == state then
		return
	end
	local wndleft,wndtop,wndright,wndbottom = self:GetWindowRect()
	local wndwidth = wndright - wndleft
	local wndheight = wndbottom - wndtop
		
	local x, y = self:HostWndPtToScreenPt(self:TreePtToHostWndPt(0, 0))
	
	Helper:SetRegValue(iWindowPosXReg, x)
	Helper:SetRegValue(iWindowPosYReg, y)
	Helper:SetRegValue(iWindowPosDXReg, x+wndwidth)
	Helper:SetRegValue(iWindowPosDYReg, y+wndheight)
end

