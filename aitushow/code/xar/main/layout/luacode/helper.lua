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

--给math库补充一个四舍五入的方法
function math.round(num)
	if "number" ~= type(num) then
		return 0
	end
	local integer, decimal = math.modf(num)
	return decimal < 0.5 and integer or integer + 1
end

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

function Helper:GetFileNameByPath(path)
	return string.match(path, ".+[\\/]([^?]+)")
end

function Helper:GetUserDataDir()
	local strPublicEnv = "%PUBLIC%"
	local strRet = tipUtil:ExpandEnvironmentStrings(strPublicEnv)
	if strRet == nil or strRet == "" or strRet == strPublicEnv then
		local nCSIDL_COMMON_APPDATA = 35 --CSIDL_COMMON_APPDATA(0x0023)
		strRet = tipUtil:GetSpecialFolderPathEx(nCSIDL_COMMON_APPDATA)
	end
	return strRet
end


Helper.timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
Helper.treeManager = XLGetObject("Xunlei.UIEngine.TreeManager")
Helper.objectFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
Helper.hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
Helper.templateManager = XLGetObject("Xunlei.UIEngine.TemplateManager")
Helper.objTreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")
Helper.xarManager = XLGetObject("Xunlei.UIEngine.XARManager")
Helper.graphicFactory = XLGetObject("Xunlei.XLGraphic.Factory.Object")

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
	local modalWndTemplate = self.templateManager:GetTemplate(wndTemplateID, "HostWndTemplate")
	if not modalWndTemplate then return end
	
	local modalWndID = wndTemplateID..".Instance"
	modalWndID = bAllowMulti and modalWndID..self.iModalWndCount or modalWndID
	LOG("modalWndID: ", modalWndID, " bAllowMulti: ", bAllowMulti)
	
	local modalWnd = modalWndTemplate:CreateInstance(modalWndID)
	if not modalWnd then return end
	
	local treeTemplate = self.templateManager:GetTemplate(treeTemplateID,"ObjectTreeTemplate")
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
	local wndTemplate = self.templateManager:GetTemplate(wndTemplateID, "HostWndTemplate")
	if not wndTemplate then return end
	
	-- local modelessWndID = wndTemplateID..".Instance"
	local modelessWndID = instanceSuffix and wndTemplateID.."."..instanceSuffix or wndTemplateID..".Instance"
	LOG("modelessWndID: ", modelessWndID, " instanceSuffix: ", instanceSuffix)
	
	local modelessWnd = wndTemplate:CreateInstance(modelessWndID)
	if not modelessWnd then 
		Helper:Assert(false, "modelessWnd is nil")
		return 
	end
	
	local treeTemplate = self.templateManager:GetTemplate(treeTemplateID,"ObjectTreeTemplate")
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

function Helper:CreateModelessWndEx(wndTemplateID, treeTemplateID, parentWnd, userData, manualWndID, manualTreeID, onBindFunc)
	LOG("-->Helper CreateModelessWndEx")
	local wndTemplate = self.templateManager:GetTemplate(wndTemplateID, "HostWndTemplate")
	if not wndTemplate then return end

	local modelessWndID = manualWndID or wndTemplateID..".Instance"
	LOG("modelessWndID: ", modelessWndID, " manualWndID: ", manualWndID)
	
	local modelessWnd = wndTemplate:CreateInstance(modelessWndID)
	if not modelessWnd then 
		Helper:Assert(false, "modelessWnd is nil")
		return 
	end
	
	local treeTemplate = self.templateManager:GetTemplate(treeTemplateID,"ObjectTreeTemplate")
	if not treeTemplate then return end
	
	local treeID = manualTreeID or treeTemplateID..".Instance"
	LOG("treeID: ", treeID, " manualTreeID: ", manualTreeID)
	
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
	
	LOG("<--Helper CreateModelessWndEx")
	return modelessWnd, tree
end

function Helper:DestoryModelessWnd(wndTemplateID, instanceSuffix, onDectoryFun)
	local wndID = instanceSuffix and wndTemplateID.."."..instanceSuffix or wndTemplateID..".Instance"
	local wnd = self.hostWndManager:GetHostWnd(wndID)
	if not wnd then
		LOG("GetHostWnd ret nil! wndID: ", wndID)
		return
	end
	wnd:Show(0)
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

function Helper:DestoryModelessWndEx(wndTemplateID, manualWndID, onDectoryFun)
	local wndID = manualWndID or wndTemplateID..".Instance"
	local wnd = self.hostWndManager:GetHostWnd(wndID)
	if not wnd then
		LOG("GetHostWnd ret nil! wndID: ", wndID)
		return
	end
	wnd:Show(0)
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

function Helper:CreateTransparentMask(targetWnd, colorStr)
	Helper:DestoryTransparentMask(targetWnd)
	
	local targetWndID = targetWnd:GetID()
	local maskWnd, maskWndTree = self:CreateModelessWnd("TransparentMaskWnd", "TransparentMaskWndTree", targetWnd, nil, targetWndID..".maskwnd")

	if not maskWnd or not maskWndTree then
		LOG("CreateTransparentMask failed！")
		return
	end
	
	local left,top,right,bottom = targetWnd:GetWindowRect()
	maskWnd:Move(left, top, right - left, bottom - top)
	
	local rootFillObj = maskWndTree:GetRootObject()
	rootFillObj:SetDestColor(colorStr or "RGBA(0,0,0,128)")
	rootFillObj:SetSrcColor(colorStr or "RGBA(0,0,0,128)")
	
	return maskWnd, maskWndTree
end

function Helper:DestoryTransparentMask(targetWnd)
	local targetWndID = targetWnd:GetID()
	return self:DestoryModelessWnd("TransparentMaskWnd", targetWndID..".maskwnd")
end

-- 可用 RealObject 或 hostwnd, enabledrop属性应该打开
function Helper:AddDropTarget(xlueObj)
	if not xlueObj then return end
	if not Helper.hostWndManager:GetHostWnd(xlueObj:GetID()) then  return end
		
	local function InputFilter(self, msg, wpram, lparam)  
		if msg == 0x0233 then --WM_DROPFILES
			local file = Helper.tipUtil:DragQueryFile(wpram)
			Helper:DispatchEvent("OnDrop", file)
		end
	end
	
	local hwnd = xlueObj:GetWndHandle()
	-- XLMessageBox("hwnd: "..tostring(hwnd).." "..tonumber(hwnd))
	tipUtil:DragAcceptFiles(hwnd)
	cookie = xlueObj:AddInputFilter(false,InputFilter)
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
--其中 id 与 text 是必须的，其他的皆为可选。
--OnInitFun调用SetEnable值，控制item的隐藏与显示(常显的item，可不提供该方法)；OnSelectFun响应用户的点击。
--若menuTable中未指定OnInitFun 与 OnSelectFun，则根据规则匹配，查找menuFunTable中对应的方法
--例如：item id为”new.notepad“，则item初始化方法名应为OnInit_new_notepad, 若id为”addData“,item click响应方法名应为 OnSelect_addData
--方法若存在于menuFunTable中，则匹配成功

function Helper:CreateMenu(x, y, parentWnd, menuTable, menuFunTable, userData)
	--menuTable 与 template 共同完成item外观的定义，template定义item共有的特征(例如字体、背景)
	--menuTable定义item间不同的特征，如文案、id、图标等
	--其他未定义的效果(例如文案居中对齐)，请在OnInitFun中调整
	local menuItemTemplID = "menu.common.item.template"
	local menuItemContainerTemplID = "menu.common.container.template"
    self:CreateMenuEx(x, y, parentWnd, menuTable, menuFunTable, userData, menuItemTemplID, menuItemContainerTemplID)
end

function Helper:AddItemToContainer(menuItemContainer, menuTable, menuFunTable)
	local tree = menuItemContainer:GetOwner()
	local menuWnd = tree:GetBindHostWnd()
	local userData = menuWnd:GetUserData()
	menuItemTemplID = userData.menuItemTemplID
	local menuItemTemplate = self.templateManager:GetTemplate(menuItemTemplID, "ObjectTemplate")
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
		
		if not menuTable[index].bSplitter then
			self:Assert("function" == type(menuTable[index].OnSelectFun), "必须提供OnSelectFun!")
		end
		menuItem:SetItemData(menuTable[index])
		menuItemContainer:AddItem(menuItem)
	end
end

function Helper:CreateMenuEx(x, y, parentWnd, menuTable, menuFunTable, userData, menuItemTemplID, menuItemContainerTemplID)
	--这两个Template都是空架子
	local menuWndTemplID = "MenuHostWnd"
	local menuTreeTemplID = "MenuHostWndTree"
	
	--创建菜单窗口
	local menuWndTemplate = self.templateManager:GetTemplate(menuWndTemplID, "HostWndTemplate")
	local menuWnd = menuWndTemplate:CreateInstance(menuWndTemplID..".Instance")
	
	--创建菜单对象树
	local menuTreeTemplate = self.templateManager:GetTemplate(menuTreeTemplID,"ObjectTreeTemplate")
	local menuTree = menuTreeTemplate:CreateInstance(menuTreeTemplID..".Instance")
		
	if nil == menuWnd then 
		self:Assert(false, "menuWnd")
		return 
	end
	--绑定
	if not userData then
		userData = {}
	end
	
	--子菜单与父菜单共用一个MenuWnd，子菜单使用该menuItemTemplID，以便
	--生成与父菜单一样的风格。子菜单的显示与消失，由item自己处理
	userData.menuItemTemplID = menuItemTemplID
	userData.menuItemContainerTemplID = menuItemContainerTemplID
	userData.parentWnd = parentWnd
	menuWnd:SetUserData(userData)
	menuWnd:BindUIObjectTree(menuTree)
	
	local root = menuTree:GetRootObject()
	local menuItemContainerTempl = self.templateManager:GetTemplate(menuItemContainerTemplID, "ObjectTemplate")
	local menuItemContainer = menuItemContainerTempl:CreateInstance(menuItemContainerTemplID..".Instance")
	root:AddChild(menuItemContainer)
	
	self:AddItemToContainer(menuItemContainer, menuTable, menuFunTable)
	menuItemContainer:SetFocus(true)
	
	local screenLeft, screenTop, screenRight, screenBottom = self.tipUtil:GetScreenRectFromPoint(x, y)
	--菜单窗口大小是整个屏幕，以方便子菜单的显示
	menuWnd:Move(screenLeft, screenTop, screenRight, screenBottom)
	containerL, containerT, containerR, containerB = self:CalcMenuPopPosition(x, y, menuItemContainer)
	--真正的菜单内容都在menuItemContainer里面
	menuItemContainer:SetObjPos(containerL, containerT, containerR, containerB)
	
	--展示菜单
	menuWnd:SetTopMost(true)
	menuWnd:TrackPopupMenu(parentWnd, screenLeft, screenTop, screenRight, screenBottom)
end

--计算菜单的弹出方向，不跑到屏幕外，若有父菜单，也要考虑父菜单的位置，不遮住父菜单(可遮住祖父菜单)
--CalcMenuPopPosition返回的是menuContainer在menuWnd中的相对位置
function Helper:CalcMenuPopPosition(x, y, curContainer, parentContainer)
	local curContainerL, curContainerT, curContainerR, curContainerB = curContainer:GetObjPos()
	local menuWidth, menuHeight= curContainerR - curContainerL, curContainerB - curContainerT
	
	local screenLeft, screenTop, screenRight, screenBottom = self.tipUtil:GetScreenRectFromPoint(x, y)
	local tarContainerL, tarContainerT, tarContainerR, tarContainerB = x, y, x + menuWidth, y + menuHeight
	
	if not parentContainer then
		--一级菜单，不能跑到屏幕外
		if screenRight - x < menuWidth then
			--鼠标太靠右，则调整菜单恰好显示完
			tarContainerL = screenRight - menuWidth - 3
			tarContainerR = tarContainerL + menuWidth
		end
		if screenBottom - y < menuHeight then
			--鼠标太靠下，则菜单向上弹
			tarContainerB = y --菜单高度一般不会大于半个屏幕高度
			tarContainerT = tarContainerB - menuHeight
		end
	else
		--有parentContainer(父菜单)，则x,y为当前被选中的item的右上角坐标，而非鼠标坐标
		parentL, parentT, parentR, parentB = parentContainer:GetObjPos()
		if screenRight - x < menuWidth then
			--太靠右，则子菜单显示到父菜单的左侧
			tarContainerL = parentL - menuWidth
			tarContainerR = tarContainerL + menuWidth
		end
		if screenBottom - y < menuHeight then
			--太靠下，则子菜单下缘与item下缘齐平
			local item = parentContainer:GetChildByIndex(0)
			local _, itemT, _, itemB = item:GetObjPos()
			local itemHeight = itemB - itemT
			
			tarContainerB = y + itemHeight
			tarContainerT = tarContainerB - menuHeight
		end
	end
	
	return tarContainerL, tarContainerT, tarContainerR, tarContainerB
end

--https://s3.amazonaws.com/github-cloud/releases/325827/8ddeba82-ce92-11e4-9812-db61045d243b.exe?response-content-disposition=attachment%3B%20filename%3DGit-1.9.5-preview20150319.exe&response-content-type=application/octet-stream&AWSAccessKeyId=AKIAISTNZFOVBIJMK3TQ&Expires=1433307278&Signature=ppcL8mMS3EVKr8e2YXLk3bcENFA%3D
--发统计的方法

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
		if not tipUtil:QueryRegKeyExists(sRegRoot, sRegPath) then
			tipUtil:CreateRegKey(sRegRoot, sRegPath)
		end
		self:Assert(tipUtil:QueryRegKeyExists(sRegRoot, sRegPath), "reg path not exisit! : "..tostring(value))
		return tipUtil:SetRegValue(sRegRoot, sRegPath, sRegKey or "", value or "")
	end
	return false
end

function Helper:GetHttpFile(url, savePath, token)
	savePath = tipUtil:ExpandEnvironmentStrings(savePath)
	tipAsynUtil:AsynGetHttpFile(url, savePath, false, function(nRet, strTargetFilePath, strHeaders)
										if 0 == nRet then
											LOG("DispatchEvent: OnDownloadSucc token: ", token, " savePath: ", savePath)
											self:DispatchEvent("OnDownloadSucc", token, savePath, url, strHeaders)
										else
											--此时nRet即为errorcode
											LOG("DispatchEvent: OnDownloadFailed token: ", token, " nRet: ", nRet)
											
											self:DispatchEvent("OnDownloadFailed", token, nRet, url, strHeaders)
										end
									end)
end

Helper:LoadLuaModule("helper_animation.lua", __document)
Helper:LoadLuaModule("helper_tip.lua", __document)