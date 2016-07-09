local g_NextMayBe = nil
local g_ViewIndex = 0	
local Status_Init = 0
local Status_Loading = 1
local Status_Normal = 2
local Status_Scaling = 3
local Status_Resizing = 4
local bCheckFileRelation = false
local g_ShowFolderTipDocIndex
local g_DeleteFileFromPoolIndex = 0
local g_AddFileToPoolIndex = 0

function SetObjVisible(obj, bVisible)
	if not obj then return end
	
	if bVisible then
		obj:SetVisible(true)
		obj:SetChildrenVisible(true)
	else
		obj:SetVisible(false)
		obj:SetChildrenVisible(false)
	end
end


function LoadTotalImageFile(self, filePath, curDocIndex)
	LOG("KuaikanLog full image。。。")
	local attr = self:GetAttribute()
	if (not attr.CurDocItem) or (not attr.CurDocItem.DocObj) or (attr.LoadingTotalImageFile == attr.CurDocItem.FilePath) then
		return
	end
	attr.LoadingTotalImageFile = filePath
	local LoadBitmapOP = Helper.APIproxy.CreateLoadBitmapOP()
	local bAutoRotate = self:IsAutoRotate()
	LoadBitmapOP:SetParam(filePath, bAutoRotate)
	local cookieId = 0
	cookieId = LoadBitmapOP:AttachListener("OnOperationComplete", function (bitmap, errerCode)
		if bitmap then 
			if curDocIndex == g_ViewIndex then	
				attr.CurDocItem.DocObj:SetSrcBitmap(bitmap)
				if attr.ScaleCallBackParam then	
					local viewObj = self:GetControlObject("client.view")
					viewObj:AsynHandleScale(attr.CurDocItem.DocObj:GetSrcBitmap(), attr.ScaleCallBackParam)
					attr.ScaleCallBackParam = nil
				end
				attr.LoadingTotalImageFile = nil
			end
		else
			LOG("KuaikanLog load full image failed errerCode=" .. errerCode)
			if curDocIndex == g_ViewIndex then	
				attr.LoadingTotalImageFile = nil
			end
		end
		LoadBitmapOP:DetachListener("OnOperationComplete", cookieId)
		LoadBitmapOP:Release()
	end)
	LoadBitmapOP:AddRef()
	LoadBitmapOP:Execute()
end


function LoadCacheImageFile(self, filePath, dirction, curDocIndex, srcFrom)
	LOG("KuaikanLog LoadCacheImageFile from " .. srcFrom .. " filePath=" .. filePath)
	local attr = self:GetAttribute()
	LOG("KuaikanLog attr.CurDocItem.FilePath=" .. attr.CurDocItem.FilePath)
	attr.CacheFilePath = filePath
	local LoadImageFileOp = Helper.APIproxy.CreateLoadImageFileOP()
	local tipUtil = Helper.APIproxy.GetTipUtil()
	local ScreenWidth, ScreenHeight = tipUtil:GetScreenRatio()
	local bAutoRotate = self:IsAutoRotate()
	LoadImageFileOp:SetParam(filePath, true, ScreenWidth, ScreenHeight, bAutoRotate)
	local cookieId = 0
	cookieId = LoadImageFileOp:AttachListener("OnOperationComplete", function (ImageDoc, errerCode, loadFilePath)
		LOG("KuaikanLog OnOperationComplete errerCode=" .. errerCode .. " loadFilePath:" .. loadFilePath)
		if errerCode == 0 then 
			if curDocIndex == g_ViewIndex then	
				if attr.CurDocItem  and loadFilePath == attr.CurDocItem.FilePath then
					g_ViewIndex = g_ViewIndex + 1
					attr.CurDocItem.DocObj = ImageDoc
					self:ResetViewObj()
					self:UpdateView(true)
					if g_NextMayBe == "PreFile" then
						
						local filePath = self:GetPreFilePath() 
						if filePath and filePath ~= attr.CurDocItem.FilePath then
							self:LoadCacheImageFile(filePath, "PreFile", g_ViewIndex, "LoadCacheImageFile")
						end
					else
						
						local filePath = self:GetNextFilePath()
						if filePath and filePath ~= attr.CurDocItem.FilePath then
							self:LoadCacheImageFile(filePath, "NextFile", g_ViewIndex, "LoadCacheImageFile")
						end
					end
					self:SetStatus(Status_Normal, "当前加载的文档没有改变")
					LoadImageFileOp:DetachListener("OnOperationComplete", cookieId)
					LoadImageFileOp:Release()
					return
				else
					if dirction == "PreFile" then
						attr.PreDocItem = {}
						attr.PreDocItem.FilePath = loadFilePath
						attr.PreDocItem.DocObj = ImageDoc
					elseif dirction == "NextFile" then
						attr.NextDocItem = {}
						attr.NextDocItem.FilePath = loadFilePath
						attr.NextDocItem.DocObj = ImageDoc
					else
						ImageDoc:Release()
						ImageDoc = nil
					end
					
					
				end
			else	
				LOG("KuaikanLog OnOperationComplete the doc is changed ")
				ImageDoc:Release()
				ImageDoc = nil
			end
		
		
		else
			LOG("KuaikanLog load failed, your sister")
			if curDocIndex == g_ViewIndex then	
				LOG("KuaikanLog load failed, your sister aaaa")
				g_ViewIndex = g_ViewIndex + 1
				if attr.CurDocItem  and loadFilePath == attr.CurDocItem.FilePath then
					self:SetStatus(Status_Init)
					self:UpdateTitle()
					self:ResetViewObj()
					self:UpdateView(true)
					
					self:ShowErrorIcon(true)
					
					local normalToolbar = self:GetControlObject("client.toolbarnew")
					normalToolbar:EnableToolbarItem({"src", "max", "min", "leftrotate", "rightrotate"}, false)
				end
			end
		end
		if  attr.CacheFilePath == loadFilePath then
			attr.CacheFilePath = nil
		end
		LoadImageFileOp:DetachListener("OnOperationComplete", cookieId)
		LoadImageFileOp:Release()
	end)
	LoadImageFileOp:AddRef()
	LoadImageFileOp:Execute()
end


function SaveImageFile(self, fnCallback, newFilePath)
	local attr = self:GetAttribute()
	local bAutoRotate = self:IsAutoRotate()
	local SaveDocFileOP = Helper.APIproxy.CreateSaveDocFileOP()
	SaveDocFileOP:SetParam(attr.CurDocItem.DocObj, newFilePath, bAutoRotate)
	local cookieId = 0
	cookieId = SaveDocFileOP:AttachListener("OnOperationComplete", function (errerCode)
		self:ShowWaitingPanel(false)
		if errerCode == 0 then
			
		else
			
		end
		SaveDocFileOP:DetachListener("OnOperationComplete", cookieId)
		SaveDocFileOP:Release()
		if fnCallback then
			fnCallback()
		end
	end)
	SaveDocFileOP:AddRef()
	SaveDocFileOP:Execute()
	
	self:ShowWaitingPanel(true)
	self:SetWaitingPanelText("正在保存旋转后的文件，请稍等...")
end

local function RoateUpdateFileList(listViewObj, filepath)
	local ChangedDirPathPath = Helper.APIproxy.GetParentPath(filepath)
	local tipUtil = Helper.APIproxy.GetTipUtil()
	local fileList = tipUtil:GetFiles(ChangedDirPathPath)
	if fileList then
		for _, v in ipairs(fileList) do
			if v.FilePath == filepath then
				local ItemObj = listViewObj:GetItemById(filepath)
				if ItemObj and ItemObj.Obj then
					ItemObj.UserData.LastWriteTime = v.LastWriteTime
					ItemObj.Obj:UpdateThumnails(true)
				end
				break
			end
		end
	end
end

--CallBack必定会被调用， 必须传1个存在的函数
function HandleRotateExit(self, CallBack)
	local attr = self:GetAttribute()
	if not attr.CurDocItem or not attr.CurDocItem.DocObj then
		CallBack()
		return
	end
	local nAngle = attr.CurDocItem.DocObj:GetRotateAngle() 
	if not nAngle then
		CallBack()
		return
	end
	nAngle = math.abs(nAngle)%360
	if nAngle == 0 then
		CallBack()
		return
	end		

	local RotateSaveAlert = Helper.Setting.GetRotateType()
	if RotateSaveAlert == "nosave" then 
		attr.CurDocItem.DocObj:ResetRotate()
	elseif RotateSaveAlert == "rensave" then
		local strLocalFiles = Helper.APIproxy.GetRenameFilePath(attr.CurDocItem.FilePath)
		if strLocalFiles then
			self:SaveImageFile(CallBack, strLocalFiles)
			return
		end	
	else		
		local extName = string.lower(Helper.APIproxy.GetFileExt(attr.CurDocItem.FilePath))
		local bCoverCopy = false
		if extName == ".bmp" or extName == ".png" or extName == ".jpe" or extName == ".jpg" or extName == ".jpeg" then
			bCoverCopy = true
		end
		local fnCoverCallBack = 
			function()
				--self:UpdateFileList(true)
				RoateUpdateFileList(self:GetControlObject("client.filelistview"), attr.CurDocItem.FilePath)
				--通知主界面
				ImagePool:UpdateFileInfo(attr.CurDocItem.FilePath)
				CallBack()
			end
		if RotateSaveAlert == "coverold" then 
			if bCoverCopy then 
				self:SaveImageFile(fnCoverCallBack)
				return
			else
				local HostWndHelper = Helper.MessageBox
				local nRet, bCheck = HostWndHelper.MessageBoxEx(self:GetOwner():GetBindHostWnd())
				if nRet == HostWndHelper.ID_NOSAVE or nRet == HostWndHelper.ID_CANCEL then	
					attr.CurDocItem.DocObj:ResetRotate()
					if bCheck then
						Helper.Setting.SetRotateType("nosave")
					end
				elseif nRet == HostWndHelper.ID_RENAMESAVE then
					local strLocalFiles = Helper.APIproxy.GetRenameFilePath(attr.CurDocItem.FilePath)
					if strLocalFiles then
						self:SaveImageFile(CallBack, strLocalFiles)
						if bCheck then
							Helper.Setting.SetRotateType("rensave")
						end
						return
					end	
				else --coverold
					self:SaveImageFile(fnCoverCallBack)
					if bCheck then
						Helper.Setting.SetRotateType("coverold")
					end
					return
				end
			end
		else --askme
			local HostWndHelper = Helper.MessageBox
			local nRet, bCheck = HostWndHelper.MessageBoxEx(self:GetOwner():GetBindHostWnd())
			if nRet == HostWndHelper.ID_NOSAVE or nRet == HostWndHelper.ID_CANCEL then
				attr.CurDocItem.DocObj:ResetRotate()
				if bCheck then
					Helper.Setting.SetRotateType("nosave")
				end
			elseif nRet == HostWndHelper.ID_COVEROLD then	
				self:SaveImageFile(fnCoverCallBack)
				if bCheck then
					Helper.Setting.SetRotateType("coverold")
				end
				return
			elseif nRet == HostWndHelper.ID_RENAMESAVE then	
				local strLocalFiles = Helper.APIproxy.GetRenameFilePath(attr.CurDocItem.FilePath)
				if strLocalFiles then
					self:SaveImageFile(CallBack, strLocalFiles)
					if bCheck then
						Helper.Setting.SetRotateType("rensave")
					end
					return
				end	
			end
		end
	end
	CallBack()
end

function LoadImageFile(self, filePath, fref, fnPreCallBack, fnAfterCalBack)
	if not Helper.APIproxy.GetTipUtil():IsCanHandleFileCheckByExt(filePath) then
		local HostWndHelper = Helper.MessageBox
		HostWndHelper.MessageBox("不支持的文件格式，无法打开", self:GetOwner():GetBindHostWnd(), true)
		return
	end
	local attr = self:GetAttribute()
	if attr.CurDocItem and attr.CurDocItem.FilePath == filePath then	
		return false
	end
	--切换图片时删除末页提示
	local tipobj = self:GetObject("firstorlasttip")
	if tipobj and tipobj:GetVisible() then
		tipobj:SetVisible(false)
		tipobj:SetChildrenVisible(false)
	end
	local function CallBack()
		if fnPreCallBack then
			fnPreCallBack()
		end	
		self:LoadImageFileImpl(filePath, fref)
		
		if fnAfterCalBack then
			fnAfterCalBack()
		end
	end	
	self:HandleRotateExit(CallBack)
	return true
end



function LoadImageFileImpl(self, filePath, fref)
	local attr = self:GetAttribute()
	if not fref then	
		if attr.PreDocItem and attr.PreDocItem.FilePath == filePath then
			fref = "PreFile"
		end
		if attr.NextDocItem and attr.NextDocItem.FilePath == filePath then
			fref = "NextFile"
		end	
	end
	
	if fref == "PreFile" then 
		if attr.NextDocItem then	
			if attr.NextDocItem.DocObj then
				attr.NextDocItem.DocObj:Release()
			end
			attr.NextDocItem = nil
		end
		
		attr.NextDocItem  = attr.CurDocItem 
		if attr.NextDocItem.DocObj then
			attr.NextDocItem.DocObj:ReleaseSrcBitmap()
		end
		
		if attr.PreDocItem then	
			if attr.PreDocItem.FilePath == filePath then	
				if attr.PreDocItem.DocObj then	
					LOG("KuaikanLog PreDocItem get")
					g_ViewIndex = g_ViewIndex + 1 
					attr.CurDocItem = attr.PreDocItem
					self:ResetViewObj()
					attr.PreDocItem = nil
					self:UpdateView(true)
					
					local filePath = self:GetPreFilePath() 
					if filePath and filePath ~= attr.CurDocItem.FilePath then
						LoadCacheImageFile(self, filePath, "PreFile", g_ViewIndex, "PreFile")
					end
					self:SetStatus(Status_Normal, "上一张")
					return true
				end
			else	
				if attr.PreDocItem.DocObj then
					attr.PreDocItem.DocObj:Release()
				end
			end
			attr.PreDocItem = nil
		end
		attr.CurDocItem = nil
	elseif fref == "NextFile" then	
		if attr.PreDocItem then 
			if attr.PreDocItem.DocObj then
				attr.PreDocItem.DocObj:Release()
			end
			attr.PreDocItem = nil
		end
		
		attr.PreDocItem = attr.CurDocItem
		if attr.PreDocItem.DocObj then
			attr.PreDocItem.DocObj:ReleaseSrcBitmap()
		end
		
		if attr.NextDocItem then
			if attr.NextDocItem.FilePath == filePath then	
				if attr.NextDocItem.DocObj then 
					LOG("KuaikanLog NextDocItem get")
					g_ViewIndex = g_ViewIndex + 1 
					attr.CurDocItem = attr.NextDocItem
					attr.NextDocItem = nil
					self:ResetViewObj()
					self:UpdateView(true)
					
					local filePath = self:GetNextFilePath()
					if filePath and filePath ~= attr.CurDocItem.FilePath then
						self:LoadCacheImageFile(filePath, "NextFile", g_ViewIndex, "NextFile")
					end
					self:SetStatus(Status_Normal, "NextFile")
					return true
				end
			else
				if attr.NextDocItem.DocObj then
					attr.NextDocItem.DocObj:Release()
				end			
			end
			attr.NextDocItem = nil
		end
		attr.CurDocItem = nil
	else	
		
		if attr.NextDocItem and attr.NextDocItem.DocObj then
			attr.NextDocItem.DocObj:Release()
		end
		attr.NextDocItem = nil
		
		if attr.CurDocItem and attr.CurDocItem.DocObj then
			attr.CurDocItem.DocObj:Release()
		end
		attr.CurDocItem = nil
		
		if attr.PreDocItem and attr.PreDocItem.DocObj then
			attr.PreDocItem.DocObj:Release()
		end
		attr.PreDocItem = nil
	end
	attr.CurDocItem = {}
	attr.CurDocItem.FilePath = filePath
	
	
	if attr.CacheFilePath == attr.CurDocItem.FilePath then	
		
		
		LOG("KuaikanLog waiting cache。。。")
		if fref == "PreFile" then
			g_NextMayBe = "PreFile"
		else
			g_NextMayBe = "NextFile"
		end
		self:SetStatus(Status_Loading)
		
		
		self:UpdateTitle()
		self:ResetViewObj()
		self:UpdateView(true)
		return true
	end
	
	LOG("KuaikanLog  filePath=" .. filePath)
	
	g_ViewIndex = g_ViewIndex + 1 
	local LoadImageFileOp = Helper.APIproxy.CreateLoadImageFileOP()
	local tipUtil = Helper.APIproxy.GetTipUtil()
	local ScreenWidth, ScreenHeight = tipUtil:GetScreenRatio()
	local bAutoRotate = self:IsAutoRotate()
	LoadImageFileOp:SetParam(filePath, true, ScreenWidth, ScreenHeight, bAutoRotate)
	local cookieId = 0
	local curDocIndex = g_ViewIndex
	cookieId = LoadImageFileOp:AttachListener("OnOperationComplete", function (ImageDoc, errerCode)
		if g_ViewIndex == curDocIndex then 
			if errerCode == 0 then 
				if attr.CurDocItem then 
					attr.CurDocItem.DocObj = ImageDoc
					self:ResetViewObj()
					self:UpdateView(true)
					
					if fref == "PreFile" then 
						local filePath = self:GetPreFilePath()
						if filePath and filePath ~= attr.CurDocItem.FilePath then
							LoadCacheImageFile(self, filePath, "PreFile", g_ViewIndex, "加载缓存")
						end
					else
						local filePath = self:GetNextFilePath()
						if filePath and filePath ~= attr.CurDocItem.FilePath then
							LoadCacheImageFile(self, filePath, "NextFile", g_ViewIndex, "加载缓存")
						end
					end
					self:SetStatus(Status_Normal)
				else
					ImageDoc:Release()
					self:SetStatus(Status_Init)
				end
			else 
				if errerCode ~= -1001 then
					self:SetStatus(Status_Init)
					self:UpdateTitle()
					self:ResetViewObj()
					self:UpdateView(true)
					
					self:ShowErrorIcon(true)
					
					local normalToolbar = self:GetControlObject("client.toolbarnew")
					normalToolbar:EnableToolbarItem({"src", "max", "min", "leftrotate", "rightrotate"}, false)
				end
			end
		end
		LoadImageFileOp:DetachListener("OnOperationComplete", cookieId)
		LoadImageFileOp:Release()
	end)
	LoadImageFileOp:AddRef()
	LoadImageFileOp:Execute(true)
	self:SetStatus(Status_Loading)
	self:UpdateTitle()
	self:ResetViewObj()
	self:UpdateView(true)
	self:ShowErrorIcon(false)
	
	local normalToolbar = self:GetControlObject("client.toolbarnew")
	normalToolbar:EnableToolbarItem({"preFile", "nextFile", "del"}, true)
end

function ShowErrorIcon(self, bShow)
	local errorIcon = self:GetControlObject("client.view.loaderror.icon")
	SetObjVisible(errorIcon, bShow)
end


function OnLoadComplete(self)
end

function UpdateView(self, bUpdateImmediately)
	local attr = self:GetAttribute()
	if (not attr.CurDocItem) or (not attr.CurDocItem.DocObj) then
		return
	end
	local viewObj = self:GetControlObject("client.view")
	viewObj:UpdateView(nil, bUpdateImmediately)
end

function GetPreFilePath(self)
	local attr = self:GetAttribute()
	if (not attr.CurDocItem) or (not attr.CurDocItem.FilePath) then 
		return nil, false
	end
	local listView = self:GetControlObject("client.filelistview")
	return listView:GetPreFile()
end


function GetNextFilePath(self)
	local attr = self:GetAttribute()
	if (not attr.CurDocItem) or (not attr.CurDocItem.FilePath) then 
		return nil, false
	end
	
	local listView = self:GetControlObject("client.filelistview")
	return listView:GetNextFile()
end

function PreFile(self)
	local attr = self:GetAttribute()
	local listView = self:GetControlObject("client.filelistview")
	
	local filePath, bLastFile = self:GetPreFilePath()
	
	if filePath then
		local tipobj = self:GetObject("firstorlasttip")
		if bLastFile then
			if tipobj and tipobj:GetVisible() then
				tipobj:SetVisible(false)
				tipobj:SetChildrenVisible(false)
			else
				local tiptextobj = tipobj:GetObject("firstorlasttip.text")
				tiptextobj:SetText("当前已是第一张，继续浏览将会返回最后一张")
				tipobj:SetVisible(true)
				tipobj:SetChildrenVisible(true)
				return
			end
		end
		local function FnCallBack()
			listView:PreFile()
		end
		self:LoadImageFile(filePath, "PreFile", FnCallBack, nil)
	end
end

function FirstFile(self)
	local listView = self:GetControlObject("client.filelistview")
	local filePath = listView:GetFirstFile()
	if filePath then
		local function FnCallBack()
			listView:SelectItem(filePath)
		end
		self:LoadImageFile(filePath, nil, FnCallBack, nil)
	end
end

function LastFile(self)
	local listView = self:GetControlObject("client.filelistview")
	local filePath = listView:GetLastFile()
	if filePath then
		local function FnCallBack()
			listView:SelectItem(filePath)
		end
		self:LoadImageFile(filePath, nil, FnCallBack, nil)
	end
end


function NextFile(self, bMust)
	local attr = self:GetAttribute()
	local listView = self:GetControlObject("client.filelistview")
	
	local filePath, bLastFile = self:GetNextFilePath()
	
	if filePath then
		local tipobj = self:GetObject("firstorlasttip")
		if bLastFile and not bMust then
			if tipobj and tipobj:GetVisible() then
				tipobj:SetVisible(false)
				tipobj:SetChildrenVisible(false)
			else
				local tiptextobj = tipobj:GetObject("firstorlasttip.text")
				tiptextobj:SetText("当前已是最后一张，继续浏览将会返回第一张")
				tipobj:SetVisible(true)
				tipobj:SetChildrenVisible(true)
				return
			end
		end
		
		local function FnCallBack()
			listView:NextFile()
		end
		self:LoadImageFile(filePath, "NextFile", FnCallBack, nil)
	end
end

function DeleteFile(self)
	local attr = self:GetAttribute()
	if attr.Status == Status_Loading or attr.Status == Status_Scaling or attr.Status == Status_Resizing then	
		return
	end
	local tipUtil = Helper.APIproxy.GetTipUtil()
	if attr.CurDocItem and attr.CurDocItem.FilePath then
		if Helper.Setting.IsDelRemind() then
			local HostWndHelper = Helper.MessageBox
			local showText = "确认要删除文件\"" .. Helper.APIproxy.GetFileNameFromPath(attr.CurDocItem.FilePath) .. "\"吗"
			local nRet = HostWndHelper.MessageBox(showText, self:GetOwner():GetBindHostWnd())
			if nRet == HostWndHelper.ID_YES then
				tipUtil:DelPathFile2RecycleBin(attr.CurDocItem.FilePath)
			end
		else
			tipUtil:DelPathFile2RecycleBin(attr.CurDocItem.FilePath)
		end
	end
end

function AdapterOrSrcScreen(self, bSrc, bAdapter)
	local attr = self:GetAttribute()
	if (not attr.CurDocItem) or (not attr.CurDocItem.DocObj) then
		return
	end
	local normalToolbar = self:GetControlObject("client.toolbarnew")
	local toolbarItem = normalToolbar:GetToolbarItem("src")
	local tipsText = toolbarItem:GetTipText()
			
	local viewObj = self:GetControlObject("client.view")
	
	local curRatio = viewObj:GetScaleRatio()
	
	local adapterRatio = viewObj:GetAdaptedScreenRatio()
	local newRatio
	if bSrc then	
		if curRatio ~= 1 then
			newRatio = 1.0
		else
			return
		end
	elseif bAdapter then
		if math.abs(curRatio-adapterRatio) < 0.000001 then
			return
		else
			newRatio = adapterRatio
		end
	else
		if math.abs(curRatio-adapterRatio) < 0.000001 then 
			newRatio = 1.0
		else	
			newRatio = adapterRatio
		end
	end
	
	if math.abs(newRatio - 1.0) < 0.000001 then
		viewObj:ResetInfo(true)
	else
		viewObj:ResetInfo()
	end
	viewObj:SetScaleRatio(newRatio)

	local bMustShowAdapter = true
	if math.abs(curRatio - 1.0) < 0.000001 then
		if math.abs(newRatio - 1.0) < 0.000001 then
			if tipsText == "适应窗口" then
				bMustShowAdapter = false
			else
				bMustShowAdapter = true
			end
		else
			bMustShowAdapter = false
		end
	else
		if math.abs(newRatio - 1.0) < 0.000001 then
			bMustShowAdapter = true
		else
			bMustShowAdapter = false
		end 
	end
	if bMustShowAdapter then
		toolbarItem:AddTipText("适应窗口")
		toolbarItem:SetIconImage("toolbar.shiping.normal", "toolbar.shiping.hover",
								 "toolbar.shiping.down", "toolbar.shiping.disable")
	else
		toolbarItem:AddTipText("实际尺寸")
		toolbarItem:SetIconImage("toolbar.yibiyi.normal", "toolbar.yibiyi.hover",
								 "toolbar.yibiyi.down", "toolbar.yibiyi.disable")
	end
end

function UpdateFileList(self, bMustReLoad)
	LOG("KuaikanLog UpdateFileList " .. tostring(bMustReLoad))
	local attr = self:GetAttribute()
	if (not attr.CurDocItem) or (not attr.CurDocItem.FilePath) then 
		return
	end
	local CurPath = Helper.APIproxy.GetParentPath(attr.CurDocItem.FilePath)
	if CurPath == "" then return end 

	
	local listViewObj = self:GetControlObject("client.filelistview")
	local curFileList = listViewObj:GetFileList()
	if #curFileList < 1 then	
		local tipUtil = Helper.APIproxy.GetTipUtil()
		local fileList = tipUtil:GetFiles(CurPath)
		if fileList then
			attr.CurFolderDir = CurPath
			if attr.MonitorCookie then
				tipUtil:UnMonitorDirChange(attr.MonitorCookie)
			end
			attr.MonitorCookie = tipUtil:MonitorDirChange(attr.CurFolderDir)
			Helper.APIproxy.SortImageListFile(self, fileList)
			for i=1, #fileList do
				local data = {}
				data.ItemId = fileList[i].FilePath
				data.UserData = {}
				data.UserData.FilePath = fileList[i].FilePath
				data.UserData.LastWriteTime = fileList[i].LastWriteTime
				if fileList[i].FilePath == attr.CurDocItem.FilePath then
					data.Selected = true
				end
				listViewObj:InsertItem(data)
			end
			
			listViewObj:Update()
		else
			attr.CurFolderDir = nil
		end
	else	
		local oldFolder = string.lower(Helper.APIproxy.GetParentPath(curFileList[1]))
		local newFolder = string.lower(Helper.APIproxy.GetParentPath(attr.CurDocItem.FilePath))
		
		local ItemObj = listViewObj:GetItemById(attr.CurDocItem.FilePath)
		if oldFolder == newFolder and ItemObj and (not bMustReLoad) then 
			LOG("KuaikanLog one family not need reload")
		else	
			listViewObj:ClearItems()
			
			local tipUtil = Helper.APIproxy.GetTipUtil()
			local fileList = tipUtil:GetFiles(CurPath)
			LOG("KuaikanLog Two tigers cannot live on the same mountain #fileList：" .. #fileList)
			if fileList then
				attr.CurFolderDir = CurPath
				if oldFolder ~= newFolder then 
					if attr.MonitorCookie then
						tipUtil:UnMonitorDirChange(attr.MonitorCookie)
					end
					attr.MonitorCookie = tipUtil:MonitorDirChange(attr.CurFolderDir)
				end
				Helper.APIproxy.SortImageListFile(self, fileList)
				for i=1, #fileList do
					local data = {}
					data.ItemId = fileList[i].FilePath
					data.UserData = {}
					data.UserData.FilePath = fileList[i].FilePath
					data.UserData.LastWriteTime = fileList[i].LastWriteTime
					if fileList[i].FilePath == attr.CurDocItem.FilePath then
						data.Selected = true
					end
					listViewObj:InsertItem(data)
				end
				
				listViewObj:Update()
			else
				attr.CurFolderDir = nil
			end
		end
	end
end


function Reset(self)
	
	local attr = self:GetAttribute()
	if attr.PreDocItem and attr.PreDocItem.DocObj then
		attr.PreDocItem.DocObj:Release()
	end
	attr.PreDocItem = nil
	
	if attr.CurDocItem and attr.CurDocItem.DocObj then
		attr.CurDocItem.DocObj:Release()
	end
	attr.CurDocItem = nil
	
	if attr.NextDocItem and attr.NextDocItem.DocObj then
		attr.NextDocItem.DocObj:Release()
	end
	attr.NextDocItem = nil
	
	
	attr.ScaleCallBackParam = nil
	
	
	g_ViewIndex = g_ViewIndex + 1
	
	local viewObj = self:GetViewObj()
	local viewBkgObj = self:GetControlObject("client.view.bkg")
	viewBkgObj:RemoveChild(viewObj)
	
	
	attr.CurFolderDir = nil
	attr.CacheFilePath = nil
	
	self:UpdateTitle()
	
	local normalToolbar = self:GetControlObject("client.toolbarnew")
	normalToolbar:EnableToolbarItem({"src", "max", "min", "preFile", "nextFile", "leftrotate", "rightrotate", "del"}, false)
end

function OnPosChange(self)
	
	local thumbnailview = self:GetControlObject("client.thumbnailview")
	thumbnailview:SetObjPos2("father.width-131", "father.height-156-70", 131, 156)
end



function OnInitControl(self)
	local attr = self:GetAttribute()
	attr.CurFolderDir = nil 		
	attr.bFullScreen = false					
	attr.FileList = {}				
	attr.PreDocItem = nil;			
	attr.CurDocItem = nil;			
	attr.NextDocItem = nil			
	attr.CacheFilePath = nil		
	attr.bBusying = false	
	self:ShowWaitingPanel(false)	
	self:SetStatus(Status_Init)
	
	self:ShowErrorIcon(false)
	self:ShowFullScreenQutiTip(false)
	--隐藏缩略图
	local imagePool = self:GetControlObject("client.filelistview")
	imagePool:ShowContent(false)
	--隐藏工具栏
	local normalToolbar = self:GetControlObject("client.toolbarnew")
	normalToolbar:ShowContent(false)
	
	local FolderMonitorManager = Helper.APIproxy.FolderMonitorManager
	local function OnFolderFileChange(oldFilePath, newFilePath, eventType)
		local ChangedDirPathPath = Helper.APIproxy.GetParentPath(oldFilePath)
		
		if attr.CurFolderDir == ChangedDirPathPath then 
			local listViewObj = self:GetControlObject("client.filelistview")
			local curFileList = listViewObj:GetFileList()
			LOG("KuaikanLog eventType:" .. eventType)
			if eventType == 5 then 
				local ItemObj = listViewObj:GetItemById(oldFilePath)
				if ItemObj then	
					local data = {}
					data.ItemId = newFilePath
					data.UserData = {}
					data.UserData.FilePath = newFilePath
					data.UserData.LastWriteTime = ItemObj.UserData.LastWriteTime
					listViewObj:ReplaceItem(oldFilePath, data)
					
					
					if attr.CurDocItem and string.lower(attr.CurDocItem.FilePath) == string.lower(oldFilePath) then
						attr.CurDocItem.FilePath = newFilePath
						
						self:UpdateTitle()
						
						if attr.CurDocItem.DocObj then
							attr.CurDocItem.DocObj:SetFileName(Helper.APIproxy.GetFileNameFromPath(newFilePath))
							attr.CurDocItem.DocObj:SetFilePath(newFilePath)
						end
					end
				else 
				end
			elseif eventType == 1 then 
				
				local tipUtil = Helper.APIproxy.GetTipUtil()
				if tipUtil:IsCanHandleFileCheckByExt(oldFilePath) then
					if not listViewObj:GetItemById(oldFilePath) then 
						local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
						g_AddFileToPoolIndex = g_AddFileToPoolIndex + 1
						local AddFileToPoolIndex = g_AddFileToPoolIndex
						local function OnTimer()
							if g_AddFileToPoolIndex == AddFileToPoolIndex then
								self:UpdateFileList(true)
								self:UpdateTitle()
							end
						end
						timerManager:SetOnceTimer(OnTimer, 300)
					else
						LOG("KuaikanLog yijing youle")
					end
				end
			elseif eventType == 2 then 
				local ItemObj = listViewObj:GetItemById(oldFilePath)
				if ItemObj then	
					if #curFileList == 1 then 
						
						listViewObj:ClearItems()
						
						self:Reset()
					else
						oldFilePath = string.lower(oldFilePath)
						if attr.CurDocItem and oldFilePath == string.lower(attr.CurDocItem.FilePath) then 
							self:NextFile(true)
							
							if attr.PreDocItem and attr.PreDocItem.DocObj then
								attr.PreDocItem.DocObj:Release()
							end
							attr.PreDocItem = nil
						elseif attr.PreDocItem and oldFilePath == string.lower(attr.PreDocItem.FilePath) then
							
							if attr.PreDocItem.DocObj then
								attr.PreDocItem.DocObj:Release()
							end
							attr.PreDocItem = nil
						elseif attr.NextDocItem and oldFilePath == string.lower(attr.NextDocItem.FilePath) then
							
							if attr.NextDocItem.DocObj then
								attr.NextDocItem.DocObj:Release()
							end
							attr.PreDocItem = nil
						end
						
						local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
						g_DeleteFileFromPoolIndex = g_DeleteFileFromPoolIndex + 1
						local DeleteFileFromPoolIndex = g_DeleteFileFromPoolIndex
						local function OnTimer()
							if g_DeleteFileFromPoolIndex == DeleteFileFromPoolIndex then
								self:UpdateFileList(true)
								self:UpdateTitle()
							end
						end
						timerManager:SetOnceTimer(OnTimer, 300)
						
					end
				else
					LOG("KuaikanLog not in list")
					
				end
			--[[elseif eventType == 3 then--覆盖原图时触发	
				local tipUtil = Helper.APIproxy.GetTipUtil()
				local function OnTimer()
					local fileList = tipUtil:GetFiles(ChangedDirPathPath)
					if fileList then
						for _, v in ipairs(fileList) do
							if v.FilePath == oldFilePath then
								local ItemObj = listViewObj:GetItemById(oldFilePath)
								if ItemObj and ItemObj.Obj then
									ItemObj.UserData.LastWriteTime = v.LastWriteTime
									ItemObj.Obj:UpdateThumnails(true)
								end
								break
							end
						end
					end
				end
				SetOnceTimer(OnTimer, 300)]]--
			end
		end
	end
	FolderMonitorManager.AttachListener(OnFolderFileChange)
end
function ShowWaitingPanel(self, bShow, bShowAni, width)
	--不做等待页面
end

function SetWaitingPanelText(self, text)
	--不做等待页面
end

function SetWaitingPanelProcessText(self, text)
	--不做等待页面
end

function ResetViewObj(self)
	local attr = self:GetAttribute()
	attr.ScaleCallBackParam = nil
	
	
	local normalToolbar = self:GetControlObject("client.toolbarnew")
	local viewObj = self:GetControlObject("client.view")
	local viewType = -1
	if viewObj then
		viewType = viewObj:GetViewType()
	end
	if attr.CurDocItem.DocObj and viewType ~= attr.CurDocItem.DocObj:GetDocType() then
		local viewLayoutObj = self:GetControlObject("client.view.layout")
		
		viewLayoutObj:RemoveChild(viewObj)
		
		local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
		if attr.CurDocItem.DocObj:GetDocType() ~= 2 then 
			viewObj = objFactory:CreateUIObject("client.view", "Kuaikan.ImageView.View.Normal")
			normalToolbar:EnableToolbarItem({"leftrotate", "rightrotate"}, true)
		else 
			viewObj = objFactory:CreateUIObject("client.view", "Kuaikan.ImageView.View.Gif")
			
			normalToolbar:EnableToolbarItem({"leftrotate", "rightrotate"}, false)
		end
		viewObj:AttachListener("OnScaleRatioChanged", true, View_OnScaleRatioChanged)
		viewObj:AttachListener("OnLButtonDbClick", true, View_OnLButtonDbClick)
		viewObj:AttachListener("OnPreFileClick", true, View_OnPreFileClick)
		viewObj:AttachListener("OnNextFileClick", true, View_OnNextFileClick)
		viewObj:AttachListener("OnRButtonDown", true, View_OnRButtonDown)
		viewObj:AttachListener("OnLButtonDown", true, View_OnLButtonDown)
		viewLayoutObj:AddChild(viewObj)
		viewObj:SetObjPos2(0,0,"father.width","father.height")
	end
	
	
	if viewObj then
		viewObj:ResetDoc(attr.CurDocItem.DocObj)
	end
	
	
	if attr.CurDocItem.DocObj then
		
		normalToolbar:EnableToolbarItem({"src", "max", "min"}, true)
		if attr.CurDocItem.DocObj:GetDocType() ~= 2 then 
			normalToolbar:EnableToolbarItem({"leftrotate", "rightrotate"}, true)
		end
		
		self:ShowErrorIcon(false)
	else
		
		
		local thumbnailview = self:GetControlObject("client.thumbnailview")
		SetObjVisible(thumbnailview, false)
	end
end

function ScaleImage(self, RatioOffset, Ration)
	local attr = self:GetAttribute()
	if (not attr.CurDocItem) or (not attr.CurDocItem.DocObj) then
		return
	end
	local viewObj = self:GetControlObject("client.view")
	local oldRatio = viewObj:GetScaleRatio()
	
	local tipUtil = Helper.APIproxy.GetTipUtil()
	offset = tipUtil:GetCurveProgress(oldRatio/10) * 1.0
	
	if RatioOffset > 0 then
		RatioOffset = offset
	else
		RatioOffset = 0-offset
	end
	local newRatio = oldRatio + RatioOffset
	if Ration then
		newRatio = Ration
	end
	if newRatio < 0.01 then
		newRatio = 0.01
	end
	
	local viewType = viewObj:GetViewType()
	if viewType ~= 2 then	
		if newRatio > 10 then	
			newRatio = 10
		end
	else	
		if newRatio > 3 then	
			newRatio = 3
		end
	end
	
	if oldRatio == newRatio then
		return
	end
	viewObj:SetScaleRatio(newRatio)
end



function SetStatus(self, newStatus, fref)
	if not fref then
		fref = ""
	end
	local attr = self:GetAttribute()
	attr.Status = newStatus
	LOG("KuaikanLog Status=" .. attr.Status .. "  fref=" .. fref)
end

function SetScaleCallBackParam(self, param)
	local attr = self:GetAttribute()
	attr.ScaleCallBackParam = param
	self:LoadTotalImageFile(attr.CurDocItem.FilePath, g_ViewIndex)
end

function UpdateTitle(self)
	local attr = self:GetAttribute()
	
	local captionObj = self:GetCaption()
	if (not attr.CurDocItem) or (not attr.CurDocItem.FilePath) then	
		captionObj:SetFilePath("", "", "", "")
		return
	end
	
	
	local listView = self:GetControlObject("client.filelistview")
	local curIndex = listView:GetSelectedItemIndex()
	local totalCount = listView:GetItemCount()
	if curIndex then
		curIndex = "第" .. curIndex .. "/" .. totalCount .. "张"
	else
		curIndex = ""
	end
	
	
	if not attr.CurDocItem.DocObj then	
		captionObj:SetFilePath(attr.CurDocItem.FilePath, "", "", curIndex)
		return
	end
	
	
	local text = "("
	local srcWidth, srcHeight = attr.CurDocItem.DocObj:GetSrcBitmapSize()
	text = text .. srcWidth .. "*" .. srcHeight .. "像素,"
	
	local fileSize = attr.CurDocItem.DocObj:GetFileSize()
	if fileSize < 1024 then
		fileSize = fileSize .. " Byte"
	elseif fileSize < 1048576 then
		fileSize = string.format("%0.2f KB", fileSize/1024)
	else
		fileSize = string.format("%0.2f MB", fileSize/1048576)
	end
	text = text .. fileSize .. ")"
	
	local viewObj = self:GetControlObject("client.view")
	local curRatio = viewObj:GetScaleRatio()
	curRatio = curRatio*100-curRatio*100%1 .. "%"
	captionObj:SetFilePath(attr.CurDocItem.FilePath, text, curRatio, curIndex)
end

function View_OnScaleRatioChanged(self)
	local owner = self:GetOwnerControl()
	owner:UpdateTitle()
	local viewObj = owner:GetControlObject("client.view")
	local curRatio = viewObj:GetScaleRatio()
	local adapterRatio = viewObj:GetAdaptedScreenRatio()
	local normalToolbar = owner:GetControlObject("client.toolbarnew")
	local bMustShowAdapter = true
	if math.abs(adapterRatio-1) <= 0.00001 then 
		if math.abs(curRatio-1) <= 0.00001 then
			bMustShowAdapter = false
		else
			bMustShowAdapter = true
		end
	else
		if math.abs(curRatio-1) <= 0.00001 then 
			bMustShowAdapter = true
		else 
			if math.abs(curRatio-adapterRatio) <= 0.00001 then
				bMustShowAdapter = false
			else
				bMustShowAdapter = true
			end
		end	
	end
	if bMustShowAdapter then
		local toolbarItem = normalToolbar:GetToolbarItem("src")
		toolbarItem:AddTipText("适应窗口")
		toolbarItem:SetIconImage("toolbar.shiping.normal", "toolbar.shiping.hover",
								 "toolbar.shiping.down", "toolbar.shiping.disable")
	else
		local toolbarItem = normalToolbar:GetToolbarItem("src")
		toolbarItem:AddTipText("实际尺寸")
		toolbarItem:SetIconImage("toolbar.yibiyi.normal", "toolbar.yibiyi.hover",
								 "toolbar.yibiyi.down", "toolbar.yibiyi.disable")
	end
	
	
	local thumbnailviewObj = owner:GetControlObject("client.thumbnailview")
	thumbnailviewObj:UpdateTitle(curRatio)
end

function View_OnLButtonDbClick(self)
	local owner = self:GetOwnerControl()
	
	local DoubleClickActionType = "0"
	if DoubleClickActionType == "1" then
		owner:FullScreenSwitch()
	else
		owner:AdapterOrSrcScreen()
	end
	
end

function View_OnRButtonDown(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if owner_attr.Status == Status_Loading or owner_attr.Status == Status_Scaling or owner_attr.Status == Status_Resizing then	
		return
	end
	if (not owner_attr.CurDocItem) or (not owner_attr.CurDocItem.DocObj) then
		return
	end
	local curX, curY = Helper.tipUtil:GetCursorPos()
	local tree = self:GetOwner()
	local wnd = tree:GetBindHostWnd()
	local GreenShieldMenu = XLGetGlobal("GreenShieldMenu")	
	local menuTable = GreenShieldMenu.ImageRClickMenu.menuTable
	local menuFunTable = GreenShieldMenu.ImageRClickMenu.menuFunTable
	local userData = {}
	userData.imageCtrl = self:GetOwnerControl()
	Helper:CreateMenu(curX, curY, wnd:GetWndHandle(), menuTable, menuFunTable, userData)
end

function View_OnPreFileClick(self)
	local owner = self:GetOwnerControl()
	owner:PreFile()
end
function View_OnNextFileClick(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	owner:NextFile()
end

function View_OnLButtonDown(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if owner_attr.bFullScreen then
		owner:FullScreenSwitch()
	end
end

local hasSend = false
function SendShowUI()
	if hasSend then return end
	hasSend = true
	--启动上报
	StatUtil.SendStat({
		strEC = "showui",
		strEA = StatUtil.GetMainVer(),
		strEL = "fullsrceen",
		strEV = 1,
	}) 
end

function FullScreenSwitch(self)
	local attr = self:GetAttribute()
	local tipUtil = Helper.APIproxy.GetTipUtil()
	local objTree = self:GetOwner()
	local hostWnd = objTree:GetBindHostWnd()
	if not attr.bFullScreen then	
		attr.bFullScreen = true
		SendShowUI()
		local l,t,r,b = hostWnd:GetWindowRect()
		attr.oldWndL,attr.oldWndT,attr.oldWndWidth,attr.oldWndHeight = l,t,r-l,b-t
		
		local xScreen, yScreen = tipUtil:GetScreenRatio()
		hostWnd:SetMaxTrackSize(xScreen, yScreen)
		
		--调整工具栏
		--[[local suspendtoolbar = self:GetControlObject("client.toolbarnew")
		suspendtoolbar:SetObjPos2(0,"father.height-70","father.width",70)
		SetObjVisible(suspendtoolbar, true)
		suspendtoolbar:ShowContent(false)]]
		
		
		--调整缩略图
		--[[local listViewObj = self:GetControlObject("client.filelistview")
		listViewObj:SetObjPos2(1, "father.height-70-70", "father.width-2", 70)
		listViewObj:ShowContent(false)]]--
		
		--调整根节点位置
		--[[local objTree = self:GetOwner()
		local treeBkg = objTree:GetUIObject("mainwnd.bkg")
		treeBkg:SetTextureID("")
		treeBkg:SetObjPos2(0, 0, "father.width", "father.height")]]--
		
		
		--设置背景色
		local viewBkgObj = self:GetControlObject("client.bkg")
		viewBkgObj:SetObjPos2(0,0,"father.width", "father.height")
		viewBkgObj:SetSrcColor("RGBA(0,0,0,180)")
		viewBkgObj:SetDestColor("RGBA(0,0,0,180)")
		viewBkgObj:SetAlpha(200)
		self:GetControlObject("client.view.bkg"):SetObjPos2(0,0,"father.width", "father.height")
		
		
		
		local captionObj = self:GetCaption()
		captionObj:SetObjPos2(0,0,0,0)
		captionObj:ShowContent(false)
		
		hostWnd:Move(0, 0, xScreen, yScreen)
		hostWnd:BringWindowToTop()
		hostWnd:SetTopMost(true)
		self:ShowFullScreenQutiTip(true)
	else 	
		attr.bFullScreen = false
		
		local xScreen, yScreen = tipUtil:GetWorkAreaSize()
		hostWnd:SetMaxTrackSize(xScreen, yScreen)
				
		
		--[[local objTree = self:GetOwner()
		local treeBkg = objTree:GetUIObject("mainwnd.bkg")
		treeBkg:SetTextureID("texture.mainwnd.bkg")
		treeBkg:SetObjPos2(0, 0, "father.width", "father.height")]]--
		
		
		local captionObj = self:GetCaption()
		captionObj:SetObjPos2(1,1,"father.width-2",35)
		captionObj:ShowContent(true)
		
		
		--[[调整工具栏
		local toolbar = self:GetControlObject("client.toolbarnew")
		toolbar:SetObjPos2(0,"father.height-42","father.width",42)
		SetObjVisible(toolbar, true)
		toolbar:ShowContent(true)]]--
		
		--设置背景色
		local viewBkgObj = self:GetControlObject("client.bkg")
		viewBkgObj:SetAlpha(255)
		viewBkgObj:SetSrcColor("RGBA(255,255,255,255)")
		viewBkgObj:SetDestColor("RGBA(255,255,255,255)")
		self:GetControlObject("client.view.bkg"):SetObjPos2(1, 36, "father.width-2", "father.height-36")
		self:GetControlObject("client.bkg"):SetAlpha(255)
		
		--[[调整列表位置
		local listViewObj = self:GetControlObject("client.filelistview")
		listViewObj:SetObjPos2(1, "father.height-70", "father.width-2", 70)]]--
		
		
		hostWnd:Move(attr.oldWndL,attr.oldWndT,attr.oldWndWidth,attr.oldWndHeight)
		hostWnd:BringWindowToTop()
		hostWnd:SetTopMost(false)
		self:ShowFullScreenQutiTip(false)
	end
end

function SuspendToolbar_MouseEnter(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	owner_attr.MouseInToolbar = true
	self:ShowContent(true)
	local imagePool = owner:GetControlObject("client.filelistview")
	imagePool:ShowContent(true)
end

function SuspendToolbar_MouseLeave(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	owner_attr.MouseInToolbar = false
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	local function OnTimer()
		if owner_attr.MouseInListView then 
			return
		end
		self:ShowContent(false)
		local imagePool = owner:GetControlObject("client.filelistview")
		imagePool:ShowContent(false)
	end
	
	timerManager:SetOnceTimer(OnTimer, 100)
end

function NormalToolbar_OnBtnClick(self, eventName, itemID)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	if itemID == "btn.src" then	
		owner:AdapterOrSrcScreen()
	elseif itemID == "btn.max" then	
		if (not owner_attr.CurDocItem) or (not owner_attr.CurDocItem.DocObj) then
			return
		end
		owner:ScaleImage(0.1)
	elseif itemID == "btn.min" then	
		if (not owner_attr.CurDocItem) or (not owner_attr.CurDocItem.DocObj) then
			return
		end
		owner:ScaleImage(-0.1)
	elseif itemID == "btn.preFile" then	
		owner:PreFile()
	elseif itemID == "btn.nextFile" then	
		owner:NextFile()
	elseif itemID == "btn.leftrotate" then	
		owner:LeftRotate()
	elseif itemID == "btn.rightrotate" then	
		owner:RightRotate()
	elseif itemID == "btn.del" then 
		owner:DeleteFile()
	end
end

function LeftRotate(self)
	local viewObj = self:GetViewObj()
	viewObj:LeftRotate()
end

function RightRotate(self)
	local viewObj = self:GetViewObj()
	viewObj:RightRotate()
end

function IsFullScreen(self)
	local attr = self:GetAttribute()
	return attr.bFullScreen
end

function GetCaption(self)
	return self:GetControlObject("client.caption")
end

function GetViewObj(self)
	return self:GetControlObject("client.view")
end

function GetThumbnailView(self)
	return self:GetControlObject("client.thumbnailview")
end

function ListView_OnItemClick(self, eventName, itemId)
	local owner = self:GetOwnerControl()
	local item = self:GetItemById(itemId)
	if item then
		
		local function FnCallBack()
			self:SelectItem(itemId)
			owner:UpdateTitle()
		end
		owner:LoadImageFile(item.UserData.FilePath, nil, FnCallBack, nil)
	end
end	

function ListView_OnPreOrNextFile(self, eventName, bNextFile)
	local owner = self:GetOwnerControl()
	if bNextFile then
		owner:NextFile()
	else
		owner:PreFile()
	end
end

function ListView_OnControlMouseEnter(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	owner_attr.MouseInListView = true
	local imagePool = owner:GetControlObject("client.filelistview")
	imagePool:ShowContent(true)
	local suspendToolbar = owner:GetControlObject("client.toolbarnew")
	suspendToolbar:ShowContent(true)
end

function ListView_OnControlMouseLeave(self)
	local owner = self:GetOwnerControl()
	local owner_attr = owner:GetAttribute()
	owner_attr.MouseInListView = false
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	local function OnTimer()
		if owner_attr.MouseInToolbar then 
			return
		end
		self:ShowContent(false)
		local suspendToolbar = owner:GetControlObject("client.toolbarnew")
		suspendToolbar:ShowContent(false)
	end
	
	timerManager:SetOnceTimer(OnTimer, 1)
end

function OpenFile(self)
	local tipUtil = Helper.APIproxy.OSUtil
	local fileFilter = "所有支持的图片格式(jpeg文件;bmp文件;png文件;gif文件;tiff文件;raw文件;其它)|*.jpg;*.jpeg;*.jpe;*.bmp;*.png;*.gif;*.tiff;*.tif;*.psd;*.ico;*.pcx;*.tga;*.wbm;*.ras;*.mng;*.hdr;*.cr2;*.nef;*.arw;*.dng;*.srf;*.raf;*.wmf|jpeg文件(*.jpg; *.jpeg; *.jpe)|*.jpg; *.jpeg; *.jpe|bmp文件(*.bmp)|*.bmp|png文件(*.png)|*.png|gif文件(*.gif)|*.gif|tiff文件(*.tiff; *.tif)|*.tiff; *.tif|raw文件(*.cr2;*.crw;*.nef)|*.cr2;*.crw;*.nef|All Files(*.*)|*.*||"
	local tbLocalFiles = tipUtil:FileDialog(true, true, fileFilter, "", "", "")
	if #tbLocalFiles == 0 then
		return
	end
	
	local function FnCallBack()
		
		self:UpdateFileList()
		
		local attr = self:GetAttribute()
		local listViewObj = self:GetControlObject("client.filelistview")
		listViewObj:SelectItem(attr.CurDocItem.FilePath)
		self:UpdateTitle()
	end
	self:LoadImageFile(tbLocalFiles[1], nil, nil, FnCallBack)
end

function GetCurDocItem(self)
	local attr = self:GetAttribute()
	return attr.CurDocItem
end

function IsAutoRotate(self)
	return false
end

function ShowFullScreenQutiTip(self, bShow)
	local attr = self:GetAttribute()
	local ScreenQutiTip = self:GetControlObject("client.fullscreen.quit")
	SetObjVisible(ScreenQutiTip, bShow)
end

function OnClickCloseBtnWhileFullScreen(self)
	local owner = self:GetOwnerControl()
	owner:FullScreenSwitch()
end

function HandleUpDownWard(self, bUp)
	local viewObj = self:GetControlObject("client.view")
	if viewObj then
		viewObj:HandleUpDownWard(bUp)
	end
end

function ThumbnailView_OnViewRectChanged(self, eventName, offsetX, offsetY)
	local owner = self:GetOwnerControl()
	local viewObj = owner:GetViewObj()
	viewObj:HandleThumbnailViewChange(offsetX, offsetY)	
end

function ThumbnailView_OnViewRectMouseWheel(self, eventName, flags)
	local owner = self:GetOwnerControl()
	local viewObj = owner:GetViewObj()
	viewObj:HandleThumbnailViewMouseWheel(flags)
end

function ThumbnailView_OnViewRectLButtonDown(self, eventName, xPercent, yPercent)
	local owner = self:GetOwnerControl()
	local viewObj = owner:GetViewObj()
	viewObj:HandleThumbnailViewLButtonDown(xPercent, yPercent)
end

function PrintImage(self)
	local attr = self:GetAttribute()
	if (not attr.CurDocItem) or (not attr.CurDocItem.DocObj) then
		return
	end
	Helper.APIproxy.GetTipUtil():PrintImage(attr.CurDocItem.FilePath)
end

function IsBusying(self)
	local attr = self:GetAttribute()
	return attr.bBusying
end


function GetCurFilePos(self)
	
	local listView = self:GetControlObject("client.filelistview")
	local curIndex = listView:GetSelectedItemIndex()
	local totalCount = listView:GetItemCount()
	
	return curIndex,totalCount
end

