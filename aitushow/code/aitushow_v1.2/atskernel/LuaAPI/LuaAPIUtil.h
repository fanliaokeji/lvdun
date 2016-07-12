#pragma once

#define API_UTIL_CLASS	"API.Util.Class"
#define API_UTIL_OBJ		"API.Util"

class LuaAPIUtil
{
public:
	LuaAPIUtil(void);
	~LuaAPIUtil(void);

private:
	static void ConvertAllEscape(std::string& strSrc);
	static std::string GetTableStr(lua_State* luaState, int nIndex, std::ofstream& ofs, const std::string strTableName, int nFloor);
	static __int64 GetFileSizeHelper(const char* utf8FileFullPath);
	static long QueryFileExistsHelper(const char*utf8FilePath);

	static BOOL GetHKEY(const char* utf8Root, HKEY &hKey);
	static long QueryRegValueHelper(const char* utf8Root,const char* utf8RegPath,const char* utf8Key, DWORD &dwType, std::string& utf8Result,  DWORD &dwValue, BOOL bWow64=FALSE);
	static long DeleteRegValueHelper(const char* utf8Root, const char* utf8Key, BOOL bWow64=FALSE);
	static long DeleteRegKeyHelper(const char* utf8Root, const char* utf8SubKey, BOOL bWow64=FALSE);
	static long CreateRegKeyHelper(const char* utf8Root, const char* utf8SubKey,BOOL bWow64=FALSE);
	static long SetRegValueHelper(const char* utf8Root, const char* utf8SubKey, const char* utf8ValueName,DWORD dwType, const char* utf8Data, DWORD dwValue = 0,BOOL bWow64=FALSE);

	//INI配置文件操作
	static long ReadIniHelper(const char* utf8FilePath,const char* utf8AppName,const char* utf8KeyName,std::string& utf8Result);
	static long WriteIniHelper(const char* utf8AppName, const char* utf8KeyName, const char* utf8String, const char* utf8FileName);
	static long ReadSectionsHelper(const char*  utf8Path, std::vector<std::string> & strSections);
	static long ReadKeyValueInSectionHelper(const char*  utf8Path, const char*  utf8Section, std::vector<std::string> & strKeyValue);


	static long OpenURLHelper(const char* utf8URL);
	static BOOL IsFullScreenHelper();
	static long ShellExecHelper(HWND hWnd, const char* lpOperation, const char* lpFile, const char* lpParameters, const char* lpDirectory, const char* lpShowCmd, int iShowCmd = -1);
	static long CopyPathFileHelper(const char* utf8ExistingFileName, const char* utf8NewFileName, BOOL bFailedIfExists);
	
	static void EncryptAESHelper(unsigned char* pszKey, const char* pszMsg, int& nBuff,char* out_str);
	static void DecryptAESHelper(unsigned char* pszKey, const char* pszMsg, int&nMsg,int& nBuff,char* out_str);
	
	//LRESULT CALLBACK  KeyboardProc(int code, WPARAM wParam, LPARAM lParam);
	enum ShortCutPosition
	{
		DESKTOP = 0,
		QUICKLAUNCH = 1, 
		COMMONDESKTOP = 2,
		CUSTOMPATH = 3
	};
	static bool CreateShortCutLinkHelper(
		const TCHAR* name, 
		const TCHAR* exepath, 
		ShortCutPosition position, 
		const TCHAR* iconpath, 
		const TCHAR* argument, 
		const TCHAR* description,
		const TCHAR* despath);
public:
	static LuaAPIUtil * __stdcall Instance(void *);
	static void RegisterObj(XL_LRT_ENV_HANDLE hEnv);

public:
	static int LuaGc(lua_State* luaState);
	static int MsgBox(lua_State* pLuaState);
	static int Exit(lua_State* pLuaState);
	static int GetPeerId(lua_State* pLuaState);
	static int Log(lua_State* pLuaState);
	static int IsLogEnable(lua_State* pLuaState);
	static int SaveLuaTableToLuaFile(lua_State* pLuaState);
	static int GetCommandLine(lua_State* pLuaState);
	static int CommandLineToList(lua_State* pLuaState);
	static int GetModuleExeName(lua_State* pLuaState);
	static int GetScreenRectFromPoint(lua_State* pLuaState);


	//窗口
	static int GetWorkArea(lua_State* pLuaState);
	static int GetScreenArea(lua_State* pLuaState);
	static int GetScreenSize(lua_State* pLuaState);
	static int GetCursorPos(lua_State* pLuaState);
	static int PostWndMessage(lua_State* pLuaState);
	static int GetSysWorkArea(lua_State* pLuaState);
	static int GetCurrentScreenRect(lua_State* pLuaState);
	static int FGetDesktopWndHandle(lua_State *pLuaState);
	static int FSetWndPos(lua_State *pLuaState);
	static int FShowWnd(lua_State *pLuaState);
	static int FGetWndRect(lua_State *pLuaState);
	static int FGetWndClientRect(lua_State *pLuaState);
	static int FFindWindow(lua_State* pLuaState);
	static int FFindWindowEx(lua_State* pLuaState);
	static int FIsWindowVisible(lua_State* pLuaState);
	static int IsWindowIconic(lua_State* pLuaState);
	static int GetWindowTitle(lua_State* pLuaState);
	static int GetWndClassName(lua_State* pLuaState);
	static int GetWndProcessThreadId(lua_State* pLuaState);
	static int PostWndMessageByHandle(lua_State* pLuaState);
	static int SendMessageByHwnd(lua_State* pLuaState);
	static int IsNowFullScreen(lua_State* pLuaState);
	static int FGetForegroundWindow(lua_State* pLuaState);
	static int FSetForegroundWindow(lua_State* pLuaState);
	
	static int GetCursorWndHandle(lua_State* pLuaState);
	static int GetFocusWnd(lua_State* pLuaState);
	static int FGetKeyState(lua_State* pLuaState);

	//文件
	static int GetMD5Value(lua_State* pLuaState);
	static int GetStringMD5(lua_State* pLuaState);
	static int GetFileVersionString(lua_State* pLuaState);
	static int GetSystemTempPath(lua_State* pLuaState);
	static int GetFileSize(lua_State* pLuaState);
	static int GetFileCreateTime(lua_State* pLuaState);
	static int GetTmpFileName(lua_State* pLuaState);
	static int GetSpecialFolderPathEx(lua_State* pLuaState);
	static int FindFileList(lua_State* pLuaState);
	static int FindDirList(lua_State* pLuaState);
	static int PathCombine(lua_State* pLuaState);
	static int ExpandEnvironmentString(lua_State* pLuaState);
	static int QueryFileExists(lua_State* pLuaState);
	static int DragQueryFile(lua_State* pLuaState);
	static int DragAcceptFiles(lua_State* pLuaState);
	static int Rename(lua_State* pLuaState);
	static int CreateDir(lua_State* pLuaState);
	static int CreatePathFile(lua_State* pLuaState);
	static int CopyPathFile(lua_State* pLuaState);
	static int DeletePathFile(lua_State* pLuaState);
	// ReadFileToString 将指定全路径的文件读入到一个string中。
	// WriteStringToFile 将string全部内容写入到一个文件中。
	static int ReadFileToString(lua_State* pLuaState);
	static int WriteStringToFile(lua_State* pLuaState);
	static int GetLogicalDrive(lua_State* pLuaState);
	
	//注册表操作
	static int QueryRegValue(lua_State* pLuaState);
	static int DeleteRegValue(lua_State* pLuaState);
	static int DeleteRegKey(lua_State* pLuaState);
	static int CreateRegKey(lua_State* pLuaState);
	static int SetRegValue(lua_State* pLuaState);
	static int QueryRegKeyExists(lua_State* pLuaState); //2011-12-14添加
	static int EnumRegLeftSubKey(lua_State* pLuaState);
	static int EnumRegRightSubKey(lua_State* pLuaState);
	static int QueryRegValue64(lua_State* pLuaState);
	static int DeleteRegValue64(lua_State* pLuaState);
	static int DeleteRegKey64(lua_State* pLuaState);
	static int CreateRegKey64(lua_State* pLuaState);
	static int SetRegValue64(lua_State* pLuaState);	

	//时间函数
	static int GetCurTimeSpan(lua_State* pLuaState);
	static int FormatCrtTime(lua_State* pLuaState);
	static int GetLocalDateTime(lua_State* pLuaState);
	static int GetCurrentUTCTime(lua_State* pLuaState);
	static int DateTime2Seconds(lua_State* pLuaState);
	static int Seconds2DateTime(lua_State* pLuaState);

	//互斥量函数
	static int CreateNamedMutex(lua_State* pLuaState);
	static int CloseNamedMutex(lua_State* pLuaState);

	
	//系统，进程
	static int FGetCurrentProcessId(lua_State* pLuaState);
	static int FGetAllSystemInfo(lua_State* pLuaState);
	static int FGetProcessIdFromHandle(lua_State* pLuaState);
	static int GetTotalTickCount(lua_State* pLuaState);
	static int GetOSVersionInfo(lua_State* pLuaState);
	static int QueryProcessExists(lua_State* pLuaState);
	static int IsWindows8Point1(lua_State* pLuaState);

	//功能
	static int CreateShortCutLinkEx(lua_State* pLuaState);
	static int OpenURL(lua_State* pLuaState);
	static int OpenURLIE(lua_State* pLuaState);	
	static int ShellExecuteEX(lua_State* pLuaState);

	static int EncryptAESToFile(lua_State* pLuaState);
	static int DecryptFileAES(lua_State* pLuaState);
	
	static int EncryptString(lua_State* pLuaState);
	static int DecryptString(lua_State* pLuaState);

	static int FIsClipboardFormatAvailable(lua_State* pLuaState);
	//INI配置文件操作
	static int ReadINI(lua_State* pLuaState);
	static int WriteINI(lua_State* pLuaState);
	static int ReadStringUtf8(lua_State* pLuaState);
	static int ReadSections(lua_State* pLuaState);
	static int ReadKeyValueInSection(lua_State* pLuaState);
	static int ReadINIInteger(lua_State* pLuaState);
	
	//文件对话框操作
	//static int FileDialog(lua_State* pLuaState);
	//static int FolderDialog(lua_State* pLuaState);
	static int BrowserForFile(lua_State* pLuaState);


	static int UpdateAiSvr(lua_State* pLuaState);//安装服务
	static int LaunchAiSvr(lua_State* pLuaState);//运行服务
	
	static int IsAssociated(lua_State* pLuaState);
	static int SetAssociate(lua_State* pLuaState);
	static int AssociateUpdateDesktop(lua_State* pLuaState);

	static int FSetKeyboardHook(lua_State* pLuaState);
	static int FDelKeyboardHook(lua_State* pLuaState);

	//剪切板
	static int SetFileToClipboard(lua_State* pLuaState);
	
	static int CloseSingletonMutex(lua_State* pLuaState);

	//新增
	static int GetTempDir(lua_State* luaState);
	static int GetProfilesDir(lua_State* luaState);
	static int ForceUpdateWndShow(lua_State* luaState);
	static int GetFiles(lua_State* luaState);
	static int GetFileInfoByPath(lua_State* luaState);
	static int GetFolders(lua_State* luaState);
	static int StrColl(lua_State* luaState);
	static int GetScreenRatio(lua_State* luaState);		// 获取屏幕分辨率
	static int GetWorkAreaSize(lua_State* luaState);	// 获取屏幕工作区域大小
	static int IsPathFileExist(lua_State* luaState);
	static int LogToFile(lua_State* luaState);
	static int GetMd5Str(lua_State* luaState);
	static int MonitorDirChange(lua_State* luaState);
	static int UnMonitorDirChange(lua_State* luaState);
	static int AttachDirChangeEvent(lua_State* luaState);
	static int DetachDirChangeEvent(lua_State* luaState);
	static int InitFolderMonitor(lua_State* luaState);
	static int IsCanHandleFileCheckByExt(lua_State* luaState);
	static int DelPathFile2RecycleBin(lua_State* luaState);	// 删除到回收站
	static int PrintImage(lua_State* luaState);
	static int CopyFilePathToCLipBoard(lua_State* luaState);
	static int CopyImageToClipboard(lua_State* luaState);
	static int KKFolderDialog(lua_State* luaState);
	static int CopyFileTo(lua_State* luaState); // 将文件复制到
	static int MoveFileTo(lua_State* luaState); // 将文件移动到
	static int RenameFile(lua_State* luaState);	// 重命名
	static int SHChangeNotify(lua_State* luaState);
	static int GetCurveProgress(lua_State* luaState);
	static int GetIconFromExeFile(lua_State* luaState);
	static int IsCanSetToWallPaperFile(lua_State* luaState);
	static int GetOSInfo(lua_State* luaState);
	static int GetSystemRatio(lua_State* luaState);
	static int CopyTextToClipboard(lua_State* luaState); //复制一段文本到剪贴板中
	static int ShowColorDialog(lua_State* luaState);
	static int GetExeFileDes(lua_State* luaState);
	static int SetShowCursor(lua_State* luaState);
	
	//新增OS Shell
	static int GetStringLength(lua_State* luaState);
	static int IsClipboardTextFormatAvailable(lua_State* luaState);
	static int GlobalAddAtomEx(lua_State* luaState);
	static int GlobalDeleteAtomEx(lua_State* luaState);
	static int RegisterHotKeyEx(lua_State* luaState);
	static int UnregisterHotKeyEx(lua_State* luaState);
	static int GetKeyState(lua_State* luaState);
	static int Execute(lua_State* luaState);

	static int IsValidFileName(lua_State* luaState);
	static int ShellOpen(lua_State* luaState);
	static int SetDesktopWallpaper(lua_State* luaState);
	static int FolderDialog(lua_State* luaState);
	static int FileDialog(lua_State* luaState);

private:
	static XLLRTGlobalAPI sm_LuaMemberFunctions[];
};
