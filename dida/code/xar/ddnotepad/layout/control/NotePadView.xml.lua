local Helper = XLGetGlobal("Helper")
local tipUtil = XLGetObject("API.Util")

function OnInitNotePadView(self)
	--初始化工具栏按钮
	local newBtn = self:GetControlObject("toolbar.new.btn")
	local cutBtn = self:GetControlObject("toolbar.cut.btn")
	local copyBtn = self:GetControlObject("toolbar.copy.btn")
	local pasteBtn = self:GetControlObject("toolbar.paste.btn")
	local saveBtn = self:GetControlObject("save.btn")
	local editCtrl = self:GetControlObject("edit.ctrl")
	local blankBkg = editCtrl:GetControlObject("edit.blank.bkg")
	
	SetEditFont(editCtrl)
	editCtrl:SetText("")
	editCtrl:SetEnable(false)
	blankBkg:SetVisible(true)
	
	-- newBtn:Enable(false)
	cutBtn:Enable(false)
	copyBtn:Enable(false)
	pasteBtn:Enable(false)
	saveBtn:Enable(false)
	
	
	--监控系统剪切板，里面有text，则粘贴按钮可用
	SetTimer(function(self)
			local CF_TEXT = 1
			local bIsPasteAvlbl = tipUtil:IsClipboardFormatAvailable(CF_TEXT)
			pasteBtn:Enable(bIsPasteAvlbl)
	end,200)
	
	Helper:AddListener("OnNotePadDrop", function(_, _, filePath) 
					local attr = self:GetAttribute()
					Helper:Assert(attr.data)
					attr.data.txtFilePath = filePath
					local year, month, day, hour, minute, second = tipUtil:GetFileCreateTime(filePath)
					attr.data.createtime = os.time({year = year, month = month, day = day, hour = hour, min = minute, sec = second})
					attr.data.title = string.match(filePath, ".*\\(.*)")
					SetData(self, attr.data)
	end)
end

--默认字体微软雅黑、12号 
function OnInitNoteFontCb(self)
	local regPath = "HKEY_CURRENT_USER\\Software\\ddnotepad\\lfFaceName"
	local lastFontName = Helper:QueryRegValue(regPath)
	-- win7默认微软雅黑，xp默认宋体
	if "string" ~= type(lastFontName) or "" == lastFontName then
		local sysVersion =  Helper:QueryRegValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\CurrentVersion")
		if tonumber(sysVersion) > 6.0 then
			lastFontName = "微软雅黑"
		else
			lastFontName = "宋体"
		end
		local ret = Helper:SetRegValue(regPath, lastFontName)
		Helper:Assert(ret, "SetRegValue failed: "..tostring(ret))
	end
	self:SetText(lastFontName)
end

function OnInitNoteSizeCb(self)
	local regPath = "HKEY_CURRENT_USER\\Software\\ddnotepad\\iPointSize"
	local lastFontSize = Helper:QueryRegValue(regPath)
	if "number" ~= type(lastFontSize) then
		Helper:SetRegValue(regPath, 12)
		lastFontSize = 12
	end
	self:SetText(tostring(lastFontSize).."px")
end

function AddNewContent(self, userData)
	
end

function OnPreViewSel(self, userData)
	--去掉edit内部插图
	local headTitle = self:GetControlObject("headtitle.edit")
	local editCtrl = self:GetControlObject("edit.ctrl")
	local createDate = self:GetControlObject("createdate.text")
	local blankBkg = editCtrl:GetControlObject("edit.blank.bkg")
	local baseEditCtrl = editCtrl:GetControlObject("newedit.edit")
	blankBkg:SetVisible(false)
	
	--设置标题
	-- headTitle:SetText()
	
	--设置创建时间
	-- createDate:SetText()
	
	--设置edit控件内容、字体字号
	-- editCtrl:SetText()
	-- baseEditCtrl:SetFontID()
end

function OnTitleEditFocusChange(self, isFocus)
	if not isFocus then--失去焦点时保存标题
		--保存标题
		local owner = self:GetOwnerControl()
		local attr = owner:GetAttribute()
		local data = attr.data
		if not data then return end
		data.title = self:GetText()
		
		owner:FireExtEvent("UpdateNoteList")
	end
end

function OnClickCancel(self)

end

function OnClickSave(self)
	--保存文本文件
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	local data = attr.data
	Helper:Assert(data)
	if not data.txtFilePath then
		--没有txtFilePath，说明是独立编辑器， 选择保存路径
		data.txtFilePath = tipUtil:FileDialog(false, ".txt", ".txt", data and data.title)
		if not data.txtFilePath then--用户选择了取消
			return
		end
	end
	local editCtrl = owner:GetControlObject("edit.ctrl")
	local baseEdit = editCtrl:GetControlObject("newedit.edit")
	--写edit中的文字到txt中

	tipUtil:WriteStringToFileEx(data.txtFilePath, string.gsub(editCtrl:GetText(), "\r", "\r\n"), data.txtCodingType)
	owner:FireExtEvent("UpdateNoteList")
	editCtrl:SetFocus(false)
	
	data.title = string.match(data.txtFilePath, ".*\\(.*)")
	SetData(owner, data)
end

local testMenuTable = 
{
	{id = "open", text = "打开记事本"},
	{id = "new.notepad",  text = "新建记事本"},
	{id = "cancel_operation", text = "取消"},
}
local testMenuFunTable = {}
function testMenuFunTable:OnSelect_open(self)
	XLMessageBox("OnClick_open item id： "..tostring(self:GetID()))
end
function testMenuFunTable:OnSelect_new_notepad(self)
	XLMessageBox("OnClick_new_notepad！item id： "..tostring(self:GetID()))
end
function testMenuFunTable:OnInit_new_notepad(self)
	Helper:LOG("OnInit_new_notepad")
	return false
end
function testMenuFunTable:OnSelect_cancel_operation(self)
	XLMessageBox("OnClick_open item id： "..tostring(self:GetID()))
end

function OnEditRButtonUp(self)
	-- XLMessageBox("OnEditRButtonUp")
	local x, y = tipUtil:GetCursorPos()
	local owner = self:GetOwner()
	local wnd = owner:GetBindHostWnd()
	local parentWndHandle = wnd:GetWndHandle()
	Helper:CreateMenu(x, y, parentWndHandle, testMenuTable, testMenuFunTable)
end

function OnLButtonUpSizeCb(self)
	-- OnEditRButtonUp(self)
	ProcessClick(self, "DropList.FontSize")
end

function SetData(self, data)
	local attr = self:GetAttribute()
	attr.data = data
	if not data then 
		--回到初始化状态
		OnInitNotePadView(self)
		return 
	end
	--设置标题、创建时间
	local headTitle = self:GetControlObject("headtitle.edit")
	local createDate = self:GetControlObject("createdate.text")
	local editCtrl = self:GetControlObject("edit.ctrl")
	local blankBkg = editCtrl:GetControlObject("edit.blank.bkg")
	local createDateText = "创建时间:"..os.date("%Y-%m-%d  %H:%M", data.createtime)
	headTitle:SetText(data.title)
	createDate:SetText(createDateText)
	
	--若data中没有txt路径，说明是第一次创建
	if not data.txtFilePath and not data.bIndependentNotePad then
		local publicPath = tipUtil:ExpandEnvironmentStrings("%PUBLIC%").."\\DIDA\\DiDaNote\\"
		if not tipUtil:QueryFileExists(publicPath) then
			tipUtil:CreateDir(publicPath)
		end
		--在PUBLIC目录创建txt文档，以createtime命名
		local txtFilePath = publicPath..tostring(data.createtime)..".txt"
		tipUtil:CreatePathFile(txtFilePath)
		if tipUtil:QueryFileExists(txtFilePath) then
			data.txtFilePath = txtFilePath
			--保存data到文件
			self:FireExtEvent("UpdateNoteList")
		end
	end
	
	if data.txtFilePath then 
		local tc, strInTxt = tipUtil:ReadFileToStringEx(data.txtFilePath)
		editCtrl:SetText(strInTxt)
		data.txtCodingType = tc
	end	
	--隐藏图片
	blankBkg:SetVisible(false)
	editCtrl:SetEnable(true)
	editCtrl:SetFocus(true)
	--读取txt中的文字到edit中
	
	-- if "string" ~= type(strInTxt) then
		-- editCtrl:SetText("点击添加记事")
		-- return 
	-- end
	
end

function ProcessClick(self, id)
	local objRootCtrl = self:GetOwnerControl()
	local objEdit = objRootCtrl:GetControlObject("edit.ctrl")
	local objDropList = objEdit:GetControlObject(id)
	local objMenuContext = objDropList:GetControlObject("Menu.Context")
	if objDropList:GetVisible() and objMenuContext:GetVisible() then
		objDropList:SetVisible(false)
		objDropList:SetChildrenVisible(false)
		
		objMenuContext:SetVisible(false)
		objMenuContext:SetChildrenVisible(false)
		return
	end
	
	objDropList:SetVisible(true)
	objDropList:SetChildrenVisible(true)
	
	objMenuContext:SetVisible(true)
	objMenuContext:SetChildrenVisible(true)
	
	objDropList:SetFocus(true)
	objDropList:SetDefaultItemHover()
end

function OnLButtonUpFontCb(self)
	ProcessClick(self, "DropList.Font")
end

function OnSelChange(self, selRangeMin, selRangeMax, selType)
	--有选中的文本，则剪切、复制按钮可用
	local owner = self:GetOwnerControl()
	local cutBtn = owner:GetControlObject("toolbar.cut.btn")
	local copyBtn = owner:GetControlObject("toolbar.copy.btn")
	
	local selText = self:GetSelText()
	if "string" == type(selText) and "" ~= selText then
		cutBtn:Enable(true)
		copyBtn:Enable(true)
	else
		cutBtn:Enable(false)
		copyBtn:Enable(false)
	end
end

function OnClickPaste(self)
	local owner = self:GetOwnerControl()
	local editCtrl = owner:GetControlObject("edit.ctrl")
	local baseEditCtrl = editCtrl:GetControlObject("newedit.edit")
	if baseEditCtrl then
		baseEditCtrl:Paste()
	end
end

function OnClickCopy(self)
	local owner = self:GetOwnerControl()
	local editCtrl = owner:GetControlObject("edit.ctrl")
	local baseEditCtrl = editCtrl:GetControlObject("newedit.edit")
	if baseEditCtrl then
		baseEditCtrl:Copy()
	end
end

function OnClickCut(self)
	local owner = self:GetOwnerControl()
	local editCtrl = owner:GetControlObject("edit.ctrl")
	local baseEditCtrl = editCtrl:GetControlObject("newedit.edit")
	if baseEditCtrl then
		baseEditCtrl:Cut()
	end
end

function OnClickNew(self)
	--保存以前的文本文件
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	local editCtrl = owner:GetControlObject("edit.ctrl")
	local text = editCtrl:GetText()
	if text and "" ~= text then
		local saveBtn = owner:GetControlObject("save.btn")
		OnClickSave(saveBtn)
	end
	editCtrl:SetText("")
	attr.data.txtFilePath = nil
end

function OnFocusFontDropList(self, bFocus)
	if bFocus then
		return
	end

	local objTree = self:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	local mouseX, mouseY = tipUtil:GetCursorPos()
	local nWndX, nWndY = objHostWnd:ScreenPtToHostWndPt(mouseX, mouseY)
	local nTreeX, nTreeY = objHostWnd:HostWndPtToTreePt(nWndX, nWndY)

	local nLeft, nTop, nRight, nBottom = self:GetAbsPos()

	if nTreeX > nLeft-5 and nTreeX < nRight+5 and nTreeY < nBottom+5 and nTreeY > nTop-25 then	
		self:SetFocus(true)
		return
	end
	
	self:SetVisible(false)
	self:SetChildrenVisible(false)
	
	local objMenuContext = self:GetControlObject("Menu.Context")
	objMenuContext:SetVisible(false)
	objMenuContext:SetChildrenVisible(false)
end

function OnInitFontDropList(self)
	self:SetVisible(false)
	self:SetChildrenVisible(false)
end

function SetEditFont(editCtrl)
	--通过注册表获取当前字体大小
	local regPath = "HKEY_CURRENT_USER\\Software\\ddnotepad\\lfFaceName"
	local lastFontName = Helper:QueryRegValue(regPath) 
	Helper:Assert(lastFontName, "lastFontSize is nil")
	
	local regSizePath = "HKEY_CURRENT_USER\\Software\\ddnotepad\\iPointSize"
	local lastFontSize = Helper:QueryRegValue(regSizePath)
	Helper:Assert(lastFontSize, "lastFontSize is nil")
	
	--设置editctrl字体
	local fontID = lastFontName.."."..lastFontSize
	local font = Helper.xarManager:GetFont(fontID)
	
	if not font then--字体未创建
		local mainXar = Helper.xarManager:GetXARByIndex(0)
		mainXar:CreateFont({["id"]=fontID, ["height"]=lastFontSize, ["facename"]=lastFontName})
	end
	
	-- local editBaseCtrl = editCtrl:GetControlObject("newedit.edit")
	editCtrl:SetFontID(lastFontName, lastFontSize)
end

function OnSelectFont(self, event, fontName)
	local regPath = "HKEY_CURRENT_USER\\Software\\ddnotepad\\lfFaceName"
	local lastFontName = Helper:QueryRegValue(regPath) 
	if fontName == lastFontName then
		return
	end
	
	local editCtrl = self:GetOwnerControl()
	local notePadView = editCtrl:GetOwnerControl()
	local fontCb = notePadView:GetControlObject("font.Combobox.btn")
	fontCb:SetText(fontName)
	
	local ret = Helper:SetRegValue(regPath, fontName)
	Helper:Assert(ret, "SetRegValue failed: "..tostring(ret))
	
	SetEditFont(editCtrl)
end

function OnSelectFontSize(self, event, sSize)
	local regPath = "HKEY_CURRENT_USER\\Software\\ddnotepad\\iPointSize"
	local lastFontSize = Helper:QueryRegValue(regPath) 
	local iSize = tonumber(sSize)
	if iSize == lastFontSize then
		return
	end
	
	local editCtrl = self:GetOwnerControl()
	local notePadView = editCtrl:GetOwnerControl()
	local sizeCb = notePadView:GetControlObject("size.Combobox.btn")
	sizeCb:SetText(sSize.."px")
	
	local ret = Helper:SetRegValue(regPath, iSize)
	Helper:Assert(ret, "SetRegValue failed: "..tostring(ret))
	
	-- 设置editctrl字体
	SetEditFont(editCtrl)
end

local OnCloseCookie = nil
function OnEditChange(self)
	--内容发生变化，保存按钮可用
	local owner = self:GetOwnerControl()
	local saveBtn = owner:GetControlObject("save.btn")
	saveBtn:Enable(true)
	
	--监听系统退出事件
	local wndTree = self:GetOwner()
	local wnd = wndTree:GetBindHostWnd()
	OnCloseCookie = OnCloseCookie or (wnd and wnd:AttachListener("OnClose", false, function()
					local text = self:GetText()
					if not text or "" == text then
						return
					end
					local attr = self:GetAttribute()
					local fileText, tc = nil
					if attr.data and attr.data.txtFilePath then
						tc, fileText = tipUtil:ReadFileToStringEx(attr.data.txtFilePath)
					end
					if fileText and "" ~= fileText and fileText ~= text then
						OnClickSave(saveBtn) 
					end
				end))
end
