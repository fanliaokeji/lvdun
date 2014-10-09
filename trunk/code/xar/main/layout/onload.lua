local gTipInfoTab = {}
local gFilterConfigInfo = {}
local gRuleInfo = {}

local gbLoadCfgSucc = false
local tipUtil = XLGetObject("GS.Util")
-----------------

function RegisterFunctionObject(self)
	local function FailExitTipWnd(self, iExitCode)
		ExitTipWnd()
	end
	local function ShowTipWnd(self,iShow)
		local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
		local frameHostWnd = hostwndManager:GetHostWnd("GreenWallTipWnd.MainFrame")
	
		frameHostWnd:Show(4)
	end
	
	local function TipConvStatistic(tStatInfo)
		ExitTipWnd()
	end
	
	local obj = {}
	obj.FailExitTipWnd = FailExitTipWnd
	obj.GetFailExitCode = GetFailExitCode
	obj.ShowTipWnd = ShowTipWnd
	obj.TipConvStatistic = TipConvStatistic
	obj.SaveUserConfigToFile = SaveUserConfigToFile
	obj.GetUserConfigFromFile = GetUserConfigFromFile
	obj.SaveUserConfigToMem = SaveUserConfigToMem
	obj.GetUserConfigFromMem = GetUserConfigFromMem
	obj.GetSpecifyFilterTableFromMem = GetSpecifyFilterTableFromMem
	obj.SaveSpecifyFilterTableToMem = SaveSpecifyFilterTableToMem
	obj.GetRuleListFromMem = GetRuleListFromMem
	obj.SaveRuleListToMem = SaveRuleListToMem
	obj.UIAutoEnableDomain = UIAutoEnableDomain

	XLSetGlobal("GreenWallTip.FunctionHelper", obj)
end


function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end


function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@greenwall_Template MainTipLog: " .. tostring(strLog))
	end
end


function QueryAllUsersDir()	--获取AllUser路径
	local bRet = false
	local strPublicEnv = "%PUBLIC%"
	local strRet = tipUtil:ExpandEnvironmentStrings(strPublicEnv)
	if strRet == nil or strRet == "" or strRet == strPublicEnv then
		local nCSIDL_COMMON_APPDATA = 35 --CSIDL_COMMON_APPDATA(0x0023)
		strRet = tipUtil:GetSpecialFolderPathEx(nCSIDL_COMMON_APPDATA)
	end
	if not IsNilString(strRet) and tipUtil:QueryFileExists(strRet) then
		bRet = true
	end
	return bRet, strRet
end


function SaveAllConfig()
	if gbLoadCfgSucc then
		SaveUserConfigToFile()
		SaveFilterConfigToFile()
		SaveRuleListToFile()
	end
end


function ExitTipWnd(statInfo)
	SaveAllConfig()

	TipLog("************ Exit ************")
	tipUtil:Exit("Exit")
end

function LoadTableFromFile(strDatFilePath)
	local tResult = nil

	if IsRealString(strDatFilePath) and tipUtil:QueryFileExists(strDatFilePath) then
		local tMod = XLLoadModule(strDatFilePath)
		if type(tMod) == "table" and type(tMod.GetSubTable) == "function" then
			local tDat = tMod.GetSubTable()
			if type(tDat) == "table" then
				tResult = tDat
			end
		end
	end
	
	return tResult
end

function PopTipWnd(OnCreateFunc)
	local bSuccess = false
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate("TipMainWnd", "HostWndTemplate" )
	local frameHostWnd = nil
	if frameHostWndTemplate then
		frameHostWnd = frameHostWndTemplate:CreateInstance("GreenWallTipWnd.MainFrame")
		if frameHostWnd then
			local objectTreeTemplate = nil
			objectTreeTemplate = templateMananger:GetTemplate("TipPanelTree", "ObjectTreeTemplate")
			if objectTreeTemplate then
				local uiObjectTree = objectTreeTemplate:CreateInstance("GreenWallTipWnd.MainObjectTree")
				if uiObjectTree then
					frameHostWnd:BindUIObjectTree(uiObjectTree)
					local ret = OnCreateFunc(uiObjectTree)
	
					if ret then
						local iRet = frameHostWnd:Create()
						if iRet ~= nil and iRet ~= 0 then
							bSuccess = true
							local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
							FunctionObj:ShowTipWnd(1)
						end
					end
				end
			end
		end
	end
	if not bSuccess then
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd(7)
	end
	
	---初始化托盘
    if frameHostWnd then
	    InitTrayTipWnd(frameHostWnd)
	end
end


function InitTrayTipWnd(objHostWnd)
    if not objHostWnd then
	    TipLog("[InitTrayTipWnd] para error")
	    return
	end

	--创建托盘
    local tipNotifyIcon = XLGetObject("GS.NotifyIcon")
	if not tipNotifyIcon then
		TipLog("[InitTrayTipWnd] not support NotifyIcon")
	    return
	end
    tipNotifyIcon:SetIcon(nil,"绿盾广告管家")
	tipNotifyIcon:Show()
	
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	
	----托盘事件响应
	function OnTrayEvent(event1,event2,event3,event4)
		local newWnd = hostwndManager:GetHostWnd("GSTrayMenuHostWnd.MainFrame")	
		if event3 ~= 512 then
			-- -- 传来其他消息时，删除窗口
			if newWnd then
				local tree = newWnd:GetBindUIObjectTree()
				local status_ctrl = tree:GetUIObject("TrayMenu.Main")
				status_ctrl:SetVisible(false)
				status_ctrl:SetChildrenVisible(false)
			end
		end
		
		--mousemove
		-- if event3 == 512 then
			-- if newWnd then
				-- local tree = newWnd:GetBindUIObjectTree()
				-- local status_ctrl = tree:GetUIObject("TrayMenu.Main")
				-- status_ctrl:SetVisible(true)
				-- status_ctrl:SetChildrenVisible(true)
			-- end
		-- end	

		-- 失去焦点
		if event3 == 8 then
		XLMessageBox(11)
			if newWnd then
				local tree = newWnd:GetBindUIObjectTree()
				local status_ctrl = tree:GetUIObject("TrayMenu.Main")
				status_ctrl:SetVisible(false)
				status_ctrl:SetChildrenVisible(false)
			end
		end
		
		--单击右键,创建并显示菜单
		if event3 == 517 then
		local bRet = objHostWnd:GetEnable()
			if bRet then
				-- _G["gShowMenu"] = true
				-- _G["NotShowStatusTip"] = true

				-- local obj = XLGetGlobal("xunlei.LuaHostWndHelper")
				-- XLSetGlobal("Thunder.ShowTrayMenu", 1)
				--obj:ShowPlatformMenu("single.menu", "tree.tray.menu.context", "context_menu", 0)
				--objHostWnd:BringWindowToTop(true)
        		CreateTrayTipWnd(objHostWnd)
			else
				-- objHostWnd:SetVisible(true)
				objHostWnd:BringWindowToTop(true)
	            XLMessageBox(tostring("objHostWnd:GetEnable() not "))			
			end
		end
		
		--双击左键
		if event3 == 0x0203 then
			-- objHostWnd:SetVisiable(true)
			objHostWnd:BringWindowToTop(true)
		end
	end

	tipNotifyIcon:Attach(OnTrayEvent)
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
			uHostWnd:BindUIObjectTree(uObjTree)
							
			local nPosCursorX, nPosCursorY = tipUtil:GetCursorPos()
			if type(nPosCursorX) ~= "number" or type(nPosCursorY) ~= "number" then
				return 
			end
			
			local objMainLayout = uObjTree:GetUIObject("TrayMenu.Main")
			if not objMainLayout then
			    return
			end	
				
			local nL, nT, nR, nB = objMainLayout:GetObjPos()				
			local nMenuContainerWidth = nR - nL
			local nMenuContainerHeight = nB - nT
			local nMenuScreenLeft = nPosCursorX
			local nMenuScreenTop = nPosCursorY - nMenuContainerHeight
			TipLog("[ShowTrayCtrlPanel] about to popup menu")
			--函数会阻塞
			local bOk = uHostWnd:TrackPopupMenu(objHostWnd, nMenuScreenLeft, nMenuScreenTop, nMenuContainerWidth, nMenuContainerHeight)
			TipLog("[CreateTrayTipWnd] end menu")

			uObjTreeMgr:DestroyTree(strObjTreeName)
			uHostWndMgr:RemoveHostWnd(strHostWndName)
		end
	end
end

-------------------------------

function FetchValueByPath(obj, path)
	local cursor = obj
	for i = 1, #path do
		cursor = cursor[path[i]]
		if cursor == nil then
			return nil
		end
	end
	return cursor
end


function SaveUserConfigToMem(tNewConfig)
	gTipInfoTab = tNewConfig
end

function GetUserConfigFromMem()
	return gTipInfoTab
end

function GetRuleListFromMem()
	return gRuleInfo
end

function SaveRuleListToMem(tNewRuleList)
	gRuleInfo = tNewRuleList
end


function GetCfgPathWithName(strCfgName)
	local bOk, strBaseDir = QueryAllUsersDir()
	if not bOk then
		return ""
	end
	
	local strCfgFilePath = tipUtil:PathCombine(strBaseDir, "GreenShield\\"..tostring(strCfgName))
	return strCfgFilePath or ""
end


function GetUserCfgPath()
	return GetCfgPathWithName("UserConfig.dat")
end

function GetFilterCfgPath()
	return GetCfgPathWithName("FilterConfig.dat")
end

function GetRuleListCfgPath()
	return GetCfgPathWithName("RuleList.dat")
end


function GetSpecifyFilterTableFromMem(strTableName)
	if not IsRealString(strTableName) then
		return gFilterConfigInfo
	end

	return gFilterConfigInfo[strTableName]
end

function SaveSpecifyFilterTableToMem(tNewTable, strTableName)
	if not IsRealString(strTableName) then
		gFilterConfigInfo = tNewTable
	else
		gFilterConfigInfo[strTableName] = tNewTable
	end
end

function SaveUserConfigToFile()
	local strUserCfgPath = GetUserCfgPath()
	if IsRealString(strUserCfgPath) then
		tipUtil:SaveLuaTableToLuaFile(gTipInfoTab, strUserCfgPath)
	end
end

function GetUserConfigFromFile()
	local strUserCfgPath = GetUserCfgPath()
	local infoTable = LoadTableFromFile(strUserCfgPath)
	return infoTable
end


function GetFilterConfigFromFile()
	local strFilterCfgPath = GetFilterCfgPath()
	local infoTable = LoadTableFromFile(strFilterCfgPath)
	return infoTable
end

function SaveFilterConfigToFile()
	local strFilterCfgPath = GetFilterCfgPath()
	if IsRealString(strFilterCfgPath) then
		tipUtil:SaveLuaTableToLuaFile(gFilterConfigInfo, strFilterCfgPath)
	end
end

function GetRuleListFromFile()
	local strRuleListPath = GetRuleListCfgPath()
	local tRuleList = LoadTableFromFile(strRuleListPath)
	return tRuleList
end

function SaveRuleListToFile()
	local strRuleListPath = GetRuleListCfgPath()
	if IsRealString(strRuleListPath) then
		tipUtil:SaveLuaTableToLuaFile(gRuleInfo, strRuleListPath)
	end
end


function ReadAllConfigInfo()	
	local infoTable = GetUserConfigFromFile()
	if type(infoTable) ~= "table" then
		TipLog("[ReadAllConfigInfo] GetUserConfigFromFile failed! ")
		return false
	end
		
	local tFilterTable = GetFilterConfigFromFile()
	if type(tFilterTable) ~= "table" then
		TipLog("[ReadAllConfigInfo] GetFilterConfigFromFile failed! ")
		return false
	end
	
	local tRuleListTable = GetRuleListFromFile()
	if type(tFilterTable) ~= "table" then
		TipLog("[ReadAllConfigInfo] GetRuleListFromFile failed! ")
		return false
	end
			
	gTipInfoTab = infoTable
	gFilterConfigInfo = tFilterTable
	gRuleInfo = tRuleListTable
	
	gbLoadCfgSucc = true	
	TipLog("[ReadAllConfigInfo] success!")
	return true
end


function SendFileDateToFilterThread()
	local bSucc = SendRuleListToFilterThread()
	if not bSucc then
		return false
	end
	
	local bSucc = SendEnableListToFilterThread()
	if not bSucc then
		return false
	end
	
	
	return true
end


function SendRuleListToFilterThread()
	local tRuleList = gRuleInfo
	
	for key, tRuleElem in pairs(tRuleList) do
		if type(tRuleElem) == "table" then
			local strDomain = tRuleElem["strDomain"]
			local tExtraPath = tRuleElem["tExtraPath"]
			if IsRealString(strDomain) and type(tExtraPath) == "table" then
				tipUtil:AddDomain(strDomain, tExtraPath)
			end
		end
	end
	
	return true
end


function SendEnableListToFilterThread()
	local tRuleList = gRuleInfo
	
	for key, tRuleElem in pairs(tRuleList) do 
		if type(tRuleElem) == "table" then
			local strDomain = tRuleElem["strDomain"]
			UIAutoEnableDomain(strDomain)
		end
	end

	return true
end

function UIAutoEnableDomain(strDomain, bForceDisable)
	if IsRealString(strDomain) then
		if bForceDisable then
			tipUtil:EnableDomain(strDomain, false)
			return
		end
	
		if IsDomainInWhiteList(strDomain) then
			tipUtil:EnableDomain(strDomain,false)
			return
		end
		
		local bBlackState = GetDomainBlackState(strDomain)
		if bBlackState == 1 then
			tipUtil:EnableDomain(strDomain, true)
		elseif bBlackState == 2 then
			tipUtil:EnableDomain(strDomain, false)
		end
	end
end


function IsDomainInWhiteList(strDomain)
	local tWhiteList = GetSpecifyFilterTableFromMem("tWhiteList")
	for key, tWhiteElem in pairs(tWhiteList) do
		local strWhiteDomain = tWhiteElem["strDomain"]
		local bStateOpen = tWhiteElem["bState"]
		if IsRealString(strWhiteDomain) and bStateOpen
			and string.find(strWhiteDomain, strDomain) then
			
			return true
		end
		
	end
	return false
end

--0 不在黑名单
--1 在黑名单，且开启过滤
--2 在黑名单，且关闭过滤
function GetDomainBlackState(strDomain)
	local tBlackList = GetSpecifyFilterTableFromMem("tBlackList")
	for key, tBlackElem in pairs(tBlackList) do
		local strBlackDomain = tBlackElem["strDomain"]
		local bStateOpen = tBlackElem["bState"]
		if IsRealString(strBlackDomain) and string.find(strBlackDomain, strDomain) then
			if bStateOpen then
				return 1
			else
				return 2
			end
		end
	end
	
	return 0
end


function CreateMainTipWnd()
	local function OnCreateFuncF(treectrl)
		local rootctrl = treectrl:GetUIObject("root.layout:root.ctrl")
		local bRet = rootctrl:SetTipData(gTipInfoTab)			
		if not bRet then
			return false
		end
	
		return true
	end
	
	PopTipWnd(OnCreateFuncF)	
end


function CreatePopupTipWnd()
	local bSuccess = false
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate("TipPopupWnd", "HostWndTemplate" )
	if frameHostWndTemplate then
		local frameHostWnd = frameHostWndTemplate:CreateInstance("GreenWallPopupWnd.MainFrame")
		if frameHostWnd then
			local objectTreeTemplate = nil
			objectTreeTemplate = templateMananger:GetTemplate("TipPopupWndTree", "ObjectTreeTemplate")
			if objectTreeTemplate then
				local uiObjectTree = objectTreeTemplate:CreateInstance("GreenWallPopupWnd.MainObjectTree")
				if uiObjectTree then
					frameHostWnd:BindUIObjectTree(uiObjectTree)
					local iRet = frameHostWnd:Create()
					if iRet ~= nil and iRet ~= 0 then
						bSuccess = true
					end
				end
			end
		end
	end
	if not bSuccess then
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd()
	end
end

function StartTimer()
	local nTimeSpanInMs = 3600 * 1000
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		SaveAllConfig()
	end, nTimeSpanInMs)
end


function TipMain() 
	RegisterFunctionObject()
	local bSuccess = ReadAllConfigInfo()	
	if not bSuccess then
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd()
		return
	end

	local bSucc = SendFileDateToFilterThread()
	if not bSucc then
		local FunctionObj = XLGetGlobal("GreenWallTip.FunctionHelper")
		FunctionObj:FailExitTipWnd()
		return
	end
	
	CreateMainTipWnd()
	CreatePopupTipWnd()
	StartTimer()
		--tipUtil:GetPeerId();
end


TipMain()
