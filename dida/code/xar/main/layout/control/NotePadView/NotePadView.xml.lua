local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local Helper = XLGetGlobal("Helper")
local tipUtil = tFunHelper.tipUtil

function OnInitNotePadView(self)
	--初始化工具栏按钮
	local newBtn = self:GetControlObject("toolbar.new.btn")
	local cutBtn = self:GetControlObject("toolbar.cut.btn")
	local copyBtn = self:GetControlObject("toolbar.copy.btn")
	local pasteBtn = self:GetControlObject("toolbar.paste.btn")
	local saveBtn = self:GetControlObject("save.btn")
	local editCtrl = self:GetControlObject("edit.ctrl")
	local headtitle = self:GetControlObject("headtitle.edit")
	local headtitleText = self:GetControlObject("headtitle.text")
	local blankBkg = editCtrl:GetControlObject("edit.blank.bkg")
	
	SetEditFont(editCtrl)
	editCtrl:SetText("")
	editCtrl:SetEnable(false)
	headtitle:SetEnable(false)
	headtitleText:SetTextColorResID("999999")
	headtitle:SetTextColorID("999999")
	headtitle:SetText("这个一定要填写")
	blankBkg:SetVisible(true)
	
	newBtn:Enable(false)
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

local bHasSelAll = false
function OnTitleEditClick(self, isFocus)
	if bHasSelAll then
		return
	end
	SetOnceTimer(function()
			self:SetSelAll()
			bHasSelAll = true
			self:SetFocus(true)
		end, 50)
end

function OnTitleEditFocusChange(self, isFocus)
	if not isFocus then--失去焦点时保存标题
		--保存标题
		bHasSelAll = false
		local owner = self:GetOwnerControl()
		local attr = owner:GetAttribute()
		local data = attr.data
		if not data then return end
		-- data.title = self:GetText()
		local saveBtn = owner:GetControlObject("save.btn")
		OnClickSave(saveBtn)
		-- owner:FireExtEvent("UpdateNoteList")
	end
end

function OnClickCancel(self)
	-- local command = tipUtil:GetCommandLine()
	-- local command = "start notepad file \"C:\\1.txt\""
	-- local path = ""
	-- if "string" == type(command) and command ~= "" then
		-- path = string.match(command, ".*file%A*\"(.*)\"")
		-- XLMessageBox(path)
	-- end
	tFunHelper.ChangeView(1)
end

function OnClickSave(self)
	--保存文本文件
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	local data = attr.data
	if not data or not data.txtFilePath then
		-- Helper:Assert(false, "no txtFilePath")
		return
	end
	local editCtrl = owner:GetControlObject("edit.ctrl")
	
	--写edit中的文字到txt中
	local str = editCtrl:GetText()
	local ret = tipUtil:WriteStringToFile(data.txtFilePath, str)
	Helper:LOG("data.txtFilePath: ", data.txtFilePath, " str: ", tipUtil:ReadFileToString(data.txtFilePath), " editCtrl:GetText(): ", str, " ret: ", ret)
	--更新标题
	local headtitleEdit = owner:GetControlObject("headtitle.edit")
	local newTitle = headtitleEdit:GetText()
	
	data.title = "" ~= newTitle and newTitle or data.title
	owner:FireExtEvent("UpdateNoteList", data)
	-- editCtrl:SetFocus(false)
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

function OnLButtonUpSizeCb(self)
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
	local headtitleText = self:GetControlObject("headtitle.text")
	local createDateText = "创建时间："..os.date("%Y-%m-%d  %H:%M", data.createtime)
	headTitle:SetText(data.title)
	createDate:SetText(createDateText)
	
	--若data中没有txt路径，说明是第一次创建
	if not data.txtFilePath then
		local publicPath = Helper:GetUserDataDir().."\\DIDA\\DiDaNote\\"
		if not tipUtil:QueryFileExists(publicPath) then
			tipUtil:CreateDir(publicPath)
		end
		--在PUBLIC目录创建txt文档，以createtime命名
		local txtFilePath = publicPath..tostring(data.createtime)..".txt"
		tipUtil:CreatePathFile(txtFilePath)
		if tipUtil:QueryFileExists(txtFilePath) then
			data.txtFilePath = txtFilePath
			--保存data到文件
			self:FireExtEvent("UpdateNoteList", data)
		end
	end
	
	if not data.txtFilePath then 
		Helper:Assert(false, "txtFilePath is nil!")
		return 
	end	
	--隐藏图片
	blankBkg:SetVisible(false)
	editCtrl:SetEnable(true)
	headTitle:SetEnable(true)
	headtitleText:SetTextColor("#3F5483")
	headTitle:SetTextColor("#3F5483")
	editCtrl:SetFocus(true)
	--读取txt中的文字到edit中
	local strInTxt = tipUtil:ReadFileToString(data.txtFilePath)
	-- if "string" ~= type(strInTxt) then
		-- editCtrl:SetText("点击添加记事")
		-- return 
	-- end
	if not strInTxt then
		Helper:LOG("strInTxt is nil")
		editCtrl:SetText("")
	else
		editCtrl:SetText(strInTxt)
	end
end

function ProcessClick(self, id)
		local tabFont = tipUtil:GetSystemAllTTFFont()
	if "table" ~= type(tabFont) then
		return
	end
	-- for index=1,#tabFont do
		-- Helper:LOG("tabFont: ", tabFont[index])
	-- end
	
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
		local mainXar = Helper.xarManager:GetXAR("main")
		mainXar:CreateFont({["id"]=fontID, ["height"]=lastFontSize, ["facename"]=lastFontName})
	end
	
	local editBaseCtrl = editCtrl:GetControlObject("newedit.edit")
	editBaseCtrl:SetFontID(fontID)
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

function OnEditChange(self)
	--内容发生变化，保存按钮可用
	local owner = self:GetOwnerControl()
	local saveBtn = owner:GetControlObject("save.btn")
	saveBtn:Enable(true)
end

function OnControlMouseLeave(self)
	--内容发生变化，保存按钮可用
	local owner = self:GetOwnerControl()
	local saveBtn = owner:GetControlObject("save.btn")
	OnClickSave(saveBtn)
end
