local Selector = {}
local Helper = XLGetGlobal("Helper")
Helper.Selector = Selector

local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")

function Selector.select(objname, ctrlID, hostname)
	local objMainWnd = hostwndManager:GetHostWnd(hostname)
	if ctrlID == "" or objname == nil then
		return objMainWnd
	end
	if not objMainWnd then
		return
	end
	local objTree = objMainWnd:GetBindUIObjectTree()
	if not objTree then
		return nil
	end
	local objRoot = objTree:GetUIObject(ctrlID)
	if objname == "" or objname == nil then
		return objRoot
	end
	return objRoot:GetControlObject(objname)
end