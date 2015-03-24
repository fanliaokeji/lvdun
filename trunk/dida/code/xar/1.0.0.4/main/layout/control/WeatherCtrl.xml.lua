local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil
local g_tWeatherResList = {
	[1] = {
		["strKey"] = "霾",
		["strResID"] = "霾",
	},
	[2] = {
		["strKey"] = "雾",
		["strResID"] = "雾",
	},
	[3] = {
		["strKey"] = "雹",
		["strResID"] = "冰雹",
	},
}



---事件
function OnInitWeatherCtrl(self)
	ShowLoadingGif(self, true)
	UpdateWeatherContent(self)
	SetUpdateTimer(self)
end


-----
function UpdateWeatherContent(objRootCtrl)
	function Sunccess(strCity,strTemp1,strWeather1)
		ShowLoadingGif(objRootCtrl, false)
		SetWeatherContent(objRootCtrl, strCity,strTemp1,strWeather1)
	end
	
	function Fail()
		ShowLoadingGif(objRootCtrl, false)
	end
	
	tFunHelper.GetWeatherInfo(Sunccess, Fail)
end


function SetUpdateTimer(objRootCtrl)
	local nTimeSpanInMs = 6*3600*1000  
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetTimer(function(item, id)
		UpdateWeatherContent(objRootCtrl)
	end, nTimeSpanInMs)
end


function SetWeatherContent(objRootCtrl, strCity, strTem, strWeather)
	SetDetailText(objRootCtrl, strWeather..strTem)
	SetWeatherImage(objRootCtrl, strWeather)

	local objZone = objRootCtrl:GetControlObject("WeatherCtrl.Zone")
	if objZone then
		objZone:SetText(strCity)
	end
	
	SetAverageText(objRootCtrl, strTem)
end


function SetDetailText(objRootCtrl, strText)
	local objDetail = objRootCtrl:GetControlObject("WeatherCtrl.Detail")
	if objDetail then
		objDetail:SetText(strText)
	end

	local nRootL, nRootT, nRootR, nRootB = objRootCtrl:GetObjPos()
	local nL, nT, nR, nB = objDetail:GetObjPos()
	local nW = nR - nL
	local nTextExtent = objDetail:GetTextExtent()
		
	local nDiff = nTextExtent-nW
	
	objRootCtrl:SetObjPos(nRootL-nDiff, nRootT, nRootR, nRootB)
	
	local nL, nT, nR, nB = objDetail:GetObjPos()
	local nFatherW = nRootR-nRootL+nDiff
	local nNewR = nFatherW-20
	objDetail:SetObjPos(nNewR-nTextExtent, nT, nNewR, nB)
end


function SetAverageText(objRootCtrl, strText)
	local objAverage = objRootCtrl:GetControlObject("WeatherCtrl.Average")
	local _, _, strBegin, strEnd = string.find(strText, "(%d*)[^%d]*(%d*).*")
	local nBegin = tonumber(strBegin)
	local nEnd = tonumber(strEnd)
	if nBegin == nil or nEnd == nil then
		return
	end
	local nAverage = math.floor((nBegin+nEnd)/2)
	if objAverage then
		objAverage:SetText(tostring(nAverage).."℃")
	end
end


function SetWeatherImage(objRootCtrl, strWeather)
	local objBitmap = GetWeatherImg(strWeather)
	
	local objImage = objRootCtrl:GetControlObject("WeatherCtrl.Image")
	if objImage then
		objImage:SetBitmap(objBitmap)
	end
end


function GetWeatherImg(strWeather)
	local objBitmap = nil
	
	local objXarManager = XLGetObject("Xunlei.UIEngine.XARManager")
	local objXar = objXarManager:GetXAR("main")
	if not objXar then
		return nil
	end
	
	objBitmap = objXar:GetBitmap(strWeather)
	if objBitmap then
		return objBitmap
	end
	
	if string.find(strWeather, "转") then
		local _, _, strFirst, strLast = string.find(strWeather, "(.*)转(.*)")
		objBitmap = objXar:GetBitmap(strLast)
		if objBitmap then
			return objBitmap
		end
		
		objBitmap = objXar:GetBitmap(strFirst)
		if objBitmap then
			return objBitmap
		end
	end
	
	if string.find(strWeather, "到") then
		local _, _, strFirst, strLast = string.find(strWeather, "(.*)到(.*)")
		objBitmap = objXar:GetBitmap(strLast)
		if objBitmap then
			return objBitmap
		end
		
		objBitmap = objXar:GetBitmap(strFirst)
		if objBitmap then
			return objBitmap
		end
	end
	
	for nIndex, tResInfo in ipairs(g_tWeatherResList) do
		if string.find(strWeather, tResInfo.strKey) then
			objBitmap = objXar:GetBitmap(tResInfo.strResID)
			if objBitmap then
				return objBitmap
			end
		end	
	end
		
	return objBitmap
end


function ShowLoadingGif(objRootCtrl, bShow)
	local objLoading = objRootCtrl:GetControlObject("WeatherCtrl.Loading")
	if not objLoading then
		return
	end
	
	objLoading:SetVisible(bShow)
	if bShow then
		objLoading:Play()
	else
		objLoading:Stop()
	end
end


------------------
function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end