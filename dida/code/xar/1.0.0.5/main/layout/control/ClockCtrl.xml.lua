function OnInitControl(self)
	ResetAllText(self)
	StartTimer(self)
	
end


function ResetAllText(objRootCtrl)
	local nText = tonumber(os.date("%H"))
	local strText1 = tostring(math.floor(nText/10))
	local strText2 = tostring(math.mod(nText, 10))
	SetUIText(objRootCtrl, strText1, "hour1")
	SetUIText(objRootCtrl, strText2, "hour2")
	
	local nText = tonumber(os.date("%M"))
	local strText1 = tostring(math.floor(nText/10))
	local strText2 = tostring(math.mod(nText, 10))
	SetUIText(objRootCtrl, strText1, "min1")
	SetUIText(objRootCtrl, strText2, "min2")
	
	local nText = tonumber(os.date("%S"))
	local strText1 = tostring(math.floor(nText/10))
	local strText2 = tostring(math.mod(nText, 10))
	SetUIText(objRootCtrl, strText1, "sec1")
	SetUIText(objRootCtrl, strText2, "sec2")
end


function SetUIText(objRootCtrl, strText, strUIKey)
	local objUIElem = objRootCtrl:GetControlObject(strUIKey)
	if objUIElem then
		objUIElem:SetText(strText)
	end	
end



function StartTimer(objRootCtrl)
	local timeMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
	timeMgr:SetTimer(function(Itm, id)
			ResetAllText(objRootCtrl)
		end, 1000)
end

