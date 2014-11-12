#pragma once

#define GS_UTIL_CLASS	"GS.Util.Class"
#define GS_UTIL_OBJ		"GS.Util"

class LuaGSUtil
{
public:
	LuaGSUtil(void);
	~LuaGSUtil(void);

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

	//INI�����ļ�����
	static long ReadIniHelper(const char* utf8FilePath,const char* utf8AppName,const char* utf8KeyName,std::string& utf8Result);
	static long WriteIniHelper(const char* utf8AppName, const char* utf8KeyName, const char* utf8String, const char* utf8FileName);
	static long ReadSectionsHelper(const char*  utf8Path, std::vector<std::string> & strSections);
	static long ReadKeyValueInSectionHelper(const char*  utf8Path, const char*  utf8Section, std::vector<std::string> & strKeyValue);


	static long OpenURLHelper(const char* utf8URL);
	static BOOL IsFullScreenHelper();
	static long ShellExecHelper(HWND hWnd, const char* lpOperation, const char* lpFile, const char* lpParameters, const char* lpDirectory, const char* lpShowCmd, int iShowCmd = -1);
	static long CopyPathFileHelper(const char* utf8ExistingFileName, const char* utf8NewFileName, BOOL bFailedIfExists);
	
	static void EncryptAESToFileHelper(const unsigned char* pszKey, const char* pszMsg, unsigned char* out_str, int& nlen);
	static void DecryptFileAESHelper(const unsigned char* pszKey, const unsigned char* pszMsg, int nlen, unsigned char* out_str);

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
	static LuaGSUtil * __stdcall Instance(void *);
	static void RegisterObj(XL_LRT_ENV_HANDLE hEnv);

public:
	//static int RegisterFilterWnd(lua_State* pLuaState);
	static int MsgBox(lua_State* pLuaState);
	static int LoadWebRules(lua_State* pLuaState);
	static int LoadVideoRules(lua_State* pLuaState);
	static int LoadUserRules(lua_State* pLuaState);

	static int AddVideoHost(lua_State* pLuaState);
	static int AddWhiteHost(lua_State* pLuaState);
	
	static int UpdateVideoHost(lua_State* pLuaState);
	static int UpdateWhiteHost(lua_State* pLuaState);

	static int FGSFilter(lua_State* pLuaState);

	static int Exit(lua_State* pLuaState);
	static int GetPeerId(lua_State* pLuaState);
	static int Log(lua_State* pLuaState);
	static int SaveLuaTableToLuaFile(lua_State* pLuaState);
	static int GetCommandLine(lua_State* pLuaState);
	static int CommandLineToList(lua_State* pLuaState);


	//����
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

	//�ļ�
	static int GetMD5Value(lua_State* pLuaState);
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
	static int Rename(lua_State* pLuaState);
	static int CreateDir(lua_State* pLuaState);
	static int CopyPathFile(lua_State* pLuaState);
	static int DeletePathFile(lua_State* pLuaState);
	// ReadFileToString ��ָ��ȫ·�����ļ����뵽һ��string�С�
	// WriteStringToFile ��stringȫ������д�뵽һ���ļ��С�
	static int ReadFileToString(lua_State* pLuaState);
	static int WriteStringToFile(lua_State* pLuaState);

	
	//ע������
	static int QueryRegValue(lua_State* pLuaState);
	static int DeleteRegValue(lua_State* pLuaState);
	static int DeleteRegKey(lua_State* pLuaState);
	static int CreateRegKey(lua_State* pLuaState);
	static int SetRegValue(lua_State* pLuaState);
	static int QueryRegKeyExists(lua_State* pLuaState); //2011-12-14���
	static int EnumRegLeftSubKey(lua_State* pLuaState);
	static int EnumRegRightSubKey(lua_State* pLuaState);
	static int QueryRegValue64(lua_State* pLuaState);
	static int DeleteRegValue64(lua_State* pLuaState);
	static int DeleteRegKey64(lua_State* pLuaState);
	static int CreateRegKey64(lua_State* pLuaState);
	static int SetRegValue64(lua_State* pLuaState);	

	//ʱ�亯��
	static int GetCurTimeSpan(lua_State* pLuaState);
	static int FormatCrtTime(lua_State* pLuaState);
	static int GetLocalDateTime(lua_State* pLuaState);
	static int GetCurrentUTCTime(lua_State* pLuaState);
	static int DateTime2Seconds(lua_State* pLuaState);
	static int Seconds2DateTime(lua_State* pLuaState);

	//����������
	static int CreateNamedMutex(lua_State* pLuaState);
	static int CloseNamedMutex(lua_State* pLuaState);

	
	//ϵͳ������
	static int FGetCurrentProcessId(lua_State* pLuaState);
	static int FGetAllSystemInfo(lua_State* pLuaState);
	static int FGetProcessIdFromHandle(lua_State* pLuaState);
	static int GetTotalTickCount(lua_State* pLuaState);
	static int GetOSVersionInfo(lua_State* pLuaState);
	static int QueryProcessExists(lua_State* pLuaState);
	static int IsWindows8Point1(lua_State* pLuaState);

	//����
	static int CreateShortCutLinkEx(lua_State* pLuaState);
	static int OpenURL(lua_State* pLuaState);
	static int OpenURLIE(lua_State* pLuaState);	
	static int ShellExecuteEX(lua_State* pLuaState);

	static int EncryptAESToFile(lua_State* pLuaState);
	static int DecryptFileAES(lua_State* pLuaState);

	//INI�����ļ�����
	static int ReadINI(lua_State* pLuaState);
	static int WriteINI(lua_State* pLuaState);
	static int ReadStringUtf8(lua_State* pLuaState);
	static int ReadSections(lua_State* pLuaState);
	static int ReadKeyValueInSection(lua_State* pLuaState);
	static int ReadINIInteger(lua_State* pLuaState);
	

private:
	static XLLRTGlobalAPI sm_LuaMemberFunctions[];
};
