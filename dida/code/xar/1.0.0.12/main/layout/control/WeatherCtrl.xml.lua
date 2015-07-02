local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

local g_bLoadImageSucc = false
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
	ShowLoadFailImg(self, false)
	UpdateWeatherContent(self)
	
	SetLoadWeatherTimer(self)
	SetUpdateTimer(self)
end


-----
function UpdateWeatherContent(objRootCtrl)
	function Sunccess(strCity,strTemp1,strWeather1)
		ShowLoadingGif(objRootCtrl, false)
		ShowLoadFailImg(objRootCtrl, false)
		SetWeatherContent(objRootCtrl, strCity,strTemp1,strWeather1)
	end
	
	function Fail()
		g_bLoadImageSucc = false
		ShowLoadingGif(objRootCtrl, false)
		ShowLoadFailImg(objRootCtrl, true)
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


function SetLoadWeatherTimer(objRootCtrl)
	local nTimeSpanInMs = 10*1000  
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	
	timerManager:SetTimer(function(item, id)
		ShowLoadingGif(objRootCtrl, false)
		if g_bLoadImageSucc then
			ShowLoadFailImg(objRootCtrl, false)
		else
			ShowLoadFailImg(objRootCtrl, true)
		end		
		
		item:KillTimer(id)
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
		g_bLoadImageSucc = true
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


function ShowLoadFailImg(objRootCtrl, bShow)
	local objLoadFail = objRootCtrl:GetControlObject("WeatherCtrl.Image.Fail")
	local objLayout = objRootCtrl:GetControlObject("WeatherCtrl.Layout")
	if not objLoadFail or not objLayout then
		return
	end

	if g_bLoadImageSucc then
		objLoadFail:SetVisible(false)
		objLayout:SetVisible(true)
		objLayout:SetChildrenVisible(true)
		return
	end
	
	objLoadFail:SetVisible(bShow)
	objLayout:SetVisible(not bShow)
	objLayout:SetChildrenVisible(not bShow)
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