function GetObjWH(obj)
	local l, t, r, b = obj:GetObjPos()
	return r-l, b-t
end

function GetObjXYWH(obj)
	local l, t, r, b = obj:GetObjPos()
	return l, t, r-l, b-t
end

--xoffset， yoffset分别表示大图距原始位置的横纵坐标偏移量， size_rates 当前大小 与 原始大小的比值
function Update(self, xoffset, yoffset, size_rates)
	local selarea = self:GetObject("selarea")
	local attr = self:GetAttribute()
	local x, y, w, h = GetObjXYWH(selarea)
	local new_w = w/size_rates
	local new_h = h/size_rates
	local new_x = x + (w-new_w)/2
	local new_y = y + (h-new_h)/2
	newnew_x = new_x - xoffset/attr.rates
	newnew_y = new_y - yoffset/attr.rates
	local insideimg = self:GetObject("insideimg")
	local _l, _t, _r, _b = insideimg:GetObjPos()
	if newnew_x < _l then
		newnew_x = _l + 1
	elseif newnew_x + new_w > _r then
		newnew_x = _r - 1
	end
	if newnew_y < _t then
		newnew_y = _t + 1
	elseif newnew_y + new_h > _b then
		newnew_y = _b -1
	end
	selarea:SetObjPos2(newnew_x, newnew_y, new_w, new_h)
	SyncInOutBmp(self)
	local title = self:GetObject("title")
	title:SetText("缩放至"..math.floor(size_rates*100).."%")
end

function SetObjBMP(obj, bitmap, rates)
	local x, y, w, h = GetObjXYWH(obj)
	local _, bw, bh = bitmap:GetInfo()
	local new_w, new_h = bw/rates, bh/rates
	local new_x, new_y = x + (w - new_w)/2, y + (h - new_h)/2
	local new_bmp = bitmap:Stretch(new_w, new_h)
	obj:SetObjPos2(new_x, new_y, new_w, new_h)
	if new_bmp then
		obj:SetBitmap(new_bmp)
	end
end

--传入大图Bitmap对象
function Init(self, bitmap)
	if not bitmap then
		return
	end
	local _, w, h = bitmap:GetInfo()
	local fillobj = self:GetObject("fillobj")
	local fw, fh = GetObjWH(fillobj)
	local attr = self:GetAttribute()
	if w/fw > h/fh then
		attr.rates = w/fw
	else
		attr.rates = h/fh
	end
	local insideimg = self:GetObject("insideimg")
	local outsideimg = self:GetObject("outsideimg") 
	SetObjBMP(insideimg, bitmap, attr.rates)
	SetObjBMP(outsideimg, bitmap, attr.rates)
	local new_w, new_h = w/attr.rates, h/attr.rates
	local new_x, new_y = (fw-new_w)/2, (fh-new_h)/2 
	local selarea = self:GetObject("selarea")
	selarea:SetObjPos2(new_x, new_y, new_w, new_h)
	selarea:SetVisible(true)
	selarea:SetChildrenVisible(true)
	SyncInOutBmp(self)
	self:GetObject("title"):SetText("1212")
	self:SetVisible(true)
	self:SetChildrenVisible(true)
end

--同步内外位图坐标
function SyncInOutBmp(self)
	local outsideimg = self:GetControlObject("outsideimg")
	local insideimg = self:GetControlObject("insideimg")
	local l, t, r, b = insideimg:GetAbsPos()
	local _l, _t, _r, _b = outsideimg:GetAbsPos()
	local dx, dy, w, h = l-_l, t-_t, r-l, b-t
	local x, y = GetObjXYWH(outsideimg)
	outsideimg:SetObjPos2(x+dx, y+dy, w, h)
end

--边界检查
function GetSelMovePos(sel, dx, dy)
	dx = math.floor(dx)
	dy = math.floor(dy)
	local l, t, r, b = sel:GetObjPos()
	local insideimg = sel:GetObject("control:insideimg")
	local _l, _t, _r, _b = insideimg:GetObjPos()
	if l+dx >= _l and r+dx <= _r then 
		l = l+dx 
		r = r+dx
	elseif dx > 0 then
		l = math.floor(l+_r-1-r) 
		r = math.floor(_r-1)
	elseif dx < 0 then
		r = math.floor(r+ _l+1-l)
		l = math.floor(_l+1)
	end
	if t+dy >= _t and b+dy <= _b then 
		t = t+dy 
		b = b+dy
	elseif dy > 0 then
		t = _b-1-b+t
		b = _b-1
	elseif dy < 0 then
		b = _t+1-t+b
		t = _t+1
	end
	return l, t, r, b
end

function selareaOnMouseMove(self, x, y)
	local ower = self:GetOwnerControl()
	local attr = ower:GetAttribute()
	if attr.press then
		local p = attr.press
		local dx, dy = x-p.x, y-p.y
		local l, t, r, b = GetSelMovePos(self, dx, dy)
		self:SetObjPos(l, t, r, b)
		SyncInOutBmp(ower)
	end
end

function MiniViewerOnInitControl(self)
	self:SetVisible(false)
	self:SetChildrenVisible(false)
	SyncInOutBmp(self)
	self:SetCursorID("IDC_HAND")
end

function selareaOnLButtonUp(self, x, y)
	local ower = self:GetOwnerControl()
	local attr = ower:GetAttribute()
	attr.press = nil
	self:SetCaptureMouse(false)
end

function closeOnClick(self)
	local owner = self:GetOwnerControl()
	owner:SetVisible(false)
	owner:SetChildrenVisible(false)
end

function selareaOnLButtonDown(self, x, y)
	local ower = self:GetOwnerControl()
	local attr = ower:GetAttribute()
	local l, t, r, b = self:GetObjPos()
	attr.press = {l=l, t=t, r=r, b=b, x=x, y=y}
	self:SetCaptureMouse(true)
end

