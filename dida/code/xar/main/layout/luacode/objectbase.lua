--用lua实现面向对象、简单的消息分发, 不依赖任何C工程
ObjectBase = {}
XLSetGlobal("ObjectBase", ObjectBase)

function ObjectBase:New(obj)
	obj = obj or {}
	setmetatable(obj, self)
	
	if not obj.listener then
		obj.listener = {}
	end
	
	self.__index = self
	return obj
end

function ObjectBase:AddListener(event, callBack, ownerObj, bAddToEnd)
	assert("string" == type(event), "param1 event must be string")
	if not self.listener[event] then
		self.listener[event] = {}
	end
	if nil == bAddToEnd then
		bAddToEnd = true
	end
	--这里不检查同一个callBack是否重复订阅同一个event
	if bAddToEnd then--添加到队列末尾
		table.insert(self.listener[event], {["callBack"] = callBack, ["ownerObj"] = ownerObj})
	else
		table.insert(self.listener[event], 1, {["callBack"] = callBack, ["ownerObj"] = ownerObj})
	end
	
	return callBack
end

function ObjectBase:DispatchEvent(event, ...)
	if not self.listener[event] then
		return
	end
	
	for i=1, #self.listener[event] do
		self.listener[event][i]["callBack"](self.listener[event][i]["ownerObj"], event, ...)
	end
end

function ObjectBase:RemoveListener(event, callBack, ownerObj)
	if not self.listener[event] then
		return
	end
	
	for i=#self.listener[event], 1, -1 do
		if self.listener[event][i]["callBack"] == callBack and 
			self.listener[event][i]["ownerObj"] == ownerObj then
			
			table.remove(self.listener[event], index)
			--重复订阅的也remove掉
		end
	end
end

function ObjectBase:RemoveAllListener()
	self.listener = {}
end