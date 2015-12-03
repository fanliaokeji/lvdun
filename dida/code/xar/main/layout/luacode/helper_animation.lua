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
