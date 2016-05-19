local Tray = {}
local Helper = XLGetGlobal("Helper")
local LOG = XLGetGlobal("LOG")
local tipUtil = XLGetObject("API.Util")
Helper.Tray = Tray

local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")

function Tray.Open()
	local strState = Tray.HostWnd:GetWindowState()
	if tostring(strState) == "min" then
		Tray.HostWnd:BringWindowToTop(true)
	end
end

function Tray.Setting()
	LOG("Tray.Setting:enter")
end

function Tray.Sysboot()
	LOG("Tray.Sysboot:enter")
end

function Tray.Suggestion()
	LOG("Tray.Suggestion:enter")
end

function Tray.Exit()
	LOG("Tray.Exit:enter")
end

Tray.HostWndName = "KuaiKnaTrayMenu.MainFrame"
Tray.MenuContent = {
	{id="tray.open", text="打开快看", OnSelectFun = Tray.Open},
	{id="tray.setting", text="软件设置...", OnSelectFun = Tray.Setting},
	{id="tray.sysboot", text="开机启动", OnSelectFun = Tray.Sysboot},
	{id="tray.suggestion", text="意见反馈", iconNormalID = "setting_check.icon", OnSelectFun = Tray.Suggestion},
	{id="tray.exit", text="退出", OnSelectFun = Tray.Exit},
}

function Tray.PopMenu(hostwnd)
	local x, y = tipUtil:GetCursorPos()
	Helper:CreateMenu(x, y, hostwnd:GetWndHandle(), Tray.MenuContent)
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

function Tray.Init(hostwnd)
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
        		Tray.PopMenu(Tray.HostWnd)
			end
		end
		
		--单击左键
		if event3 == 0x0202 then
			Tray.Open(Tray.HostWnd)	
		end
		
		--点击气泡
		if event3 == 1029 then
			Tray.Open(Tray.HostWnd)	
		end
		--mousemove
		if event3 == 512 then
			Tray.Update()
		end
	end
	Tray.TrayObject:Attach(OnTrayEvent)
	Tray.Update()
	Tray.TrayObject:Show()
end

function Tray.Hide()
	local TrayObject = Tray.TrayObject
	if TrayObject then
		TrayObject:Hide()
	end
end
