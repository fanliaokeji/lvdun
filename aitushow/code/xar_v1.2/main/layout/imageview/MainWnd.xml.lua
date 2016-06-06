local dragUtil = Helper.APIproxy.LuaDragDropProcessor

function Wnd_OnCreate(self)
	local state, width, height = Helper.Setting.GetImageWindowConfig()
	local dftwidth, dftheight = 800, 600
	if state ~= nil then
		if state == "max" then
			self:Max()
		else
			width = tonumber(width) or dftwidth
			height = tonumber(height) or dftheight
			if width < dftwidth then
				width = dftwidth
			end
			if height < dftheight then
				height = dftheight
			end
			self:Move(0,0, width, height)
			self:Center()
		end
	else
		self:Move(0,0, dftwidth, dftheight)
		self:Center()
	end
end
function Wnd_OnDestroy(self)

end

function Wnd_OnClose(self)
	local objtree = self:GetBindUIObjectTree()
	local uiclient = objtree:GetUIObject("mainwnd.client")
	local caption = uiclient:GetControlObject("client.caption")
	local sysbtn = caption:GetControlObject("client.caption.sysbtn")
	sysbtn:FireExtEvent("OnClose")
	return 0, true
end

function OnShowWindow(self, bShow)
	if bShow then
		Helper.Tray.HostWnd = self
	end
end

function Wnd_OnSize(self, _type, width, height)
	local tree = self:GetBindUIObjectTree()
	local mainClient = tree:GetUIObject( "mainwnd.client")
	local captionObj = mainClient:GetCaption()
	local systemBtn = captionObj:GetSystemBtn()
	XLMessageBox(tostring(_type))
	if _type == "min" then
		return
	elseif _type == "max" then
		if tree ~= nil then
			if systemBtn ~= nil then
				systemBtn:SetMaxState( false )
			end
		end
		Helper.Setting.SetImageWindowConfig("max")
	elseif _type == "restored" then
		if tree ~= nil then
			if systemBtn ~= nil then
				systemBtn:SetMaxState( true )
			end
		end
		if not mainClient:IsFullScreen() then
			Helper.Setting.SetImageWindowConfig("normal", width, height)
		end
	end
	local rootObject = tree:GetRootObject()
	rootObject:SetObjPos(0, 0, width, height)
end

function OnInitControl(self)
end

function OnDestroy(self)
end

function OnLButtonDown(self)
end

function OnDragEnter(self, IDataObject, keyState, x, y)	
	return dragUtil.OnDragEnter( IDataObject, keyState, x, y )
end

function OnDragQuery(self, IDataObject, keyState, x, y)	
	return dragUtil.OnDragQuery( IDataObject, keyState, x, y );
end


function OnDragOver(self, IDataObject, keyState, x, y)
	return dragUtil.OnDragOver( IDataObject, keyState, x, y );
end

function OnDragLeave(self)
	return dragUtil.OnDragLeave();
end

function OnDrop(self, IDataObject, keyState, x, y)
	local dropAccept,disposed, send_to_next, tfilenames = dragUtil.OnDrop( IDataObject, keyState, x, y );
	if #tfilenames > 0 then
		local function fn()
			self:UpdateFileList()
			local attr = self:GetAttribute()
			local listViewObj = self:GetControlObject("client.filelistview")
			listViewObj:SelectItem(attr.CurDocItem.FilePath)
		end
		AsynCall(function ()
			self:LoadImageFile(tfilenames[1], nil, nil, fn)
		end)
	end
	return dropAccept,disposed, send_to_next
end