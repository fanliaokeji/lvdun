local Helper = XLGetGlobal("Helper")
Helper.MessageBox = {}
local LOG = XLGetGlobal("LOG")
local tipUtil = XLGetObject("API.Util")

local MessageBox = Helper.MessageBox
--和位运算一样，个位表示点击什么按钮， 十位表示是否勾选
MessageBox.ID_CANCEL		= 0x01
MessageBox.ID_YES 			= 0x02
MessageBox.ID_RENAMESAVE 	= 0x03
MessageBox.ID_COVEROLD 		= 0x04
MessageBox.ID_NOSAVE 		= 0x05
MessageBox.ID_CHECK			= 0x10
MessageBox.ID_UNCHECK		= 0x20

function MessageBox.MessageBox(strText, objHostWnd, bHideBtns, bHideIcons, hasCheckBox)
	local nRet = Helper:CreateModalWnd("MessageBoxWnd","MessageBoxWndTree", objHostWnd, {strText, bHideBtns, bHideIcons, hasCheckBox})
	local bithigh = math.floor(nRet/0x10)*0x10
	local bitlow = math.fmod(nRet, 0x10)
	local bCheck = false
	if bithigh == MessageBox.ID_CHECK then
		bCheck = true
	end
	return bitlow, bCheck
end

function MessageBox.MessageBoxEx(objHostWnd)
	local nRet = Helper:CreateModalWnd("MessageBoxExWnd","MessageBoxExWndTree", objHostWnd)
	local bithigh = math.floor(nRet/0x10)*0x10
	local bitlow = math.fmod(nRet, 0x10)
	local bCheck = false
	if bithigh == MessageBox.ID_CHECK then
		bCheck = true
	end
	return bitlow, bCheck
end