local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local tipUtil = tFunHelper.tipUtil

function OnInitCalendarPreView(self)
	local weatherBkg = self:GetControlObject("weather.bkg")--preview的大背景
	local weatherBkgRedID = nil
	local function Sunccess(strCity,strTemp1,strWeather)
		if "string" == type(strWeather) then
			if string.find(strWeather, "晴") then
				weatherBkgRedID = "weather.sunny.bkg"
			elseif string.find(strWeather, "雨") then
				weatherBkgRedID = "weather.rain.bkg"
			elseif string.find(strWeather, "雪") then
				weatherBkgRedID = "weather.snow.bkg"
			elseif string.find(strWeather, "风") then
				weatherBkgRedID = "weather.typhoon.bkg"
			end
		end
		if weatherBkgRedID then
			weatherBkg:SetTextureID(weatherBkgRedID)
		end
	end
	
	local function Fail()
		
	end
	
	tFunHelper.GetWeatherInfo(Sunccess, Fail)
end
