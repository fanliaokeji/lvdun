local tipUtil = XLGetObject("API.Util")

function CheckAssociate()
	local usercfg = Helper.Setting.GetAssociateConfig()
	LOG("CheckAssociate:usercfg = "..tostring(usercfg))
	if not usercfg or usercfg == "" then
		return
	end
	local lose = ""
	for ext in string.gfind(usercfg, "([^;]+);?") do
		LOG("CheckAssociate, ext = "..tostring(ext))
		ext = string.lower(ext)
		if not tipUtil:IsAssociated(ext) then
			lose = lose..ext..";"
		end
	end
	LOG("CheckAssociate, lose = "..tostring(lose))
	if lose ~= "" then
		tipUtil:SetAssociate(lose, true)
		local tray = Helper.Tray.TrayObject
		if tray then
			tray:ShowNotifyIconTip(true, "文件关联提醒\n快看已为您关联了图片文件格式，您可通过快看直接打开各类图片，可点击此处进行修改。")
		end
	end
end
--注册托盘初始化事件
Helper.Tray.fnEventInit = CheckAssociate