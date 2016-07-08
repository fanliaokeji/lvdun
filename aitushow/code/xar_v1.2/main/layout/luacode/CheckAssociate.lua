local tipUtil = XLGetObject("API.Util")

--notneedshowtip默认为nil， 为nil时弹气泡
function CheckAssociate(notneedshowtip)
	local usercfg = Helper.Setting.GetAssociateConfig()
	LOG("CheckAssociate:usercfg = "..tostring(usercfg))
	if not usercfg or usercfg == "" then
		return
	end
	--count是当前关联数
	local lose, count = "", 0
	for ext in string.gfind(usercfg, "([^;]+);?") do
		LOG("CheckAssociate, ext = "..tostring(ext))
		ext = string.lower(ext)
		if not tipUtil:IsAssociated(ext) then
			lose = lose..ext..";"
		end
		count = count + 1
	end
	LOG("CheckAssociate, lose = "..tostring(lose))
	if lose ~= "" then
		--抢关联上报
		StatUtil.SendStat({
			strEC = "associate",
			strEA = "auto",
			strEL = tostring(count),
			strEV = 1,
		}) 
		tipUtil:SetAssociate(lose, true)
		local tray = Helper.Tray.TrayObject
		if not notneedshowtip and tray and not Helper.Tray.bHide then
			tray:ShowNotifyIconTip(true, "文件关联提醒\n快看已为您关联了图片文件格式，您可通过快看直接打开各类图片，可点击此处进行修改。")
		end
	end
	SetTimer(function()
			CheckAssociate(true)
		end, 600*1000)
end
--注册托盘初始化事件
Helper.Tray.fnEventInit = CheckAssociate
--设成全局
Helper.CheckAssociate = CheckAssociate