--封装动画方法，挂到Helper.Ani
local ObjectBase = XLGetGlobal("ObjectBase")
local Helper = XLGetGlobal("Helper")

local Animation = {}
Helper.Ani = Animation

Animation.AniMap = {}
function Animation:StopObjectAni(object)
	if not object or not object:GetHandle() then return end
	
	local ani = self.AniMap[object:GetHandle()]
	if ani then
		self.AniMap[object:GetHandle()] = nil
		ani:ForceStop()
	end
end

function Animation:RunPosChangeAni(obj, startLeft, startTop, startRight, startBottom, endLeft, endTop, endRight, endBottom, aniFinishCallBack, totalTime)
	self:StopObjectAni(obj)
	totalTime = totalTime or 500
	
	local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")
	local posChangeAni = aniFactory:CreateAnimation("PosChangeAnimation")
	posChangeAni:SetTotalTime(totalTime)
	posChangeAni:SetKeyFrameRect(startLeft, startTop, startRight, startBottom, endLeft, endTop, endRight, endBottom)
	
	local objHandle = obj:GetHandle()
	local function onAniFinish(_,old,new)
		if new == 4 then
			self.AniMap[objHandle] = nil
			if aniFinishCallBack then aniFinishCallBack(obj) end
		end
		return true
	end
	
	posChangeAni:BindObj(obj)
	local tree = obj:GetOwner()
	tree:AddAnimation(posChangeAni)
	posChangeAni:AttachListener(true,onAniFinish)
	
	self.AniMap[objHandle] = posChangeAni
	posChangeAni:Resume()
	return posChangeAni
end

function Animation:RunSeqFrameAni(object, seqResID, fun, total_time, loop)	
	if not object then
		XLMessageBox("not object ")
		return
	end
	self:StopObjectAni(obj)
	totalTime = totalTime or 500
	if loop == nil then
		loop = true
	end
	
	object:SetVisible(true)
	local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")
	local seqFramAni = aniFactory:CreateAnimation("SeqFrameAnimation")
	seqFramAni:SetLoop(loop)
	seqFramAni:SetResID(seqResID)
	seqFramAni:SetTotalTime(total_time)
	
	local objHandle = object:GetHandle()
	local function OnAniFinish(_, old, new)
		if new == 4 then
			self.AniMap[objHandle] = nil
			if fun then fun(obj) end
		end
		return true
	end
	
	seqFramAni:BindImageObj(object)
	local tree = object:GetOwner()
	tree:AddAnimation(seqFramAni)
	seqFramAni:AttachListener(true, OnAniFinish)
	
	self.AniMap[objHandle] = seqFramAni
	seqFramAni:Resume()
	return seqFramAni
end
