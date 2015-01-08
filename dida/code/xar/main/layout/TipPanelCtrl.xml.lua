local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

----方法----
function SetTipData(self, infoTab) 
	
	CreateFilterListener(self)
	return true
end


---事件--
function OnCloseWnd(self)
	tipUtil:Exit("test")
end



--监听事件
function CreateFilterListener(objRootCtrl)
	local objFactory = XLGetObject("APIListen.Factory")
	if not objFactory then
		tFunHelper.TipLog("[CreateFilterListener] not support APIListen.Factory")
		return
	end
	
	local objListen = objFactory:CreateInstance()	
	objListen:AttachListener(
		function(key,...)	

			tFunHelper.TipLog("[CreateFilterListener] key: " .. tostring(key))
			
			local tParam = {...}	
			-- if tostring(key) == "" then
				-- OnFilterResult(tParam)
			-- end
			
		end)
end


function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end


function IsRealString(AString)
    return type(AString) == "string" and AString ~= ""
end

