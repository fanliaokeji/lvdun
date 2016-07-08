local Tray = {}
local Helper = XLGetGlobal("Helper")
local LOG = XLGetGlobal("LOG")
local tipUtil = XLGetObject("API.Util")
Helper.Tray = Tray
local Setting = Helper.Setting

local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")

function Tray.Open()
	Tray.HostWnd:BringWindowToTop(true)
end

function Tray.Setting()
	LOG("Tray.Setting:enter")
	Helper:CreateModalWnd("SettingWnd","SettingWndTree", Tray.HostWnd)
end

function Tray.Sysboot()
	LOG("Tray.Sysboot:enter")
	Setting.SetSysBoot(not Setting.IsSysBoot())
end

function Tray.Suggestion()
	LOG("Tray.Suggestion:enter")
end

function Tray.Exit()
	local function realexit()
		Tray.HostWnd:Show(0)
		--退出
		StatUtil.Exit()
	end
	local clientobj = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	if clientobj then
		clientobj:HandleRotateExit(realexit)
	else
		realexit()
	end
	
end

Tray.HostWndName = "MenuHostWnd.Instance"

function Tray.PopMenu()
	CreateTrayTipWnd(Tray.HostWnd)
end

function Tray.Update(strText)
	if not Tray.TrayObject then
		return
	end
	local trayobj = Tray.TrayObject
	trayobj:ShowNotifyIconTip(false)
	local strImagePath = __document .. "\\..\\..\\..\\..\\..\\res\\shortcut.ico"
	trayobj:SetIcon(strImagePath, strText or "快看")
end

function Tray.Init(hostwnd, needshow)
	if not hostwnd then
		LOG("Tray.Init: hostwnd = nil")
		return
	end
	Tray.HostWnd = hostwnd
	Tray.TrayObject = XLGetObject("Notify.Icon")
	if not Tray.TrayObject then
		LOG("Tray.Init: not support NotifyIcon")
	    return
	end
	function OnTrayEvent(event1,event2,event3,event4)
		--单击右键,创建并显示菜单
		if event3 == 517 then
			local newWnd = hostwndManager:GetHostWnd(Tray.HostWndName)	
			if not newWnd then
        		Tray.PopMenu()
			end
		end
		
		--单击左键
		if event3 == 0x0202 then
			Tray.Open()	
		end
		
		--点击气泡
		if event3 == 1029 then
			Tray.Open()
			Helper:CreateModalWnd("SettingWnd","SettingWndTree", Tray.HostWnd, "文件关联")
		end
		--mousemove
		if event3 == 512 then
			Tray.Update()
		end
	end
	Tray.TrayObject:Attach(OnTrayEvent)
	Tray.Update()
	if needshow then
		Tray.Show()
	else
		Tray.Hide()
	end
	if type(Tray.fnEventInit) == "function" then
		Tray.fnEventInit()
	end
end

function Tray.Hide()
	Tray.bHide = true
	local TrayObject = Tray.TrayObject
	if TrayObject then
		TrayObject:Hide()
	end
end

function Tray.Show()
	Tray.bHide = false
	local TrayObject = Tray.TrayObject
	if TrayObject then
		TrayObject:Show()
	end
end


function CreateTrayTipWnd(objHostWnd)
	local uTempltMgr = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local uHostWndMgr = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local uObjTreeMgr = XLGetObject("Xunlei.UIEngine.TreeManager")

	if uTempltMgr and uHostWndMgr and uObjTreeMgr then
		local uHostWnd = nil
		local strHostWndTempltName = "TipTrayWnd"
		local strHostWndTempltClass = "HostWndTemplate"
		local strHostWndName = "GSTrayMenuHostWnd.MainFrame"
		local uHostWndTemplt = uTempltMgr:GetTemplate(strHostWndTempltName, strHostWndTempltClass)
		if uHostWndTemplt then
			uHostWnd = uHostWndTemplt:CreateInstance(strHostWndName)
		end

		local uObjTree = nil
		local strObjTreeTempltName = "TrayMenuTree"
		local strObjTreeTempltClass = "ObjectTreeTemplate"
		local strObjTreeName = "GSTrayMenuWnd.MainObjectTree"
		local uObjTreeTemplt = uTempltMgr:GetTemplate(strObjTreeTempltName, strObjTreeTempltClass)
		if uObjTreeTemplt then
			uObjTree = uObjTreeTemplt:CreateInstance(strObjTreeName)
		end

		if uHostWnd and uObjTree then
			--函数会阻塞
			local bSucc = ShowPopupMenu(uHostWnd, uObjTree)
			
			if bSucc and uHostWnd:GetMenuMode() == "manual" then
				uObjTreeMgr:DestroyTree(strObjTreeName)
				uHostWndMgr:RemoveHostWnd(strHostWndName)
			end
		end
	end
end


function ShowPopupMenu(uHostWnd, uObjTree)
	uHostWnd:BindUIObjectTree(uObjTree)
					
	local nPosCursorX, nPosCursorY = tipUtil:GetCursorPos()
	if type(nPosCursorX) ~= "number" or type(nPosCursorY) ~= "number" then
		return false
	end
	
	local nScrnLeft, nScrnTop, nScrnRight, nScrnBottom = tipUtil:GetScreenArea()
	
	local objMainLayout = uObjTree:GetUIObject("TrayMenu.Main")
	if not objMainLayout then
	    return false
	end	
		
	local nL, nT, nR, nB = objMainLayout:GetObjPos()				
	local nMenuContainerWidth = nR - nL
	local nMenuContainerHeight = nB - nT
	local nMenuScreenLeft = nPosCursorX
	local nMenuScreenTop = nPosCursorY - nMenuContainerHeight
	
	if nMenuScreenLeft+nMenuContainerWidth > nScrnRight - 10 then
		nMenuScreenLeft = nPosCursorX - nMenuContainerWidth
	end
	
	-- uHostWnd:SetFocus(false) --先失去焦点，否则存在菜单不会消失的bug
	
	--函数会阻塞
	local bOk = uHostWnd:TrackPopupMenu(objHostWnd, nMenuScreenLeft, nMenuScreenTop, nMenuContainerWidth, nMenuContainerHeight)
	
	return bOk
end