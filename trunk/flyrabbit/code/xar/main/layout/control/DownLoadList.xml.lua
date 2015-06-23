local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")
local tFunHelper = XLGetGlobal("Project.FunctionHelper")
local tipUtil = tFunHelper.tipUtil


--方法  
function UpdateFileList(objRootCtrl)
	RemoveFileList(objRootCtrl)
	
	local tFileList = tRabbitFileList:GetFileList()
	if type(tFileList) ~= "table" then
		return
	end
	
	for nIndex, tFileItemContent in ipairs(tFileList) do
		local objFileItem = CreateFileItem(objRootCtrl, tFileItemContent, nIndex)
	end	
	
	UpdateScrollBar(objRootCtrl, #tFileList)
end


function GetSelectItemIndex(objRootCtrl)
	local attr = objRootCtrl:GetAttribute()
	return attr.SelectItemIndex
end


function GetSelectItemObject(objRootCtrl)
	local nIndex = objRootCtrl:GetSelectItemIndex()
	return GetFileItemByIndex(objRootCtrl, nIndex)
end


function GetFileItemUIByIndex(objRootCtrl, nIndex)
	return GetFileItemByIndex(objRootCtrl, nIndex)
end


---事件
function OnInitControl(self)
	UpdateFileList(self)
end


function OnLButtonUp(self)
	ClearSelectState(self)
	tFunHelper.UpdateFileStateUI()
end


function OnClickFileItem(self)
	local objRootCtrl = self:GetOwnerControl()
	local attr = objRootCtrl:GetAttribute()
	local nLastIndex = attr.SelectItemIndex
	
	local nCurIndex = self:GetItemIndex()
	attr.SelectItemIndex = nCurIndex
	if nLastIndex == nCurIndex then
		tFunHelper.UpdateBottomStyle()
		return
	end
	
	if nLastIndex ~= 0 then 
		local objLastItem = GetFileItemByIndex(objRootCtrl, nLastIndex)
		if objLastItem then
			objLastItem:SetSelectState(false)
		end
	end
	
	tFunHelper.UpdateBottomStyle()
end

---
function GetFileItemByIndex(objRootCtrl, nIndex)
	local strKey = "FileItem_"..tostring(nIndex)
	local objFileItem = objRootCtrl:GetControlObject(strKey)
	return objFileItem
end


function RemoveFileList(objRootCtrl)
	local objContainer = objRootCtrl:GetControlObject("DownLoadList.Container")
	objContainer:RemoveAllChild()
end


function CreateFileItem(objRootCtrl, tFileItemContent, nIndex)
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local objFather = objRootCtrl:GetControlObject("DownLoadList.Container")

	local strKey = "FileItem_"..tostring(nIndex)
	local objFileItem = objFactory:CreateUIObject(strKey, "DownLoadItem")
	objFather:AddChild(objFileItem)
	
	SetFileItemPos(objRootCtrl, objFileItem, nIndex)
	objFileItem:SetItemIndex(nIndex)
	objFileItem:SetFileContent(tFileItemContent)
	objFileItem:AttachListener("OnClick", false, OnClickFileItem)
end


function SetFileItemPos(objRootCtrl, objFileItem, nIndex)
	local attr = objRootCtrl:GetAttribute()
	local nWidth = attr.ItemWidth
	local nHeight = attr.ItemHeight
	local nSpanW = attr.ItemSpanW
	local nSpanH = attr.ItemSpanH
	
	local nNewTop = (nIndex-1)*(nHeight+nSpanH)

	objFileItem:SetObjPos2(0, nNewTop, "father.width", nHeight)
end



--滚动条
function UpdateScrollBar(objRootCtrl, nTotalLineCount)
	local attr = objRootCtrl:GetAttribute()
	attr.nTotalLineCount = nTotalLineCount
		
	ResetScrollBar(objRootCtrl)
end

function ResetScrollBar(objRootCtrl)
	if objRootCtrl == nil then
		return false
	end
	local objScrollBar = objRootCtrl:GetControlObject("listbox.vscroll")
	if objScrollBar == nil then
		return false
	end
	
	local attr = objRootCtrl:GetAttribute()
	local nLinePerPage = attr.nLinePerPage
	local nTotalLineCount = attr.nTotalLineCount
	
	local nItemHeight = GetItemHeight(objRootCtrl)
	local nMaxHeight = nItemHeight * nTotalLineCount
	local nPageSize = nItemHeight * nLinePerPage
	
	objScrollBar:SetScrollRange( 0, nMaxHeight - nPageSize, true )
	objScrollBar:SetPageSize(nPageSize, true)
	local _, endPos = objScrollBar:GetScrollRange()
	
	local curScrollPos = endPos or 0
	objScrollBar:SetScrollPos(curScrollPos, true)
	objRootCtrl:MoveItemListPanel(curScrollPos)
		
	if nLinePerPage == 0 or nLinePerPage >= nTotalLineCount then
		objRootCtrl:MoveItemListPanel(0)
		objScrollBar:SetVisible(false)
		objScrollBar:SetChildrenVisible(false)
		return true
	else
		objScrollBar:SetVisible(true)
		objScrollBar:SetChildrenVisible(true)
		objScrollBar:Show(true)
	end
	
	return true
end


function SetScrollPos(self, nScrollPos)
	local objRootCtrl = self
	local objScrollBar = objRootCtrl:GetControlObject("listbox.vscroll")
	if objScrollBar == nil then
		return
	end
	
	objScrollBar:SetScrollPos(nScrollPos, true)
end


function ProcessScrollWhenWheel(self, x, y, distance)
	local objRootCtrl = self
	local objScrollBar = objRootCtrl:GetControlObject("listbox.vscroll")
	
	local nScrollPos = objScrollBar:GetScrollPos()
	local nItemHeight = GetItemHeight(objRootCtrl)
		
    if distance > 0 then
		objScrollBar:SetScrollPos( nScrollPos - nItemHeight, true )
    else		
		objScrollBar:SetScrollPos( nScrollPos + nItemHeight, true )
    end

	local nNewScrollPos = objScrollBar:GetScrollPos()
	objRootCtrl:MoveItemListPanel(nNewScrollPos)
end


function CLB__OnScrollBarMouseWheel(self, name, x, y, distance)
	local objRootCtrl = self:GetOwnerControl()
	ProcessScrollWhenWheel(objRootCtrl, x, y, distance)
end


function CLB__OnScrollMousePosEvent(self)
	local objRootCtrl = self:GetOwnerControl()
	local nScrollPos = self:GetScrollPos()
	
	objRootCtrl:MoveItemListPanel(nScrollPos)
end


function MoveItemListPanel(self, nScrollPos)
	local objRootCtrl = self
	
	local objContainer = objRootCtrl:GetControlObject("DownLoadList.Container")
	if not objContainer then
		return
	end
	
	local objScrollBar = objRootCtrl:GetControlObject("listbox.vscroll")
	local nBegin, nEnd = objScrollBar:GetScrollRange()
	if nScrollPos>nEnd then
		nScrollPos = nEnd
	end
	
	local nL, nT, nR, nB = objContainer:GetObjPos()
	local nHeight = nB-nT
	local nNewT = 0-nScrollPos
	
	objContainer:SetObjPos(nL, nNewT, nR, nNewT+nHeight)
end


function GetItemHeight(self)
	local attr = self:GetAttribute()
	return attr.ItemHeight+attr.ItemSpanH
end


function ClearSelectState(objRootCtrl)
	local attr = objRootCtrl:GetAttribute()
	local nIndex = attr.SelectItemIndex
	
	if nIndex ~= 0 then
		local objLastItem = GetFileItemByIndex(objRootCtrl, nIndex)
		if objLastItem then
			objLastItem:SetSelectState(false)
			attr.SelectItemIndex = 0
		end
	end
end


------------------

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end


function RouteToFather(self)
	self:RouteToFather()
end

