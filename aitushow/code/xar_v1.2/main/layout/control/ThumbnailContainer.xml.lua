local seedCount = 123 --使用os.time()做种子并不靠谱

--计算每行应显示多少列、可视区域共能显示多少行
function GetPageLayout(self)
	local attr = self:GetAttribute()
	local width,hieght = Helper:GetObjWH(self:GetControlObject("Background"))
	
	local zoomPercent = attr.curZoomPercent or attr.defaultZoomPercent or 10
	zoomPercent = zoomPercent / 100
	local picWidth = math.round(attr.MinWidth + (attr.MaxWidth - attr.MinWidth)*zoomPercent)
	local picHeight = math.round(attr.MinHeight + (attr.MaxHeight - attr.MinHeight)*zoomPercent)
	
	local columnCount = math.floor(width/(picWidth + attr.SpaceH))
	local lineCount = math.ceil(hieght/(picHeight + attr.SpaceV))
	
	return lineCount, columnCount, lineCount * columnCount, picWidth, picHeight
end

function Zoom(self, percent)
	percent = math.max(1, percent)
	percent = math.min(100, percent)
	local attr = self:GetAttribute()
	attr.curZoomPercent = percent
	
	local scrollPos = self:GetControlObject("Container.ScrollBar"):GetScrollPos()
	if attr.pageManager and attr.pageManager.bInit then
		attr.pageManager:Init(self, ImagePool.tPictures, scrollPos)
	end
end

--===========================行管理类，不足时创建，多余时销毁=========================
local LineClass = {}
function LineClass:New()
	local lineObj = {}
	setmetatable(lineObj, self)
	self.__index = self
	
	return lineObj
end

function LineClass:Init(ctrlSelf)
	self.objList = {}
	self.indexMapObj = {} --这个表的下标不一定从1开始
	self.selectedObjList = {}
	self.ctrlSelf = ctrlSelf
	self.containerObj = ctrlSelf:GetControlObject("Container")
	self.indexBegin = 0
	self.indexEnd = 0
end

function LineClass:ClearLineData()
	--切换Line之前，应先将本行的数据清理掉
	self.beginIndex = nil
	self.endIndex = nil
	for i=1, #self.objList do
		self.objList[i]:Clear()
	end
end

function LineClass:RemoveLineObj()
	self.beginIndex = nil
	self.endIndex = nil
	for i=#self.objList, 1, -1 do
		self.containerObj:RemoveChild(self.objList[i])
		table.remove(self.objList, i)
	end
end

function LineClass:ShowThumbnailByRange(tPictures, indexBegin, indexEnd)
	if not indexBegin or not indexEnd then
		return
	end
	if 0 == indexBegin and 0 == indexEnd then
		return
	end
	-- LOG("ShowThumbnailByRange: indexBegin: ", indexBegin, " indexEnd: ", indexEnd)
	local function CreateObj(count)--加到尾部
		LOG("CreateObj, count: ", count, " curObjList count: ", #self.objList)
		for i=1, count do
			math.randomseed(os.time()+seedCount)
			seedCount = seedCount + 1
			local randomID = "thumbnail."..tostring(math.random(999999))
			local obj = Helper.objectFactory:CreateUIObject(randomID, "Thumbnail")
			self.containerObj:AddChild(obj)
			--先不处理选中
			obj:AttachListener("OnSelect", false, function(_,_,bSelect) OnSelectThumbnail(self.ctrlSelf, obj, bSelect) end)
			
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
	
	local requiredFilesIndex = {}
	Helper:Assert(#self.objList == count, "#self.objList ~= count!!")
	self.indexMapObj = {} --清空上次的映射
	for i=indexBegin, indexEnd do
		local obj = self.objList[i-indexBegin+1]
		obj:SetIndex(i)
		self.indexMapObj[i] = obj --做个映射，缩略图更新的时候用
		local bImageLoaded = obj:SetData(tPictures[i])
		if not bImageLoaded then
			table.insert(requiredFilesIndex, i)
		end
	end
	--返回需要请求的图片
	return requiredFilesIndex
end


--===========可视区域(即为Page)管理类，负责在滚动、缩放时调度LineClass Object==========
local PageManager = {}
function PageManager:New()
	local containerObj = {}
	setmetatable(containerObj, self)
	self.__index = self
	return containerObj
end

function PageManager:ClearAllData()
	for i=1, #self.lineList do
		self.lineList[i]:ClearLineData()
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

--为防止连续滚动时，请求过多的不在显示区域缩略图，
--使用任务栈做倒序。实际上，该栈的容量为1
function PageManager:PushTask(requiredFiles)
	if "table" ~= type(requiredFiles) then return end
	self.taskStack = requiredFiles
	if self.taskStackCookie then--后进来的任务，会冲掉上一个任务
		KillTimer(self.taskStackCookie)
		self.taskStackCookie = nil
	end
	self.taskStackCookie = SetOnceTimer(function()
		self.taskStackCookie = nil
		ImagePool:QueryThumbByIndexTable(self.taskStack)
	end, 500)
end

function PageManager:Init(ctrlSelf, tPictures, scrollPos)
	if not self.bInit then--以下属性只需初始化一次
		self.bInit = true
		self.lineList = {}
		self.ctrlSelf = ctrlSelf
		self.containerObj = ctrlSelf:GetControlObject("Container")
	end
	self:ClearAllData()
	
	self.tPictures = tPictures
	local lineCount, columnCount, pageCount, picWidth, picHeight = self.ctrlSelf:GetPageLayout()
	local containerHeight = self:CalaContainerNeedHeight()
	self.containerObj:SetObjPos2(0, 0, "father.width - 10", containerHeight)
	
	self:ResetScrollBar()
	
	--创建N+2行Thumbnail对象，多出的两行用于对付滚动
	if lineCount+2 > #self.lineList then
		for i=1, lineCount+2 - #self.lineList do
			local line = LineClass:New()
			line:Init(self.ctrlSelf)
			table.insert(self.lineList, line)
		end
	elseif lineCount+2 < #self.lineList then--多余的行删掉
		for i=#self.lineList, lineCount+3, -1 do
			self.lineList[i]:RemoveLineObj()
			table.remove(self.lineList, i)
		end
	end
	
	local scrollBar = self.ctrlSelf:GetControlObject("Container.ScrollBar")
		
	if scrollPos then
		scrollPos = math.max(0, scrollPos)
		local RangeBegin, RangeEnd = scrollBar:GetScrollRange()
		scrollPos = math.min(RangeEnd, scrollPos)
	else
		scrollPos = 0
		scrollBar:SetScrollPos(0, true)
	end
	self:ShowThumbnailByScrollPos(scrollPos)
end

function PageManager:CalaContainerNeedHeight()
	local lineCount, columnCount, pageCount, picWidth, picHeight = self.ctrlSelf:GetPageLayout()
	local ctrlSelfAttr = self.ctrlSelf:GetAttribute()
	local containerHeight = math.ceil(#self.tPictures/columnCount) * (picHeight + ctrlSelfAttr.SpaceV) - ctrlSelfAttr.SpaceV
	
	LOG("containerHeight: ", containerHeight)
	return containerHeight
end

function PageManager:ResetScrollBar()
	local _, containerHieght = Helper:GetObjWH(self.containerObj)
	LOG("ResetScrollBar, containerHieght: ", containerHieght)
	
	local scrollBar = self.ctrlSelf:GetControlObject("Container.ScrollBar")
	local _, fatherHeight = Helper:GetObjWH(self.ctrlSelf:GetControlObject("Background"))
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

--以下是处理滚动的两个主要的方法
function PageManager:ShowThumbnailByScrollPos(scrollPos)
	LOG("ShowThumbnailByScrollPos: ", scrollPos)
	local lineCount, columnCount, pageCount, picWidth, picHeight = self.ctrlSelf:GetPageLayout()
	local _, containerHeight = Helper:GetObjWH(self.containerObj)
	--计算当前scrollPos第一行图片是第几行
	local ctrlSelfAttr = self.ctrlSelf:GetAttribute()
	local lineNum = math.ceil(scrollPos/(picHeight + ctrlSelfAttr.SpaceV))
	lineNum = math.max(lineNum, 1)
	local indexBegin = (lineNum-1)*columnCount + 1
	
	local indexEnd = 1
	local requiredFiles = {}
	for i=1, #self.lineList do
		if indexBegin > #self.tPictures then
			break
		end
		indexEnd = math.min(indexBegin + columnCount - 1, #self.tPictures)
		local tmpFiles = self.lineList[i]:ShowThumbnailByRange(self.tPictures, indexBegin, indexEnd)
		MergeTable(tmpFiles, requiredFiles)
		indexBegin = indexEnd + 1
	end
	self.containerObj:SetObjPos2(0, -scrollPos, "father.width-10", containerHeight)
	self:PushTask(requiredFiles)
end

function PageManager:SwapLineByScrollPos(scrollPos)
	local lineCount, columnCount, pageCount, picWidth, picHeight = self.ctrlSelf:GetPageLayout()
	local _, bkgH = Helper:GetObjWH(self.ctrlSelf:GetControlObject("Background"))
	local containerL, containerT, _, containerB = self.containerObj:GetObjPos()
	local containerHeight = containerB-containerT
	local requiredFiles = nil
	if -containerT < scrollPos then --画面向上滑动(滚动条向下滑动)
		local indexBegin = self.lineList[#self.lineList].indexEnd + 1
		if indexBegin > #self.tPictures then 
			self.containerObj:SetObjPos2(0, -scrollPos, "father.width-10", containerHeight)
			return
		end --最后一行了
		local firstLineObj = self.lineList[1].objList[1]
		local _, _, _, objB = firstLineObj:GetObjPos()
		
		if containerT + objB < 0 then--第一行缩略图的底部已经显示不出来了,就把它插到最后一行
			local tmpLine = self.lineList[1]
			table.remove(self.lineList, 1)
			local indexEnd = math.min(indexBegin+columnCount-1, #self.tPictures)
			requiredFiles = tmpLine:ShowThumbnailByRange(self.tPictures, indexBegin, indexEnd)
			table.insert(self.lineList, tmpLine)
		end
	elseif -containerT > scrollPos then--画面向下滑动(滚动条向上滑动)
		local indexBegin = self.lineList[1].indexBegin - columnCount
		if indexBegin < 1 then 
			self.containerObj:SetObjPos2(0, -scrollPos, "father.width-10", containerHeight)
			return 
		end --第一行了
		local lastLineObj = self.lineList[#self.lineList].objList[1]
		if not lastLineObj then
			LOG("lastLineObj is nil !!!!!!!!!!!!!!!!!!!")
		end
		local _,objT = lastLineObj:GetObjPos()
		
		if containerT + objT > bkgH then --最后一行的顶部已经显示不出来了
			local tmpLine = self.lineList[#self.lineList]
			table.remove(self.lineList, #self.lineList)
			local indexEnd = math.min(indexBegin+columnCount-1, #self.tPictures)
			requiredFiles = tmpLine:ShowThumbnailByRange(self.tPictures, indexBegin, indexEnd)
			table.insert(self.lineList, 1, tmpLine)
		end
	end
	
	self.containerObj:SetObjPos2(0, -scrollPos, "father.width-10", containerHeight)
	--单行滚动时说明滚动的较慢，不使用任务栈
	ImagePool:QueryThumbByIndexTable(requiredFiles)
end


-- ======================以下是控件中响应事件的方法=======================
function OnVScroll(self, fun, _type, pos)
	local ownerCtrl = self:GetOwnerControl()
	local scrollPos = self:GetScrollPos()	
	
    if _type ==1 then--点击向上按钮或上方空白
        self:SetScrollPos( scrollPos - 44, true )
	elseif _type==2 then--点击向下按钮或下方空白
		self:SetScrollPos( scrollPos + 44, true )
    end

	scrollPos = self:GetScrollPos()
	local ownerCtrlAttr = ownerCtrl:GetAttribute()
	local pageManager = ownerCtrlAttr.pageManager
	local L, T, R, B = pageManager.containerObj:GetObjPos()
	
	local lineCount, columnCount, pageCount, picWidth, picHeight = pageManager.ctrlSelf:GetPageLayout()
	-- if #pageManager.tPictures <= pageCount then return end --无需滚动
	
	local scrollDistance = math.abs(scrollPos + T)
	if scrollDistance >= picHeight then
		--如果滚动的太快(距离太大)，就没必要换页了
		ownerCtrlAttr.pageManager:ShowThumbnailByScrollPos(scrollPos)
	else
		ownerCtrlAttr.pageManager:SwapLineByScrollPos(scrollPos)
	end
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

function OnSelectThumbnail(self, obj, bSelect)
	local attr = self:GetAttribute()
	local pageManager = attr.pageManager
	local id = obj:GetID()
	if pageManager.selectedObj then 
		pageManager.selectedObj:Select(false)
	end
	
	if not bSelect then
		pageManager.selectedObj = nil
	else
		pageManager.selectedObj = obj
	end
end

function OnImagePoolSetPath(self, sPath)
	local attr = self:GetAttribute()
	attr.pageManager = attr.pageManager or PageManager:New()
	attr.pageManager:Init(self, ImagePool.tPictures)
end

function OnImagePoolPicUpdate(self, info, index)
	--这里面进来的会比较频繁，所以尽量少打日志、优化处理时间
	local attr = self:GetAttribute()
	for i=1, #attr.pageManager.lineList do
		if attr.pageManager.lineList[i].indexMapObj[index] then
			attr.pageManager.lineList[i].indexMapObj[index]:SetImage(info)
			return
		end
	end
end

function OnImagePoolSortFinished(self)
	local attr = self:GetAttribute()
	if not attr.pageManager or not attr.pageManager.bInit then
		return
	end
	
	local scrollPos = self:GetControlObject("Container.ScrollBar"):GetScrollPos()
	attr.pageManager:ShowThumbnailByScrollPos(scrollPos)
end

function OnPosChange(self, oldLeft, oldTop, oldRight, oldBottom, newLeft, newTop, newRight, newBottom)
	local attr = self:GetAttribute()
	if not attr.pageManager or not attr.pageManager.bInit then
		return
	end
	local scrollPos = self:GetControlObject("Container.ScrollBar"):GetScrollPos()
	attr.pageManager:Init(self, ImagePool.tPictures, scrollPos)
end

--若发生变化的index位于当前显示区域之后，是不用更新界面的
local function UpdateUIByIndex(self, index)
	local attr = self:GetAttribute()
	if not attr.pageManager or not attr.pageManager.bInit then
		return
	end
	local indexEnd = attr.pageManager.lineList[#attr.pageManager.lineList].indexEnd
	LOG("UpdateUIByIndex: index: ", " indexEnd: ", indexEnd)
	
	local containerHeight = attr.pageManager:CalaContainerNeedHeight()
	attr.pageManager.containerObj:SetObjPos2(0, 0, "father.width - 10", containerHeight)
	attr.pageManager:ResetScrollBar()
	local scrollPos = self:GetControlObject("Container.ScrollBar"):GetScrollPos()
	-- if index < indexEnd then
		-- attr.pageManager:ShowThumbnailByScrollPos(scrollPos)
		attr.pageManager:Init(self, ImagePool.tPictures, scrollPos)
	-- end
end

function OnInitControl(self)
	ImagePool:AddListener("OnSetPath", function(_,_, sPath) OnImagePoolSetPath(self, sPath) end)
	ImagePool:AddListener("OnPicUpdate", function(_,_, info, index) OnImagePoolPicUpdate(self, info, index) end)
	ImagePool:AddListener("OnSortFinished", function(_,_, sortKey) OnImagePoolSortFinished(self) end)
	
	--以下三个事件触发的概率较小，处理起来可以不用太抠门
	--UpdateUIByChangedIndex会对整个页面进行重新显示
	ImagePool:AddListener("OnAddFile", function(_,_, index, endIndex)  UpdateUIByIndex(self, index) end)
	ImagePool:AddListener("OnDeleteFile", function(_,_, info, index) 	
		LOG("OnDeleteFile index: ", index)
		UpdateUIByIndex(self, index) 
	end)
	ImagePool:AddListener("OnRenameFile", function(_,_, info, index, sOldPath, sNewPath) UpdateUIByIndex(self, index) end)
end