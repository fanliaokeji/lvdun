local Helper = XLGetGlobal("Helper")
local Selector = Helper.Selector
function GetMainWndHost(parentHwnd)
	local mainhost = Selector.select("", "", "MainWnd.Instance")
	local imghost = Selector.select("", "", "ImageWnd.Instance")
	if mainhost and mainhost:GetWndHandle() == parentHwnd then
		return mainhost
	end
	if imghost and imghost:GetWndHandle() == parentHwnd then
		return imghost
	end
end



function OnCreate(self)
	local HostWnd = GetMainWndHost(self:GetOwner())
	local l, t, r, b = HostWnd:GetWindowRect()
	local w, h = r-l, b-t
	local _l, _t, _r, _b = self:GetWindowRect()
	local new_l = l + math.floor((w-(_r - _l))/2)
	local new_t = t + math.floor((h-(_b - _t))/2)
	local new_r = new_l + _r - _l
	local new_b = new_t + _b - _t
	self:Move(new_l, new_t, new_r, new_b)
end

function OnShowWindow(self, bVisible)
	
end