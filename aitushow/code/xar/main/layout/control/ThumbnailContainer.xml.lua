local Helper = XLGetGlobal("Helper")
local graphicUtil = XLGetObject("GRAPHIC.Util")
local seedCount = 123 --使用os.time()做种子并不靠谱
--说明：
--内存中始终只有三屏缩略图控件(Thumbnail Object)。
--当窗口被拉大、或缩略图被缩小时，会继续创建
--窗口被缩小、或缩略图被放大时，丢弃尾页部分控件，始终保持3屏控件
--通过向Thumbnail Object填不同的数据来实现显示不同的图片，从而避免重复的创建、销毁

local PageClass = {}
function PageClass:New()
	local pageObj = {}
	setmetatable(pageObj, self)
	self.__index = self
	return pageObj
end

function PageClass:Init(ctrlSelf)
	self.objList = {}
	self.selectedObjList = {}
	self.ctrlSelf = ctrlSelf
	self.containerObj = ctrlSelf:GetControlObject("Container")
	self.indexBegin = 0
	self.indexEnd = 0
end

--bSelect
function PageClass:OnSelectThumbnail(obj, bSelect)
	local id = obj:GetID()
	if self.selectedObj then 
		self.selectedObj:Select(false)
	end
	
	if not bSelect then
		-- self.selectedObjList[id] = nil
		self.selectedObj = nil
	else
		--暂时只支持单选
		-- for k,v in pairs(self.selectedObjList) do
			-- v:Select(false)
		-- end
		-- self.selectedObjList[id] = obj
		self.selectedObj = obj
	-- else
		-- XLMessageBox("bSelect: "..tostring(bSelect).." self.selectedObjList[id]: "..tostring(self.selectedObjList[id]))
	end
end

function PageClass:ShowThumbnailByRange(tPictures, indexBegin, indexEnd)
	if not indexBegin or not indexEnd then
		return
	end
	if 0 == indexBegin and 0 == indexEnd then
		return
	end
	LOG("ShowThumbnailByRange: indexBegin: ", indexBegin, " indexEnd: ", indexEnd)
	local function CreateObj(count)--加到尾部
		LOG("CreateObj, count: ", count, " curObjList count: ", #self.objList)
		for i=1, count do
			math.randomseed(os.time()+seedCount)
			seedCount = seedCount + 1
			local randomID = "thumbnail."..tostring(math.random(999999))
			
			LOG("randomID: ", randomID)
			local obj = Helper.objectFactory:CreateUIObject(randomID, "Thumbnail")
			self.containerObj:AddChild(obj)
			obj:AttachListener("OnSelect", false, function(_,_,bSelect) self:OnSelectThumbnail(obj, bSelect) end)
			
			table.insert(self.objList, obj)
		end
	end
	
	local function RemoveObj(count)--从尾部删
		LOG("RemoveObj, count: ", count, " curObjList count: ", #self.objList)
		for i=#self.objList, #self.objList-count+1, -1 do
			local obj = self.objList[i]
			if obj then
				self.containerObj:RemoveChild(obj)
			end
			table.remove(self.objList, i)
		end
	end
	self.indexBegin = indexBegin
	self.indexEnd = indexEnd
	local count = indexEnd - indexBegin + 1
	if count > #self.objList then
		--控件不够用,就继续生产
		CreateObj(count - #self.objList)
	elseif count < #self.objList then
		RemoveObj(#self.objList - count)
	end
	
	local requiredFiles = {}
	Helper:Assert(#self.objList == count, "#self.objList ~= count!!")
	for i=indexBegin, indexEnd do
		local obj = self.objList[i-indexBegin+1]
		obj:SetIndex(i)
		local bImageLoaded = obj:SetData(tPictures[i])
		if not bImageLoaded then
			table.insert(requiredFiles, tPictures[i].szPath)
		end
	end
	--返回需要请求的图片
	return requiredFiles
end

function PageClass:UpdateImgInfoByPath(tPictures, path, tImgInfo)
	for i=1, #self.objList do
		local data = self.objList[i]:GetData()
		if data and data.szPath and string.upper(data.szPath) == string.upper(path) then
			self.objList[i]:SetImage(tImgInfo)
			tPictures[i+self.indexBegin-1].xlhBitmap = tImgInfo.xlhBitmap
			tPictures[i+self.indexBegin-1].uWidth = tImgInfo.uWidth
			tPictures[i+self.indexBegin-1].uHeight = tImgInfo.uHeight
			tPictures[i+self.indexBegin-1].szType = tImgInfo.szType
			tPictures[i+self.indexBegin-1].fifType = tImgInfo.fifType
			return true
		end
	end
	return false
end

function PageClass:ClearPageData()
	--切换Page之前，先将本页的数据清理掉
	self.beginIndex = nil
	self.endIndex = nil
	for i=1, #self.objList do
		self.objList[i]:Clear()
		LOG("ClearPageData i: ", i)
	end
	LOG("ClearPageData out")
end

--管理三个page
local PageManager = {}
PageManager.bInit = false
function PageManager:New()
	local containerObj = {}
	setmetatable(containerObj, self)
	self.__index = self
	return containerObj
end

function PageManager:ClearAll()
	for i=1, 3 do
		local page = self.pageList[i]
		if page then
			page:ClearPageData()
		end
	end
end

local function MergeTable(tSrc, tDst)
	if type(tSrc) ~= "table" or type(tDst) ~= "table" then
		return
	end
	
	for i=1, #tSrc do
		table.insert(tDst, tSrc[i])
	end
	return tDst
end

function PageManager:Init(ctrlSelf, tPictures)
	if not self.bInit then--第一次初始化
		self.bInit = true
		self.pageList = {}
		self.ctrlSelf = ctrlSelf
		self.containerObj = ctrlSelf:GetControlObject("Container")
		for i=1, 3 do
			local page = PageClass:New()
			page:Init(ctrlSelf)
			table.insert(self.pageList, page)
		end
	end
	self:ClearAll()
	if 0 == #tPictures then
		for i=1, 3 do
			self.pageList[i]:ClearPageData()
		end
		LOG("PageManager Init return")
		return
	end
	self.tPictures = tPictures
	local lineCount, columnCount, pageCount, picWidth, picHeight = self.ctrlSelf:GetPageLayout()
	local containerHeight = self:CalaContainerNeedHeight()
	self.containerObj:SetObjPos2(0, 0, "father.width - 10", containerHeight)
	
	self:ResetScrollBar()
	
	local beginIndex = 1
	local endIndex = 1
	
	local requiredFiles = {}
	for i=1, 3 do
		if beginIndex > #tPictures then
			return
		end
		endIndex = pageCount * i
		if #tPictures < endIndex then
			endIndex = #tPictures
		end
		local tmpFiles = self.pageList[i]:ShowThumbnailByRange(tPictures, beginIndex, endIndex)
		MergeTable(tmpFiles, requiredFiles)
		beginIndex = endIndex + 1
	end
	if "table" == type(requiredFiles) and #requiredFiles > 0 then
		graphicUtil:GetMultiImgInfoByPaths(requiredFiles)
	end
end

function PageManager:CalaContainerNeedHeight()
	local lineCount, columnCount, pageCount, picWidth, picHeight = self.ctrlSelf:GetPageLayout()
	local ctrlSelfAttr = self.ctrlSelf:GetAttribute()
	local containerHeight = math.ceil(#self.tPictures/columnCount) * (picHeight + ctrlSelfAttr.SpaceV) - ctrlSelfAttr.SpaceV
	
	LOG("containerHeight: ", containerHeight)
	return containerHeight
end

function PageManager:ResetScrollBar()
	local _, containerT, _, containerB = self.containerObj:GetObjPos()
	local containerHieght = containerB - containerT
	LOG("ResetScrollBar, containerHieght: ", containerHieght)
	
	local scrollBar = self.ctrlSelf:GetControlObject("Container.ScrollBar")
	local fatherObj = self.ctrlSelf:GetControlObject("Background")
	local _, fatherT, _, fatherB = fatherObj:GetObjPos()
	local fatherHeight = fatherB - fatherT
	if fatherHeight < containerHieght then
		LOG("ResetScrollBar, SetScrollRange, 0, ", containerHieght - fatherHeight)
		scrollBar:SetScrollRange(0, containerHieght - fatherHeight, true)
		scrollBar:SetPageSize(fatherHeight)
		scrollBar:SetVisible(true)
		scrollBar:SetChildrenVisible(true)
		scrollBar:Show(true)
	else
		scrollBar:SetVisible(false)
		scrollBar:SetChildrenVisible(false)
	end
end

--这
function PageManager:ClearXLHBitmapBut(indexBegin, indexEnd)
	local curUseMemInKB = collectgarbage("count")
	LOG("current use memory count: ", curUseMemInKB, " KB")
	local _begin = 1
	local _end = indexBegin
	if _end ~= _begin then
		for i=_begin, _end do
			self.tPictures[i].xlhBitmap = nil
			self.tPictures[i].uWidth = nil
			self.tPictures[i].uHeight = nil
			self.tPictures[i].fifType = nil
			self.tPictures[i].szType = nil
		end
	end
	_begin = indexEnd + 1
	_end = #self.tPictures
	if _end ~= _begin then
		for i=_begin, _end do
			self.tPictures[i].xlhBitmap = nil
			self.tPictures[i].uWidth = nil
			self.tPictures[i].uHeight = nil
			self.tPictures[i].fifType = nil
			self.tPictures[i].szType = nil
		end
	end
	collectgarbage("collect")
	local newUseMemInKB = collectgarbage("count")
	LOG("current use memory count: ", curUseMemInKB, " KB, free ", curUseMemInKB - newUseMemInKB, " KB total")
end

function PageManager:ShowPagesByScrollPos(scrollPos)
	LOG("ShowPagesByScrollPos: ", scrollPos)
	local rangeBegin, rangeEnd = self:GetCurShowIndexRange()
	local lineCount, columnCount, pageCount, picWidth, picHeight = self.ctrlSelf:GetPageLayout()
	--计算当前scrollPos第一行图片是第几行
	local ctrlSelfAttr = self.ctrlSelf:GetAttribute()
	local lineNum = math.ceil(scrollPos/(picHeight + ctrlSelfAttr.SpaceV))
	if lineNum < 1 then lineNum = 1 end
	local indexBegin = (lineNum-1)*columnCount + 1
	local indexEnd   = 0
	LOG("ShowPagesByScrollPos: indexBegin: ", indexBegin)
	local requiredFiles = {}
	for i=1, 3 do
		indexEnd = indexBegin + pageCount
		if indexEnd > #self.tPictures then
			indexEnd = #self.tPictures
		end
		local tmpFiles = self.pageList[i]:ShowThumbnailByRange(self.tPictures, indexBegin, indexEnd)
		requiredFiles = MergeTable(tmpFiles, requiredFiles)
		
		indexBegin = indexEnd + 1
		if indexBegin > #self.tPictures then
			break
		end
	end
	if "table" == type(requiredFiles) and #requiredFiles > 0 then
		LOG("GetMultiImgInfoByPaths indexBegin: ", requiredFiles[1], " indexEnd: ", requiredFiles[#requiredFiles])
		graphicUtil:GetMultiImgInfoByPaths(requiredFiles)
	end
	local containerL, containerT, containerR, containerB = self.containerObj:GetObjPos()
	local containerHeight = containerB-containerT
	self.containerObj:SetObjPos2(containerL, -scrollPos, "father.width-10", containerHeight)
	LOG("ShowPagesByScrollPos: indexEnd: ", indexEnd)
	
	--[indexBegin, indexEnd]除此之外的,xlhBitmap句柄通通置空、以便lua解释器回收内存
	self:ClearXLHBitmapBut(indexBegin, indexEnd)
end

--请求的缩略图句柄，在这里异步返回
function PageManager:OnGetMultiImgInfoCallBack(key, tImgInfo) 
	--干两件事：更新到界面、保存到tPictures。
	LOG("OnGetMultiImgInfoCallBack key: ", key, " tImgInfo.szPath: ", tImgInfo.szPath)
	for i=1, 3 do
		if self.pageList[i]:UpdateImgInfoByPath(self.tPictures, tImgInfo.szPath, tImgInfo) then
			LOG("OnGetMultiImgInfoCallBack found in page: ", i, "!! path: ", tImgInfo.szPath)
			break
		else
			LOG("OnGetMultiImgInfoCallBack not foundin page: ", i, "! path: ", tImgInfo.szPath)
		end
	end
	
	--如果curPage,forwordPage,backwordPage三页里都没找到，说明当前返回的图片不在显示区域
	--直接丢弃掉，下次显示时重新申请
	LOG("OnGetMultiImgInfoCallBack can not find in 3 pages, tImgInfo.szPath: ", tImgInfo and tImgInfo.szPath)
	tImgInfo = nil
	
	-- for i=1, #self.tPictures do
		-- if self.tPictures[i].szPath == tImgInfo.szPath then
			-- self.tPictures[i].xlhBitmap = tImgInfo.xlhBitmap
			-- return
		-- end
	-- end
end

function PageManager:GetCurShowPageIndex()
	LOG("GetCurShowPageIndex")
	local rangeBegin, rangeEnd = self:GetCurShowIndexRange()
	local topPageIndex = 0
	for i=1,3 do
		if self.pageList[i].indexBegin == rangeBegin then
			topPageIndex = i
		end
	end
	
	local bottomPageIndex = 0
	for i=1,3 do
		if self.pageList[i].indexEnd == rangeEnd then
			bottomPageIndex = i
		end
	end
	
	local middlePageIndex = 0
	for i=1,3 do
		if self.pageList[i].indexBegin > rangeBegin and self.pageList[i].indexEnd < rangeEnd then
			middlePageIndex = i
		end
	end
	
	--只有当tPictures数量大于3屏的时候，才有必要调该方法，此时三个Page一定都显示了图片
	Helper:Assert(topPageIndex > 0 and middlePageIndex > 0 and bottomPageIndex > 0, "error page index")
	LOG("GetCurShowPageIndex, topPageIndex: ", topPageIndex, " middlePageIndex: ", middlePageIndex, " bottomPageIndex: ", bottomPageIndex)
	return topPageIndex, middlePageIndex, bottomPageIndex
end

function PageManager:GetCurShowIndexRange()
	--page1,2,3的显示顺序不一定，有可能3在最上面，1在中间。。。
	--最小的非0index就是rangeBegin
	local rangeBegin = self.pageList[1].indexBegin
	if rangeBegin > self.pageList[2].indexBegin and 0 ~= self.pageList[2].indexBegin then
		rangeBegin = self.pageList[2].indexBegin
	end
	if rangeBegin > self.pageList[3].indexBegin and 0 ~= self.pageList[3].indexBegin then
		rangeBegin = self.pageList[3].indexBegin
	end
	
	--最大的index就是rangeEnd
	local rangeEnd = self.pageList[1].indexEnd
	if rangeEnd < self.pageList[2].indexEnd then
		rangeEnd = self.pageList[2].indexEnd
	end
	if rangeEnd < self.pageList[3].indexEnd then
		rangeEnd = self.pageList[3].indexEnd
	end
	LOG("GetCurShowIndexRange , rangeBegin: ", rangeBegin, " rangeEnd: ", rangeEnd)
	return rangeBegin, rangeEnd
end

--暂时只支持单选
function PageManager:GetCurSelectedThumbnails()
	if not self.bInit then
		return
	end
	for i=1,3 do
		if self.pageList[i].selectedObj then
			return self.pageList[i].selectedObj
		end
	end
end

function PageManager:DeleteThumbnailByPath(path)
	for i=1, #self.tPictures do 
		if string.upper(self.tPictures[i].szPath) == string.upper(path) then
			table.remove(self.tPictures, i)
			local scrollBar = self.ctrlSelf:GetControlObject("Container.ScrollBar")
			local scrollPos = scrollBar:GetScrollPos()
			self:ShowPagesByScrollPos(scrollPos)
			return
		end
	end
end

function PageManager:OnCtrlPosChange()
	LOG("OnCtrlPosChange")
	if not self.bInit then 
		return
	end
	local containerHeight = self:CalaContainerNeedHeight()
	local containerL, containerT, containerR, containerB = self.containerObj:GetObjPos() 
	self.containerObj:SetObjPos2(containerL, containerT, "father.width - 10", containerHeight)
	self:ResetScrollBar()
	local scrollBar = self.ctrlSelf:GetControlObject("Container.ScrollBar")
	local scrollPos = scrollBar:GetScrollPos()
	self:ShowPagesByScrollPos(scrollPos)
end

--换页，响应滚动条滚动：向上滚动，则将backwordPage移到最上面去
--distance大于0向下滚动；小于0向上滚动
function PageManager:MovePages(scrollPos)
	--不大于三屏的，不用考虑换页
	local _,_, thumbnailCount = self.ctrlSelf:GetPageLayout()
	local containerL, containerT, containerR, containerB = self.containerObj:GetObjPos()
	local containerHeight = containerB-containerT
	if thumbnailCount*3 >= #self.tPictures then
		self.containerObj:SetObjPos2(containerL, -scrollPos, "father.width-10", containerHeight)
		return
	end
	
	Helper:Assert(containerT <= 0, "containerObj top pos must Less than or equal to 0！ containerT: "..tostring(containerT))
	
	local topPageIndex, middlePageIndex, bottomPageIndex = self:GetCurShowPageIndex()
	local topPage    = self.pageList[topPageIndex]
	local middlePage = self.pageList[middlePageIndex]
	local bottomPage = self.pageList[bottomPageIndex]
	local _, _, _, _, picHeight = self.ctrlSelf:GetPageLayout()
	local ctrlSelfAttr = self.ctrlSelf:GetAttribute()
	local lineHeight = ctrlSelfAttr.SpaceV + picHeight
	if -containerT < scrollPos then --画面向上滑动(滚动条向下滑动)
		local lastMiddlePageObj = middlePage.objList[#middlePage.objList]
		local _, _, _, lastMiddlePageObjB = lastMiddlePageObj:GetObjPos()
		if lastMiddlePageObjB + containerT <= 15 and bottomPage.indexEnd < #self.tPictures then
			--向上滑动画面时，只有当中间page快要完全滑到窗口上方时，才进行换页(将topPage挪到底部)
			local indexBegin = bottomPage.indexEnd + 1
			local indexEnd   = indexBegin + #middlePage.objList - 1 --这里一定要用中间page的孩子计数，上、下Page都有可能不全
			if indexEnd > #self.tPictures then
				indexEnd = #self.tPictures
			end
			--所谓的‘换页’，实际上就是重新设定page的显示的tPictures中的index的范围
			local requiredFiles = topPage:ShowThumbnailByRange(self.tPictures, indexBegin, indexEnd)
			if "table" == type(requiredFiles) and #requiredFiles > 0 then
				graphicUtil:GetMultiImgInfoByPaths(requiredFiles)
			end
		end
		
		self.containerObj:SetObjPos2(containerL, -scrollPos, "father.width-10", containerHeight)
	elseif -containerT > scrollPos then--画面向下滑动(滚动条向上滑动)
		local lastTopPageObj = topPage.objList[#topPage.objList]
		local _, _, _, lastTopPageObjB = lastTopPageObj:GetObjPos()
		if  (lastTopPageObjB + containerT) >= -15 and topPage.indexBegin > 1 then
			--当中间page即将完全滑出窗口下方时，进行换页
			local indexBegin = topPage.indexBegin - #middlePage.objList
			if indexBegin < 1 then
				indexBegin = 1
			end
			local indexEnd = topPage.indexBegin - 1
			local requiredFiles = bottomPage:ShowThumbnailByRange(self.tPictures, indexBegin, indexEnd)
			if "table" == type(requiredFiles) and #requiredFiles > 0 then
				graphicUtil:GetMultiImgInfoByPaths(requiredFiles)
			end
		end		
		self.containerObj:SetObjPos2(containerL, -scrollPos, "father.width-10", containerHeight)
	end
end

--计算每行应显示多少列、共能显示多少行
function GetPageLayout(self)
	local ownerTree = self:GetOwner()
	local centerObj = ownerTree:GetUIObject("ThumbnailContainer.Center")
	local attr = self:GetAttribute()
	local L, T, R, B = centerObj:GetObjPos()
	local width = R - L
	local hieght = B - T
	
	local zoomPercent = attr.curZoomPercent or attr.defaultZoomPercent or 10
	zoomPercent = zoomPercent / 100
	local picWidth = math.round(attr.MinWidth + (attr.MaxWidth - attr.MinWidth)*zoomPercent)
	local picHeight = math.round(attr.MinHeight + (attr.MaxHeight - attr.MinHeight)*zoomPercent)
	
	local columnCount = math.floor(width/(picWidth + attr.SpaceH))
	local lineCount = math.ceil(hieght/(picHeight + attr.SpaceV))
	
	return lineCount, columnCount, lineCount * columnCount, picWidth, picHeight
end



--tPictures格式:{
-- {"szPath"=, "szExt"=, "utcLastWriteTime"=, "uFileSize"=, "uWidth"=, "uHeight"=, "szType"=, "xlhBitmap"=},
-- {"szPath"=, "szExt"=, "utcLastWriteTime"=, "uFileSize"=, "uWidth"=, "uHeight"=, "szType"=, "xlhBitmap"=},
-- ...
-- }前四个属性在GetDirSupportImgPaths时就能同步获取到；后四个属性要通过GetMultiImgInfoByPaths异步获取，
--OnGetMultiImgInfoCallBack事件里返回
function SetFolder(self, sPath)
	if sPath == "C:" then
		sPath = "C:\\"
	end
	LOG("SetFolder sPath: ", sPath)
	
	local attr = self:GetAttribute()
	attr.sPath = sPath
	
	--将上一个目录产生的xlhBitmap销毁
	attr.tPictures = nil
	
	--获取目录中受支持的图片
	attr.tPictures = graphicUtil:GetDirSupportImgPaths(sPath)
	attr.pageManager:Init(self, attr.tPictures)
	
	LOG("SetFolder out")
	--之后就是监听缩放、滚动，来调整Page
end

function Zoom(self, percent)
	local attr = self:GetAttribute()
	attr.curZoomPercent = percent
	attr.pageManager:Init(self, attr.tPictures)
end

function GetZoomPercent(self)

end

--在SetFolder之前调用，可即时生效。在之后调用，需手动调一次Refresh
function SetDefaultZoomPercent(self, percent)
	local attr = self:GetAttribute()
	attr.defaultZoomPercent = percent
end

function GetSelectedThumbnailCtrlID(self)

end

function Refresh(self)

end

function OnVScroll(self, fun, _type, pos)
	local ownerCtrl = self:GetOwnerControl()
	local scrollPos = self:GetScrollPos()	
	--点击向上按钮或上方空白
    if _type ==1 then
        self:SetScrollPos( scrollPos - 44, true )
	--点击向下按钮或下方空白
    elseif _type==2 then
		self:SetScrollPos( scrollPos + 44, true )
    end

	scrollPos = self:GetScrollPos()
	
	--大于0向下滚动；小于0向上滚动
	local ownerCtrlAttr = ownerCtrl:GetAttribute()
	LOG("Move Pages: scrollPos: ", scrollPos)
	local pageManager = ownerCtrlAttr.pageManager
	local rangeBegin, rangeEnd = pageManager:GetCurShowIndexRange()
	local lineCount, columnCount, pageCount, picWidth, picHeight = pageManager.ctrlSelf:GetPageLayout()
	local pageHeight = lineCount * (picHeight + ownerCtrlAttr.SpaceV)
	local firstLineNum = math.ceil(rangeBegin/columnCount)
	local lastLineNum = math.ceil(rangeEnd/columnCount)
	local posT = firstLineNum * (picHeight + ownerCtrlAttr.SpaceV)
	local posB = lastLineNum * (picHeight + ownerCtrlAttr.SpaceV)
	if scrollPos < posT - pageHeight or  scrollPos > posB + pageHeight then
		--如果滚动的太快(距离太大)，就没必要换页了
		ownerCtrlAttr.pageManager:ShowPagesByScrollPos(scrollPos)
	else
		ownerCtrlAttr.pageManager:MovePages(scrollPos)
	end
	
	return true
end

function OnScrollBarMouseWheel(self, name, x, y, distance)
	local ThumbPos = self:GetThumbPos()
    self:SetThumbPos(ThumbPos - distance/10)
end

function OnBkgMouseWheel(self, x, y, distance)
	local ctrl = self:GetOwnerControl()
	local scrollBar = ctrl:GetControlObject("Container.ScrollBar")
	local ThumbPos = scrollBar:GetThumbPos()
    scrollBar:SetThumbPos(ThumbPos - distance/10)
end

function OnInitControl(self)
	local attr = self:GetAttribute()
	attr.defaultZoomPercent = 10
	attr.pageManager = PageManager:New()
	local scrollBar = self:GetControlObject("Container.ScrollBar")
	
	scrollBar:SetVisible(false)
	scrollBar:SetChildrenVisible(false)
	graphicUtil:AttachListener(function(key, tImgInfo) attr.pageManager:OnGetMultiImgInfoCallBack(key, tImgInfo) end)
end

function OnPosChange(self, oldLeft, oldTop, oldRight, oldBottom, newLeft, newTop, newRight, newBottom)
	local attr = self:GetAttribute()
	attr.pageManager:OnCtrlPosChange(oldLeft, oldTop, oldRight, oldBottom, newLeft, newTop, newRight, newBottom)
end