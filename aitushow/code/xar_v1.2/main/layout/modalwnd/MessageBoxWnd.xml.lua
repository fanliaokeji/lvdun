local Helper = XLGetGlobal("Helper")
local Selector = Helper.Selector
function GetMainWndHost(parentHwnd)
	local mainhost = Selector.select("", "", "MainWnd.Instance")
	local imghost = Selector.select("", "", "Kuaikan.MainWnd.Instance")
	if mainhost and mainhost:GetWndHandle() == parentHwnd then
		return mainhost
	end
	if imghost and imghost:GetWndHandle() == parentHwnd then
		return imghost
	end
end


function OnCreate(self)
	local HostWnd = GetMainWndHost(self:GetOwner())
	if not HostWnd then
		return 
	end
	local l, t, r, b = HostWnd:GetWindowRect()
	local w, h = r-l, b-t
	local _l, _t, _r, _b = self:GetWindowRect()
	local sw, sh = _r - _l, _b - _t
	local new_l = l + math.floor((w-sw)/2)
	local new_t = t + math.floor((h-sh)/2)
	if new_l < 0 then
		new_l = 0
	end
	if new_t < 0 then
		new_t = 0
	end
	local new_r = new_l + sw
	local new_b = new_t + sh
	self:Move(new_l, new_t, new_r, new_b)
end

function OnShowWindow(self, bVisible)
	local objtree = self:GetBindUIObjectTree()
	local MainText = objtree:GetUIObject("root"):GetObject("MainText")
	local yes, no = objtree:GetUIObject("root"):GetObject("yes"), objtree:GetUIObject("root"):GetObject("no")
	local userdata = self:GetUserData()
	if type(userdata) == "table" then
		if type(userdata[1]) == "string" then
			MainText:SetText(userdata[1])
		end
		if userdata[2] then
			yes:Show(false)
			no:Show(false)
		else
			yes:Show(true)
			no:Show(true)
		end
	end
end