-------事件---
function OnSelect_Open(self)
	Helper.Tray.Open()
end

function OnSelect_Setting(self)
	Helper.Tray.Setting()
end


function OnSelect_Sysboot(self)
	Helper.Tray.Sysboot()
end


function OnInit_Sysboot(self)
	local attr = self:GetAttribute()
	if Helper.Setting.IsSysBoot() then
		attr.Icon = "setting_check.icon"
	else
		attr.Icon = "setting_uncheck.icon"
	end
end


function OnSelect_Yijian(self)
	Helper.Tray.Suggestion()
end


function OnSelect_Exit(self)
	Helper.Tray.Exit()
end