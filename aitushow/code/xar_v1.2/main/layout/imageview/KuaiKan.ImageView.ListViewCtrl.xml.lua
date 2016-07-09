local g_ImgMaxWidth = 185
local g_ImgMaxHeight = 185

function SetObjVisible(obj, bVisible)
	if not obj then 
		return 
	end
	if bVisible then
		obj:SetVisible(true)
		obj:SetChildrenVisible(true)
	else
		obj:SetVisible(false)
		obj:SetChildrenVisible(false)
	end
end

function Item_OnInitControl(self)
	self:Reset()
end

function Item_OnDestroy(self)

end

function Item_Reset(self)
	local attr = self:GetAttribute()
	self:SetObjPos(0,0,0,0)
	local thumbnail = self:GetControlObject("client.thumbnail")
	if attr.ItemInfo then
		attr.ItemInfo.Obj = nil
		attr.ItemInfo = nil
	end
end

function Item_SetData(self, data)
	local attr = self:GetAttribute()
	attr.ItemInfo = data
	self:Refresh()
end

function Item_Refresh(self)
	self:UpdateBkg()
	self:SetDefaultThumbnail()
	self:UpdateThumnails()
end

function Item_UpdateBkg(self)
	local attr = self:GetAttribute()
	local bkgObj = self:GetControlObject("client.bkg")
	if attr.ItemInfo.Selected then
		bkgObj:SetSrcColor("4FC4F6")
		bkgObj:SetDestColor("4FC4F6")
	else
		bkgObj:SetSrcColor("394264")
		bkgObj:SetDestColor("394264")
	end
end

function Item_UpdateThumnails(self, bMustReGetThumnail)
	local attr = self:GetAttribute()	
	if (not bMustReGetThumnail) and  attr.ItemInfo.Thumbnails then
		self:SetThumbnail(attr.ItemInfo.Thumbnails.Bitmap, attr.ItemInfo.Thumbnails.Width, attr.ItemInfo.Thumbnails.Height)
	else
		local owner = self:GetOwnerControl()
		owner:LoadThumbnails(attr.ItemInfo.ItemId, g_ImgMaxWidth, g_ImgMaxHeight, not bMustReGetThumnail)
	end
end

function Item_SetDefaultThumbnail(self, extName)
	local attr = self:GetAttribute()
	local thumbnailObj = self:GetControlObject("client.thumbnail")
	thumbnailObj:SetObjPos2("(father.width-32)/2", "(father.height-32)/2", 32, 32)
	if extName then
		thumbnailObj:SetResID("default_icon"..extName)
	end
end

function Item_SetThumbnail(self, bitmap, bitmapWidth, bitmapHeight)
	local thumbnailLayout = self:GetControlObject("client.thumbnail.layout")
	local thumbnailObj = self:GetControlObject("client.thumbnail")
	local l,t,r,b = thumbnailLayout:GetObjPos()
	local width, height = r-l, b-t
	if (bitmapWidth <= width) and (bitmapHeight <= height) then
		width = bitmapWidth
		height = bitmapHeight
	else
		local ratio = bitmapWidth/bitmapHeight
		local needWidth = math.ceil(ratio*height)
		if needWidth > width then
			height = math.ceil(width/ratio)
		else
			width = needWidth
		end
	end
	thumbnailObj:SetObjPos2((r-l-width)/2, (b-t-height)/2, width, height)
	thumbnailObj:SetBitmap(bitmap)
end

function Item_UpdateSelectStatus(self)
	self:UpdateBkg()
end

function Item_OnMouseEnter(self)
	local owner = self:GetOwnerControl()
	local ownerAttr = owner:GetAttribute()
	if not ownerAttr.ItemInfo.Selected then
		self:SetSrcColor("4FC4F6")
		self:SetDestColor("4FC4F6")
	end
end

function Item_OnMouseLeave(self)
	local owner = self:GetOwnerControl()
	local ownerAttr = owner:GetAttribute()
	if ownerAttr.ItemInfo and (not ownerAttr.ItemInfo.Selected) then
		self:SetSrcColor("394264")
		self:SetDestColor("394264")
	end
end

function Item_OnMouseWheel(self)
	self:RouteToFather()
end

function Item_OnChar(self)

end

function Item_OnKeyDown(self)

end

function Item_OnLButtonDown(self)
	local owner = self:GetOwnerControl()
	owner:FireExtEvent("OnItemClick")
end

function Reset(self)
	local attr = self:GetAttribute()
	attr.FileItemList = {}
	attr.Id2DataItemMap = {}
	attr.SelectedList = {}
	attr.ThumbnailsLoading = {}
	attr.CurPos = 0
	
	if attr.ThumbnailsLoader then
		attr.ThumbnailsLoader:Clear()
	end
end

function OnInitControl(self)
	local attr = self:GetAttribute()
	
	local ThumbnailsLoader = Helper.APIproxy.CreateLoader()
	if ThumbnailsLoader then
		attr.LoadThumbnailCookie = ThumbnailsLoader:AttatchLoadCompleteEvent(function (bitmap, width, height, filePath)
			if bitmap then
				local attr = self:GetAttribute()
				local Item = attr.Id2DataItemMap[filePath]
				if Item then
					Item.Thumbnails = {}
					local LuaImageProcessor = Helper.APIproxy.GetLuaImageProcessor()
					Item.Thumbnails.Bitmap = LuaImageProcessor:PreMultiplyBitmap(bitmap)
					Item.Thumbnails.Bitmap = bitmap
					Item.Thumbnails.Width = width
					Item.Thumbnails.Height = height
					if Item.Obj then
						Item.Obj:SetThumbnail(bitmap, width, height)
					end
				end
			end
			if filePath then
				attr.ThumbnailsLoading[filePath] = nil
			end
		end)
		attr.ThumbnailsLoader = ThumbnailsLoader
	end
	
	self:Reset()
	self:SetEnable(true)
end

function OnDestroy(self)
	local attr = self:GetAttribute()
	if attr.ThumbnailsLoader and attr.LoadThumbnailCookie then
		attr.ThumbnailsLoader:DetatchLoadCompleteEvent(attr.LoadThumbnailCookie)
		attr.LoadThumbnailCookie = nil
		attr.ThumbnailsLoader = nil
	end
end

function InsertItem(self, data, bUpdateView)
	local attr = self:GetAttribute()
	if type(data) ~= "table" then
		return false
	end
	if not data.ItemId then
		return false
	end
	data.ItemId = string.lower(data.ItemId)
	if attr.Id2DataItemMap[data.ItemId] then	
		return true
	end
	
	local item = {}
	item.ItemId = data.ItemId
	item.UserData = data.UserData
	if data.Selected then
		item.Selected = true
	else
		item.Selected = false
	end
	item.Obj = nil
	
	attr.Id2DataItemMap[item.ItemId] = item
	table.insert(attr.FileItemList, item)
	if item.Selected then
		table.insert(attr.SelectedList, item)
	end
	
	if bUpdateView then
		self:Update()
	end
end

function ReplaceItem(self, itemId, data)
	local attr = self:GetAttribute()
	if (not itemId) or type(data) ~= "table" or (not data.ItemId) then 
		return
	end
	itemId = string.lower(itemId)
	if not attr.Id2DataItemMap[itemId] then	
		return
	end
	data.ItemId = string.lower(data.ItemId)
	
	if itemId == data.ItemId then
		for i=1, #attr.FileItemList do
			if attr.FileItemList[i].ItemId == itemId then
				attr.FileItemList[i].ItemId = data.ItemId
				attr.FileItemList[i].UserData = data.UserData
				attr.Id2DataItemMap[itemId] = nil
				attr.Id2DataItemMap[data.ItemId] = attr.FileItemList[i]
				attr.FileItemList[i].Thumbnails = nil
				break
			end
		end
	else
		
		if attr.Id2DataItemMap[data.ItemId] then
			attr.Id2DataItemMap[data.ItemId] = nil
		end
		for i=1, #attr.FileItemList do
			if attr.FileItemList[i].ItemId == data.ItemId then
				table.remove(attr.FileItemList, i)
				break
			end
		end
		
		for i=1, #attr.FileItemList do
			if attr.FileItemList[i].ItemId == itemId then
				attr.FileItemList[i].ItemId = data.ItemId
				attr.FileItemList[i].UserData = data.UserData
				attr.Id2DataItemMap[itemId] = nil
				attr.Id2DataItemMap[data.ItemId] = attr.FileItemList[i]
				attr.FileItemList[i].Thumbnails = nil
				break
			end
		end
	end
	self:Update()
end

function DeleteItem(self, itemId)
	local attr = self:GetAttribute()
	if not itemId then 
		return
	end
	itemId = string.lower(itemId)
	if not attr.Id2DataItemMap[itemId] then	
		return
	end
	for i=1, #attr.FileItemList do
		if attr.FileItemList[i].ItemId == itemId then
			table.remove(attr.FileItemList, i)
			break
		end
	end
	
	for i=1, #attr.SelectedList do
		if attr.SelectedList[i].ItemId == itemId then
			table.remove(attr.SelectedList, i)
			break;
		end
	end
	attr.Id2DataItemMap[itemId] = nil
	self:Update()
end

function ClearItems(self)
	self:Reset()
	self:Update()
end

function Update(self)
	local attr = self:GetAttribute()
	local contentObj = self:GetControlObject("Client.Content")
	if #attr.FileItemList == 0 then	
		
		local contentBkg = self:GetControlObject("Client.Content.bkg")
		contentBkg:SetObjPos(0, 0, 0 , 0)
		
		local objCount = contentObj:GetChildCount()
		for i = 0, objCount-1 do
			local obj = contentObj:GetChildByIndex(i)
			obj:Reset()
		end
		
		return
	end
	
	local l, t, r, b = contentObj:GetObjPos()
	local ClientWidth = r - l
	
	local ItemWidth = attr.XInternal + attr.ItemWidth
	local ItemCount = #attr.FileItemList
	local needWidth = ItemWidth * ItemCount - attr.XInternal
	
	local startPos = 0	
	local NeedToShowItemCount = 0 	
	local startObjIndex = 1 	
	
	if needWidth <= ClientWidth then	
		startPos = (ClientWidth-needWidth)/2
		NeedToShowItemCount = ItemCount
		startObjIndex = 1
	else
		if attr.CurPos + ClientWidth > needWidth then
			attr.CurPos = needWidth - ClientWidth
		end
		
		startObjIndex = math.floor((attr.CurPos)/ItemWidth)
		startPos = startObjIndex*ItemWidth - attr.CurPos
		startObjIndex = startObjIndex + 1
		
		NeedToShowItemCount = math.ceil((math.abs(startPos)+ClientWidth)/ItemWidth)
		if startObjIndex + NeedToShowItemCount -1 > ItemCount then
			NeedToShowItemCount = ItemCount - startObjIndex+1
		end
	end
	
	local startPosBack = startPos
	
	
	local objCount = contentObj:GetChildCount()
	for i = 0, objCount-1 do
		local obj = contentObj:GetChildByIndex(i)
		obj:Reset()
	end
	if objCount < NeedToShowItemCount then
		self:CreateItemObj(NeedToShowItemCount - objCount)
	end
	
	for i = 1, NeedToShowItemCount do
		local obj = contentObj:GetChildByIndex(i-1)
		obj:SetObjPos2(startPos, attr.TopMargin, attr.ItemWidth, attr.ItemHeight)
		
		local Item = attr.FileItemList[i-1+startObjIndex]
		Item.Obj = obj
		obj:SetData(Item)
		startPos = startPos + ItemWidth
	end
	local contentBkg = self:GetControlObject("Client.Content.bkg")
	contentBkg:SetObjPos(startPosBack-5, 0, startPos+2 , "father.height")
end

function CreateItemObj(self, num)
	local attr = self:GetAttribute()
	local contentObj = self:GetControlObject("Client.Content")
	local objCount = contentObj:GetChildCount()
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	for i = 1, num do
		local obj = objFactory:CreateUIObject("item." .. i+objCount, "Kuaikan.ListViewCtrlItem")
		contentObj:AddChild(obj)
		
		
		obj:AttachListener("OnItemClick", true, function (itemObj)
			if attr.bEnable then
				local itemAttr = itemObj:GetAttribute()
				local selectedItem = self:GetSelectedItem()
				if selectedItem and selectedItem.ItemId == itemAttr.ItemInfo.ItemId then
					return
				end
				self:FireExtEvent("OnItemClick", itemAttr.ItemInfo.ItemId)
			end
		end)
	end
end


function ClearSelectStatus(self)
	local attr = self:GetAttribute()
	if #attr.SelectedList > 0 then
		for i=1, #attr.SelectedList do
			attr.SelectedList[i].Selected = false
			if attr.SelectedList[i].Obj then
				attr.SelectedList[i].Obj:UpdateSelectStatus()
			end
		end
		attr.SelectedList = {}
	end
end

function OnMouseWheel(self, x, y, distance, flags)
	local owner = self:GetOwnerControl()
	local ownerAttr = owner:GetAttribute()
	if ownerAttr.bEnable then
		if distance < 0 then
			ownerAttr.CurPos = ownerAttr.CurPos  + (ownerAttr.XInternal + ownerAttr.ItemWidth)
			if ownerAttr.CurPos < 0 then
				ownerAttr.CurPos = 0
			end
			owner:Update()
		else
			if ownerAttr.CurPos == 0 then
				return
			end
			ownerAttr.CurPos = ownerAttr.CurPos  - (ownerAttr.XInternal + ownerAttr.ItemWidth)
			if ownerAttr.CurPos < 0 then
				ownerAttr.CurPos = 0
			end
			owner:Update()
			
			
		end
	end
end

function LoadThumbnails(self, ItemId, width, height, bShowDefaultThumbnail)
	local attr = self:GetAttribute()
	ItemId = string.lower(ItemId)
	local Item = attr.Id2DataItemMap[ItemId]
	local filePath = ItemId
	if attr.ThumbnailsLoading[filePath] then	
		return
	end
	local lastWriteTime = Item.UserData.LastWriteTime
	
	local tipUtil = Helper.APIproxy.GetTipUtil()
	local tempDir = tipUtil:GetTempDir()
	local imgCachePath = tempDir .. "\\KuaiKan_{1E27A1DA-16F4-4436-AB6F-A83FCC6DD850}\\KKThumb\\"
	local extName = Helper.APIproxy.GetFileExt(filePath)
	extName = string.lower(extName)
	local extNameOrigi = extName
	if extName ~= ".png" or extName ~= ".ico" or extName ~= ".gif" then
		extName = ".jpg"
	end
	
	local md5Str =  tipUtil:GetMd5Str(attr.Id2DataItemMap[ItemId].UserData.FilePath .. lastWriteTime)
	local thumbnailsFilename = imgCachePath .. md5Str .. extName
	local osUtil = Helper.APIproxy.OSUtil
	if not osUtil:QueryFileExists(thumbnailsFilename) and bShowDefaultThumbnail then	
		Item.Obj:SetDefaultThumbnail(extNameOrigi)
	end
	if attr.ThumbnailsLoader then
		attr.ThumbnailsLoader:LoadThumbnails(filePath, thumbnailsFilename, width, height)
		attr.ThumbnailsLoading[filePath] = true
	end
end

function GetSelectedItem(self)
	local attr = self:GetAttribute()
	if #attr.SelectedList > 0 then
		return attr.SelectedList[1]
	end
	return nil
end

function SelectItem(self, itemId)
	if not itemId then 
		return
	end
	itemId = string.lower(itemId)
	local attr = self:GetAttribute()
	if not attr.Id2DataItemMap[itemId] then	
		return
	end
	
	local selectedItem = self:GetSelectedItem()
	if selectedItem and selectedItem.ItemId == itemId then
		return
	end
	
	self:ClearSelectStatus()
	attr.Id2DataItemMap[itemId].Selected = true
	if attr.Id2DataItemMap[itemId].Obj then
		attr.Id2DataItemMap[itemId].Obj:UpdateSelectStatus()
	end
	table.insert(attr.SelectedList, attr.Id2DataItemMap[itemId])
	self:Update()
end

function GetItemById(self, itemId)
	itemId = string.lower(itemId)
	local attr = self:GetAttribute()
	return attr.Id2DataItemMap[itemId]
end

function SetEnable(self, bEnable)
	local attr = self:GetAttribute()
	attr.bEnable = bEnable
end

function GetEnable(self)
	local attr = self:GetAttribute()
	return attr.bEnable
end

function GetItemCount(self)
	local attr = self:GetAttribute()
	return #attr.FileItemList
end

function PreFile(self)
	local attr = self:GetAttribute()
	local SelectedItemIndex = self:GetSelectedItemIndex()
	if not SelectedItemIndex then	
		return nil, nil
	end
	local index
	if SelectedItemIndex == 1 then
		index = #attr.FileItemList
	else
		index = SelectedItemIndex - 1
	end
	if attr.FileItemList[index] then
		self:SelectItem(attr.FileItemList[index].ItemId)
	end
end

function NextFile(self)
	
	local attr = self:GetAttribute()
	local SelectedItemIndex = self:GetSelectedItemIndex()
	if not SelectedItemIndex then	
		return nil, nil
	end
	LOG("KuaikanLog NextFile SelectedItemIndex:" .. SelectedItemIndex)
	local index
	if SelectedItemIndex == #attr.FileItemList then
		index = 1
	else
		index = SelectedItemIndex + 1
	end
	if attr.FileItemList[index] then
		self:SelectItem(attr.FileItemList[index].ItemId)
	end
end

function GetFileList(self)
	local attr = self:GetAttribute()
	local fileList = {}
	for i=1, #attr.FileItemList do
		table.insert(fileList, attr.FileItemList[i].UserData.FilePath)
	end
	return fileList
end

function GetPreFile(self)
	local attr = self:GetAttribute()
	local SelectedItemIndex = self:GetSelectedItemIndex()
	if not SelectedItemIndex then	
		return nil, nil
	end

	local index
	local bFirst = false
	if SelectedItemIndex == 1 then
		index = #attr.FileItemList
		bFirst = true
	else
		index = SelectedItemIndex - 1
	end
	return attr.FileItemList[index].UserData.FilePath, bFirst
end

function GetNextFile(self)
	local attr = self:GetAttribute()
	local SelectedItemIndex = self:GetSelectedItemIndex()
	if not SelectedItemIndex then	
		return nil, nil
	end
	
	local index
	local bLast = false
	if SelectedItemIndex == #attr.FileItemList then
		index = 1
		bLast = true
	else
		index = SelectedItemIndex + 1
	end
	LOG("KuaikanLog SelectedItemIndex=" .. SelectedItemIndex .. "  index=" .. index)
	return attr.FileItemList[index].UserData.FilePath, bLast
end

function GetFirstFile(self)
	local attr = self:GetAttribute()
	if attr.FileItemList[1] then
		return attr.FileItemList[1].UserData.FilePath
	end
end

function GetLastFile(self)
	local attr = self:GetAttribute()
	if #attr.FileItemList > 0 then
		return attr.FileItemList[#attr.FileItemList].UserData.FilePath
	end
end

function ShowContent(self, bShow)
	local attr = self:GetAttribute()
	local bkgObj = self:GetControlObject("Client.Bkg")
	SetObjVisible(bkgObj, bShow and true or false)
end

function OnPosChange(self)
	local attr = self:GetAttribute()
	if #attr.FileItemList < 1 then
		return
	end
	self:Update()
end

function GetSelectedItemIndex(self)
	local attr = self:GetAttribute()
	for i=1, #attr.FileItemList do
		if attr.FileItemList[i].Selected then
			return i
		end
	end	
	return nil
end