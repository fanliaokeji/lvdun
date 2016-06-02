#pragma once
#include <strsafe.h>
#include "freeimage/FreeImage.h"

#define SE_GROUP_INTEGRITY                 (0x00000020L)
#define TokenIntegrityLevel					25
class CImageUtility
{
public:
	typedef struct _TOKEN_MANDATORY_LABEL {
		SID_AND_ATTRIBUTES Label;
	} TOKEN_MANDATORY_LABEL, *PTOKEN_MANDATORY_LABEL;

	CImageUtility(void);
	~CImageUtility(void);

	static bool DeleteDir(const std::wstring& wstrTempDirect);
	static bool IsCanHandleFileCheckByExt(const std::wstring& wstrTempDirect);	// 检查是不是可以处理的文件
	static bool IsCanSetToWallPaperFile(const std::wstring& wstrTempDirect);	// 检查是不是可以设置为说面背景的文件
	static bool IsCanBatchRotateFile(const std::wstring& wstrTempDirect);	// 检查是不是可以做批量旋转的文件
	static bool IsCanSuperBatchFile(const std::wstring& wstrTempDirect);	// 检查是不是可以做高级批处理
	static bool IsCanSaveToSameTypeFile(const std::wstring& wstrTempDirect);	// 检查是不是可以保存为相同格式的文件
	static BOOL IsLegalPath(std::wstring& wstrPathName); //判断一个路径是否是合法的系统路径
	static BOOL GetOSInfo(std::wstring &strOSDesc, std::wstring &strOSVersion);	// 获取操作系统版本
	static wstring LowerStr(wstring& wstrSrcStr);
	static BOOL CreateMediumIntegrityProcess(PCTSTR pszApplicationName, PTSTR pszCommandLine, PPROCESS_INFORMATION pPI,  BOOL bShowWnd = false);
	static BOOL PrintImage(wchar_t* path,wchar_t* file_name);
	static wstring GetFileDesInfo(wstring& wstrFilePath);
	static wstring GetDisplayMonitorICCFilePath();
	static BOOL IS_Vista_Or_More();

private:
	static BOOL GetDisplayMonitorICC( wchar_t* full_path, int char_count );
	//
};

//替换一个字符串中所有的某个子串为新的值
// str:原字符串
// old_value: 被替换的值
// new_value: 新的值
wstring& replace_all_distinct(wstring& str, const wstring& old_value, const wstring& new_value);
