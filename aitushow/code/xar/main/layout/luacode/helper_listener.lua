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
	local strCmd = tostring(tParam[1])
	if string.find(string.lower(strCmd), "/sstartfrom%s+localfile") then
		--打开本地文件
		local filepath = string.match(strCmd, "\"([^\"]+)\"[^\"]*$")
		if not filepath or not tipUtil:QueryFileExsits(filepath) then
			return
		end
		local imgctrl = Helper.Selector.select("", "FrameWnd.ImageCtrl", "ImageWnd.Instance")
		if not imgctrl then
			Helper:CreateModelessWnd("ImageWnd","ImageWndTree")
			imgctrl = Helper.Selector.select("", "FrameWnd.ImageCtrl", "ImageWnd.Instance")
		end
		if not imgctrl then
			return
		end
		imgctrl:SetImagePath(filepath)
		--隐藏主窗口
		local MainHostWnd = Helper.Selector.select("", "", "MainWnd.Instance")
		if MainHostWnd then
			MainHostWnd:Show(0)
		end
	else
		local MainHostWnd = Helper.Selector.select("", "", "MainWnd.Instance")
		local ImgHostWnd = Helper.Selector.select("", "", "ImageWnd.Instance")
		if ImgHostWnd and not MainHostWnd then
			ImgHostWnd:BringWindowToTop(true)
		elseif MainHostWnd and not ImgHostWnd then
			MainHostWnd:BringWindowToTop(true)
		elseif MainHostWnd and ImgHostWnd then
			local bmv, biv = MainHostWnd:GetVisible(), ImgHostWnd:GetVisible()
			if bmv and not biv then
				MainHostWnd:BringWindowToTop(true)
			elseif biv and not bmv then
				ImgHostWnd:BringWindowToTop(true)
			elseif biv and bmv then
				MainHostWnd:Show(0)
				ImgHostWnd:BringWindowToTop(true)
			else
				MainHostWnd:BringWindowToTop(true)
			end
		end
	end
	if string.find(strCmd, "embedding") then
		--需要隐藏到托盘
		local MainHostWnd = Helper.Selector.select("", "", "MainWnd.Instance")
		if MainHostWnd then
			MainHostWnd:Show(0)
		end
		local ImgHostWnd = Helper.Selector.select("", "", "ImageWnd.Instance")
		if ImgHostWnd then
			ImgHostWnd:Show(0)
		end
	end
end

function Listener.OnKeyDown(tParam)
	local imageCtrl = Selector.select("", "FrameWnd.ImageCtrl", "ImageWnd.Instance")
	if not imageCtrl or not imageCtrl:GetVisible() then
		LOG("Listener.OnKeyDown failed")
		return
	end
	if tParam[2] == VK_LEFT then
		local picData, prevIndex = imageCtrl:GetPrevPic()
		if prevIndex then
			imageCtrl:SetImageByIndex(prevIndex)
		end
	elseif tParam[2] == VK_RIGHT then
		local picData, nextIndex = imageCtrl:GetNextPic()
		if nextIndex then
			imageCtrl:SetImageByIndex(nextIndex)
		end
	end
end

