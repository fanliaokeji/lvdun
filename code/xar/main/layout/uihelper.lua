local tipUtil = XLGetObject("GS.Util")
local tFunctionHelper = XLGetGlobal("GreenWallTip.FunctionHelper")
local Helper = XLGetGlobal("Helper")

local appKey = "1824709228"
local lvdunImageUrl = "http://www.lvdun123.com/images/interface.jpg"
local lvdunHomePageUrl = "http://www.lvdun123.com"
--所需参数：appkey、title、count、pic、
--ralateUid=5458208915关联用户的UID，分享微博会@该用户(可选)*/ 绿盾广告管家UID=5458208915
local shareToWeiboUrl = "http://v.t.sina.com.cn/share/share.php?"

--所需参数：url、desc、title、pics
local shareToQQUrl = "http://connect.qq.com/widget/shareqq/index.html?"

--所需参数：url、showcount、desc、title、pics
local shareToQQZoneUrl = "http://sns.qzone.qq.com/cgi-bin/qzshare/cgi_qzshare_onekey?"

--所需参数：href、text、name、image
local shareToDoubanUrl = "http://www.douban.com/share/service?"

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@uihelper: " .. tostring(strLog))
	end
end

function GetMainWndInst()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local objMainWnd = hostwndManager:GetHostWnd("GreenWallTipWnd.MainFrame")
	return objMainWnd
end

function GetMainCtrlChildObj(strObjName)
	local objMainWnd = GetMainWndInst()
	if not objMainWnd then
		return nil
	end
	
	local objTree = objMainWnd:GetBindUIObjectTree()
	
	if not objTree then
		TipLog("[GetMainCtrlChildObj] get main wnd or tree failed")
		return nil
	end
	
	local objRootCtrl = objTree:GetUIObject("root.layout:root.ctrl")
	if not objRootCtrl then
		TipLog("[GetMainCtrlChildObj] get objRootCtrl failed")
		return nil
	end 

	return objRootCtrl:GetControlObject(tostring(strObjName))
end

function OpenPanel(strNewCtrlName)
	local objMainBodyCtrl = GetMainCtrlChildObj("TipCtrl.MainWnd.MainBody")
	if objMainBodyCtrl == nil then
		return false
	end
	
	local strCurCtrlName = objMainBodyCtrl:GetCurrentCtrlName()
	if strCurCtrlName ~= strNewCtrlName then
		objMainBodyCtrl:ChangePanel(strNewCtrlName)
		return true
	end
	
	return false
end

function IncVideoFiltTime(nVideoIncSec)
	local objMainBody = GetMainCtrlChildObj("TipCtrl.MainWnd.MainBody")
	local objAdvCount = objMainBody:GetChildObjByCtrlName("ChildCtrl_AdvCount")
	local objClockCtrl = objAdvCount:GetControlObject("ChildCtrl_AdvCount.Low.Time")
	local tUserConfig = tFunctionHelper.ReadConfigFromMemByKey("tUserConfig") or {}
	local nCurFiltTime = tUserConfig["nFiltVideoTimeSec"] or 0
	local nNewFiltTime = nCurFiltTime+nVideoIncSec
	nNewFiltTime = TryClearTime(nNewFiltTime)
	
	objClockCtrl:SetTime(nNewFiltTime)
	tUserConfig["nFiltVideoTimeSec"] = nNewFiltTime
	tFunctionHelper.SaveConfigToFileByKey("tUserConfig")
end

--满2年清零
function TryClearTime(nTimeInSec)
	local n2YearInSec = 2*365*24*3600
	if nTimeInSec < n2YearInSec then
		return nTimeInSec
	end
	
	return 0
end

function UpdateWindow()
	local objMainWnd = GetMainWndInst()
	objMainWnd:UpdateWindow()
end

--分享接口
function GetShareUrl(sShareType,title,desc)
	if not title then
		title = "绿盾广告管家"
	end
	if not desc then
		desc = "良心软件啊，竟然能过滤所有视频网站的缓冲广告。免费，亲测有效。看视频再也不用等广告简直爽到哭，下载地址：http://www.lvdun123.com/"
	end
	
	local url = ""
	if "weibo" == string.lower(sShareType) then
		url = url..shareToWeiboUrl
		url = url.."appkey="..appKey
		url = url.."&count=1"
		url = url.."&ralateUid=5458208915"
		url = url.."&title="..Helper:UrlEncode(desc)
		url = url.."&pic="..Helper:UrlEncode(lvdunImageUrl)
	elseif "qq" == string.lower(sShareType) then
		url = url..shareToQQUrl
		url = url.."url="..Helper:UrlEncode(lvdunHomePageUrl)
		url = url.."&title="..Helper:UrlEncode(title)
		url = url.."&desc="..Helper:UrlEncode(desc)
		url = url.."&pics="..Helper:UrlEncode(lvdunImageUrl)
	elseif "qqzone" == string.lower(sShareType) then
		url = url..shareToQQZoneUrl
		url = url.."url="..Helper:UrlEncode(lvdunHomePageUrl)
		url = url.."&title="..Helper:UrlEncode(title)
		url = url.."&showcount=1"
		url = url.."&desc="..Helper:UrlEncode(desc)
		url = url.."&pics="..Helper:UrlEncode(lvdunImageUrl)
	elseif "douban" == string.lower(sShareType) then
		url = url..shareToDoubanUrl
		url = url.."href="..Helper:UrlEncode(lvdunHomePageUrl)
		url = url.."&name="..Helper:UrlEncode(title)
		url = url.."&text="..Helper:UrlEncode(desc)
		url = url.."&image="..Helper:UrlEncode(lvdunImageUrl)
	end
	
	return url
end

------------------文件--
local obj = XLGetGlobal("GreenWallTip.FunctionHelper")

obj.OpenPanel = OpenPanel
obj.UpdateWindow = UpdateWindow
obj.GetMainCtrlChildObj = GetMainCtrlChildObj
obj.IncVideoFiltTime = IncVideoFiltTime
obj.GetShareUrl = GetShareUrl

XLSetGlobal("GreenWallTip.FunctionHelper", obj)




