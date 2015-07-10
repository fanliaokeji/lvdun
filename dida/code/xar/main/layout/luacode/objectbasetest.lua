ObjectBase = XLGetGlobal("ObjectBase")

FlyRabbit = ObjectBase:new()
XLSetGlobal("FlyRabbit", FlyRabbit)

function FlyRabbit:LoadList()
	--加载本地lua
	
	--每加载一条记录，发一次事件
end

function FlyRabbit:AddItem(url, path, ...)
	--合法性判读
	
	--写入table
	
	--发事件
end

function FlyRabbit:RemoveItem(url, path, ...)
	--合法性判读
	
	--从table删除该item
	
	--发事件
end

function FlyRabbit:CleanItem(url, path, ...)
	--清空table
	--发事件
end

function FlyRabbit:EditItem(url, path, ...)
	--编辑
	--发事件
end

function FlyRabbit:UpdateItemProcess(url, path, ...)
	--编辑
	--发事件
end

local view = {}
function view:CallBackA(event, param1, param2)
	-- print("callBackA"..param1)
	XLMessageBox("callBackA"..param1)
end
function view:CallBackAA(event, param1, param2)
	-- print("callBackA"..param1)
	XLMessageBox("callBackAA"..param1)
end

FlyRabbit:AddListener("eventA", view.CallBackA, view)

FlyRabbit:AddListener("eventA", view.CallBackAA, view, false)
FlyRabbit:AddListener("eventB", function(_, event, param1) XLMessageBox("eventB callBack "..param1) end)

FlyRabbit:DispatchEvent("eventA", "  hello!")

FlyRabbit:DispatchEvent("eventB", "  hello!")

-- FlyRabbit:removeListener("eventA",  FlyRabbit.callBackA, FlyRabbit)
-- FlyRabbit:dispatchEvent("eventA", "  hello!")
-- FlyRabbit:dispatchEvent("eventB", "  hello!")

