function CreateMainWnd()
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate("Wizard.MainWnd","HostWndTemplate")
	if frameHostWndTemplate == nil then
		return
	end

	local frameHostWnd = frameHostWndTemplate:CreateInstance("Wizard.MainWnd.Instance")
	if frameHostWnd == nil then
		return
	end
	local objectTreeTemplate = templateMananger:GetTemplate("Wizard.MainObjTree","ObjectTreeTemplate")
	if objectTreeTemplate == nil then
		return
	end


	local uiObjectTree = objectTreeTemplate:CreateInstance("Wizard.MainObjTree.Instance")
	if uiObjectTree == nil then
		return
	end

	frameHostWnd:BindUIObjectTree(uiObjectTree)
	
	local app = XLGetObject("Wizard.App")
	
	-- ��ȡ����ѡ������� �������Ƿ�͸�� ������Ӧ�ĳ�ʼ��
	local opaque = app:GetInt("ConfigGraphics", "ConfigGraphics_MainWndOpaque", 0)
	if opaque == 1 then
		frameHostWnd:SetLayered(false)
	else
		--��ȡ͸����
		local alpha = app:GetInt("ConfigGraphics", "ConfigGraphics_MainWndAlpha", 200)
		local bkg = uiObjectTree:GetUIObject("app.bkg")
		bkg:SetBkgAlpha(alpha)
	end

	-- ��ȡ����ѡ������� �����Ƿ���ë����Ч��
	local enableBlur = app:GetInt("ConfigGraphics", "ConfigGraphics_EnableBlur", 0)
	
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	if opaque == 0 and enableBlur == 1 then
		hostwndManager:SetSystemBlur(true)
	else
		hostwndManager:SetSystemBlur(false)
	end
	
	frameHostWnd:Create()
	
	return frameHostWnd
end

local function OnConfigChange(section, key, value)
	
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local mainWnd = hostwndManager:GetHostWnd("Wizard.MainWnd.Instance")
	
	if section == "ConfigGraphics" then
		if key == "ConfigGraphics_MainWndOpaque" then
			if value == 0 then
				mainWnd:SetLayered(true)
				local enableBlur = app:GetInt("ConfigGraphics", "ConfigGraphics_EnableBlur", 0)
				if enableBlur == 1 then
					hostwndManager:SetSystemBlur(true)
				end
			else
				mainWnd:SetLayered(false)
			end
		elseif key == "ConfigGraphics_EnableBlur" then
			if value == 0 then
				hostwndManager:SetSystemBlur(false)
			else
				hostwndManager:SetSystemBlur(true)
			end
		end
	end
end

local app = XLGetObject("Wizard.App")
app:AttachListener("ConfigChange", OnConfigChange)

local hostwnd = CreateMainWnd()
if hostwnd ~= nil then
end