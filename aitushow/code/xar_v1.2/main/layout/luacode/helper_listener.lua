local Helper = XLGetGlobal("Helper")
Helper.Listener = {}
local Listener = Helper.Listener
local LOG = XLGetGlobal("LOG")
local Selector = Helper.Selector
local tipUtil = XLGetObject("API.Util")

local VK_PRIOR 	= 	0x21--pageUp
local VK_NEXT 	= 	0x22--pageDown
local VK_END 	= 	0x23
local VK_HOME 	= 	0x24
local VK_LEFT 	= 	0x25
local VK_UP 	= 	0x26
local VK_RIGHT 	= 	0x27
local VK_DOWN 	= 	0x28
local VK_ESC	=	0x1B
local VK_DELETE	=	0x2E

function Listener.Init()
	tipUtil:SetKeyboardHook()
	local objFactory = XLGetObject("APIListen.Factory")
	if objFactory then
		local objListen = objFactory:CreateInstance()	
		objListen:AttachListener(
			function(key,...)
				local tParam = {...}	
				if tostring(key) == "OnCommandLine" then
					Listener.OnCommandLine(tParam)
				elseif tostring(key) == "OnKeyDown" then
					Listener.OnKeyDown(tParam)
				end
			end)
	else
		LOG("[Listener.Init] failed")
	end
end
Listener.Init()

function Listener.OnCommandLine(tParam)
	--如果设置了退出标记则不处理, 并强制退出
	if StatUtil.ExitFlag then
		StatUtil.Exit(true)
		LOG("Listener.OnCommandLine, StatUtil.ExitFlag = "..tostring(StatUtil.ExitFlag))
		return
	end
	local strCmd = tostring(tParam[1])
	--打开本地文件
	strCmd = string.gsub(string.lower(strCmd), "^[\"\']?[^%.]+%.exe[\"\']?", "")
	strCmd = tostring(strCmd)
	LOG("tostring(tParam[1]) = "..tostring(tParam[1])..", strCmd = "..tostring(strCmd))
	local filepath = string.match(strCmd, "\"([^\"]+)\"[^\"]*$")
	LOG("Listener.OnCommandLine: filepath: ", filepath)
	if not filepath or not tipUtil:QueryFileExists(filepath) then
		filepath = strCmd
	end
	if filepath and tipUtil:QueryFileExists(filepath) then
		local imgctrl = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
		if not imgctrl then
			Helper:CreateModelessWnd("Kuaikan.MainWnd","Kuaikan.MainObjTree")
			imgctrl = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
		end
		if not imgctrl then
			LOG("Listener.OnCommandLine: not imgctrl: ")
			return
		end
		LOG("Listener.OnCommandLine: SetImagePath: ", filepath)
		imgctrl:LoadImageFile(filepath, nil, nil, function() imgctrl:UpdateFileList() end)
		--如果存在主窗口，隐藏主窗口
		local MainHostWnd = Helper.Selector.select("", "", "MainWnd.Instance")
		if MainHostWnd and MainHostWnd:GetWindowState() ~= "hide" then
			Listener.LastShowWnd = MainHostWnd
		else
			Listener.LastShowWnd = nil
		end
		if MainHostWnd then
			MainHostWnd:Show(0)
		end
		--将图片窗口放到最前
		local ImgHostWnd = Helper.Selector.select("", "", "Kuaikan.MainWnd.Instance")
		if ImgHostWnd then
			ImgHostWnd:BringWindowToTop(true)
		end
	else
		local MainHostWnd = Helper.Selector.select("", "", "MainWnd.Instance")
		local ImgHostWnd = Helper.Selector.select("", "", "Kuaikan.MainWnd.Instance")
		--其它命令行将主界面放到最前
		if ImgHostWnd and ImgHostWnd:GetWindowState() ~= "hide" then
			Listener.LastShowWnd = ImgHostWnd
		else
			Listener.LastShowWnd = nil
		end
		if ImgHostWnd then
			ImgHostWnd:Show(0)
		end
		if not MainHostWnd then
			MainHostWnd = Helper:CreateModelessWnd("MainWnd","MainWndTree")
		end
		if MainHostWnd then
			MainHostWnd:BringWindowToTop(true)
		end
	end
	--[[if string.find(strCmd, "embedding") then
		--需要隐藏到托盘
		local MainHostWnd = Helper.Selector.select("", "", "MainWnd.Instance")
		if MainHostWnd then
			MainHostWnd:Show(0)
		end
		local ImgHostWnd = Helper.Selector.select("", "", "ImageWnd.Instance")
		if ImgHostWnd then
			ImgHostWnd:Show(0)
		end
	end]]--
end

function Listener.OnKeyDown(tParam)
	--如果设置了退出标记则不处理
	if StatUtil.ExitFlag then
		LOG("Listener.OnCommandLine, StatUtil.ExitFlag = "..tostring(StatUtil.ExitFlag))
		return
	end
	local imageCtrl = Helper.Selector.select("", "mainwnd.client", "Kuaikan.MainWnd.Instance")
	if not imageCtrl or not imageCtrl:GetVisible() then
		LOG("Listener.OnKeyDown failed")
		return
	end
	if tParam[2] == VK_LEFT then
		imageCtrl:PreFile()
	elseif tParam[2] == VK_RIGHT then
		imageCtrl:NextFile()
	elseif tParam[2] == VK_ESC then
		if imageCtrl:IsFullScreen() then
			imageCtrl:FullScreenSwitch()
		else
			local sysbtn = imageCtrl:GetObject("client.caption:client.caption.sysbtn")
			sysbtn:FireExtEvent("OnClose")
		end
	elseif tParam[2] == VK_DELETE then
		imageCtrl:DeleteFile()
	end
end

