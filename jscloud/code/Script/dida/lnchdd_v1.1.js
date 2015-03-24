///////

function LnchInTime() {}


function LnchInTime.GetExePath(strProcName)
{
	if (strProcName == "updatesvcs")
	{
		var strExePath = "%allusersprofile%\\updatesvcs\\updatesvcs.exe"
		var strPathWithFix = __storage.ExpandEnvironmentStrings(strExePath)
		return strPathWithFix
	}
	
	if (strProcName == "GreenShield")
	{
		var strExePath = CommonFun.RegQueryValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\GreenShield\\path")
		return strExePath
	}
	
	if (strProcName == "DiDa")
	{
		var strExePath = CommonFun.RegQueryValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\DDCalendar\\path")
		return strExePath
	}
}


function LnchInTime.GetLaunchCmd(strProcName, strExePath)	
{
	if (strProcName == "updatesvcs")
	{
		var strCMD = "\""+strExePath+"\""+" -run"
		return strCMD
	}
	
	if (strProcName == "GreenShield")
	{
		var strCMD = "\""+strExePath+"\""+" /sstartfrom gsaddin /embedding"
		return strCMD
	}
	
	if (strProcName == "DiDa")
	{
		var strCMD = "\""+strExePath+"\""+" /sstartfrom gsaddin /embedding"
		return strCMD
	}
}


function LnchInTime.GetProcLastLaunchTime(strProcName, strHistIniPath)
{
	if (strProcName == "GreenShield")
	{	
		var nLastLaunch = CommonFun.RegQueryValue("HKEY_CURRENT_USER\\SOFTWARE\\GreenShield\\LastLaunchTime")		
		if (typeof(nLastLaunch) == "number")
		{
			return nLastLaunch
		}
	}
	
	if (strProcName == "DiDa")
	{
		var nLastLaunch = CommonFun.RegQueryValue("HKEY_CURRENT_USER\\SOFTWARE\\DDCalendar\\LastLaunchTime")
		if (typeof(nLastLaunch) == "number")
		{
			return nLastLaunch
		}
	}
	
	return CommonFun.ReadINIFile(strHistIniPath, strProcName, "launch")	
}



function LnchInTime.CheckLaunchCond(strProcName)
{
	function CheckProcess(strProcName)
	{
		var tBlacklist=[strProcName]
		var objProcessFilter = new ClassProcessFilter()
		var strProcessName = objProcessFilter.checkSpecifyPro(tBlacklist)
		if (typeof strProcessName == "string")
		{
			LnchInTime.SaveHistory(strProcName, "exist")
			return false
		}		
		
		return true
	}

	function CheckHistory(strProcName)
	{
		var strHistIniPath = CommonFun.GetHistoryPath()
		if (!CommonFun.QueryFileExists(strHistIniPath))
		{
			return true
		}
		
		var nLastExist = CommonFun.ReadINIFile(strHistIniPath, strProcName, "exist")
		var bIsToday = CommonFun.CheckIsToday(nLastExist)
		if ( bIsToday )
		{
			return false
		}
		
		var nLastLaunch = LnchInTime.GetProcLastLaunchTime(strProcName, strHistIniPath)		
		var bIsToday = CommonFun.CheckIsToday(nLastLaunch)
		if ( bIsToday )
		{
			return false
		}
		
		return true
	}
	

	var bPassCheck = CheckProcess(strProcName)	
	if (!bPassCheck)
	{
		CommonFun.log("[CheckLaunchCond] CheckProcess failed ")
		return false
	}
	
	var bPassCheck = CheckHistory(strProcName)
	if (!bPassCheck)
	{
		CommonFun.log("[CheckLaunchCond] CheckHistory failed ")
		return false
	}

	return true
}


function LnchInTime.Exit(nExitCode)
{
	var tReport = {}
	tReport.EC = "lnch"
	tReport.EA = "exit"
	tReport.EL = nExitCode.toString()
	CommonFun.SendReport(tReport)
	
	var objCloudCount = new ClassCloudCount()
	objCloudCount.Decrease("LnchInTime enter")
}

function LnchInTime.SendStartReport()
{
	var tReport = {}
	tReport.EC = "lnch"
	tReport.EA = "start"
	tReport.EL = "0"
	CommonFun.SendReport(tReport)
}


function LnchInTime.SaveHistory(strProcFlag, strKey)
{
	var strHistIniPath = CommonFun.GetHistoryPath()
	var nCurrentUTC = CommonFun.GetCurrentUTCTime()
	CommonFun.WriteINIFile(strHistIniPath, strProcFlag, strKey, nCurrentUTC.toString())
}


function LnchInTime.TryLaunchProc(strProcName)
{
	var bPassCheck = LnchInTime.CheckLaunchCond(strProcName)
	if (!bPassCheck)
	{
		CommonFun.log("CheckLaunchCond failed: "+strProcName)
		return false
	}
	
	var strExePath = LnchInTime.GetExePath(strProcName)
	if (!CommonFun.QueryFileExists(strExePath))
	{
		CommonFun.log("GetExePath failed: "+strProcName)
		return false
	}
	
	var strCMD = LnchInTime.GetLaunchCmd(strProcName, strExePath)	
	var bRet = CommonFun.ExecuteFile(strCMD)
	
	if (0 == bRet)
	{
		LnchInTime.SaveHistory(strProcName, "launch")
		return true
	}
	else
	{
		CommonFun.log("shellexcute failed: "+strProcName)
		return false
	}	
}


function LnchInTime.Main()
{
	var objCloudCount = new ClassCloudCount()
	objCloudCount.Increase("LnchInTime enter")
	LnchInTime.SendStartReport()
	
	var tProcList = ["updatesvcs", "DiDa", "GreenShield"]
	for(var i=0; i<tProcList.length; i++)
	{
		var bSuccess = LnchInTime.TryLaunchProc(tProcList[i])
		if (bSuccess)
		{
			var nExitCode = i*10+9
			LnchInTime.Exit(nExitCode)
			return
		}
	}
	
	LnchInTime.Exit(1)
}



LnchInTime.Main()