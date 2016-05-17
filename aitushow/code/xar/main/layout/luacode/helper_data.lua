local Helper = XLGetGlobal("Helper")

-- 定长队列，超出长度，自动丢弃尾部元素
local FixedLengthQueue = {}
FixedLengthQueue.queueLength = 5
FixedLengthQueue.data = {}

function FixedLengthQueue:New()
	local obj = {}
	setmetatable(obj, self)
	self.__index = self
	
	return obj
end

function FixedLengthQueue:GetTableLength()
	return self.queueLength
end

function FixedLengthQueue:SetTableLength(newLength)
	if "number" == type(newLength) then
		self.queueLength = newLength
	end
end

function FixedLengthQueue:Insert(item)
	table.insert(self.data, item)
	if #self.data > self.queueLength then
		table.remove(self.data, #self.data)
	end
end

function FixedLengthQueue:Remove(index)
	if #self.data >= index then
		table.remove(self.data, index)
	end
end

Helper.FixedLengthQueue = FixedLengthQueue