local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")

function GenerateFont()
	local XARManager = XLGetObject("Xunlei.UIEngine.XARManager")
	local xar = XARManager:GetXAR("main")
	
	local strFaceName = "微软雅黑"
	local bIsUAC = tFunctionHelper.IsUACOS()
	if not bIsUAC then
		strFaceName = "黑体"
	end	
	
	local tFontSize = {10, 11, 12, 13, 14, 15, 16, 24}
	for _, nSize in pairs(tFontSize) do
		local strFontID = "font.default"..tostring(nSize)
		xar:CreateFont({["id"]=strFontID, ["height"]=nSize, ["facename"]=strFaceName})
	end
	
	xar:CreateFont({["id"]="default.font", ["height"]=12, ["facename"]=strFaceName})
	xar:CreateFont({["id"]="font.default12.bold", ["height"]=12, ["facename"]=strFaceName, ["weight"]="bold"})
end


GenerateFont()



