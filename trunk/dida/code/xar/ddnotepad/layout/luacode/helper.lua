--该lua中只定义相对独立的一些方法，不依赖于具体工程
--依赖：本lua依赖tipUtil对象的几个方法，工程间移植时，
--应保证tipUtil对象至少提供以下几个方法：
--Log、IsLogEnable、QueryFileExists、GetFileSize、SaveLuaTableToLuaFile
local ObjectBase = XLGetGlobal("ObjectBase")
local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")

function LOG(...)--该方法使用频率较高，单独挂到全局一份
	if type(tipUtil.Log) ~= "function" then
		return
	end
	
	local printResult = ""
	for i = 1, #arg do
		printResult = printResult..tostring(arg[i])
	end
	
	local traceInfo = debug.traceback("", 2)
	local s = string.gsub(traceInfo, "\n", " < ") 
	s =string.sub(s,1, 512)
	
	--应控制调用堆栈长度与日志内容长度
	tipUtil:Log(printResult..s)
end
XLSetGlobal("LOG", LOG)

--全局的事件，方适合用Helper的AddListener、DispatchEvent处理
local Helper = ObjectBase:New()
XLSetGlobal("Helper", Helper)

Helper.ObjectBase = ObjectBase
Helper.tipUtil = tipUtil
Helper.tipAsynUtil = tipAsynUtil

function Helper:LOG(...)
	LOG(...)
end

function Helper:Assert(condition, errmsg)
	if condition then
		return
	end
	
	errmsg = errmsg or "Assert Failed!"
	local info = debug.traceback(errmsg, 2)
	if tipUtil:IsLogEnable() then
		XLMessageBox(info)
	end
end

function Helper:IsRealString(str)
	return "string" == type(str) and str ~= ""
end

function Helper:LoadLuaTable(sFilePath)
	if type(str) ~= "string" or str == "" then
		return nil
	end
	
	if not tipUtil:QueryFileExists(sFilePath) then
		LOG("QueryFileExists false! sFilePath: ", sFilePath)
		return nil
	end
	
	--XLLoadModule加载空的lua文件会返回不确定的结果，这里要单独处理
	if tipUtil:GetFileSize(sFilePath) <= 0 then
		LOG("GetFileSize is 0! sFilePath: ", sFilePath)
		return nil
	end
	
	local tMod = XLLoadModule(sFilePath)
	if type(tMod) ~= "table" or type(tMod.GetSubTable) ~= "function" then
		LOG("tMod is not table or GetSubTable is not function")
		return nil
	end
	
	local tResult = tMod.GetSubTable()
	if "table" == type(tResult) then
		return tResult
	end
	
	LOG("tResult is not table")
	return nil
end

function Helper:SaveLuaTable(tTable, sFilePath)
	if type(tTable) ~= "table" then
		self:Assert(false, "The first parameter's type of function SaveLuaTable must be table")
		return
	end
	-- tipUtil提供的SaveLuaTableToLuaFile方法，若lua文件所在的目录不存在，则保存失败
	local folder = string.match(sFilePath, "(.+)\\.+")
	if not tipUtil:QueryFileExists(folder) then
		tipUtil:CreateDir(folder)
	end
	tipUtil:SaveLuaTableToLuaFile(tTable, sFilePath)
end

function Helper:LoadLuaModule(tFile, curDocPath)
--tFile可以传lua文件绝对路径、相对路径
	if "table" == type(tFile) then
		for index, value in ipairs(tFile) do
			if "string" == type(value) and value ~= "" then
				local dstPath = curDocPath.."\\..\\"..value
				if XLModuleExists(dstPath) then
					XLUnloadModule(dstPath)
					XLLoadModule(dstPath)
				else
					XLLoadModule(dstPath)
				end
				
			end
		end
	elseif "string" == type(tFile) and tFile ~= ""then
		if curDocPath then
			tFile = curDocPath.."\\..\\"..tFile
		end
		if XLModuleExists(tFile) then
			XLUnloadModule(tFile)
			XLLoadModule(tFile)
		else
			XLLoadModule(tFile)
		end
	end
end

function Helper:CreateReadOnlyTable(t)
	local proxy = {}
	local mt = { __index = t, __newindex = function (t, key, value) self:Assert(false, "Not Allow to Assign ReadOnly Table") end }
	setmetatable(proxy, mt)
	return proxy
end

function Helper:GetFileExt(sFilePath)--可以是文件名、路径、url
	if not sFilePath or "" == sFilePath then
		self:Assert(false, "The parameter or GetFileExt must be string")
		return
	end
	return string.match(sFilePath, ".*(%.%w+).*")
end

--UrlEncode、UrlDecode需大量测试
function Helper:UrlEncode(srcUrl)
	if type(srcUrl) ~= "string" or "" == srcUrl then
		self:Assert(false, "The parameter or UrlEncode must be string")
		return
	end
	
	srcUrl = string.gsub(srcUrl, "([%W])", function(c) 
			if c == "!" or c == "'" or c == "(" or c == ")" or c == "*" or c == "-" or c == "." or c == "_" or c == "~" then
				return
			end
			return string.format("%%%02X", string.byte(c)) 
	end)
    return srcUrl
end

function Helper:UrlDecode(srcUrl)
	if type(srcUrl) ~= "string" or "" == srcUrl then
		self:Assert(false, "The parameter or UrlDecode must be string")
		return
	end
	
	srcUrl = string.gsub(srcUrl, '%%(%x%x)', function(hex) return string.char(tonumber(hex, 16)) end)
    return string.gsub(srcUrl, "+", " ")
end

Helper.timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
Helper.treeManager = XLGetObject("Xunlei.UIEngine.TreeManager")
Helper.objectFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
Helper.hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
Helper.templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
Helper.objTreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")
Helper.xarManager = XLGetObject("Xunlei.UIEngine.XARManager")

function Helper:KillAllTimer()
	return self.timerManager:KillAllTimer()
end

Helper.iModalWndCount = 0
--userData, bAllowMulti, onDectoryFun, onBindFunc可无
function Helper:CreateModalWnd(wndTemplateID, treeTemplateID, parentWnd, userData, bAllowMulti, onDectoryFun, onBindFunc)
--onDectoryFun, 窗口关闭前，调用的回调函数，可处理ModalWnd中用户的选择。可也在窗体控件中处理
--userData, 调用者想传的数据
--bAllowMulti, 若想用一对wndTemplate + treeTemplate 创建多个模态框，将该值传true
	self.iModalWndCount = self.iModalWndCount + 1
	local modalWndTemplate = self.templateMananger:GetTemplate(wndTemplateID, "HostWndTemplate")
	if not modalWndTemplate then return end
	
	local modalWndID = wndTemplateID..".Instance"
	modalWndID = bAllowMulti and modalWndID..self.iModalWndCount or modalWndID
	LOG("modalWndID: ", modalWndID, " bAllowMulti: ", bAllowMulti)
	
	local modalWnd = modalWndTemplate:CreateInstance(modalWndID)
	if not modalWnd then return end
	
	local treeTemplate = self.templateMananger:GetTemplate(treeTemplateID,"ObjectTreeTemplate")
	if not treeTemplate then return end
	
	local treeID = treeTemplateID..".Instance"
	treeID = bAllowMulti and treeID..self.iModalWndCount or treeID
	LOG("treeID: ", treeID, " bAllowMulti: ", bAllowMulti)
	
	local tree = treeTemplate:CreateInstance(treeID)
	if not tree then return end
	
	--绑定对象树
	modalWnd:BindUIObjectTree(tree)
	if "function" == type(onBindFunc) then
		onBindFunc(modalWnd, tree)
	end
	
	--传入userData
	modalWnd:SetUserData(userData)
	
	local ret = modalWnd:DoModal(parentWnd)
	if "function" == type(onDectoryFun) then
		onDectoryFun(modalWnd, tree, ret)
	end
	
	--解绑
	modalWnd:UnbindUIObjectTree()
	--销毁树
	self.treeManager:DestroyTree(tree)--这里是传id还是tree，还是两者皆可。。。
	--销毁窗口
	self.hostWndManager:RemoveHostWnd(modalWndID)
	
	return ret
end

function Helper:CreateModelessWnd(wndTemplateID, treeTemplateID, parentWnd, userData, instanceSuffix, onBindFunc)
	LOG("-->Helper CreateModelessWnd")
	local wndTemplate = self.templateMananger:GetTemplate(wndTemplateID, "HostWndTemplate")
	if not wndTemplate then return end
	
	-- local modelessWndID = wndTemplateID..".Instance"
	local modelessWndID = instanceSuffix and wndTemplateID.."."..instanceSuffix or wndTemplateID..".Instance"
	LOG("modelessWndID: ", modelessWndID, " instanceSuffix: ", instanceSuffix)
	
	local modelessWnd = wndTemplate:CreateInstance(modelessWndID)
	if not modelessWnd then 
		Helper:Assert(false, "modelessWnd is nil")
		return 
	end
	
	local treeTemplate = self.templateMananger:GetTemplate(treeTemplateID,"ObjectTreeTemplate")
	if not treeTemplate then return end
	
	local treeID = instanceSuffix and treeTemplateID.."."..instanceSuffix or treeTemplateID..".Instance"
	LOG("treeID: ", treeID, " instanceSuffix: ", instanceSuffix)
	
	local tree = treeTemplate:CreateInstance(treeID)
	if not tree then return end
	
	--绑定对象树
	modelessWnd:BindUIObjectTree(tree)
	if "function" == type(onBindFunc) then
		onBindFunc(modelessWnd, tree)
	end
	
	--传入userData
	modelessWnd:SetUserData(userData)
	
	--创建窗口
	modelessWnd:Create(parentWnd)
	
	LOG("<--Helper CreateModelessWnd")
	return modelessWnd, tree
end

function Helper:DestoryModelessWnd(wndTemplateID, instanceSuffix, onDectoryFun)
	local wndID = instanceSuffix and wndTemplateID.."."..instanceSuffix or wndTemplateID..".Instance"
	local wnd = self.hostWndManager:GetHostWnd(wndID)
	if not wnd then
		LOG("GetHostWnd ret nil! wndID: ", wndID)
		return
	end
	
	local tree = wnd:UnbindUIObjectTree()
	if not tree then
		LOG("UnbindUIObjectTree ret nil!")
		return
	end
	if "function" == type(onDectoryFun) then
		onDectoryFun(wnd, tree)
	end
	self.treeManager:DestroyTree(tree)
	self.hostWndManager:RemoveHostWnd(wndID)
end

--menuTable格式：
--{
	--item1:{id, text, bSplitter, iconID, hotKey, userData, OnInitFun, OnSelectFun, 
	-- OnShowSubMenuFun, SubMenu = {...},
	--}
	--item2:{id, text, bSplitter, iconID, hotKey, userData, OnInitFun, OnSelectFun, 
	-- OnShowSubMenuFun, SubMenu = {...},
	--}
--}
--其中 id 与 text 是必须的，其他的皆可不传。OnInitFun 与 OnSelectFun 根据规则匹配，查找menuFunTable中对应的方法
--例如：item id为”new.notepad“，则item初始化方法名应为OnInit_new_notepad, 若id为”addData“,item click响应方法名应为 OnClick_addData
--OnInit_new_notepad方法若存在于menuFunTable中，则匹配成功

function Helper:CreateMenu(x, y, parentWnd, menuTable, menuFunTable, userData)
	-- local menuTemplateID = "menu.object.template"
	local menuWndTemplID = "wnd.common.menu"
	local menuTreeTemplID = "tree.common.menu"
	local menuObjID = "menu.common"
	local menuItemTemplID = "item.common.menu"
    self:CreateMenuEx(x, y, parentWnd, menuTable, menuFunTable, userData, menuWndTemplID, menuTreeTemplID, menuObjID, menuItemTemplID)
end

function Helper:CreateMenuEx(x, y, parentWnd, menuTable, menuFunTable, userData, menuWndTemplID, menuTreeTemplID, menuObjID, menuItemTemplID)
	-- 防止 不通过 CreateMenuEx 创建的菜单存在，导致出现2个菜单的问题
	if _G["gMenu"] then 
		self:Assert(false, "gMenugMenu")
		return 
	end
	
	--创建菜单窗口
	local menuWndTemplate = self.templateMananger:GetTemplate(menuWndTemplID, "HostWndTemplate")
	local menuWndID = menuWndTemplID..".Instance"
	local menuWnd = menuWndTemplate:CreateInstance(menuWndID)

	--创建菜单对象树
	local menuTreeTemplate = self.templateMananger:GetTemplate(menuTreeTemplID,"ObjectTreeTemplate")
	local menuTreeID = menuTreeTemplID..".Instance"
	local menuTree = menuTreeTemplate:CreateInstance(menuTreeID)
		
	if nil == menuWnd then 
		self:Assert(false, "menuWnd")
		return 
	end
	--绑定
	menuWnd:SetUserData(userData)
	menuWnd:BindUIObjectTree(menuTree)
	
	local menuContext = menuTree:GetUIObject(menuObjID)
	local menuItemTemplate = self.templateMananger:GetTemplate(menuItemTemplID, "ObjectTemplate")
	self:Assert(menuItemTemplate)
	
	for index = 1, #menuTable do
		local menuItem = menuItemTemplate:CreateInstance(menuTable[index].id)
		self:Assert(menuItem)
		
		if "function" ~= type(menuTable[index].OnInitFun) then
			local menuItem_InitFunName = "OnInit_"..string.gsub(menuTable[index].id, "%.", "_")
			self:LOG("menuItem_InitFunName: ", menuItem_InitFunName)
			if "table" == type(menuFunTable) and "function" == type(menuFunTable[menuItem_InitFunName]) then
				menuTable[index].OnInitFun = menuFunTable[menuItem_InitFunName]
			end
		end
		
		if "function" ~= type(menuTable[index].OnSelectFun) then
			local menuItem_SelFunName = "OnSelect_"..string.gsub(menuTable[index].id, "%.", "_")
			self:LOG("menuItem_SelFunName: ", menuItem_SelFunName)
			if "table" == type(menuFunTable) and "function" == type(menuFunTable[menuItem_SelFunName]) then
				menuTable[index].OnSelectFun = menuFunTable[menuItem_SelFunName]
			end
		end
		
		menuItem:SetText(menuTable[index].text)
		menuItem:AttachListener("OnInit", false, function() 
													if "function" == type(menuTable[index].OnInitFun) then
														menuTable[index].OnInitFun(menuItem) 
													end
								end)
		menuItem:AttachListener("OnSelect", false, function()
													if "function" == type(menuTable[index].OnSelectFun) then
														menuTable[index].OnSelectFun(menuItem) 
													end
								end)
		
		menuContext:AddItem(menuItem)
	end
	
	menuContext:SetFocus(true)
	
	local left, top = x, y
	local objLeft, objTop, objRight, objBottom = menuContext:GetObjPos()
	local width, height = objRight - objLeft, objBottom - objTop
	-- if left + width > screenRight then
		-- left = screenRight - width
	-- end
	
	local root = menuTree:GetRootObject()
	local root_left, root_top, root_right, root_bottom = root:GetObjPos()
	
	local screenLeft, screenTop, screenRight, screenBottom = tipUtil:GetScreenRectFromPoint(x, y)
	root_left = root_left + left - screenLeft
	root_top = root_top + top - screenTop
	root_right = root_right + left - screenLeft
	root_bottom = root_bottom + top - screenTop
	menuContext:SetPopStatus(1,1)
	
	root:SetObjPos(root_left, root_top, root_right, root_bottom)	
	self:LOG("root pos: root_left: ", root_left," root_top: ", root_top , " width: ", root_right - root_left," height: " , root_bottom - root_top)
	self:LOG("menuWnd pos: screenLeft: ", screenLeft," screenTop: ", screenTop , " width: ", screenRight-screenLeft," height: " , screenBottom-screenTop-1)
	menuWnd:SetFocus(true)
	menuWnd:Move(screenLeft, screenTop+1, screenRight-screenLeft, screenBottom-screenTop-1)
	-- local ret = menuWnd:TrackPopupMenu(parentWnd, screenLeft, screenTop+1, screenRight-screenLeft, screenBottom-screenTop-1)
	root:SetVisible(true)
	root:SetChildrenVisible(true)
	self:LOG("TrackPopupMenu ret: ", tostring(ret))
	
	menuWnd:AttachListener("OnDestroy", false, function()  _G["gMenu"] = nil end)
	-- self:Assert(false, "OnDestroy Menu")
	-- if menuWnd:GetMenuMode() == "manual" then	
		-- self.objTreeManager:DestroyTree(menuTreeID)
		-- self.hostWndManager:RemoveHostWnd(menuWnd)
	-- end
	
end

--https://s3.amazonaws.com/github-cloud/releases/325827/8ddeba82-ce92-11e4-9812-db61045d243b.exe?response-content-disposition=attachment%3B%20filename%3DGit-1.9.5-preview20150319.exe&response-content-type=application/octet-stream&AWSAccessKeyId=AKIAISTNZFOVBIJMK3TQ&Expires=1433307278&Signature=ppcL8mMS3EVKr8e2YXLk3bcENFA%3D
--发统计的方法
function Helper:SendConvStatistic(tStat)
	local tStatInfo = tStat or {}
	local strDefaultNil = "null"
	
	local strCID = self:GetPeerID()
	local strEC = tStatInfo.strEC 
	local strEA = tStatInfo.strEA 
	local strEL = tStatInfo.strEL
	local strEV = tStatInfo.strEV
	
	if not self:IsRealString(strEC) then
		strEC = strDefaultNil
	end
	
	if not self:IsRealString(strEA) then
		strEA = strDefaultNil
	end
	
	if not self:IsRealString(strEL) then
		strEL = strDefaultNil
	end
	
	if tonumber(strEV) == nil then
		strEV = 1
	end
	
	local strUrl = "http://www.google-analytics.com/collect?v=1&tid=UA-58424540-1&cid="..tostring(strCID)
						.."&t=event&ec="..tostring(strEC).."&ea="..tostring(strEA)
						.."&el="..tostring(strEL).."&ev="..tostring(strEV)
	
	
	tipAsynUtil:AsynSendHttpStat(strUrl, function() end)
end

function Helper:GetPeerID()
	local strPeerID = self:QueryRegValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\PeerId")
	if self:IsRealString(strPeerID) then
		return strPeerID
	end

	local strRandPeerID = tipUtil:GetPeerId()
	if not self:IsRealString(strRandPeerID) then
		return ""
	end
	
	self:SetRegValue("HKEY_LOCAL_MACHINE\\Software\\DDCalendar\\PeerId", strRandPeerID)
	return strRandPeerID
end

--注册表方法
function Helper:QueryRegValue(sPath)
	if not self:IsRealString(sPath) then
		return nil
	end
	
	local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
	if self:IsRealString(sRegRoot) and self:IsRealString(sRegPath) then
		return tipUtil:QueryRegValue(sRegRoot, sRegPath, sRegKey or "")
	end
	return nil
end
function Helper:QueryRegValueEx(sPath, sRegKey)
	if not self:IsRealString(sPath) then
		return nil
	end
	
	local sRegRoot, sRegPath= string.match(sPath, "^(.-)[\\/](.*)")
	if self:IsRealString(sRegRoot) and self:IsRealString(sRegPath) then
		return tipUtil:QueryRegValue(sRegRoot, sRegPath, sRegKey or "")
	end
	return nil
end

function Helper:DeleteRegKey(sPath)
	if not self:IsRealString(sPath) then
		return nil
	end
	
	local sRegRoot, sRegPath = string.match(sPath, "^(.-)[\\/](.*)")
	if self:IsRealString(sRegRoot) and self:IsRealString(sRegPath) then
		LOG("sRegRoot: ", sRegRoot, " sRegPath: ", sRegPath)
		return tipUtil:DeleteRegKey(sRegRoot, sRegPath)
	end
	return nil
end

function Helper:SetRegValue(sPath, value)
	-- tipUtil提供的SetRegValue方法，若sRegPath不存在，则设置失败
	if not self:IsRealString(sPath) then
		self:Assert(false, "sPath is not"..tostring(sPath))
		return false
	end
	local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
	if self:IsRealString(sRegRoot) and self:IsRealString(sRegPath) then
		--若路径不存在，先创建之
		-- XLMessageBox("sRegRoot: "..tostring(sRegRoot).." sRegPath: "..tostring(sRegPath))
		if not tipUtil:QueryRegKeyExists(sRegRoot, sRegPath) then--这里需优化
			tipUtil:CreateRegKey(sRegRoot, sRegPath)--.."\\"..sRegKey
		end
		self:Assert(tipUtil:QueryRegKeyExists(sRegRoot, sRegPath), "reg path not exisit! : "..tostring(value))
		LOG("sRegRoot: ", sRegRoot, " sRegPath: ", sRegPath, " sRegKey: ", sRegKey, " value: ", value)
		return tipUtil:SetRegValue(sRegRoot, sRegPath, sRegKey or "", value or "")
	end
	return false
end
function Helper:SetRegValueEx(sPath, sRegKey, value)
	-- tipUtil提供的SetRegValue方法，若sRegPath不存在，则设置失败
	if not self:IsRealString(sPath) then
		self:Assert(false, "sPath is not"..tostring(sPath))
		return false
	end
	local sRegRoot, sRegPath = string.match(sPath, "^(.-)[\\/](.*)")
	if self:IsRealString(sRegRoot) and self:IsRealString(sRegPath) then
		--若路径不存在，先创建之
		if not tipUtil:QueryRegKeyExists(sRegRoot, sRegPath) then--这里需优化
			tipUtil:CreateRegKey(sRegRoot, sRegPath)--.."\\"..sRegKey
		end
		self:Assert(tipUtil:QueryRegKeyExists(sRegRoot, sRegPath), "reg path not exisit! : "..tostring(value))
		LOG("sRegRoot: ", sRegRoot, " sRegPath: ", sRegPath, " sRegKey: ", sRegKey, " value: ", value)
		return tipUtil:SetRegValue(sRegRoot, sRegPath, sRegKey or "", value or "")
	end
	return false
end

function Helper:GetCommandStrValue(strKey)
	local bRet, strValue = false, nil
	local cmdString = tipUtil:GetCommandLine()
	
	if string.find(cmdString, strKey .. " ") then
		local cmdList = tipUtil:CommandLineToList(cmdString)
		if cmdList ~= nil then	
			for i = 1, #cmdList, 1 do
				local strTmp = tostring(cmdList[i])
				if strTmp == strKey 
					and not string.find(tostring(cmdList[i + 1]), "^/") then		
					bRet = true
					strValue = tostring(cmdList[i + 1])
					break
				end
			end
		end
	end
	return bRet, strValue
end
--封装动画方法