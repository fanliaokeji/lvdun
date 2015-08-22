
function SetExternal( self, webevent )
	local attr = self:GetAttribute()
	attr.External = webevent
end

function ReportGoogle(strKey)
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local tStatInfo = {}
	tStatInfo.strEL = FunctionObj.GetInstallSrc() or ""
	tStatInfo.strEA = FunctionObj.GetMinorVer() or ""
	tStatInfo.strEV = 1
	tStatInfo.strEC = strKey
	FunctionObj.TipConvStatistic(tStatInfo)
end

function IsRealString(str) return type(str) == "string" and str ~= "" end

function GetDefaultBrowser()
	local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
	local strBrowserPath = ""
	--xp 系统只读一项注册表
	if not FunctionObj.IsUACOS() then
		strBrowserPath = FunctionObj.RegQueryValue("HKEY_CLASSES_ROOT\\http\\shell\\open\\command\\")
	else
		local strProgID = FunctionObj.RegQueryValue("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell"
											.."\\Associations\\UrlAssociations\\http\\UserChoice\\Progid")
		if not IsRealString(strProgID) then
			return ""
		end
		
		local strRegCmdPath = "HKEY_CLASSES_ROOT\\"..strProgID.."\\shell\\open\\command\\"
		strBrowserPath = FunctionObj.RegQueryValue(strRegCmdPath)
	end
	return strBrowserPath
end

local gCurRetryTimes = 0	
local gTotalRetryTimes = 1
function Navigate( self, url )
	local attr = self:GetAttribute()
	attr.ErrorUrls = {}
	attr.CompleteUrls = {}
    local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	
	local browser = self:GetControlObject( "browser" )
	if browser ~= nil then
		self:RemoveChild(browser)
	end
	browser = objFactory:CreateUIObject( "browser", "WebBrowseObject" )
	self:AddChild( browser )
	browser:SetVisible( false )
	browser:SetDocHostFlags(8)
	browser:SetChildrenVisible( false )
	if attr.External ~= nil then
		browser:SetExternal( attr.External )
	end
	browser:SetObjPos( "0", "0", "father.width", "father.height" )
	local cookie, ret = browser:AttachListener("OnNavigateError", false, function( self, URL )
												table.insert( attr.ErrorUrls, URL )
												for i = 1, #attr.CompleteUrls do
													if attr.CompleteUrls[ i ] == URL then
														table.remove( attr.CompleteUrls, i )
														break
													end
												end
												return true
											 end)
	browser:AttachListener( "OnNavigateComplete2", false, function( self, URL )
													local error_ = false
													for i = 1, #attr.ErrorUrls do
														if attr.ErrorUrls[ i ] == URL then
															error_ = true
															break
														end
													end
													if not error_ then
														table.insert( attr.CompleteUrls, URL )
													end
													return true
												 end )
	browser:AttachListener( "OnCommandStateChange", false, function( obj, command, enable )
																self:FireExtEvent( "OnCommandStateChange", command, enable )
																return true
														   end )
	browser:AttachListener( "OnNewWindow3", false,  function(obj, flags, url_context, url)
														local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
														local frameHostWnd = hostwndManager:GetHostWnd("TipCommon.Instance")
														frameHostWnd:Show(0)
														if frameHostWnd then
															local tData = frameHostWnd:GetUserData()
															ReportGoogle("tipclick_"..tData[1])
															local objtree = frameHostWnd:GetBindUIObjectTree()
															frameHostWnd:UnbindUIObjectTree()
															local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")
															objtreeManager:DestroyTree(objtree)
															frameHostWnd:Destroy()
															hostwndManager:RemoveHostWnd("TipCommon.Instance")
														end
														
														local tipUtil = XLGetObject("API.Util")
														local strBrowserExePath = GetDefaultBrowser()
														local bCancel = false
														if IsRealString(strBrowserExePath) and tipUtil:QueryFileExists(strBrowserExePath) then
															tipUtil:ShellExecute(0, "open", strBrowserExePath, url, 0, "SW_SHOW")
															bCancel = true
														end
														return 0, 0, bCancel, true
												   end )
	browser:Navigate( url )
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	attr.timer = timerManager:SetTimer( function ( item, id )
								if not browser:GetBusy() then
									timerManager:KillTimer( id )
									attr.timer = nil
									for i = 1, #attr.ErrorUrls do
										for j = 1, #attr.CompleteUrls do
											if attr.ErrorUrls[ i ] == attr.CompleteUrls[ j ] then
												table.remove( attr.CompleteUrls, j )
												break
											end
										end
									end
									local FunctionObj = XLGetGlobal("DiDa.FunctionHelper") 
									if #attr.CompleteUrls > 0 then
										if type(FunctionObj.ShowPopupWndByName) == "function" then
											FunctionObj.ShowPopupWndByName("TipCommon.Instance", true)
											browser:SetVisible( true )
											browser:SetChildrenVisible( true )
										end
									else
										browser:SetVisible( false )
										browser:SetChildrenVisible( false )
										self:RemoveChild( browser )
										attr.errorurl = url
										if gCurRetryTimes < gTotalRetryTimes then
											gCurRetryTimes = gCurRetryTimes + 1
											ReloadPage(self)
										else
											local objtree = self:GetOwner()
											local hostwmd = objtree:GetBindHostWnd()
											local tData = hostwmd:GetUserData()
											ReportGoogle("tiperrorclose_"..tData[1])
										end
									end
								end
						   end, 500 )
end

function OnDestroy( self )
	local attr = self:GetAttribute()
	if attr.timer ~= nil then
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
		timerManager:KillTimer( attr.timer )
	end
	return true
end

function ReloadPage(self)
	local attr = self:GetAttribute()
	self:Navigate(attr.errorurl)
	return true
end
