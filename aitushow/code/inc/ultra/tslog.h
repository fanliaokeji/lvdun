/************************************************************************************************************************************
created:	2008/03/13
created:	13:3:2008   10:56
filename: 	tslog.h
file path:	tslog
file base:	tslog
file ext:	h
author:		张伟文

purpose:	TSLOG日志系统
eg.
配置文件格式:
TSLOG.ini

/////////////////////////TSLOG.ini/////////////////////////////
;[Output]
;DebugView                  =ON     ;ON 打开 OFF 关闭                                                           默认关
;FileLog                    =ON     ;ON 打开 OFF 关闭                                                           默认关

;[Common]
;Level                      = TRACE   ; 共有TRACE,DEBUG,INFO,WARN,ERROR,FATAL                                   DEBUG
;Include                    = *       ; *表示任意 打印包括*字符的日志                                           *		
;Exclude                    = a;b     ;           打印不包括"a" 和 "b" 字符的日志用";"分隔	

;[FileLogOption]
;LogFilePath                = C:\TSLOG\  ;日志存放目录                                                           C:\tslog		
;MaxLogFileSize             = 20480     ;日志大小 20480KB                                                       20480	
;MaxLogFileCnt              = 20        ;当前进程日志文件最多数目                                               20
;MaxCountOfHistroyProcess   = 10        ;只保留最新 10 个活动进程的所有日志                                     10

[Output]
DebugView                   =ON
FileLog                     =ON

[Common]
Level                       = TRACE 
Include                     = *
Exclude                     =   <DapCtrl>

[FileLogOption]
LogFilePath                 = C:\TSLOG\
MaxLogFileSize              = 2048
MaxLogFileCnt               = 20
MaxCountOfHistroyProcess    = 5  
/////////////////////////TSLOG.ini/////////////////////////////

/////////////////////////program////////////////////////////////////////////////////////////////////////////////////////////////////
//日志系统将增加模块堆开销 : 
//4096 + 512 + 2048 + 4096 + 65*4 + (4096+512)*sizeof(TCHAR) + 3*260*sizeof(TCHAR) 
// = 11K + 5K * sizeof(TCHAR) 
// = 16K (def _MBCS)
//   | 21K(def _UNICODE)
//TSLOG_VERSION	0x00010027 : 
// 性能: TSLOG不打印         300K次 TRACEAUTO 10942ms	SIMPLE : 3484
// 性能: TSLOG只打印文件日志 300K次 TRACEAUTO 15000ms	SIMPLE : 6500 77M
// 性能: TSLOG只打DebugView  300K次 TRACEAUTO 18500ms	SIMPLE : 9700	
// 性能: TSLOG都打           300K次 TRACEAUTO 23790ms 127M  SIMPLE : 13400 77M

#define TSLOG                                      //必选宏,不声明将会把所有日志功能取消
#define TSLOG_GROUP "TSLOG"                        //可选,默认为 "TSLOG"
#include "tslog.h"                                 //如上配置,日志程序将根据 C:\TSLOG_CONFIG\TSLOG.ini 定义的策略打印

int _tmain(int argc, _TCHAR* argv[])
{	
	TSTRACEAUTO();                             //函数入口处调用此调数,可自动打印出函数参数
	if(INSTALLTSLOGINI("E:\\Program Files\\Thunder Network\\Thunder\\Program\\TSLOG.ini"))
	{
		//成功复制配置文件至  TSLOG_CONFIG_DIR 目录下,即开启应用日志配置,一般只运行一次,如在注册COM件函数里调用 (DllRegisterServer)
	}	
	TSDEBUG(_T("打印各种级别日志,分别可以写成%s,%s,%s,%s"),_T("TSTRACE"),_T("TSWARN"), _T("TSERROR"), _T("TSFATAL"));
	TSFATAL4CXX(123123<<"C++风格打,可以使用这类函数")
} 
///////////////////////program///////////////////////////////////////////////////////////////////////////////////////////////////////

*************************************************************************************************************************************/
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef __TSLOG_H_2008_03_13
#define __TSLOG_H_2008_03_13
/********************************************************************************************************
	tslog大小不变!	定义版本,每次更改加1,版本号用来判断预留空间的使用情况
********************************************************************************************************/
#define TSLOG_VERSION	0x00026100 //@@@@version
//兼容LOG4C
#define LOG4C_TRACE		TSTRACE
#define LOG4C_DEBUG		TSDEBUG
#define LOG4C_INFO		TSINFO
#define LOG4C_WARN		TSWARN
#define LOG4C_ERROR		TSERROR
#define LOG4C_FATAL		TSFATAL
//V2.0 兼容LOG4CPLUS
#define LOG4CPLUS_INIT(param)
#define LOG4CPLUS_DECLARE(logger)
#define LOG4CPLUS_IMPLEMENT(logger, name)
#define LOG4CPLUS_CLASS_DECLARE(logger)
#define LOG4CPLUS_CLASS_IMPLEMENT(classname, logger, name)

#define LOG4CPLUS_TRACE _TSTRACE4CXX
#define LOG4CPLUS_DEBUG _TSDEBUG4CXX
#define LOG4CPLUS_INFO _TSINFO4CXX
#define LOG4CPLUS_WARN _TSWARN4CXX
#define LOG4CPLUS_ERROR _TSERROR4CXX
#define LOG4CPLUS_FATAL _TSFATAL4CXX

#define LOG4CPLUS_THIS_TRACE _TSTRACE4CXX
#define LOG4CPLUS_THIS_DEBUG _TSDEBUG4CXX
#define LOG4CPLUS_THIS_INFO _TSINFO4CXX
#define LOG4CPLUS_THIS_WARN _TSWARN4CXX
#define LOG4CPLUS_THIS_ERROR _TSERROR4CXX
#define LOG4CPLUS_THIS_FATAL _TSFATAL4CXX

#define LOG4CPLUS_TRACE_STR LOG4CPLUS_TRACE
#define LOG4CPLUS_TRACE_METHOD LOG4CPLUS_TRACE
#define LOG4CPLUS_DEBUG_STR LOG4CPLUS_DEBUG
#define LOG4CPLUS_DEBUG_METHOD LOG4CPLUS_DEBUG
#define LOG4CPLUS_INFO_STR LOG4CPLUS_INFO
#define LOG4CPLUS_INFO_METHOD LOG4CPLUS_INFO
#define LOG4CPLUS_WARN_STR LOG4CPLUS_WARN
#define LOG4CPLUS_WARN_METHOD LOG4CPLUS_WARN
#define LOG4CPLUS_ERROR_STR LOG4CPLUS_ERROR
#define LOG4CPLUS_ERROR_METHOD LOG4CPLUS_ERROR
#define LOG4CPLUS_FATAL_STR LOG4CPLUS_FATAL
#define LOG4CPLUS_FATAL_METHOD LOG4CPLUS_FATAL
#define TS4CXX TSDEBUG4CXX
#define LOG4CXX TSDEBUG4CXX
#define TS4CXX2 TSDEBUG4CXX2
#define LOG4CXX2 TSDEBUG4CXX2
//特殊函数别名
#define TSINSTALL INSTALLTSLOGINI	//安装日志配置文件
#define TSLOGINSTALL INSTALLTSLOGINI
#define TSAUTO TSTRACEAUTO
#define TS  TSAUTO
#define TSAUTO2 TSTRACEAUTO2
#define TS2  TSAUTO2
#define TSDUMP TSHEXDUMP

#define TSISDEBUGVALID ISTSDEBUGVALID
#define TSDEBUGVALID ISTSDEBUGVALID

#define TSISINFOVALID ISTSINFOVALID
#define TSINFOVALID ISTSINFOVALID

#define TSISWARNVALID ISTSWARNVALID
#define TSWARNVALID ISTSWARNVALID

#define TSISERRORVALID ISTSERRORVALID
#define TSERRORVALID ISTSERRORVALID

#define TSISFATALVALID ISTSFATALVALID
#define TSFATALVALID ISTSFATALVALID

//特别宏
#if (defined(TSLOG) || defined(TSLOG_SIMPLE) || defined(TSLOG2) || defined(TSLOG_COMPLEX))  //编译选项
#define ISTSDEBUGVALID CTSLog::IsDebugValid
#define ISTSINFOVALID CTSLog::IsInfoValid
#define ISTSWARNVALID CTSLog::IsWarnValid
#define ISTSERRORVALID CTSLog::IsErrorValid
#define ISTSFATALVALID CTSLog::IsFatalValid
#else
#define ISTSDEBUGVALID() 1 ?  false : 0
#define ISTSINFOVALID()  1 ?  false : 0
#define ISTSWARNVALID()  1 ?  false : 0
#define ISTSERRORVALID()  1 ?  false : 0
#define ISTSFATALVALID()  1 ?  false : 0
#endif

//是否生效
#if (defined(TSLOG) || defined(TSLOG_SIMPLE) || defined(TSLOG2) || defined(TSLOG_COMPLEX))  //编译选项
#if _MSC_VER <1300	//VC.net以上才可以使用
# error Microsoft Visual C++.net or above is supported.
#endif
#ifndef	__cplusplus
# error Used for C++ programs only.
#endif
//警告使用非推荐宏
#if ((defined(TSLOG) || defined(TSLOG2) || defined(TSLOG_COMPLEX)) && !defined(_WIN32))
#ifndef TSLOG_SIMPLE
#define TSLOG_SIMPLE
#endif 
#pragma message("warning: define macro 'TSLOG', 'TSLOG2'/'TSLOG_COMPLEX', and undefine macro '_WIN32', default : [#define TSLOG_SIMPLE]")
#endif
//TSLOG2 === TSLOG_COMPLEX
#if (defined(TSLOG2) && !defined(TSLOG_COMPLEX))
#define TSLOG_COMPLEX
#endif
//设置优先级 TSLOG_SIMPLE > TSLOG > TSLOG_COMPLEX
#if (defined(TSLOG_COMPLEX) && defined(TSLOG_SIMPLE))
#undef TSLOG_COMPLEX
#pragma message("warning: define macro 'TSLOG_SIMPLE' and 'TSLOG2'/'TSLOG_COMPLEX', so #undef TSLOG2 or #undef TSLOG_COMPLEX, default : [#define TSLOG_SIMPLE]")
#endif
#if (defined(TSLOG_COMPLEX) && defined(TSLOG))
#pragma message("warning: define macro 'TSLOG' and 'TSLOG2'/'TSLOG_COMPLEX', so #undef TSLOG2 or #undef TSLOG_COMPLEX, default : [#define TSLOG]")
#undef TSLOG_COMPLEX
#endif
#if (defined(TSLOG) && defined(TSLOG_SIMPLE))
#undef TSLOG
#pragma message("warning: define macro 'TSLOG_SIMPLE' and 'TSLOG', so #undef TSLOG, default : [#define TSLOG_SIMPLE]")
#endif
//使用不推荐宏
#ifdef TSLOG_SIMPLE
#pragma message("warning: using : [#define TSLOG_SIMPLE]")
#endif


//兼容MFC ATL 的 TRACE
#ifdef TRACE
#undef TRACE
#endif
#define TRACE TSTRACE

#define TSLOG_GROUP_DEFAULT "TSLOG" 

//默认宏定义
#ifndef TSLOG_GROUP
#define TSLOG_GROUP "TSLOG"
#pragma message("warning: undefine macro 'TSLOG_GROUP', default : [#define TSLOG_GROUP \"" TSLOG_GROUP "\"]")
#endif
//默认配置目录
#ifndef TSLOG_CONFIG_DIR
#define TSLOG_CONFIG_DIR "C:\\TSLOG_CONFIG\\"
//#pragma message("warning: undefine macro 'TSLOG_CONFIG_DIR', default : [#define TSLOG_CONFIG_DIR \"" TSLOG_CONFIG_DIR "\"]")
#endif

//As long as shared memory can be no change in tslog
#include <tchar.h>	//_T
#include <windows.h>
#include <string>	//std::string strstr
#include <vector>	//std:vector
//#include <stdio.h>		//_vsntprintf()
#include <algorithm>	//sort
#include <time.h>		//time
#include  <sys/stat.h>	//stat
//#include <stdlib.h>		//_tsplitpath
#include <OAIdl.h>
#include <MLang.h> //codepage
#pragma comment(lib, "Version.lib") //VerQueryValue
#pragma comment(lib, "Winmm.lib") //timeGetTime

//Private Data
#define __TINI_SUFFIX _T(".ini")
#define __TTXT_SUFFIX _T(".txt")
#define __TTSLOG_CONFIG_FILE_PATH  __TTSLOG_CONFIG_DIR __TTSLOG_GROUP __TINI_SUFFIX
#define __TTSLOG_CONFIG_FILE_PATH2  __TTSLOG_CONFIG_DIR __TTSLOG_GROUP __TTXT_SUFFIX
#define __TTSLOG_CONFIG_FILE_PATH_DEFAULT  __TTSLOG_CONFIG_DIR __TTSLOG_GROUP_DEFAULT __TINI_SUFFIX
#define __TTSLOG_CONFIG_FILE_PATH_DEFAULT2  __TTSLOG_CONFIG_DIR __TTSLOG_GROUP_DEFAULT __TTXT_SUFFIX

#define MAX_USERDATA_SIZE		(4096 - 1)	//用户数据长
#define MAX_PRIVATEDATA_SIZE	256		//TSLOG内部数据
#define MAX_LOGFILE_INDEX		999		//日志文件名使用的索引
#define MAX_HEX_DUMP_SIZE  		512
//no config, default
#define DEFAULT_LOGFILE_PATH				"C:\\TSLOG\\"
#define __TDEFAULT_LOGFILE_PATH				_T(DEFAULT_LOGFILE_PATH)
#define DEFAULT_MAX_LOGFILE_SIZE			20480
#define MAX_MAX_LOGFILE_SIZE				(DEFAULT_MAX_LOGFILE_SIZE * 100)
#define MIN_MAX_LOGFILE_SIZE				1
#define DEFAULT_MAX_LOGFILE_COUNT			20
#define MAX_MAX_LOGFILE_COUNT				1000
#define MIN_MAX_LOGFILE_COUNT				1
#define DEFAULT_MAX_HISTROY_PROCESS_COUNT	10
#define MAX_MAX_HISTROY_PROCESS_COUNT		100
#define MIN_MAX_HISTROY_PROCESS_COUNT		-1
#define KBTYE								1024

#pragma pack(push,1)
class CTSLog
{
#ifndef EXCEPTION_EXECUTE_HANDLER
#define EXCEPTION_EXECUTE_HANDLER 1
#endif
#define OUTPUTLOG_FORLEVEL(XLevel, pszFormat)  SectionProtect sp(& _cs); va_list ap; va_start(ap, pszFormat);OutputLog(XLevel, pszFormat, ap);	va_end(ap);
public:
typedef enum
{
	LEVEL_TRACE,
	LEVEL_DEBUG,
	LEVEL_INFO,
	LEVEL_WARN,
	LEVEL_ERROR,
	LEVEL_FATAL,
	LEVEL_PROMPT, //打印提示信息, 内部使用
	LEVEL_OFF
}TLevel;
public:
	inline	void			TSTrace(LPCTSTR pszFormat, ...){ OUTPUTLOG_FORLEVEL(LEVEL_TRACE, pszFormat); }
	inline	void			TSDebug(LPCTSTR pszFormat, ...){ OUTPUTLOG_FORLEVEL(LEVEL_DEBUG, pszFormat) }
	inline	void			TSInfo (LPCTSTR pszFormat, ...){ OUTPUTLOG_FORLEVEL(LEVEL_INFO, pszFormat) }
	inline	void			TSWarn (LPCTSTR pszFormat, ...){ OUTPUTLOG_FORLEVEL(LEVEL_WARN, pszFormat) }
	inline	void			TSError(LPCTSTR pszFormat, ...){ OUTPUTLOG_FORLEVEL(LEVEL_ERROR, pszFormat) }
	inline	void			TSFatal(LPCTSTR pszFormat, ...){ OUTPUTLOG_FORLEVEL(LEVEL_FATAL, pszFormat) }
	inline	void			TSLog(LPCTSTR pszLogFilePath,LPCTSTR pszFormat, ...); //打印提示信息, 内部使用
	inline  void			TSHexDump(LPCVOID p, ULONG uSize, LPCTSTR pszPrompt = NULL, TLevel level = LEVEL_TRACE);
	inline  BOOL			InstallTSLogIni(LPCTSTR szSrcIniAbsolutePath);
//V2.0 重载<<
#define OVERFLOW_CHECK()  \
	if((DWORD)-1 == m_dwUsedSize) \
		m_dwUsedSize = m_dwMaxLogDataSize;		
	inline  CTSLog& 		operator << (const bool _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			if(_Val)
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, (LPCTSTR)_T("true"));
			else
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, (LPCTSTR)_T("false"));
			OVERFLOW_CHECK();
		}		
		return *this;
	}
	//字符
	inline  CTSLog& 		operator << (const TCHAR _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			if(isprint((int)_Val))
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%c"), _Val);
			else
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%d"), (INT)_Val);
			OVERFLOW_CHECK();
		}		
		return *this;
	}
#if (defined(UNICODE) || defined(_UNICODE))
	inline  CTSLog& 		operator << (const char _Val)
#else
	inline  CTSLog& 		operator << (const wchar_t  _Val)
#endif
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			if(isprint((int)_Val))
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%C"), _Val);
			else
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%d"), (INT)_Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const signed char _Val)
	{
		m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%u"), (INT)_Val);
		if((DWORD)-1 == m_dwUsedSize)
			m_dwUsedSize = m_dwMaxLogDataSize;
		return *this;
	}
	inline  CTSLog& 		operator << (const unsigned char _Val)
	{
		m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%u"), (UINT)_Val);
		if((DWORD)-1 == m_dwUsedSize)
			m_dwUsedSize = m_dwMaxLogDataSize;
		return *this;
	}
	//2字节整数
	inline  CTSLog& 		operator << (const signed short _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%hd"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
#if (defined _WCHAR_T_DEFINED && defined _NATIVE_WCHAR_T_DEFINED)
	//与 operator <<(WCHAR)冲突
	inline  CTSLog& 		operator << (const unsigned short _Val)
	{		
		m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%hu"), _Val);
		if((DWORD)-1 == m_dwUsedSize)
			m_dwUsedSize = m_dwMaxLogDataSize;
		return *this;
	}
#endif	 
	//四字节整数
	/*
//#if (defined _WIN64)
	inline  CTSLog& 		operator << (const  unsigned int _W64 _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%I64u"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
//#endif
	*/
	inline  CTSLog& 		operator << (const  signed int  _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%d"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}	
	inline  CTSLog& 		operator << (const unsigned int  _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%u"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const long  _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%ld"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const unsigned long  _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%lu"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const signed __int64  _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%I64d"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const  unsigned __int64 _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%I64u"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//float
	inline  CTSLog& 		operator << (const float _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%f"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//double
	inline  CTSLog& 		operator << (const double _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%f"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//long double
	inline  CTSLog& 		operator << (const long double _Val)  
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%lf"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const void * _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("0x%p"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//字符串
	inline  CTSLog& 		operator << (const TCHAR * _Val) throw()
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			__try
			{
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%s"), _Val);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				m_dwUsedSize +=_sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("\t(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x\t"), _Val, GetExceptionCode());
			}
			if((DWORD)-1 == m_dwUsedSize)
				m_dwUsedSize = (int)_tcslen(m_pszLogDataA);
			OVERFLOW_CHECK();
		}
		return *this;
	}
#if (defined(UNICODE) || defined(_UNICODE))
	inline  CTSLog& 		operator << (const char * _Val) throw()
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			WCHAR szBufferW[MAX_USERDATA_SIZE + 1] = {0};
			INT nLen = 0;
			UINT code_page = GetStringCodePage(_Val);
			__try
			{
				nLen = MultiByteToWideChar(code_page  ,0,_Val,-1,NULL,0);
				if(0 == nLen)
				{
					code_page = 0;
					nLen = MultiByteToWideChar(code_page  ,0,_Val,-1,NULL,0);
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{	
				nLen = 0;
				m_dwUsedSize +=_sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("\t(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x\t"), _Val, GetExceptionCode());				
				if((DWORD)-1 == m_dwUsedSize)
					m_dwUsedSize = (int)_tcslen(m_pszLogDataA);
				OVERFLOW_CHECK();
			}
			if(0 < nLen)
			{
				if(MAX_USERDATA_SIZE + 1 < nLen)
				{
					CHAR szLogDataW[MAX_USERDATA_SIZE + 1] = {0};
					memcpy(szLogDataW, _Val, MAX_USERDATA_SIZE * sizeof(CHAR));
					_Val = szLogDataW;
					nLen = MAX_USERDATA_SIZE + 1;
					nLen = MultiByteToWideChar(code_page, 0,_Val, -1, szBufferW,nLen);
					if(0 == nLen)
					{
						code_page = 0;
						nLen = MultiByteToWideChar(code_page, 0,_Val, -1, szBufferW,nLen);
					}
				}
				else
				{
					nLen = MultiByteToWideChar(code_page, 0,_Val, -1, szBufferW,nLen);
					if(0 == nLen)
					{
						code_page = 0;
						nLen = MultiByteToWideChar(code_page, 0,_Val, -1, szBufferW,nLen);
					}
				}
				if(0 < nLen)
				{
					__try
					{
						m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%s"), szBufferW);
					}
					__except(EXCEPTION_EXECUTE_HANDLER)
					{
						m_dwUsedSize +=_sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("\t(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x\t"), szBufferW, GetExceptionCode());
					}
					if((DWORD)-1 == m_dwUsedSize)
						m_dwUsedSize = (int)_tcslen(m_pszLogDataA);
					OVERFLOW_CHECK();
				}
			}
		}
		return *this;
	}
#else
	inline  CTSLog& 		operator << (const wchar_t * _Val) throw()
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			CHAR szBufferA[MAX_USERDATA_SIZE + 1] = {0};	//调用频繁
			INT nLen = 0;
			__try
			{
				nLen = WideCharToMultiByte(936, 0, _Val, -1,NULL, 0 ,NULL, NULL);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				nLen = 0;
				m_dwUsedSize +=_sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("\t(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x\t"), _Val, GetExceptionCode());
				if((DWORD)-1 == m_dwUsedSize)
					m_dwUsedSize = (int)_tcslen(m_pszLogDataA);
				OVERFLOW_CHECK();
			}
			if(0 < nLen)
			{
				if(MAX_USERDATA_SIZE + 1 < nLen)
				{
					WCHAR szLogDataW[MAX_USERDATA_SIZE + 1] = {0};
					memcpy(szLogDataW, _Val, MAX_USERDATA_SIZE * sizeof(WCHAR));
					_Val = szLogDataW;
					nLen = MAX_USERDATA_SIZE + 1;
					nLen = WideCharToMultiByte(936, 0, _Val, -1, szBufferA, nLen,NULL,NULL);
					if(0 == nLen)
					{

						nLen = WideCharToMultiByte(936, 0, _Val, -1, szBufferA, nLen,NULL,NULL);
					}
				}
				else
					nLen = WideCharToMultiByte(936, 0, _Val, -1, szBufferA, nLen,NULL,NULL);
				if(0 < nLen)
				{
					__try
					{
						m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%s"), szBufferA);
					}
					__except(EXCEPTION_EXECUTE_HANDLER)
					{
						m_dwUsedSize +=_sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("\t(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x\t"), szBufferA, GetExceptionCode());
					}
					if((DWORD)-1 == m_dwUsedSize)
						m_dwUsedSize = (int)_tcslen(m_pszLogDataA);
					OVERFLOW_CHECK();
				}
			}
		}
		return *this;
	}
#endif
	//IID
	inline  CTSLog& 		operator << (REFGUID _Val)  throw()
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
 			OLECHAR szGUID[40];
			HRESULT hr = S_OK;
			hr = StringFromGUID2(_Val, szGUID, 40);
			if(SUCCEEDED(hr))
				this->operator << (szGUID);
			else
			{
				this->operator << ("{StringFromIID ERROR}");                                
			}
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//tagVARIANT
	inline  CTSLog& 		operator << (const tagVARIANT _Val)  throw()
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			const tagVARIANT* pVar = &_Val;
			__try
			{
				switch (pVar->vt)
				{
				case VT_EMPTY:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{VT_EMPTY}"));
					break;
				case VT_NULL:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{VT_NULL}"));
					break;
				case VT_ERROR:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_ERROR}"), pVar->scode, pVar->scode);
					break;
				case VT_I1:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_I1}"), (INT)pVar->bVal, (INT)pVar->bVal);
					break;
				case VT_I2:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_I2}"), (INT)pVar->iVal, (INT)pVar->iVal);
					break;
				case VT_I4:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_I4}"), (INT)pVar->intVal, (INT)pVar->intVal);
					break;
				case VT_I8:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%I64x(%I64d) VT_I8}"), (ULONGLONG)pVar->ullVal, (LONGLONG)pVar->llVal);
					break;
				case VT_UI1:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%u) VT_UI1}"), (UINT)pVar->bVal, (UINT)pVar->bVal);
					break;
				case VT_UI2:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%u) VT_UI2}"), (UINT)pVar->iVal, (UINT)pVar->iVal);
					break;
				case VT_UI4:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%u) VT_UI4}"), (UINT)pVar->intVal, (UINT)pVar->intVal);
					break;
				case VT_UI8:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%I64x(%I64u) VT_UI8}"), (ULONGLONG)pVar->ullVal, (LONGLONG)pVar->llVal);
				case VT_BOOL:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_BOOL}"), (INT)pVar->intVal, (INT)pVar->intVal);
					break;
				case VT_INT:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_INT}"), (INT)pVar->intVal, (INT)pVar->intVal);
				case VT_UINT:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%u) VT_UINT}"), (UINT)pVar->intVal, (UINT)pVar->intVal);
				case VT_HRESULT:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_HRESULT}"), (UINT)pVar->intVal, (INT)pVar->intVal);
					break;
				case VT_R8:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{%f VT_R8}"), pVar->dblVal);
					break;
				case VT_BSTR:
					(*this)<<"{L\""<<(pVar->bstrVal)<<"\" VT_BSTR}";
					break; 
				case VT_LPWSTR:
					(*this)<<"{L\""<<(pVar->bstrVal)<<"\" VT_LPWSTR}";
					break;
				case VT_LPSTR:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{\"%s\" VT_LPSTR}"), pVar->pcVal);
					break;
				case VT_DISPATCH:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%p VT_DISPATCH}"), pVar->pdispVal);
					break;
				default:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%I64x(%I64d) VT_?[0x%08x(%d)]}"), (ULONGLONG)pVar->ullVal , (LONGLONG)pVar->llVal, pVar->vt, pVar->vt);
					break;
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize,_T("{VT_?[0x%08x(%d)], (!!!)Access Error}"), pVar->vt, pVar->vt);
			}
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//std::string
	inline 	 CTSLog& 		operator << (const std::basic_string<WCHAR> _Val)  throw()	
	{
		this->operator << (_Val.c_str());
		return *this;
	}
	inline 	 CTSLog& 		operator << (const std::basic_string<CHAR> _Val)  throw()	
	{
		this->operator << (_Val.c_str());
		return *this;
	}
	//从缓冲到输出
	inline void				TSLog4CXX_Flush(const TLevel level)
	{
		m_dwUsedSize = 0;
		OutputLog(level, &m_pszLogDataA[0]);
		LeaveCriticalSection(&_cs);
	}
//End V2.0
public:
	inline	static			CTSLog & CTSLog::GetInstance(LPCTSTR pszCallFunName = NULL, LPCVOID = 0);
	inline	static			BOOL IsModuleUnloading(LPCTSTR szFunction)
	{
		if(s_release.m_bIsModuleUnloading)
		{
			TCHAR szDebugViewMsg[MAX_PRIVATEDATA_SIZE] = {0}; //module exit, so alloc memory from stack 
			_sntprintf(szDebugViewMsg, MAX_PRIVATEDATA_SIZE, _T("[%d] <WARN> <%s> "), GetCurrentThreadId(), CTSLog::GetCurrentModuleName());
			std::basic_string<TCHAR> strFunction = (std::basic_string<TCHAR>)szDebugViewMsg + (std::basic_string<TCHAR>) _T("[") + (std::basic_string<TCHAR>)szFunction +_T("]") +_T(" Module Unloading!!!\n");
			OutputDebugString(strFunction.c_str());
		}
		return s_release.m_bIsModuleUnloading;
	}
	static  inline  BOOL			IsLogValid(CTSLog::TLevel l){ return (s_pThis->m_eLevel<=l && (s_pThis->m_bDebugViewLog || s_pThis->m_bFileLog));}
	static	inline	BOOL			IsDebugValid(){ return (s_pThis->m_eLevel<= LEVEL_DEBUG && (s_pThis->m_bDebugViewLog || s_pThis->m_bFileLog)); }
	static	inline	BOOL			IsInfoValid(){ return (s_pThis->m_eLevel<= LEVEL_INFO && (s_pThis->m_bDebugViewLog || s_pThis->m_bFileLog)); }
	static	inline	BOOL			IsWarnValid(){ return (s_pThis->m_eLevel<= LEVEL_WARN && (s_pThis->m_bDebugViewLog || s_pThis->m_bFileLog)); }
	static	inline	BOOL			IsErrorValid(){ return (s_pThis->m_eLevel<= LEVEL_ERROR && (s_pThis->m_bDebugViewLog || s_pThis->m_bFileLog)); }
	static	inline	BOOL			IsFatalValid(){ return (s_pThis->m_eLevel<= LEVEL_FATAL && (s_pThis->m_bDebugViewLog || s_pThis->m_bFileLog)); }
	static inline BOOL				IsTRACEValid()
	{
		return (s_pThis && (s_pThis->m_eLevel <= LEVEL_TRACE) && (s_pThis->m_bDebugViewLog || s_pThis->m_bFileLog));
	}
	static  inline  UINT			GetStringCodePage(const char* szData);
///////////////////////!defined TSLOG_SIMPLE//////////////////////// only win32, use 'TSLOG'/'TSLOG_COMPLEX', or use 'SLOG_SIMPLE'
#ifndef TSLOG_SIMPLE
public:
	static 	inline  LPCTSTR			TraceAuto(LPCSTR pszFuncSig, DWORD dwEBP, BOOL bIsStatic = FALSE) throw();
private:
	typedef enum { CT_STDCALL = 0, CT_CDECL_CALL = 1, CT_THISCALL = 2, CT_FASTCALL = 3, CT_STDTHISCALL, CT_CDECLTHISCALL, CT_OTHERCALL = -1} CALLTYPE_TSLOG;
	typedef enum { PT_INVALID = -1, PT_VOID,PT_bool,PT_INT, PT_UINT, PT_INT64,  PT_UINT64,
		PT_CHAR, PT_WCHAR, PT_CHARPOINT, PT_WCHARPOINT, PT_CHARREF, 
		PT_INT1, PT_UINT1, /*PT_INT1REF,*/ PT_UINT1REF, PT_INT2, PT_UINT2, PT_INT2REF, PT_UINT2REF, PT_INTREF, PT_UINTREF,
		PT_CHARPOINTREF, PT_WCHARREF,PT_WCHARPOINTREF,PT_boolREF,PT_INT64REF, PT_UINT64REF, PT_PENDPOINT, PT_FLOAT,PT_DOUBLE,PT_PEND, PT_PENDREF, PT_VARIANT, PT_VARIANTREF, PT_GUID, PT_GUIDREF} PARAM_TYPE;
	struct stParamTypeInfo 
	{
		CTSLog::PARAM_TYPE pt;
		INT nValidCommaPos;
	};
	static inline BOOL				GetParamInfo(LPCSTR pszFuncSig, BOOL bIsStatic, CALLTYPE_TSLOG & ct, std::vector<stParamTypeInfo>& vecIndex2TypeName, BOOL& bRetClass);
	static inline BOOL				GetParamTypeAndLen(LPCSTR pszRaw, PARAM_TYPE& pt, INT& nLen);
	static inline BOOL				TranslateCodePage(char ** pszValA);
	static inline DWORD				GetSafeValidValue(const CTSLog::PARAM_TYPE &tslog_eParamType,LPSTR tslog_szRegValue, DWORD tslog_nRegValue, DWORD tslog_nRegValue2 = 0,
										LPINT tslog_pnRegValue = NULL, LPINT tslog_pnRegValue2 = NULL , LPSTR tslog_pRegValueA = NULL , LPWSTR  tslog_pRegValueW = NULL, LPINT pnRet = NULL) throw();
#endif
////////////////////////End !defined TSLOG_SIMPLE///////////////////////
private:
//////////////////////defined(UNICODE)/////////////////////////
#if (defined(UNICODE) || defined(_UNICODE))
#define TFUNCTION2(x) L ## x
#define TFUNCTION(x) TFUNCTION2(x)
#define __TFUNCTION__  TFUNCTION(__FUNCTION__)
#define TFUNCSIG2(x) L ## x
#define TFUNCSIG(x) TFUNCSIG2(x)
#define __TFUNCSIG__  TFUNCSIG(__FUNCSIG__)
#define TTIMESTAMP2(x) L ## x
#define TTIMESTAMP(x) TTIMESTAMP2(x)
#define __TTIMESTAMP__  TTIMESTAMP(__TIMESTAMP__)
#define TTSLOG_CONFIG_DIR2(x) L ## x
#define TTSLOG_CONFIG_DIR(x) TTSLOG_CONFIG_DIR2(x)
#define __TTSLOG_CONFIG_DIR TTSLOG_CONFIG_DIR(TSLOG_CONFIG_DIR)
#define TTSLOG_GROUP2(x) L ## x
#define TTSLOG_GROUP(x) TTSLOG_GROUP2(x)
#define __TTSLOG_GROUP TTSLOG_GROUP(TSLOG_GROUP)
#define __TTSLOG_GROUP_DEFAULT TTSLOG_GROUP(TSLOG_GROUP_DEFAULT)
#else
#define __TFUNCTION__ __FUNCTION__
#define __TFUNCSIG__ __FUNCSIG__
#define __TTIMESTAMP__ __TIMESTAMP__
#define __TTSLOG_CONFIG_DIR TSLOG_CONFIG_DIR
#define __TTSLOG_GROUP TSLOG_GROUP
#define __TTSLOG_GROUP_DEFAULT TSLOG_GROUP_DEFAULT
#endif
//////////////////////End defined(UNICODE)/////////////////////////
private:
#if (defined(UNICODE) || defined(_UNICODE))
	static inline BOOL				UnicodeToMultiByte(const std::basic_string<WCHAR> &strSrc, std::string &strDst);
	static inline BOOL				MultiByteToUnicode(const std::string& strSrc, std::basic_string<WCHAR> &strDst);
#endif //End defined(UNICODE)
	static inline LONGLONG GetTickCount64(void)
	{		
		__asm __emit 0x0F;
		__asm __emit 0x31;
	}
	static inline  LPCTSTR			GetCurrentModuleVersion(void);
	static inline HINSTANCE			GetCurrentModuleHandle(void)
	{
		static HINSTANCE hCurrentModule = 0;
		if(NULL == hCurrentModule)
		{
			MEMORY_BASIC_INFORMATION m = { 0 };
			VirtualQuery(&hCurrentModule, &m, sizeof(MEMORY_BASIC_INFORMATION));
			hCurrentModule = (HINSTANCE) m.AllocationBase;
		}
		return hCurrentModule;
	}
private:	
	typedef	struct _FileInfo{std::basic_string<TCHAR>	strFilePath_; FILETIME stFileTime_;	}FileInfo;
	typedef std::pair<INT, FileInfo> PAIRPID2FILEINFO;
	inline	static bool		FileCmp(const PAIRPID2FILEINFO &val1, const PAIRPID2FILEINFO &val2);
private:
	inline	void			PrintModuleInit(void);
	inline  LPCTSTR			GetCurrentProcessName(void);
	inline	static LPCTSTR	GetCurrentModuleName(void);
	inline	BOOL			CheckConfigFileChanged(void);
	inline  BOOL			MakeDirectoryA(LPCSTR lpszDestDirA);
	inline  BOOL			MakeDirectory(LPCTSTR lpszDestPath);
	inline	void			ResetParams(BOOL bModuleInit = FALSE);
	inline	BOOL			GetConfig(BOOL bModuleInit = FALSE);
	inline	void			OutputLog(TLevel level, LPCTSTR pszFormat, va_list ap = NULL, LPCTSTR pszLogFilePath = NULL, BOOL bCheckFileChanged = TRUE) throw();
	inline	void			WriteToLogFile(TLevel level, LPCTSTR pszFileLogMsg, DWORD dwLen, LPCTSTR pszLogFilePath) throw();
	inline	BOOL			GetNthFilePathName(INT nFileCnt, LPTSTR pOleFilePathName, LPTSTR pNewFilePathName);
	inline	BOOL			DeleteHistoryLogFiles() throw();
	inline  void			DeleteAllFilesOfPid(const INT nPid);
	inline	int				FindPidFromLogFilePath(LPCTSTR pszFilePath);
	inline	void			TSPrompt(LPCTSTR pszFormat, ...); //打印提示信息, 内部使用
private:
inline	CTSLog(){};
inline	~CTSLog(){};
private:
#pragma pack(push, 1)
	class SectionProtect
	{
	public:
		SectionProtect(LPCRITICAL_SECTION lpCriticalSection, BOOL = FALSE) : m_lpCriticalSection(NULL)
		{
			if (NULL != lpCriticalSection)
			{
				EnterCriticalSection(lpCriticalSection);
				m_lpCriticalSection = lpCriticalSection;
			}
		}
		~SectionProtect()
		{
			if (m_lpCriticalSection)
			{
				LeaveCriticalSection(m_lpCriticalSection);
				m_lpCriticalSection = NULL;
			}
		}
		void Submit()
		{
			if (m_lpCriticalSection)
			{
				LeaveCriticalSection(m_lpCriticalSection);
				m_lpCriticalSection = NULL;
			}
		}
	private:
		LPCRITICAL_SECTION m_lpCriticalSection;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	typedef struct _tagTSGHeapVector
	{
		LPSTR pszData;
		_tagTSGHeapVector* pNext;
	} TSGHeapVector ,*PTSGHeapVector;
#pragma pack(pop)

#pragma pack(push, 1)
	class CTSGHeapVector
	{
	public:
		CTSGHeapVector() :m_pTSGHeapVector(NULL)
		{}
		void Add(PTSGHeapVector pTSGHeapVector)
		{
			if(NULL == pTSGHeapVector)
				return;
			if(NULL == m_pTSGHeapVector)
			{
				m_pTSGHeapVector = pTSGHeapVector;
				return;
			}
			PTSGHeapVector pTmpTSGHeapVector = NULL;
			pTmpTSGHeapVector = m_pTSGHeapVector;
			for ( ; NULL != pTmpTSGHeapVector->pNext ; pTmpTSGHeapVector = pTmpTSGHeapVector->pNext)
				;
			pTmpTSGHeapVector->pNext = pTSGHeapVector;
		}
		void Add(LPCTSTR lpszFilter)
		{
			if(NULL == lpszFilter)
				return;
			{
#if (defined(UNICODE) || defined(_UNICODE))
				std::string strpv;
				CTSLog::UnicodeToMultiByte(lpszFilter, strpv);
				LPCSTR lpszFilter = strpv.c_str();
#endif
				size_t size = strlen(lpszFilter);
				if(0 >= size)
					return;
				HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED,(size + 1) * sizeof(CHAR) + sizeof(LPSTR) + sizeof(_tagTSGHeapVector*));
				PTSGHeapVector pTSGHeapVector = (PTSGHeapVector)GlobalLock(hGlobal);
				if(NULL == pTSGHeapVector)
					return;
				pTSGHeapVector->pszData = (LPSTR)pTSGHeapVector + sizeof(LPSTR)/sizeof(CHAR) + sizeof(_tagTSGHeapVector*);
				memcpy(pTSGHeapVector->pszData, lpszFilter, (size + 1) * sizeof(CHAR));
				pTSGHeapVector->pNext = NULL;
				Add(pTSGHeapVector);
			}
		}
		void Clear()
		{
			PTSGHeapVector pTmpTSGHeapVector = NULL;
			PTSGHeapVector pDelTSGHeapVector = NULL;
			pTmpTSGHeapVector = m_pTSGHeapVector;
			while(pTmpTSGHeapVector)
			{
				pDelTSGHeapVector = pTmpTSGHeapVector;
				pTmpTSGHeapVector = pTmpTSGHeapVector->pNext;
				GlobalUnlock(pDelTSGHeapVector);
				GlobalFree(pDelTSGHeapVector);
				pDelTSGHeapVector = NULL;
			}
			m_pTSGHeapVector = NULL;
		}
		BOOL Find(LPCTSTR lpszFilter)
		{
			PTSGHeapVector pTmpTSGHeapVector = NULL;
			pTmpTSGHeapVector = m_pTSGHeapVector;
			while(pTmpTSGHeapVector)
			{
#if (defined(UNICODE) || defined(_UNICODE))
				std::string strpv;
				CTSLog::UnicodeToMultiByte(lpszFilter, strpv);
				LPCSTR lpszFilter = strpv.c_str();
#endif
				if( 0 == strcmp(pTmpTSGHeapVector->pszData, lpszFilter))
					return TRUE;
				else
					pTmpTSGHeapVector = pTmpTSGHeapVector->pNext;					
			}
			return FALSE;
		}
		BOOL SubFind(LPCTSTR lpszFilter)
		{
			PTSGHeapVector pTmpTSGHeapVector = NULL;
			pTmpTSGHeapVector = m_pTSGHeapVector;
			while(pTmpTSGHeapVector)
			{
#if (defined(UNICODE) || defined(_UNICODE))
				std::string strpv;
				CTSLog::UnicodeToMultiByte(lpszFilter, strpv);
				LPCSTR lpszFilter = strpv.c_str();
#endif
				if( 0 != strstr(lpszFilter, pTmpTSGHeapVector->pszData))
					return TRUE;
				else
					pTmpTSGHeapVector = pTmpTSGHeapVector->pNext;
			}
			return FALSE;
		}
	private:	//size don't be changed
		PTSGHeapVector m_pTSGHeapVector;
		DWORD m_dwReserve[20];
	};
#pragma pack(pop)
public:
	class CEnterLeaveFunCallBack
	{
	public:
#define MAX_LEAVE_FUNNAME_LENGTH 256
#define ENTER_FUNCTION_FLAG _T("→")
#define LEAVE_FUNCTION_FLAG _T("←")
		CEnterLeaveFunCallBack(LPCTSTR pszCallFunName ,LPCTSTR pszFunction, LPCTSTR pszFuncSig,LPCVOID pvThis = 0, BOOL bIsSimple = FALSE, DWORD dwEBP = 0, LPCSTR pszFuncSigA = NULL)
			:m_pvThis(pvThis), m_pszFunction(pszFunction), m_pszFuncSig(pszFuncSig), m_pszCallFunName(pszCallFunName), m_dwEBP(dwEBP), m_pszFuncSigA(pszFuncSigA)
		{
			if(NULL == CTSLog::s_pThis || CTSLog::s_pThis->s_release.m_bIsModuleUnloading)
			{
				TCHAR szDebugViewMsg[1024] = {0}; //module exit, so alloc memory from stack 
				_sntprintf(szDebugViewMsg, MAX_PRIVATEDATA_SIZE, _T("[%d] <WARN> <%s> "), GetCurrentThreadId(), CTSLog::GetCurrentModuleName());
				std::basic_string<TCHAR> strFunction = (std::basic_string<TCHAR>)szDebugViewMsg + (std::basic_string<TCHAR>) _T("[") + 
					m_pszFunction +_T("] ") + (std::basic_string<TCHAR>)m_pszCallFunName + _T(" Module Unloading!!!") +_T("\n");
				OutputDebugString(strFunction.c_str());
				return;
			}
			SectionProtect sp(&CTSLog::s_pThis->_cs);
			CTSLog::GetInstance(m_pszFunction, m_pvThis).OutputLog(LEVEL_TRACE, m_pszCallFunName, NULL);
			if(bIsSimple)
				;
			else
				LeaveCriticalSection(&CTSLog::s_pThis->_cs);//TraceAuto ++ EnterCriticalSection
		}
		~CEnterLeaveFunCallBack()
		{
			TCHAR szFunction[MAX_LEAVE_FUNNAME_LENGTH];
			_sntprintf(szFunction , MAX_LEAVE_FUNNAME_LENGTH - 1, _T("%s%s"), LEAVE_FUNCTION_FLAG, &m_pszFunction[OFFSET_LEAVE_FUNCTION_FLAG]);
			if(NULL == CTSLog::s_pThis)
			{
				TCHAR szDebugViewMsg[MAX_PRIVATEDATA_SIZE] = {0}; //module exit, so alloc memory from stack 
				_sntprintf(szDebugViewMsg, MAX_PRIVATEDATA_SIZE, _T("[%d] WARN <%s> "), GetCurrentThreadId(), CTSLog::GetCurrentModuleName());
				std::basic_string<TCHAR> strFunction = (std::basic_string<TCHAR>)szDebugViewMsg + (std::basic_string<TCHAR>) _T("[") + 
					szFunction + _T("] ") + /*(std::basic_string<TCHAR>)m_pszCallFunName +*/ _T(" Module Unloading!!!") + _T("\n");
				OutputDebugString(strFunction.c_str());
				return;
			}
			if(m_dwEBP) 
			{
#ifndef TSLOG_SIMPLE
				LPCTSTR tslog_lpctszFuncSig = CTSLog::TraceAuto(m_pszFuncSigA,m_dwEBP,NULL == m_pvThis);
				CTSLog::GetInstance(szFunction , m_pvThis).OutputLog(LEVEL_TRACE, tslog_lpctszFuncSig, NULL);
				LeaveCriticalSection(&CTSLog::s_pThis->_cs);//TraceAuto ++ EnterCriticalSection
#endif
			}
			else
			{
				CTSLog::GetInstance(szFunction , m_pvThis).TSTrace( m_pszFuncSig);
			}			
		}
	private:
		LPCVOID m_pvThis;
		LPCTSTR m_pszFunction;	  // function name					CTSGHeapVector::Add
		LPCTSTR m_pszFuncSig;	  // prototype +  param				BOOL CTSGHeapVector::Add(LPVOID)
		LPCSTR	m_pszFuncSigA;
		LPCTSTR m_pszCallFunName; // prototype +  param + value		BOOL CTSGHeapVector::Add(LPVOID = 0x00000000)		
	private:
		static const size_t OFFSET_LEAVE_FUNCTION_FLAG;
		DWORD	m_dwEBP;
	};

#pragma pack(push, 1)
	class CRelease
	{
		friend class CTSLog;
		friend class CTSLog::CEnterLeaveFunCallBack;
	private:
		volatile LONG  m_bIsModuleUnloading;
		HMODULE m_hLang;
		SYSTEM_INFO m_si;
	public:
		CRelease(INT):m_bIsModuleUnloading(FALSE), m_hLang(NULL)
		{
			InitializeCriticalSection(&CTSLog::s_cs);
			memset(&m_si, 0x00, sizeof(SYSTEM_INFO));
			GetSystemInfo(&m_si);
			m_hLang = LoadLibrary(_T("mlang.dll"));
			CTSLog::s_dwTlsIndex = TlsAlloc();
			CTSLog::s_dwTlsIndex_this = TlsAlloc();
			if(TLS_OUT_OF_INDEXES == CTSLog::s_dwTlsIndex)
				CTSLog::s_dwTlsIndex = TlsAlloc();
			if(TLS_OUT_OF_INDEXES == CTSLog::s_dwTlsIndex_this)
				CTSLog::s_dwTlsIndex_this = TlsAlloc();
			CTSLog::GetInstance();//保证~CRelease( )不异常，且打印模块初始化，退出等日志
		}
		~CRelease( )// 使用静态成员的析构函数，实现单一实例的释放
		{
			::InterlockedExchangeAdd(&m_bIsModuleUnloading,1L);
			s_pThis->TSPrompt(_T("Module Unload. Path = %s, Current LogGroup : \"%s\", ModuleID : %d, RefCount : %d"), s_szModuleFileName, __TTSLOG_GROUP, s_nModuleID , s_pThis->_lRef - 1);
			DeleteCriticalSection(&CTSLog::s_cs);
			TlsFree(CTSLog::s_dwTlsIndex);
			TlsFree(CTSLog::s_dwTlsIndex_this);
			if( NULL == s_hMutex )
				return;
			::WaitForSingleObject( s_hMutex, INFINITE ); //待第一个初始化完成,其它模块不构建TSLOG实例,因此不需要使用Mutex
			HANDLE hMutex = s_hMutex;
			s_hMutex = NULL;
			LONG nRef = ::InterlockedDecrement((volatile LONG*)&s_pThis->_lRef );
			if( nRef <= 0 )
			{
				if(INVALID_HANDLE_VALUE != s_pThis->_hConfigFileChanged)
					FindCloseChangeNotification(s_pThis->_hConfigFileChanged);
				if( INVALID_HANDLE_VALUE != s_pThis->m_hLogFile )
					::CloseHandle( s_pThis->m_hLogFile );
				DeleteCriticalSection(&s_pThis->_cs);
				s_pThis->m_ghvInclude.Clear();
				s_pThis->m_ghvExclude.Clear();
				HGLOBAL hGlobal = (HGLOBAL)s_pThis->m_pszLogDataA;
				if(hGlobal)
				{
					GlobalUnlock(hGlobal);
					GlobalFree(hGlobal);
					hGlobal = NULL;
					s_pThis->m_pszLogDataA = NULL;
				}
			}
			if( INVALID_HANDLE_VALUE != s_pThis->s_hLogFile)
				::CloseHandle( s_pThis->s_hLogFile );			
			::UnmapViewOfFile( s_pThis );
			s_pThis = NULL;
			::CloseHandle( s_hFileMap );
			s_hFileMap = NULL;
			if (hMutex)
			{
				::ReleaseMutex( hMutex );
				::CloseHandle( hMutex );
			}
			if(m_hLang)
			{
				FreeLibrary(m_hLang);
				m_hLang = NULL;
			}
		}
	};
#pragma pack(pop)	

private:
	static HANDLE	s_hMutex;			// 只用于同步初始化和终结化
	static HANDLE	s_hFileMap;			// 文件影射句柄
	static CTSLog * s_pThis;			// 影射视图指针
	static HINSTANCE s_hInst;			// 模块句柄
	static TCHAR s_szModuleFileName[_MAX_PATH];
	static CRITICAL_SECTION	s_cs;
	static DWORD s_dwTlsIndex;
	static DWORD s_dwTlsIndex_this;
 	static CRelease s_release;			// 用它的析构函数释放单一实例
	static LONG32 s_lVer;
	static HANDLE s_hLogFile;
	static DWORD s_dwLogFileCnt;
	static INT32 s_nModuleID;
/////////////////////variable must have same size/////////////////////////
private:
	LONG32				_lVer;			//版本,当前为
	LONG32				_cbThis;			//sizeof TSLOG
	volatile LONG     	_lRef;			// 模块计数
	CRITICAL_SECTION	_cs;			// 用于同步对文件影射内容的访问
	HANDLE				_hConfigFileChanged;//配置文件变化监视句柄
private:
	BOOL			m_bDebugViewLog;	//是否打印DebugView日志
	BOOL			m_bFileLog;			//是否打印文件日志

	TLevel			m_eLevel;			//日志级别
	CTSGHeapVector	m_ghvInclude;		//过滤项
	CTSGHeapVector	m_ghvExclude;		//滤除项

	DWORD			m_dwMaxLogFileSize;	//日志文件最大尺寸
	INT32			m_nMaxLogFileCnt;	//日志文件数目限制
	INT32			m_nMaxHistroyProcessCnt;//历史日志进程数目限制

	HANDLE			m_hLogFile;			//日志文件句柄
	INT32			m_nLogFileCnt;		//第几个日志文件
	INT32			m_nFileLogLineCnt;	//文件日志中的行号	
	DWORD			m_dwCurFileSize;
	CHAR			m_szLogFileDirA[MAX_PATH];	
	//DWORD			m_dwReserve[20];		//V1.0 保留80个字节	
	//V2.0			2008-11-20	TSLOG_VERSION 0x00020000
	DWORD			m_dwReserve[16];
	DWORD			m_dwAllLogFileCnt;
	LPTSTR			m_pszLogDataA;		//指向全局堆 当成char大小来看以防multi/unicode模块合在一起使用
	DWORD			m_dwMaxLogDataSize;  //全局堆大小,字符数! 非字节大小
	DWORD			m_dwUsedSize;		//全局堆使用大小,字符数! 非字节大小
};
#pragma pack(pop)
//避免多文件包时编译错误
__declspec(selectany) HANDLE CTSLog::s_hMutex = NULL;
__declspec(selectany) HANDLE CTSLog::s_hFileMap = NULL;
__declspec(selectany) CTSLog* CTSLog::s_pThis = NULL;
__declspec(selectany) HINSTANCE CTSLog::s_hInst = NULL;
__declspec(selectany) TCHAR CTSLog::s_szModuleFileName[_MAX_PATH];
__declspec(selectany) CRITICAL_SECTION CTSLog::s_cs;
__declspec(selectany) DWORD CTSLog::s_dwTlsIndex = TLS_OUT_OF_INDEXES;
__declspec(selectany) DWORD CTSLog::s_dwTlsIndex_this = TLS_OUT_OF_INDEXES;
__declspec(selectany) CTSLog::CRelease CTSLog::s_release(1);
__declspec(selectany) LONG32 CTSLog::s_lVer = TSLOG_VERSION;
__declspec(selectany) const size_t CTSLog::CEnterLeaveFunCallBack::OFFSET_LEAVE_FUNCTION_FLAG = _tcslen(LEAVE_FUNCTION_FLAG);
__declspec(selectany) HANDLE CTSLog::s_hLogFile = INVALID_HANDLE_VALUE;
__declspec(selectany) DWORD CTSLog::s_dwLogFileCnt = 0;
__declspec(selectany) INT32 CTSLog::s_nModuleID = 0;
#if (defined(UNICODE) || defined(_UNICODE))
BOOL CTSLog::UnicodeToMultiByte(const std::basic_string<WCHAR> &strSrc, std::string &strDst)
{
	INT nSrcUnicodeLen = 0;
	nSrcUnicodeLen = (INT)strSrc.length();
	if(0 == nSrcUnicodeLen)
		return TRUE;
	static INT nLen = 0; 
	nLen = WideCharToMultiByte(936, 0, strSrc.c_str(), -1,NULL, 0 ,NULL, NULL);
	CHAR szBuf[MAX_USERDATA_SIZE + 1] = {0};
	LPSTR pszBuf = szBuf;
	if(MAX_USERDATA_SIZE < nLen)
		nLen = MAX_USERDATA_SIZE;
	INT nResult = 0;
	nResult = WideCharToMultiByte(936, 0, strSrc.c_str(), -1, pszBuf, nLen,NULL,NULL);
	if( nResult <= 0)
		return FALSE;
 	if(pszBuf)
		strDst = pszBuf;
	return TRUE;
}
BOOL CTSLog::MultiByteToUnicode(const std::string& strSrc, std::basic_string<WCHAR> &strDst)
{
	INT nSrcAnsiLen = 0;
	nSrcAnsiLen = (INT)strSrc.length();
	if(0 == nSrcAnsiLen)
		return TRUE;
	UINT code_page = GetStringCodePage(strSrc.c_str());
	INT nLen = 0;
	nLen = MultiByteToWideChar(code_page,0,strSrc.c_str(),-1,NULL,0);
	if(0 == nLen)
	{
		code_page = 0;
		nLen = MultiByteToWideChar(code_page,0,strSrc.c_str(),-1,NULL,0);
	}
	if(MAX_USERDATA_SIZE < nLen)
		nLen = MAX_USERDATA_SIZE;
	WCHAR szBuf[MAX_USERDATA_SIZE + 1] = {0};
	LPWSTR pszBuf = szBuf;
	INT nResult = 0;
	nResult = MultiByteToWideChar(code_page,0,strSrc.c_str(), -1, pszBuf,nLen);
	if(nResult <= 0 )
		return FALSE;	
 	if(pszBuf)
		strDst = pszBuf;
	return TRUE;
}
#endif //End (defined(UNICODE) || defined(_UNICODE))
LPCTSTR CTSLog::GetCurrentModuleVersion()
{
#define CB_FILE_VERSION 0x168
	HINSTANCE s_hInst = reinterpret_cast<HINSTANCE>(GetCurrentModuleHandle());
	TCHAR szModuleFileName[_MAX_PATH] = {0};
	LPTSTR pszInfoBuf = NULL;
	VS_FIXEDFILEINFO* pvi = NULL;
	UINT  cbTranslate = 0;
	static TCHAR szVerInfo[CB_FILE_VERSION] = {0};
	::GetModuleFileName( s_hInst,szModuleFileName , MAX_PATH);
	DWORD dwInfoSize = 0, dwHandle = 0;
	dwInfoSize = GetFileVersionInfoSize(szModuleFileName,&dwHandle);
	if((DWORD)0 == dwInfoSize)
		return NULL;
	 pszInfoBuf = new TCHAR[dwInfoSize];
	 if(NULL == pszInfoBuf)
		 return NULL;
	if(GetFileVersionInfo(szModuleFileName,0,dwInfoSize,pszInfoBuf))
	{
		if(VerQueryValue(pszInfoBuf, _T("\\"),(LPVOID*)&pvi,&cbTranslate))
		{
			_sntprintf(szVerInfo,CB_FILE_VERSION, _T("FileVersion : %d.%d.%d.%d [ FileVersionMS : 0x%08x, FileVersionLS : 0x%08x, ProductVersionMS : 0x%08x, ProductVersionLS : 0x%08x, ") 
				_T("Signature : 0x%08x, FileFlags : 0x%08x, FileFlagsMask : 0x%08x, FileOS : 0x%08x, ")
				_T("FileType : 0x%08x, FileSubType : 0x%08x, FileDateMS : 0x%08x, FileDateLS : 0x%08x ]") ,
				HIWORD(pvi->dwFileVersionMS), LOWORD(pvi->dwFileVersionMS), HIWORD(pvi->dwFileVersionLS), LOWORD(pvi->dwFileVersionLS),
				pvi->dwFileVersionMS, pvi->dwFileVersionLS, pvi->dwProductVersionMS, pvi->dwProductVersionLS ,
				pvi->dwSignature, pvi->dwFileFlags, pvi->dwFileFlagsMask, pvi->dwFileOS,
				pvi->dwFileType, pvi->dwFileSubtype, pvi->dwFileDateMS, pvi->dwFileDateLS);
		}
	}
	delete pszInfoBuf;
	pszInfoBuf = NULL;
	return szVerInfo[0] ? szVerInfo : NULL;
}

CTSLog & CTSLog::GetInstance(LPCTSTR pszCallFunName, LPCVOID pvThis)
{
	if( s_pThis && s_pThis->_lRef >0 )//多线程启动时，同步等 InterlockedExchange( &s_pThis->_nRef, 1 );完成
	{
		TlsSetValue(s_dwTlsIndex, (LPVOID)pszCallFunName);
		TlsSetValue(s_dwTlsIndex_this, (LPVOID)pvThis);
		return *s_pThis;
	}
	if (s_release.m_bIsModuleUnloading)
	{
		MessageBox(NULL, _T("启动异常"), _T("错误"), MB_OK);
		TerminateProcess(GetCurrentProcess(), (UINT)-1);
		return *s_pThis;
		//ExitThread((UINT)-1);
	}
	SectionProtect sp(&s_cs);
	if( s_pThis )
	{
		TlsSetValue(s_dwTlsIndex, (LPVOID)pszCallFunName);
		TlsSetValue(s_dwTlsIndex_this, (LPVOID)pvThis);
		return *s_pThis;
	}
	s_hInst = reinterpret_cast<HINSTANCE>(GetCurrentModuleHandle());
	::GetModuleFileName(s_hInst, s_szModuleFileName, MAX_PATH - 1);
	TCHAR szMutex[MAX_PATH] = _T("#mutexTSLOG") __TTSLOG_GROUP;
	TCHAR szFileMap[MAX_PATH] = _T("#filemapTSLOG") __TTSLOG_GROUP;
	DWORD dwPID = GetCurrentProcessId();
	_sntprintf( szMutex + _tcslen(szMutex), MAX_PATH/2, _T("%d"), dwPID);
	_sntprintf( szFileMap + _tcslen(szFileMap),MAX_PATH/2 , _T("%d"), dwPID);
	s_hMutex = CreateMutex(NULL, true, szMutex);
	if(NULL == s_hMutex)
	{
		::MessageBox(NULL, _T("CreateMutex failure"), s_szModuleFileName, MB_OK | MB_ICONERROR);
		::ExitProcess((UINT)-1);
	}
	bool bExist = (ERROR_ALREADY_EXISTS == ::GetLastError());
	if(bExist)
		::WaitForSingleObject(s_hMutex, INFINITE);				// 等待第一个模块把本单一实例创建完成，或Release完成
	s_hFileMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CTSLog), szFileMap);
	if( NULL == s_hFileMap )
	{
		::MessageBox( NULL, _T("CreateFileMapping failure"), s_szModuleFileName, MB_OK | MB_ICONERROR );
		::ReleaseMutex( s_hMutex );
		::CloseHandle( s_hMutex );
		s_hMutex = NULL;
		::ExitProcess((UINT)-1);
	}
	bExist = (ERROR_ALREADY_EXISTS == ::GetLastError());
	// 如果第一个模块刚创建单一实例，又立即Release，会导致第二个模块hMutex已存在，但hFileMap已关闭的情况。此时，第二个模块当作新建单一实例	 
	s_pThis = (CTSLog *) ::MapViewOfFile(s_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CTSLog));		
	if(NULL == s_pThis)
	{
		::MessageBox(NULL, _T("MapViewOfFile failure"), s_szModuleFileName, MB_OK | MB_ICONERROR);
		::CloseHandle( s_hFileMap );
		s_hFileMap = NULL;
		::ReleaseMutex( s_hMutex );
		::CloseHandle( s_hMutex );
		s_hMutex = NULL;
		::ExitProcess((UINT)-1);
	}
	TlsSetValue(s_dwTlsIndex, (LPVOID)pszCallFunName);
	TlsSetValue(s_dwTlsIndex_this, (LPVOID)pvThis);
	s_release.m_bIsModuleUnloading = FALSE; //禁止 Release 优化,这一行非写不可,2当模块退出时，不检测日志是否被更改
	if( bExist )
	{
		if(MAX_USERDATA_SIZE + 1 > s_pThis->m_dwMaxLogDataSize)
		{
			HGLOBAL hGlobal = (HGLOBAL)s_pThis->m_pszLogDataA;
			if(hGlobal)
			{
				GlobalUnlock(hGlobal);
				GlobalFree(hGlobal);
				hGlobal = NULL;
				s_pThis->m_dwMaxLogDataSize = 0;
				s_pThis->m_pszLogDataA = NULL;
			}
			hGlobal = GlobalAlloc(GMEM_FIXED, (MAX_USERDATA_SIZE + 1) * sizeof(WCHAR));
			s_pThis->m_pszLogDataA = (LPTSTR)GlobalLock(hGlobal);
			if(s_pThis->m_pszLogDataA)
			{
				memset(s_pThis->m_pszLogDataA, 0x00, (MAX_USERDATA_SIZE + 1) * sizeof(WCHAR));
				s_pThis->m_dwMaxLogDataSize = MAX_USERDATA_SIZE + 1;
			}
		}
	}
	else
	{
		LONG32 size = (LONG32)sizeof(CTSLog);
		memset(s_pThis, 0x00, size);
		HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, (MAX_USERDATA_SIZE + 1) * sizeof(WCHAR));
		s_pThis->m_pszLogDataA = (LPTSTR)GlobalLock(hGlobal);
		if(s_pThis->m_pszLogDataA)
		{
			memset(s_pThis->m_pszLogDataA, 0x00, (MAX_USERDATA_SIZE + 1) * sizeof(WCHAR));
			s_pThis->m_dwMaxLogDataSize = MAX_USERDATA_SIZE + 1;
		}
		::InitializeCriticalSection( &s_pThis->_cs );
		s_pThis->_lVer = TSLOG_VERSION;
		s_pThis->_cbThis  = size;
		s_pThis->ResetParams(TRUE);
		s_pThis->GetConfig(TRUE);
	}
	::InterlockedExchangeAdd((volatile LONG*)&s_pThis->_lRef, 1L );
	s_nModuleID = s_pThis->_lRef;	
	s_dwLogFileCnt = (INVALID_HANDLE_VALUE == s_pThis->m_hLogFile && s_pThis->m_bFileLog) ? s_pThis->m_dwAllLogFileCnt + 1 : s_pThis->m_dwAllLogFileCnt;
	s_pThis->PrintModuleInit();	
	::ReleaseMutex( s_hMutex );
	return *s_pThis;
}
void	CTSLog::PrintModuleInit(void)
{
	static bool bFirstInit = true;
	TCHAR szData[MAX_USERDATA_SIZE + 1] = {0};
	if(bFirstInit)
	{
		bFirstInit = false;
		_sntprintf(szData, MAX_USERDATA_SIZE, _T("Module Loaded. Entry Address(0x%08x), Path = %s, Current LogGroup : \"%s\", ModuleID : %d, RefCount : %d, TSLOG_VERSION_IN_FILEMAP : 0x%08x, TSLOG_VERSION_IN_STAITC : 0x%08x"),
			(ULONG)(ULONG_PTR)GetCurrentModuleHandle(), s_szModuleFileName, __TTSLOG_GROUP, s_nModuleID, _lRef, _lVer, s_lVer);
	}
	else
	{
		_sntprintf(szData, MAX_USERDATA_SIZE, _T("Entry Address(0x%08x), Path = %s, Current LogGroup : \"%s\", ModuleID : %d, RefCount : %d, TSLOG_VERSION_IN_FILEMAP : 0x%08x, TSLOG_VERSION_IN_STAITC : 0x%08x"),
			(ULONG)(ULONG_PTR)GetCurrentModuleHandle(), s_szModuleFileName, __TTSLOG_GROUP, s_nModuleID, _lRef, _lVer, s_lVer);
	}	
	TSPrompt(szData);
	LPCTSTR pszVertionType = NULL;
	LPCTSTR pszLogFilePath = NULL;
#if (defined(UNICODE) || defined(_UNICODE))
	pszVertionType =  L"'UNICODE'";
	std::basic_string<WCHAR>strLogFilePathW;
	MultiByteToUnicode(m_szLogFileDirA,strLogFilePathW );
	pszLogFilePath = strLogFilePathW.c_str();	
#else
	pszVertionType = "'MBCS'",
	pszLogFilePath = m_szLogFileDirA;
#endif
	TSPrompt(_T("BuildType : %s, FileVersionInfo : { %s } Rebuild Time : %s, LogFileDir : %s, AllLogFileCount : %lu"), 
		pszVertionType, GetCurrentModuleVersion(), __TTIMESTAMP__, (INVALID_HANDLE_VALUE == m_hLogFile) ? NULL : pszLogFilePath, m_dwAllLogFileCnt);	
}
LPCTSTR CTSLog::GetCurrentProcessName(void)
{
	static TCHAR szfname[_MAX_FNAME] = {0};
	if('\0' != szfname[0])
		return szfname;
	TCHAR szCurProcessName[_MAX_FNAME] = {0};
	TCHAR szdrive[_MAX_DRIVE] = {0};
	TCHAR szdir[_MAX_DIR] = {0};
	TCHAR szext[_MAX_EXT] = {0};
	GetModuleFileName(NULL,szCurProcessName , _MAX_PATH);
	_tsplitpath(szCurProcessName, szdrive, szdir, szfname, szext );
	return szfname;
}
LPCTSTR CTSLog::GetCurrentModuleName(void)
{
	static TCHAR szCurModuleName[_MAX_FNAME] = {0};
	if('\0' != szCurModuleName[0])
		return szCurModuleName;
	HINSTANCE s_hInst = reinterpret_cast<HINSTANCE>(GetCurrentModuleHandle());
	TCHAR szModuleFileName[_MAX_PATH] = {0};
	LPTSTR pszModuleFileName = szModuleFileName;
	::GetModuleFileName( s_hInst,szModuleFileName , MAX_PATH);
	pszModuleFileName += _tcslen(szModuleFileName);
	while ('\\' != *(--pszModuleFileName)) ;
		pszModuleFileName++;
	_tcsncpy(szCurModuleName, pszModuleFileName,_MAX_PATH); //
	pszModuleFileName = szCurModuleName;			//去除 后缀名
	while ( '\0' != *pszModuleFileName)
	{
		pszModuleFileName++;
		if( '.'== *pszModuleFileName)
		{
			*pszModuleFileName = '\0';
			break;
		}
	}
 	return szCurModuleName;
}
BOOL CTSLog::CheckConfigFileChanged(void)
{
#define FILE_NOTIFY_TSLOG (FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY)
	INT nStatus = 0;
	if(s_release.m_bIsModuleUnloading)
		return FALSE;
	if(INVALID_HANDLE_VALUE == _hConfigFileChanged)
	{
		_hConfigFileChanged = FindFirstChangeNotification(__TTSLOG_CONFIG_DIR,  FALSE, FILE_NOTIFY_TSLOG);
		if(INVALID_HANDLE_VALUE != _hConfigFileChanged) //存在这个目录
		{
			if(INVALID_FILE_ATTRIBUTES == GetFileAttributes(__TTSLOG_CONFIG_FILE_PATH) 
				&& INVALID_FILE_ATTRIBUTES == GetFileAttributes(__TTSLOG_CONFIG_FILE_PATH2)
				&& INVALID_FILE_ATTRIBUTES == GetFileAttributes(__TTSLOG_CONFIG_FILE_PATH_DEFAULT)
				&& INVALID_FILE_ATTRIBUTES == GetFileAttributes(__TTSLOG_CONFIG_FILE_PATH_DEFAULT2))
				return FALSE;		//不存在
			GetConfig();
		}
		return TRUE; //下一条才打印
	}
	nStatus = WaitForSingleObject(_hConfigFileChanged, 0);
	switch(nStatus)
	{
	case WAIT_OBJECT_0:
		FindCloseChangeNotification(_hConfigFileChanged);
		_hConfigFileChanged = INVALID_HANDLE_VALUE;
		if(GetConfig())
			_hConfigFileChanged = FindFirstChangeNotification(__TTSLOG_CONFIG_DIR,  FALSE, FILE_NOTIFY_TSLOG);
		return TRUE;
		break;
	case WAIT_TIMEOUT:
		return FALSE;
		break;
	default:
		_hConfigFileChanged = FindFirstChangeNotification(__TTSLOG_CONFIG_DIR,  FALSE, FILE_NOTIFY_TSLOG);
		return FALSE;
		break;
	}
}
BOOL CTSLog::MakeDirectoryA(LPCSTR lpszDestDirA)
{
	LPSTR lpszHead = (LPSTR)lpszDestDirA;
	LPSTR lpszDir = (LPSTR)lpszDestDirA;
	CHAR szTempDir[_MAX_PATH] = {0};
	DWORD dwAttributes = 0;
	if (NULL == lpszDir)
		return FALSE;
	while ( '\0' != *(++lpszDir))
	{
		if ( '\\' == *lpszDir)
		{
			memcpy(szTempDir, lpszHead, (lpszDir - lpszHead) * sizeof(CHAR));
			szTempDir[lpszDir - lpszHead] = '\0';
			dwAttributes = GetFileAttributesA(szTempDir);
			if((DWORD)0xFFFFFFFF == dwAttributes || !(FILE_ATTRIBUTE_DIRECTORY & dwAttributes))
				if(!CreateDirectoryA(szTempDir, NULL))
					return FALSE;
		}
	}
	return TRUE;
}
BOOL CTSLog::MakeDirectory(LPCTSTR lpszDestPath) //no other version of MakeDirectoryA
{
	LPTSTR lpszHead = (LPTSTR)lpszDestPath;
	LPTSTR lpszDir = (LPTSTR)lpszDestPath;
	TCHAR szTempDir[_MAX_PATH] = {0};
	DWORD dwAttributes = 0;
	if (NULL == lpszDir)
		return FALSE;
	while ( '\0' != *(++lpszDir))
	{
		if ( '\\' == *lpszDir)
		{
			memcpy(szTempDir, lpszHead, (lpszDir - lpszHead) * sizeof(TCHAR));
			szTempDir[lpszDir - lpszHead] = '\0';
			dwAttributes = GetFileAttributes(szTempDir);
			if( INVALID_FILE_ATTRIBUTES == dwAttributes || !(FILE_ATTRIBUTE_DIRECTORY & dwAttributes))
				if(!CreateDirectory(szTempDir, NULL))
					return FALSE;
		}
	}
	return TRUE;
}
void CTSLog::ResetParams(BOOL bModuleInit)
{
#ifdef _DEBUG
	m_bDebugViewLog				= TRUE;
	m_bFileLog					= TRUE;
	m_eLevel						= LEVEL_TRACE;
#else
	m_bDebugViewLog				= FALSE;
	m_bFileLog					= FALSE;
	m_dwCurFileSize				= 0;
	if(INVALID_HANDLE_VALUE != m_hLogFile)
	{
		CloseHandle(m_hLogFile);
		m_hLogFile = INVALID_HANDLE_VALUE;
	}
#endif
	m_ghvInclude.Clear();
	m_ghvExclude.Clear();
	m_ghvInclude.Add(_T("*"));
	m_dwMaxLogFileSize			= DEFAULT_MAX_LOGFILE_SIZE;
	m_nMaxLogFileCnt			= DEFAULT_MAX_LOGFILE_COUNT;
	m_nMaxHistroyProcessCnt		= DEFAULT_MAX_HISTROY_PROCESS_COUNT;
	if(bModuleInit)
	{
		m_hLogFile					= INVALID_HANDLE_VALUE;
		_hConfigFileChanged			= INVALID_HANDLE_VALUE;
		m_nLogFileCnt				= 0;
		m_nFileLogLineCnt			= 0;		
		strncpy(m_szLogFileDirA, DEFAULT_LOGFILE_PATH, strlen(DEFAULT_LOGFILE_PATH) + 1);
	}
}
BOOL CTSLog::GetConfig(BOOL bModuleInit) //如果没有配置文件，将不打印日志，历史日志也没有必要删除
{	
	struct _stat stBuf;	
	if(0 != _tstat(__TTSLOG_CONFIG_FILE_PATH, &stBuf)) 
	{
		//TSLOG_GROUP.ini不存在
		if(0 != _tstat(__TTSLOG_CONFIG_FILE_PATH2, &stBuf))
		{
			//TSLOG_GROUP.txt也不存在			
			if(0 != _tstat(__TTSLOG_CONFIG_FILE_PATH_DEFAULT, &stBuf))
			{
				//tslog.ini不存在
				if(0 != _tstat(__TTSLOG_CONFIG_FILE_PATH_DEFAULT2, &stBuf))
				{
					//tslog.txt不存在
					ResetParams(FALSE);
					return FALSE;
				}
				else 
				{
					MoveFile(__TTSLOG_CONFIG_FILE_PATH_DEFAULT2, __TTSLOG_CONFIG_FILE_PATH_DEFAULT);
				}
			}
			CopyFile(__TTSLOG_CONFIG_FILE_PATH_DEFAULT, __TTSLOG_CONFIG_FILE_PATH, FALSE);
		}
		else
		{
			//存在tslog.txt不存在tslog.ini, 把 tslog.txt改名为tslog.ini
            MoveFile(__TTSLOG_CONFIG_FILE_PATH2, __TTSLOG_CONFIG_FILE_PATH);
		}
	}
	LPCTSTR pszConfigFileName = __TTSLOG_CONFIG_FILE_PATH;	
	BOOL bWriteConfig = FALSE;
	BOOL bLastFileLog = m_bFileLog;
	if((_off_t)0 == stBuf.st_size)
	{
		HANDLE hFile = CreateFile(pszConfigFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0 , NULL);
		if(INVALID_HANDLE_VALUE != hFile)			
		{
			DWORD dwByteWritten = 0;
			LPCSTR szBufferA = ";[Output]\r\n"
				";DebugView                  =ON     ;ON 打开 OFF 关闭                                                           默认关		\r\n"
				";FileLog                    =ON     ;ON 打开 OFF 关闭                                                           默认关		\r\n"
				"\r\n"
				";[Common]\r\n"
				";Level                      = TRACE   ; 共有TRACE,DEBUG,INFO,WARN,ERROR,FATAL                                   DEBUG		\r\n"
				";Include                    = *       ; *表示任意 打印包括*字符的日志                                           *			\r\n"
				";Exclude                    = a;b     ;           打印不包括\"a\" 和 \"b\" 字符的日志用\";\"分隔							\r\n"
				"\r\n"
				";[FileLogOption]\r\n"
				";LogFilePath这个参数若需运行时生效必须先设置 FileLog=OFF，保存当前文件，再设置 FileLog=ON， 保存当前文件，其它参数在保存后立即生效\r\n"
				";LogFilePath                = C:\\TSLOG\\   ;日志存放目录                                                          C:\\tslog\\\r\n"
				";MaxLogFileSize             = 20480       ;日志大小 20480KB                                                      20480		\r\n"
				";MaxLogFileCnt              = 20          ;当前进程日志文件最多数目                                              20			\r\n"
				";MaxCountOfHistroyProcess   = 10          ;只保留最新 10 个活动进程的所有日志                                    10			\r\n"
				"\r\n";
			WriteFile(hFile, szBufferA, (DWORD)strlen(szBufferA), &dwByteWritten, NULL);
			CloseHandle(hFile);
			bWriteConfig = TRUE;
		}
	}	
	TCHAR szReturnedString[2048] = {0};
	GetPrivateProfileString(_T("Output"), _T("DebugView"), _T("ON"), szReturnedString, sizeof(szReturnedString)/sizeof(TCHAR), pszConfigFileName);	//是否在DebugView中打印日志
	if(0 == _tcsicmp(szReturnedString, _T("ON")) || _tcsicmp(szReturnedString, _T("1")) == 0)
		m_bDebugViewLog = TRUE;
	else if(0 == _tcsicmp(szReturnedString, _T("OFF")) || 0 == _tcsicmp(szReturnedString, _T("0")))
		m_bDebugViewLog = FALSE;
	if(bWriteConfig)
		WritePrivateProfileString(_T("Output"), _T("DebugView"), m_bDebugViewLog ? _T("ON") : _T("OFF"), pszConfigFileName);
	GetPrivateProfileString(_T("Output"), _T("FileLog"), _T("ON"), szReturnedString, sizeof(szReturnedString)/sizeof(TCHAR), pszConfigFileName);	//是否在日志文件中打印日志
	if(0  == _tcsicmp(szReturnedString, _T("ON")) || 0 == _tcsicmp(szReturnedString, _T("1")))
		m_bFileLog = TRUE;
	else if(0 == _tcsicmp(szReturnedString, _T("OFF")) || 0 == _tcsicmp(szReturnedString, _T("0")))
		m_bFileLog = FALSE;
	if(bWriteConfig)
		WritePrivateProfileString(_T("Output"), _T("FileLog"), m_bFileLog ? _T("ON") : _T("OFF"), pszConfigFileName);	
	GetPrivateProfileString(_T("Common"), _T("Level"), _T("TRACE"), szReturnedString, sizeof(szReturnedString)/sizeof(TCHAR), pszConfigFileName);
	if(0 == _tcsicmp(szReturnedString, _T("TRACE")))
		m_eLevel = LEVEL_TRACE;
	else if(0 == _tcsicmp(szReturnedString, _T("DEBUG")))
		m_eLevel = LEVEL_DEBUG;
	else if(0 == _tcsicmp(szReturnedString, _T("INFO")))
		m_eLevel = LEVEL_INFO;
	else if(0 == _tcsicmp(szReturnedString, _T("WARN")))
		m_eLevel = LEVEL_WARN;
	else if(0 == _tcsicmp(szReturnedString, _T("ERROR")))
		m_eLevel = LEVEL_ERROR;
	else if(0 == _tcsicmp(szReturnedString, _T("FATAL")))
		m_eLevel = LEVEL_FATAL;
	if(bWriteConfig)
		WritePrivateProfileString(_T("Common"), _T("Level"), 
				(LEVEL_TRACE == m_eLevel) ? _T("TRACE") : 
			(LEVEL_DEBUG == m_eLevel) ? _T("DEBUG") : 
			(LEVEL_INFO == m_eLevel) ? _T("INFO") : 
			(LEVEL_WARN == m_eLevel) ? _T("WARN") : 
			(LEVEL_ERROR == m_eLevel) ? _T("ERROR") : 
			(LEVEL_FATAL == m_eLevel) ? _T("FATAL") : 
			_T("TRACE"), pszConfigFileName);
	//过滤项
	LPTSTR pToken = NULL;
	std::basic_string<TCHAR> strInclude;
	std::basic_string<TCHAR> strExclude;
	m_ghvInclude.Clear();
	GetPrivateProfileString(_T("Common"), _T("Include"), _T("*"), szReturnedString, sizeof(szReturnedString)/sizeof(TCHAR), pszConfigFileName);
	strInclude = szReturnedString;
#define FILT_SEP _T(";,|")
	pToken = _tcstok(szReturnedString, FILT_SEP); //以分号分隔	
	while(pToken != NULL)
	{
		if(0 == _tcsicmp(pToken, _T("*"))) //如果遇到"*", 把所有已加入的清除掉, 只要"*" 
		{
			m_ghvInclude.Clear();
			m_ghvInclude.Add(pToken);
			if(bWriteConfig)
				WritePrivateProfileString(_T("Common"), _T("Include"), _T("*"), pszConfigFileName);
			break;
		}
		m_ghvInclude.Add(pToken);
		pToken = _tcstok(NULL, FILT_SEP);
	}
	//滤除项
	m_ghvExclude.Clear();
	GetPrivateProfileString(_T("Common"), _T("Exclude"), _T(""), szReturnedString, sizeof(szReturnedString)/sizeof(TCHAR), pszConfigFileName);
	strExclude = szReturnedString;
	pToken = _tcstok(szReturnedString, _T(";")); //以分号分隔	
	if(NULL == pToken && bWriteConfig)		
		WritePrivateProfileString(_T("Common"), _T("Exclude"), _T(""), pszConfigFileName);
	while(pToken != NULL)
	{
		if(0 == _tcsicmp(pToken, _T("*"))) //如果遇到"*", 把所有已加入的清除掉, 只要"*"
		{
			m_ghvExclude.Clear();
			m_ghvExclude.Add(pToken);
			break;
		}
		m_ghvExclude.Add(pToken);
		pToken = _tcstok(NULL, FILT_SEP);
	}
	LPCTSTR pszLogFilePath = NULL; 
	LPCSTR pszConfigFileNameA = NULL;
#if (defined(UNICODE) || defined(_UNICODE))
	std::string strConfigFileName;
	UnicodeToMultiByte(pszConfigFileName,strConfigFileName );
	pszConfigFileNameA = strConfigFileName.c_str();
	std::basic_string<WCHAR>strLogFilePathW;
	MultiByteToUnicode(m_szLogFileDirA, strLogFilePathW);
	pszLogFilePath = strLogFilePathW.c_str();
#else
	pszConfigFileNameA = pszConfigFileName;
	pszLogFilePath = m_szLogFileDirA;
#endif
	if(m_bFileLog)
	{
		if(!bLastFileLog || 0 == m_dwAllLogFileCnt)//上次日志为关或第一次
		{
			GetPrivateProfileStringA(("FileLogOption"), ("LogFilePath"), DEFAULT_LOGFILE_PATH, m_szLogFileDirA, sizeof(m_szLogFileDirA), pszConfigFileNameA);		//日志文件路径
			if(0 == strlen(m_szLogFileDirA) || strlen(m_szLogFileDirA) >= sizeof(m_szLogFileDirA))
				strncpy(m_szLogFileDirA,  DEFAULT_LOGFILE_PATH, sizeof(m_szLogFileDirA));
			if(m_szLogFileDirA[strlen(m_szLogFileDirA)-1] != '\\')//如果没有"\", 加上
				strncat(m_szLogFileDirA,  ("\\"), sizeof(m_szLogFileDirA));
			s_dwLogFileCnt = DWORD(-1);
			if(!bModuleInit)
				PrintModuleInit();
		}		
		m_dwMaxLogFileSize	= GetPrivateProfileInt(_T("FileLogOption"), _T("MaxLogFileSize"), DEFAULT_MAX_LOGFILE_SIZE, pszConfigFileName);		//日志文件的最大尺寸
		if(m_dwMaxLogFileSize < MIN_MAX_LOGFILE_SIZE || m_dwMaxLogFileSize > MAX_MAX_LOGFILE_SIZE) //最大设置不能超过200M
		{
			m_dwMaxLogFileSize = DEFAULT_MAX_LOGFILE_SIZE;
			WritePrivateProfileString(_T("FileLogOption"), _T("MaxLogFileSize"),_T("20480"), pszConfigFileName);
		}
		m_nMaxLogFileCnt = GetPrivateProfileInt(_T("FileLogOption"), _T("MaxLogFileCnt"), DEFAULT_MAX_LOGFILE_COUNT, pszConfigFileName);		//日志文件的数量限制
		if(m_nMaxLogFileCnt < MIN_MAX_LOGFILE_COUNT || m_nMaxLogFileCnt > MAX_MAX_LOGFILE_COUNT) //最大设置不能超过100个文件
		{
			m_nMaxLogFileCnt = DEFAULT_MAX_LOGFILE_COUNT;
			WritePrivateProfileString(_T("FileLogOption"), _T("MaxLogFileCnt"),_T("20"), pszConfigFileName);
		}
		m_nMaxHistroyProcessCnt = GetPrivateProfileInt(_T("FileLogOption"), _T("MaxCountOfHistroyProcess"), DEFAULT_MAX_HISTROY_PROCESS_COUNT, pszConfigFileName);		//历史进程数目限制
		if(m_nMaxHistroyProcessCnt < MIN_MAX_HISTROY_PROCESS_COUNT || m_nMaxHistroyProcessCnt > MAX_MAX_HISTROY_PROCESS_COUNT) //最大设置不能超过100个文件
		{
			m_nMaxHistroyProcessCnt = DEFAULT_MAX_HISTROY_PROCESS_COUNT;
			WritePrivateProfileString(_T("FileLogOption"), _T("MaxCountOfHistroyProcess"),_T("10"), pszConfigFileName);
		}
		if(bWriteConfig)
		{
			WritePrivateProfileStringA(("FileLogOption"), ("LogFilePath"), m_szLogFileDirA, pszConfigFileNameA);		//日志文件路径
			if((DWORD)DEFAULT_MAX_LOGFILE_SIZE == m_dwMaxLogFileSize)			
				WritePrivateProfileString(_T("FileLogOption"), _T("MaxLogFileSize"),_T("20480"), pszConfigFileName);
			if((INT32)DEFAULT_MAX_LOGFILE_COUNT == m_nMaxLogFileCnt)			
				WritePrivateProfileString(_T("FileLogOption"), _T("MaxLogFileCnt"),_T("20"), pszConfigFileName);
			if((INT32)DEFAULT_MAX_HISTROY_PROCESS_COUNT == m_nMaxHistroyProcessCnt)			
				WritePrivateProfileString(_T("FileLogOption"), _T("MaxCountOfHistroyProcess"),_T("10"), pszConfigFileName);
		}
	}
	else
	{
		if(INVALID_HANDLE_VALUE != m_hLogFile)
		{
			CloseHandle(m_hLogFile);
			m_hLogFile = INVALID_HANDLE_VALUE;
			m_dwCurFileSize	= 0;
		}
	}
	if(!bModuleInit)
	{
		TSPrompt(_T("TSLog Config File : %s : \r\n00000000\t00:00:00.000\t[0000] [Output]\r\n00000000\t00:00:00.000\t[0000] DebugView=%s\r\n")
			_T("00000000\t00:00:00.000\t[0000] FileLog=%s\r\n00000000\t00:00:00.000\t[0000] [Common]\r\n00000000\t00:00:00.000\t[0000] Level=%s\r\n") 
			_T("00000000\t00:00:00.000\t[0000] Include=%s\r\n00000000\t00:00:00.000\t[0000] Exclude=%s\r\n00000000\t00:00:00.000\t[0000] [FileLogOption]\r\n")
			_T("00000000\t00:00:00.000\t[0000] LogFilePath=%s\r\n00000000\t00:00:00.000\t[0000] MaxLogFileSize=%d\r\n")
			_T("00000000\t00:00:00.000\t[0000] MaxLogFileCnt=%d\r\n00000000\t00:00:00.000\t[0000] MaxCountOfHistroyProcess=%d\r\n"),
			pszConfigFileName, 
			m_bDebugViewLog ? _T("ON") : _T("OFF"),
			m_bFileLog ? _T("ON") : _T("OFF"),
			(LEVEL_TRACE == m_eLevel) ? _T("TRACE") : (LEVEL_DEBUG == m_eLevel) ? _T("DEBUG") : (LEVEL_INFO == m_eLevel) ? _T("INFO") : 
		(LEVEL_WARN == m_eLevel) ? _T("WARN") : 	(LEVEL_ERROR == m_eLevel) ? _T("ERROR") : (LEVEL_FATAL == m_eLevel) ? _T("FATAL") : _T("TRACE"),
			strInclude.c_str(),
			strExclude.c_str(),
			pszLogFilePath,		
			m_dwMaxLogFileSize,
			m_nMaxLogFileCnt,
			m_nMaxHistroyProcessCnt
			);
		DeleteHistoryLogFiles(); //一个模块只保留最近m_nMaxHistroyProcessCnt个日志文件, 其余的文件删除
	}
	return TRUE;
}
void CTSLog::OutputLog(TLevel level, LPCTSTR pszFormat, va_list ap, LPCTSTR pszLogFilePath, BOOL bCheckFileChanged)
{
	//SectionProtect sp(& _cs);
	LPTSTR _pszCallFunName = NULL;
	if(bCheckFileChanged)
	{
		_pszCallFunName = (LPTSTR)TlsGetValue(s_dwTlsIndex);
		CheckConfigFileChanged();
		TlsSetValue(s_dwTlsIndex, _pszCallFunName);
	}
	if(!m_bDebugViewLog && !m_bFileLog) //
		return;
	if(level < m_eLevel)
		return;
	//#		Time	[PID] [TID] <模块名> 级别 信息\t\r\n
	//样例:
	//00000000\t00:00:00.000\t[012345] [012345] TRACE  <MyModule> User Msg\t\r\n
	TCHAR szWholeMsg[MAX_USERDATA_SIZE + 1] = {0};
	LPTSTR pWholeMsg = szWholeMsg;// pWholeMsg = szWholeMsg;
	LPTSTR pDebugViewMsg = NULL;
	LPTSTR pLevelMsg = NULL;
	LPTSTR pUserMsg = NULL;
	size_t  nDebugViewMsgoffset = 0;
	size_t  nLevelMsgoffset = 0;
	size_t  nUserMsgoffset = 0;
//	size_t  nLen = 0;
	//pWholeMsg = szWholeMsg; // new TCHAR[MAX_USERDATA_SIZE];
	//ZeroMemory(pWholeMsg, (MAX_USERDATA_SIZE + MAX_PRIVATEDATA_SIZE)*sizeof(TCHAR));
	//1. 加入前面的项, 即#		Time	[PID] [TID] 
	SYSTEMTIME nowtime = {0};
	static DWORD nCurProcessId = 0;
	if((DWORD)0 == nCurProcessId)
		nCurProcessId = (DWORD)GetCurrentProcessId();
	GetLocalTime(&nowtime);
	nDebugViewMsgoffset = _sntprintf(pWholeMsg, MAX_PRIVATEDATA_SIZE, _T("%08d\t%02d:%02d:%02d.%03d\t[%d] "),
		m_nFileLogLineCnt++,nowtime.wHour, nowtime.wMinute, nowtime.wSecond, nowtime.wMilliseconds, nCurProcessId );
	pDebugViewMsg = pWholeMsg + nDebugViewMsgoffset; //向OutputDebugString传递的字符串的首地址
	//2.加入 [TID] 
	nLevelMsgoffset = _sntprintf(pDebugViewMsg, MAX_PRIVATEDATA_SIZE, _T("[%d] "), GetCurrentThreadId());
	pLevelMsg = pDebugViewMsg + nLevelMsgoffset; //级别字符串的首地址
	//nUserMsgoffset =  _tcslen(_T("TRACE  ")) + _tcslen(GetCurrentrentModuleName()) + _tcslen(_T("<> "));
	//3. 加入日志级别和模块名
	//nLen = nUserMsgoffset;
	switch(level)
	{
	case LEVEL_TRACE:	nUserMsgoffset = _sntprintf(pLevelMsg, MAX_PRIVATEDATA_SIZE, _T("TRACE  <%s> "), GetCurrentModuleName());	break;
	case LEVEL_DEBUG:	nUserMsgoffset = _sntprintf(pLevelMsg, MAX_PRIVATEDATA_SIZE, _T("DEBUG  <%s> "), GetCurrentModuleName());	break;
	case LEVEL_INFO:	nUserMsgoffset = _sntprintf(pLevelMsg, MAX_PRIVATEDATA_SIZE, _T("INFO   <%s> "), GetCurrentModuleName());	break;
	case LEVEL_WARN:	nUserMsgoffset = _sntprintf(pLevelMsg, MAX_PRIVATEDATA_SIZE, _T("WARN   <%s> "), GetCurrentModuleName());	break;
	case LEVEL_ERROR:	nUserMsgoffset = _sntprintf(pLevelMsg, MAX_PRIVATEDATA_SIZE, _T("ERROR  <%s> "), GetCurrentModuleName());	break;
	case LEVEL_FATAL:	nUserMsgoffset = _sntprintf(pLevelMsg, MAX_PRIVATEDATA_SIZE, _T("FATAL  <%s> "), GetCurrentModuleName());	break;
	case LEVEL_PROMPT:	nUserMsgoffset = _sntprintf(pLevelMsg, MAX_PRIVATEDATA_SIZE, _T("PROMPT <%s> "), GetCurrentModuleName());	break;
	default:
		return;
	}
	//nUserMsgoffset = _tcslen(pLevelMsg);
	pUserMsg = pLevelMsg + nUserMsgoffset; //用户信息的字符串的首地址
	//4.增加打印  00000000\t00:00:00.000\t[012345] [012345] TRACE  <MyModule> [CALLFUNNAME]
	size_t nFunNameLen = 0;
	LPTSTR s_pszCallFunName = NULL;
	LPDWORD s_pdwThis = 0;
	pUserMsg[0] = _T('[');
	__try
	{
		s_pszCallFunName = bCheckFileChanged ? (LPTSTR)TlsGetValue(s_dwTlsIndex) : 0;
		s_pdwThis = bCheckFileChanged ? (LPDWORD) TlsGetValue(s_dwTlsIndex_this) : 0;
		if (NULL == s_pszCallFunName)
			nFunNameLen = 0;
		else
		{
			nFunNameLen = _tcslen(s_pszCallFunName);
			memcpy(&pUserMsg[1], s_pszCallFunName, (nFunNameLen) * sizeof(TCHAR));
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		nFunNameLen = _tcslen(_T("!!!"));
		memcpy(&pUserMsg[1], _T("!!!"), (nFunNameLen) * sizeof(TCHAR));
	}
	pUserMsg[nFunNameLen + 1] = _T(']');
	pUserMsg[nFunNameLen + 2] = _T(' ');
	pUserMsg += nFunNameLen + 3;
	if(NULL != s_pdwThis)	//
	{
		INT s_nThisLen = 0;
		DWORD s_dwThis = 0;
		__try
		{
			memcpy(&s_dwThis,&s_pdwThis, sizeof(DWORD));
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			s_dwThis =(DWORD)-1;
		}
		pUserMsg[0] = _T('[');
		_sntprintf(&pUserMsg[1],MAX_PRIVATEDATA_SIZE,_T("0x%08x"), s_dwThis);
		s_nThisLen = (INT)_tcslen(pUserMsg);
		pUserMsg[s_nThisLen] = _T(']');
		pUserMsg[s_nThisLen + 1] = _T(' ');
		pUserMsg += s_nThisLen + 2;
	}
#define FINALFLAG _T("\t\r\n")
//	size_t  nFinalFlagLen = _tcslen(_T("\t\r\n"));
	__try
	{
		if(NULL == ap)
			nUserMsgoffset = _sntprintf(pUserMsg, MAX_USERDATA_SIZE - (pUserMsg-pWholeMsg) - 64, _T("%s"), pszFormat);
		else
			nUserMsgoffset = _vsntprintf(pUserMsg, MAX_USERDATA_SIZE - (pUserMsg-pWholeMsg) - 64, pszFormat, ap);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		nUserMsgoffset = _sntprintf(pUserMsg, MAX_USERDATA_SIZE - (pUserMsg-pWholeMsg) - 64, _T("(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x"), pszFormat, GetExceptionCode());
	}
	if((size_t)-1 == nUserMsgoffset)
		nUserMsgoffset = _tcslen(pUserMsg);
	LPTSTR pEnd = pUserMsg + nUserMsgoffset - 1;
	while( '\t' == *pEnd ||  '\r' == *pEnd || '\n' == *pEnd)//清除最后的制表符, 回车和换行
		*pEnd-- = 0;
	//_tcsncat(++pEnd, FINALFLAG , nFinalFlagLen); //在后面加\t\r\n
	LARGE_INTEGER li = {0};
	li.QuadPart = GetTickCount64();
	nUserMsgoffset = _sntprintf(++pEnd, 32, _T("\t'%hu '%hu%s"), HIWORD(li.LowPart), LOWORD(li.LowPart), FINALFLAG); 
	pEnd += nUserMsgoffset;
	//_tcsncat(++pEnd, FINALFLAG , nFinalFlagLen); //在后面加\t\r\n
	//5. 过滤, 可能不用打印
	if(LEVEL_PROMPT != level) //只有用户信息才需要过滤, 如果是提示信息, 则不用过滤, 提示信息必须打印
	{
		if(!m_ghvInclude.Find(_T("*")) && !m_ghvInclude.SubFind(pDebugViewMsg))
			return;
		if(m_ghvExclude.Find(_T("*")) || m_ghvExclude.SubFind(pDebugViewMsg))
			return;
	}
	if(m_bDebugViewLog)
		OutputDebugString(pDebugViewMsg);
	if(m_bFileLog)
		WriteToLogFile(level, pWholeMsg, DWORD(pEnd - pWholeMsg), pszLogFilePath);
}
UINT CTSLog::GetStringCodePage(const char* szData)
{
	UINT code_page = 0;//default value
	if(NULL == szData)
		return code_page;
	IMultiLanguage2*  pMLang = NULL;
	HRESULT hr = S_FALSE;
	HRESULT hresult = S_FALSE;    
	int srclen = (int)strlen(szData); 
	DetectEncodingInfo dei[8] = {0};		
	int dei_num = 8;
	INT confidence = 0;
	CoInitialize(NULL);
	__try
	{
		hr = CoCreateInstance(  CLSID_CMultiLanguage, NULL,CLSCTX_INPROC_SERVER,
			IID_IMultiLanguage2, (void**)&pMLang ); //
		if(SUCCEEDED(hr) && pMLang)
		{
			hresult = pMLang->DetectInputCodepage(MLDETECTCP_NONE, 0 , (char *)szData, &srclen , dei , &dei_num);  
			if (SUCCEEDED(hresult))
			{
				confidence = 50;
				for (int i = 0 ; i < dei_num; i++)
				{
					code_page = dei[i].nCodePage;
					if(936 == code_page)
						break;
					else if(confidence < dei[i].nConfidence)
					{
						confidence = dei[i].nConfidence;						
					}
				}
			}
			pMLang->Release();
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		s_pThis->TSPrompt(_T(" IMultiLanguage2 Fatal. code_page = %lu, datalen = %d, CoCreateInstance = %lu, pMLang = %lu, dei_num = %d, confidence = %d, DetectInputCodepage = %d "), 
			code_page, srclen, hr, pMLang, dei_num, confidence, hresult);
	}
	CoUninitialize();
	return code_page;
}
void CTSLog::WriteToLogFile(TLevel level, LPCTSTR pszFileLogMsg, DWORD dwLen, LPCTSTR pszLogFilePath)
{	
	if(NULL != pszLogFilePath)
		if(INVALID_HANDLE_VALUE == s_hLogFile)
			if(MakeDirectory(pszLogFilePath))
				s_hLogFile = CreateFile(pszLogFilePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0 , NULL);
	//该进程第一次打文件日志, 还没有打开日志文件
	if(m_hLogFile == INVALID_HANDLE_VALUE)
	{
		//从日志中得到模块名
		DWORD dwAttri = GetFileAttributesA(m_szLogFileDirA);
		if(dwAttri != INVALID_FILE_ATTRIBUTES && dwAttri & FILE_ATTRIBUTE_DIRECTORY) //目录存在
			;
		else //目录不存在, 创建目录
		{
			if(!MakeDirectoryA(m_szLogFileDirA))
			{
				if(!GetTempPathA(_MAX_PATH, m_szLogFileDirA))
					return;
#if (defined(UNICODE) || defined(_UNICODE))
				_snprintf(m_szLogFileDirA, _MAX_PATH, "%s%S\\", m_szLogFileDirA, __TTSLOG_GROUP);
#else
				_snprintf(m_szLogFileDirA, _MAX_PATH, "%s%s\\", m_szLogFileDirA, __TTSLOG_GROUP);
#endif
				if(!MakeDirectoryA(m_szLogFileDirA))
					return;
			}
		}
		//构造完整路程加文件名
		TCHAR szLogFilePathName[MAX_PATH] = {0};
		GetNthFilePathName(++m_nLogFileCnt, szLogFilePathName, NULL);
		m_dwAllLogFileCnt++; 
		//打开日志文件
		m_hLogFile =  CreateFile(szLogFilePathName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0 , NULL);
		if(INVALID_HANDLE_VALUE == m_hLogFile)
		{
			DWORD dwErrCode = GetLastError();
			if((DWORD)ERROR_DISK_FULL == dwErrCode)
			{
				m_bFileLog = FALSE;
				WritePrivateProfileString(_T("Output"), _T("FileLog"), _T("OFF"), __TTSLOG_CONFIG_FILE_PATH);
#ifdef _DEBUG				
				MessageBox(NULL, _T("Disk is full."), s_szModuleFileName, MB_OK);
#endif
				return;
			}
			else if((DWORD)ERROR_ACCESS_DENIED == dwErrCode)
			{
				//尝试在临时目录下建文件
				if(!GetTempPathA(_MAX_PATH, m_szLogFileDirA))
					return;				
#if (defined(UNICODE) || defined(_UNICODE))
				_snprintf(m_szLogFileDirA, _MAX_PATH, "%sTSLOG\\%S\\", m_szLogFileDirA, __TTSLOG_GROUP);
#else
				_snprintf(m_szLogFileDirA, _MAX_PATH, "%sTSLOG\\%s\\", m_szLogFileDirA, __TTSLOG_GROUP);
#endif
				GetNthFilePathName(m_nLogFileCnt, szLogFilePathName, NULL);
				m_hLogFile =  CreateFile(szLogFilePathName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0 , NULL);
				if(INVALID_HANDLE_VALUE == m_hLogFile)
					return;				
			}
			else
				return;
			
		}
	}
	DWORD dwByteWritten = 0;
#if (defined(UNICODE) || defined(_UNICODE))
	//可以先把Unicode字符串转成ANSI字符串
	CHAR szBufferA[MAX_USERDATA_SIZE + MAX_PRIVATEDATA_SIZE + 1] = {0};	//调用频繁
	INT nLen = 0;
	nLen = WideCharToMultiByte(936, 0, pszFileLogMsg, -1,NULL, 0 ,NULL, NULL);
	if(0 >= nLen)
		return;
	else if(MAX_USERDATA_SIZE + MAX_PRIVATEDATA_SIZE  < nLen)
		return;
	dwLen = WideCharToMultiByte(936, 0, pszFileLogMsg, -1, szBufferA, nLen,NULL,NULL);	
	if(0 >= dwLen)
		return;
	dwLen--;
	if(MAX_USERDATA_SIZE + 1 < dwLen)
		dwLen = MAX_USERDATA_SIZE + 1;
	if(INVALID_HANDLE_VALUE != s_hLogFile)
	{
		SetFilePointer(s_hLogFile,0, 0, FILE_END);
		WriteFile(s_hLogFile, szBufferA, dwLen, &dwByteWritten, NULL);
		CloseHandle(s_hLogFile);
		s_hLogFile = INVALID_HANDLE_VALUE;
	}
	//磁盘满停止打印
	if(!WriteFile(m_hLogFile, szBufferA, dwLen, &dwByteWritten, NULL))
	{
		if((DWORD)ERROR_DISK_FULL == GetLastError())
		{
			m_bFileLog = FALSE;
			WritePrivateProfileString(_T("Output"), _T("FileLog"), _T("OFF"), __TTSLOG_CONFIG_FILE_PATH);
		}
		return;
	}
#else
	if(INVALID_HANDLE_VALUE != s_hLogFile)
	{
		SetFilePointer(s_hLogFile,0, 0, FILE_END);
		WriteFile(s_hLogFile,pszFileLogMsg, dwLen, &dwByteWritten, NULL);
		CloseHandle(s_hLogFile);
		s_hLogFile = INVALID_HANDLE_VALUE;
	}
	//磁盘满停止打印
	if(!WriteFile(m_hLogFile, pszFileLogMsg, dwLen, &dwByteWritten, NULL))
	{
		if((DWORD)ERROR_DISK_FULL == GetLastError())
		{
			m_bFileLog = FALSE;
			WritePrivateProfileString(_T("Output"), _T("FileLog"), _T("OFF"), __TTSLOG_CONFIG_FILE_PATH);
		}
		return;
	}
#endif
	//模块首次打印日志 这个不能置前,要不然会冲掉上一次打印的。
	if(s_dwLogFileCnt < m_dwAllLogFileCnt)
	{
		s_dwLogFileCnt = m_dwAllLogFileCnt;
		PrintModuleInit();
	}
	else if(s_dwLogFileCnt > m_dwAllLogFileCnt)
		s_dwLogFileCnt = m_dwAllLogFileCnt;

	//如果文件大小超过指定大小, 则把之前所有的日志文件改名
	//static volatile DWORD dwSize = 0;	//GetFileSize(m_hLogFile, NULL);
	if(LEVEL_PROMPT != level)
		m_dwCurFileSize += dwByteWritten; 
	if(m_dwCurFileSize >= m_dwMaxLogFileSize * 1 * KBTYE) //m_dwMaxLogSize的单位是KB
	{
		m_dwCurFileSize = 0;
		FlushFileBuffers(m_hLogFile);
		CloseHandle(m_hLogFile);
		TCHAR szLogFilePathName[MAX_PATH] = {0};
		memset(szLogFilePathName, 0x00, sizeof(szLogFilePathName));
		if (m_nLogFileCnt >= m_nMaxLogFileCnt)	//20 - 20 = 1 >= 0
		{
			GetNthFilePathName(m_nLogFileCnt - m_nMaxLogFileCnt + 1, szLogFilePathName, NULL); //20 - 20 + 1 =1
			DeleteFile(szLogFilePathName); // del 1.log , next create 21.log
		}
		//构造完整路程加文件名
		//TCHAR szLogFilePathName[MAX_PATH] = {0};
		memset(szLogFilePathName, 0x00, sizeof(szLogFilePathName));
		GetNthFilePathName(m_nLogFileCnt,  NULL, szLogFilePathName);
		if(MAX_LOGFILE_INDEX <= ++m_nLogFileCnt)
			m_nLogFileCnt = 0;
		m_dwAllLogFileCnt++;
		//打开日志文件
		m_hLogFile =  CreateFile(szLogFilePathName, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL,CREATE_ALWAYS, 0,NULL);
		s_dwLogFileCnt = m_dwAllLogFileCnt;
		if(m_hLogFile)
			PrintModuleInit();
	}
}
BOOL CTSLog::GetNthFilePathName(INT nFileCnt, LPTSTR pOleFilePathName, LPTSTR pNewFilePathName)
{
	//pOleFilePathName和pNewFilePathName可以有一个为NULL, 表示不关心该项, 但不能同时为NULL
	if(NULL == pOleFilePathName && NULL == pNewFilePathName)
		return FALSE;
	DWORD dwPID = GetCurrentProcessId();
	LPCTSTR pszCurModuleName = GetCurrentModuleName();
	TCHAR szModuleFileName[_MAX_PATH] = {0};
	TCHAR szdrive[_MAX_DRIVE] = {0};
	TCHAR szdir[_MAX_DIR] = {0};
	TCHAR szfname[_MAX_FNAME] = {0};
	TCHAR szext[_MAX_EXT] = {0};
	GetModuleFileName(NULL,szModuleFileName , _MAX_PATH);
	_tsplitpath(szModuleFileName, szdrive, szdir, szfname, szext);
	LPCTSTR pszLogFilePath = NULL;
#if defined(UNICODE) || defined(_UNICODE)
	std::basic_string<WCHAR> strLogFilePathW;
	MultiByteToUnicode(m_szLogFileDirA, strLogFilePathW);
	pszLogFilePath = strLogFilePathW.c_str();
#else
	pszLogFilePath = m_szLogFileDirA;
#endif
	if(pOleFilePathName != NULL)
	{
		if(0 == nFileCnt) //no reach
			_sntprintf(pOleFilePathName, _MAX_PATH, _T("%s\\%s[%d].log"), pszLogFilePath, pszCurModuleName, dwPID); //no reach
		else
			_sntprintf(pOleFilePathName, _MAX_PATH, _T("%s\\%s.%s[%d].%03d.log"), pszLogFilePath, __TTSLOG_GROUP,szfname , dwPID, nFileCnt);
	}
	if(pNewFilePathName != NULL)
		_sntprintf(pNewFilePathName, _MAX_PATH,  _T("%s\\%s.%s[%d].%03d.log"), pszLogFilePath, __TTSLOG_GROUP, szfname, dwPID,nFileCnt+1);
	return TRUE;
}
BOOL CTSLog::DeleteHistoryLogFiles()
{
	WIN32_FIND_DATA wfd;
	HANDLE hfh = INVALID_HANDLE_VALUE;			//find handle
	//找到该模块对应的所有日志文件, 压入vector	//每个list条目包含文件全路径和最后修改时间	//MAPPID2FILEINFO MathFile;
	std::vector<PAIRPID2FILEINFO> MathFile;
	std::vector<PAIRPID2FILEINFO>::iterator iter;
	INT nPid = 0;
	LPCTSTR pszLogFilePath = NULL;
#if (defined(UNICODE) || defined(_UNICODE))
	std::basic_string<WCHAR> strLogFilePathW;
	MultiByteToUnicode(m_szLogFileDirA, strLogFilePathW);
	pszLogFilePath = strLogFilePathW.c_str();
#else
	pszLogFilePath = m_szLogFileDirA;
#endif
	TCHAR szPathWildcard[MAX_PATH] = {0};
	if(-1 == m_nMaxHistroyProcessCnt)
	{
		_sntprintf(szPathWildcard, sizeof(szPathWildcard)/sizeof(TCHAR), _T("%s*.log"),
			pszLogFilePath);//可能有Module[4048].log和Module[4048].1.log		
	}
	else
	{
		_sntprintf(szPathWildcard, sizeof(szPathWildcard)/sizeof(TCHAR), _T("%s%s.%s[*].*.log"),
			pszLogFilePath, __TTSLOG_GROUP,  GetCurrentProcessName());//可能有Module[4048].log和Module[4048].1.log
	}
	
	hfh = FindFirstFile(szPathWildcard, &wfd);
	FileInfo fileinfo;
	FileInfo* pfileinfo = NULL;
	std::basic_string<TCHAR> strPathName;
	if(hfh != INVALID_HANDLE_VALUE)
	{
		do
		{
			strPathName = pszLogFilePath;
			strPathName += wfd.cFileName;
			if(-1 == m_nMaxHistroyProcessCnt)
			{
				DeleteFile(strPathName.c_str());
				continue;
			}			
			nPid = FindPidFromLogFilePath(strPathName.c_str());
			if(0 > nPid)
				continue;
			iter = MathFile.begin();
			while(iter != MathFile.end())
			{
				if (nPid == iter->first)
				{
					pfileinfo = &(iter->second);
					if(pfileinfo->stFileTime_.dwHighDateTime > wfd.ftLastWriteTime.dwHighDateTime)
					{
						pfileinfo->strFilePath_ = strPathName;
						pfileinfo->stFileTime_ = wfd.ftLastWriteTime;
					}
					else if(pfileinfo->stFileTime_.dwHighDateTime == wfd.ftLastWriteTime.dwHighDateTime &&
						pfileinfo->stFileTime_.dwLowDateTime >  wfd.ftLastWriteTime.dwLowDateTime)		//与上同,方便阅读
					{
						pfileinfo->strFilePath_ = strPathName;
						pfileinfo->stFileTime_ = wfd.ftLastWriteTime;
					}
					//直接返回
					break;
				}
				iter++;
			}
			if(iter == MathFile.end())
			{
				fileinfo.strFilePath_ = strPathName;
				fileinfo.stFileTime_ = wfd.ftLastWriteTime;
				MathFile.push_back(std::make_pair(nPid, fileinfo));
			}
		}while(FindNextFile(hfh, &wfd));
	}
	//else 没有找到匹配的文件
	FindClose(hfh);
	INT32 nCurrentSize =(INT32)MathFile.size();
	INT32 nSubSize = (INVALID_HANDLE_VALUE == m_hLogFile) ? 0 : -1;
	TSPrompt(_T("DeleteLogFile, MaxCountOfHistroyProcess : %d, CurrentHistroyProcess : %d, CurrentLogFileHandle : 0x%08x, PathWildcard : %s"), 
		m_nMaxHistroyProcessCnt, nCurrentSize + nSubSize, m_hLogFile, szPathWildcard);
	if((INT32)nCurrentSize + nSubSize > m_nMaxHistroyProcessCnt) //删除对应进程的全有日志
	{
		sort(MathFile.begin(), MathFile.end(), FileCmp);		//先按最后修改时间排序
		for(INT32 i = 0; i < nCurrentSize + nSubSize - m_nMaxHistroyProcessCnt; i++)
		{
			TSPrompt(_T("DeleteLogFiles, AllFilesOfPid : %lu(0x%08x)"),  MathFile[i].first, MathFile[i].first);
			DeleteAllFilesOfPid( MathFile[i].first);					//删除对应进程的全有日志
		}
	}
	MathFile.clear();
	return TRUE;
}
void CTSLog::DeleteAllFilesOfPid(INT nPid)
 {
	LPCTSTR pszLogFilePath = NULL;
#if (defined(UNICODE) || defined(_UNICODE))
	 std::basic_string<WCHAR> strLogFilePathW;
	 MultiByteToUnicode(m_szLogFileDirA, strLogFilePathW);
	 pszLogFilePath = strLogFilePathW.c_str();
#else
	 pszLogFilePath = m_szLogFileDirA;
#endif
	 TCHAR szPathWildcard[MAX_PATH] = {0};
	_sntprintf(szPathWildcard, sizeof(szPathWildcard)/sizeof(TCHAR), _T("%s%s.%s[%d].*.log"),
		pszLogFilePath, __TTSLOG_GROUP,  GetCurrentProcessName(), nPid);//可能有Module[4048].log和Module[4048].1.log
 	HANDLE hfh = INVALID_HANDLE_VALUE;			//find handle
	WIN32_FIND_DATA wfd;
	hfh = FindFirstFile(szPathWildcard, &wfd);
	std::basic_string<TCHAR> strPathName;
	if(hfh != INVALID_HANDLE_VALUE)
	{
		do
		{
			strPathName = pszLogFilePath;			
			strPathName += wfd.cFileName;
            if(!DeleteFile(strPathName.c_str()))
				TSPrompt(_T("DeleteLogFile Failed, ErrorCode : %d(0x%08x), LogFilePath : %s"), GetLastError(), GetLastError(), strPathName.c_str());
		}while(FindNextFile(hfh, &wfd));
	}
	FindClose(hfh);
 }
INT CTSLog::FindPidFromLogFilePath(LPCTSTR pszFilePath)
{
	 LPTSTR pszTemp = (LPTSTR)pszFilePath;
	 TCHAR szPID[9] = {0};
	 if (NULL == pszFilePath)
		 return -1;
	 size_t size = _tcslen(pszFilePath);
	 INT nStartPos = 0;
	 INT nEndPos = 0;
	 while (0 < size)
	 {
		 size--;
		 if(']' == pszTemp[size])
			 nEndPos = (INT)size;
		 else if('[' == pszTemp[size])
			 nStartPos = (INT)size;
	 }
	 if((0 != nStartPos ) && (0 != nEndPos) && (nEndPos - nStartPos < 9))
	 {
		 memcpy(szPID,&pszTemp[nStartPos + 1], (nEndPos - nStartPos - 1) * sizeof(TCHAR));
		 return _tstoi(szPID);
	 }
	 return -1;
 }
BOOL CTSLog::InstallTSLogIni(LPCTSTR szSrcIniAbsolutePath)
{
	SectionProtect sp(& _cs);
	struct _stat stSrcbuf;
	struct _stat stDestbuf;
	time_t tTime = time(NULL);
	struct tm* ptmTime = localtime(&tTime);
	if(0 != _tstat(szSrcIniAbsolutePath, &stSrcbuf))
	{
		TSPrompt(_T("Source File %s don't find."),szSrcIniAbsolutePath);
		return FALSE;
	}
	if(0 == _tstat(__TTSLOG_CONFIG_FILE_PATH, &stDestbuf))
	{
		if(stSrcbuf.st_mtime <= stDestbuf.st_mtime) //只针对有更新文件时，才复制
		{
			TSPrompt(_T("Don't Update TSLog Config File. Source File %s Last Modify Time = %ld, Target File %s Last Modify Time = %ld"),szSrcIniAbsolutePath , stSrcbuf.st_mtime, __TTSLOG_CONFIG_FILE_PATH, stDestbuf.st_mtime );
			return FALSE;
		}
		TCHAR szTargetFilePath[_MAX_PATH] = {0};
		TCHAR szTime[40] = {0};
		_sntprintf(szTime, sizeof(szTime) / sizeof(TCHAR), _T("%4d-%02d-%02d") , 1900 + ptmTime->tm_year, 1 + ptmTime->tm_mon ,ptmTime->tm_mday);
		_sntprintf(szTargetFilePath, sizeof(szTargetFilePath)/ sizeof(TCHAR), _T("%s(%s)"), __TTSLOG_CONFIG_FILE_PATH, szTime);
		if(INVALID_FILE_ATTRIBUTES != GetFileAttributes(szTargetFilePath))
			DeleteFile(szTargetFilePath);
		_trename(__TTSLOG_CONFIG_FILE_PATH, szTargetFilePath);
		TSPrompt(_T("Rename Old ConfigFile %s to Other File %s, ErrorCode : %d(0x%08x)"),__TTSLOG_CONFIG_FILE_PATH , szTargetFilePath, GetLastError());
	}
	else	//路径不存在，重建
	{
		LPSTR pszConfigDirA = NULL;
#if defined(UNICODE) || defined(_UNICODE)
		std::string strConfigDir;
		UnicodeToMultiByte(__TTSLOG_CONFIG_DIR, strConfigDir);
		pszConfigDirA = (LPSTR)strConfigDir.c_str();
#else
		pszConfigDirA = __TTSLOG_CONFIG_DIR;
#endif
		if(!MakeDirectoryA(pszConfigDirA))
		{
			TSPrompt(_T("Fail to Make Directory %s"), __TTSLOG_CONFIG_DIR);
			return FALSE;
		}
	}
	BOOL bRet = CopyFile(szSrcIniAbsolutePath, __TTSLOG_CONFIG_FILE_PATH, FALSE );
	TSPrompt(_T("Copy Source File %s to Target File %s, ErrorCode : %d(0x%08x)"),szSrcIniAbsolutePath, __TTSLOG_CONFIG_FILE_PATH, GetLastError());
	return bRet;
}
void CTSLog::TSPrompt(LPCTSTR pszFormat, ...) //private
{
	TlsSetValue(s_dwTlsIndex, NULL);
	TlsSetValue(s_dwTlsIndex_this, NULL);
	va_list ap; 
	va_start(ap, pszFormat);
	OutputLog(LEVEL_PROMPT, pszFormat, ap,NULL, FALSE);
	va_end(ap);	
}
void CTSLog::TSLog(LPCTSTR pszLogFilePath,LPCTSTR pszFormat, ...)
{
	SectionProtect sp(& _cs); 
	if(NULL == pszLogFilePath && INVALID_HANDLE_VALUE != _hConfigFileChanged)
	{
		//not call by GetConfig, if (NULL == _hConfigFileChanged) output prompt log.
		static TCHAR szLogFilePath[_MAX_PATH] = {0};		
		LPCTSTR pszProcessName = NULL;
		if(NULL == pszProcessName)
			pszProcessName = GetCurrentModuleName();
		if('\0'== szLogFilePath[0])
			_sntprintf(szLogFilePath, _MAX_PATH, _T("%s%s.%s.log"), __TDEFAULT_LOGFILE_PATH, __TTSLOG_GROUP, pszProcessName);	
		pszLogFilePath = szLogFilePath;
	}
	va_list ap; 
	va_start(ap, pszFormat);
	OutputLog(LEVEL_PROMPT, pszFormat, ap,pszLogFilePath, NULL != pszLogFilePath);
	va_end(ap);	 
}
#pragma warning(push)
#pragma warning(disable : 4702 )
void CTSLog::TSHexDump(LPCVOID p, ULONG uSize, LPCTSTR pszPrompt, TLevel level)
{
#define MAX_HEX_DUMP_COUNT 16
#define MAX_ALL_HEX_DUMP_SIZE MAX_HEX_DUMP_COUNT * MAX_HEX_DUMP_SIZE
	if(uSize > MAX_ALL_HEX_DUMP_SIZE)
	{
		TSWarn(_T("Current HexDump is %d, Max HexDump Size is %d, Some of the Excess will be Cut."),uSize, MAX_ALL_HEX_DUMP_SIZE);
		uSize = MAX_ALL_HEX_DUMP_SIZE;		
	}
	EnterCriticalSection(& _cs);
	ULONG uNextSize = 0;
	static BYTE* pNext = NULL;
	static INT nIndex = 1;
	static INT nCount = 1;
	//va_list ap = (va_list)_ADDRESSOF(p);
	if(NULL == pNext)
	{
		nIndex = 1;
		nCount = (uSize - 1) / (MAX_HEX_DUMP_SIZE) + 1;
		TCHAR szDumpHead[MAX_HEX_DUMP_SIZE] = {0};
		_sntprintf(szDumpHead, MAX_HEX_DUMP_SIZE, _T("TSLOG MEMORY HEXDUMP Begin [%p], Total Lenght : [%lu] bytes, Prompt : %s"), p, uSize, pszPrompt);
		OutputLog(level, szDumpHead, NULL);
	}
	if(MAX_HEX_DUMP_SIZE < uSize)
	{
		uNextSize = uSize - (MAX_HEX_DUMP_SIZE);
		uSize = MAX_HEX_DUMP_SIZE;
		pNext = (BYTE*)p + uSize;		
	}
	else
	{
		uNextSize = 0;
		pNext = NULL;		
	}
	//static MEMORY_BASIC_INFORMATION info;
	//static SIZE_T dwBufferSize = 0;
	//dwBufferSize = VirtualQuery(p, &info, (SIZE_T)uSize);	
	static TCHAR szHex[MAX_PRIVATEDATA_SIZE + MAX_USERDATA_SIZE + 1] = {0};	
	ULONG cbWLineCount = 0;
	ULONG cbFLineSize = 0;
	ULONG cbDataSize = 0;
	ULONG i = 0;
	BYTE* pbyData = NULL;
	pbyData = (BYTE*)p;
	BYTE* pszData = NULL;
	pszData = (BYTE*)p;	
	cbWLineCount = uSize / 16;
	cbFLineSize  = uSize % 16;
	cbDataSize = 0;
	cbDataSize += _sntprintf(&szHex[cbDataSize], MAX_PRIVATEDATA_SIZE, _T("HEXDUMP As Follows(%d/%d): \r\n00000000\t00:00:00.000\t[0000] %s - HEXDUMP %d(0x%08x) bytes from [0x%p]\r\n"), nIndex++, nCount,
								pszPrompt ? pszPrompt : __TTSLOG_GROUP, uSize, uSize, p);
#define EXCEPTION_HANDLE() if(0 < uNextSize) { uNextSize = 0; pNext = NULL;} goto output
#define TOPRINTCHAR(X)  isprint((int)X) && X != '%' ? X : '.'
	for (i = 0; i < cbWLineCount; i++, pbyData += 16,pszData += 16)
	{
		__try
		{
			cbDataSize += _sntprintf(&szHex[cbDataSize], MAX_PRIVATEDATA_SIZE + MAX_USERDATA_SIZE - cbDataSize, 
				_T("00000000\t00:00:00.000\t[0000] 0x%p %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %3s %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\r\n"),pbyData,
				pbyData[0],pbyData[1],pbyData[2],pbyData[3],pbyData[4],pbyData[5],pbyData[6],pbyData[7],
				pbyData[8],pbyData[9],pbyData[10],pbyData[11],pbyData[12],pbyData[13],pbyData[14],pbyData[15],_T(""),
				TOPRINTCHAR(pszData[0]),TOPRINTCHAR(pszData[1]),TOPRINTCHAR(pszData[2]),TOPRINTCHAR(pszData[3]),
				TOPRINTCHAR(pszData[4]),TOPRINTCHAR(pszData[5]),TOPRINTCHAR(pszData[6]),TOPRINTCHAR(pszData[7]),
				TOPRINTCHAR(pszData[8]),TOPRINTCHAR(pszData[9]),TOPRINTCHAR(pszData[10]),TOPRINTCHAR(pszData[11]),
				TOPRINTCHAR(pszData[12]),TOPRINTCHAR(pszData[13]),TOPRINTCHAR(pszData[14]),TOPRINTCHAR(pszData[15])
				);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			cbDataSize += _sntprintf(&szHex[cbDataSize], MAX_PRIVATEDATA_SIZE + MAX_USERDATA_SIZE - cbDataSize , _T("00000000\t00:00:00.000\t[0000] (!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x"), pbyData, GetExceptionCode());
			EXCEPTION_HANDLE();
			break;
		}
	}		
	for (i = 0, pbyData = (BYTE*)p + 16 * cbWLineCount; i < cbFLineSize; i++, pbyData++)
	{
		if(0 == i)
			cbDataSize +=_sntprintf(&szHex[cbDataSize], MAX_PRIVATEDATA_SIZE + MAX_USERDATA_SIZE - cbDataSize, _T("00000000\t00:00:00.000\t[0000] 0x%p "), pbyData);
		__try
		{
			cbDataSize += _sntprintf(&szHex[cbDataSize], MAX_PRIVATEDATA_SIZE + MAX_USERDATA_SIZE - cbDataSize, _T("%02x "), pbyData[0]);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			cbDataSize += _sntprintf(&szHex[cbDataSize], MAX_PRIVATEDATA_SIZE + MAX_USERDATA_SIZE - cbDataSize, _T("(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x"), pbyData, GetExceptionCode());
			EXCEPTION_HANDLE();
			break;
		}
	}
	if(0 < cbFLineSize)
	{
		TCHAR szFormat[8];
		_sntprintf(szFormat, 7, _T("%%%ds "), (16 - cbFLineSize) * 3 + 3);
		cbDataSize += _sntprintf(&szHex[cbDataSize], MAX_PRIVATEDATA_SIZE + MAX_USERDATA_SIZE - cbDataSize, szFormat, _T(""));
	}
	for (i = 0, pszData = (BYTE*)p + 16 * cbWLineCount; i < cbFLineSize; i++, pszData++)
	{
		__try
		{
			cbDataSize += _sntprintf(&szHex[cbDataSize], MAX_PRIVATEDATA_SIZE + MAX_USERDATA_SIZE - cbDataSize, _T("%c"), TOPRINTCHAR(pszData[0]));
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			cbDataSize += _sntprintf(&szHex[cbDataSize], MAX_PRIVATEDATA_SIZE + MAX_USERDATA_SIZE - cbDataSize, _T("(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x"), pbyData, GetExceptionCode());
			EXCEPTION_HANDLE();
			break;
		}
	}
output:
	OutputLog(level, szHex);
//	va_end(ap);
	if(0 < uNextSize)
		TSHexDump(pNext, uNextSize, pszPrompt, level);
	LeaveCriticalSection(& _cs);	
}
#pragma warning(pop)
bool CTSLog::FileCmp(const PAIRPID2FILEINFO &val1, const PAIRPID2FILEINFO &val2)
{
	if(val1.second.stFileTime_.dwHighDateTime != val2.second.stFileTime_.dwHighDateTime)
		return val1.second.stFileTime_.dwHighDateTime < val2.second.stFileTime_.dwHighDateTime;
	else
		return val1.second.stFileTime_.dwLowDateTime < val2.second.stFileTime_.dwLowDateTime;
}
#ifndef TSLOG_SIMPLE
BOOL CTSLog::GetParamTypeAndLen( LPCSTR pszRaw, PARAM_TYPE& pt, INT& nLen)
{
	LPSTR pData = (LPSTR)pszRaw;
	if(NULL == pszRaw)
		return FALSE;
	nLen = 4;
	if(0 == strcmp(pszRaw, "void"))
		pt = PT_VOID;
	else if(0 == strcmp(pszRaw, "bool" ) || 0 == strcmp(pszRaw, "const bool") ||
		0 == strcmp(pszRaw, "volatile bool" ) || 0 == strcmp(pszRaw, "volatile const bool"))
		pt = PT_bool;
	else if(0 == strcmp(pszRaw, "bool &" ) || 0 == strcmp(pszRaw, "const bool &") ||
		0 == strcmp(pszRaw, "volatile bool &" ) || 0 == strcmp(pszRaw, "volatile const bool &"))
		pt = PT_boolREF;
	else if (0 == strcmp(pszRaw, "char &") ||  0 == strcmp(pszRaw, "const char &") ||
		0 == strcmp(pszRaw, "volatile char &") ||  0 == strcmp(pszRaw, "volatile const char &"))
		pt = PT_CHARREF;	
	else if(0 == strcmp(pszRaw, "volatile char") || 0 == strcmp(pszRaw, "volatile const char") ||
		0 == strcmp(pszRaw, "char") || 0 == strcmp(pszRaw, "const char"))
		pt = PT_CHAR;
	else if(0 == strcmp(pszRaw, "wchar_t") || 0 == strcmp(pszRaw, "const wchar_t") ||
		0 == strcmp(pszRaw, "volatile wchar_t") || 0 == strcmp(pszRaw, "volatile const wchar_t"))
		pt = PT_WCHAR;
	else if( 0 == strcmp(pszRaw, "wchar_t &") || 0 == strcmp(pszRaw, "const wchar_t &") ||
		0 == strcmp(pszRaw, "volatile wchar_t &") || 0 == strcmp(pszRaw, "volatile const wchar_t &"))
		pt = PT_WCHARREF;
	else if(0 == strcmp(pszRaw, "unsigned char") || 0 == strcmp(pszRaw, "const unsigned char") ||
		0 == strcmp(pszRaw, "volatile unsigned char") || 0 == strcmp(pszRaw, "volatile const unsigned char"))
		pt = PT_UINT1;
	else if(0 == strcmp(pszRaw, "short") || 0 == strcmp(pszRaw, "const short") ||
		0 == strcmp(pszRaw, "volatile short") || 0 == strcmp(pszRaw, "volatile const short"))
		pt = PT_INT2;
	else if(0 == strcmp(pszRaw, "unsigned short") || 0 == strcmp(pszRaw, "const unsigned short") ||
		0 == strcmp(pszRaw, "volatile unsigned short") || 0 == strcmp(pszRaw, "volatile const unsigned short"))
		pt = PT_UINT2;
	else if(0 == strcmp(pszRaw, "int") || 0 == strcmp(pszRaw, "const int") ||
		0 == strcmp(pszRaw, "volatile int") || 0 == strcmp(pszRaw, "volatile const int"))
		pt = PT_INT;
	else if(0 == strcmp(pszRaw, "unsigned int") || 0 == strcmp(pszRaw, "const unsigned int") ||
		0 == strcmp(pszRaw, "volatile unsigned int") || 0 == strcmp(pszRaw, "volatile const unsigned int"))
		pt = PT_UINT;
	else if(0 == strcmp(pszRaw, "long") || 0 == strcmp(pszRaw, "const long") ||
		0 == strcmp(pszRaw, "volatile long") || 0 == strcmp(pszRaw, "volatile const long"))
		pt = PT_INT;
	else if(0 == strcmp(pszRaw, "unsigned long") || 0 == strcmp(pszRaw, "const unsigned long") ||
		0 == strcmp(pszRaw, "volatile unsigned long") || 0 == strcmp(pszRaw, "volatile const unsigned long"))
		pt = PT_UINT;
	else if(0 == strcmp(pszRaw, "__w64 int") || 0 == strcmp(pszRaw, "__w64 const int") ||
		0 == strcmp(pszRaw, "__w64 long") || 0 == strcmp(pszRaw, "__w64 const long") ||
		0 == strcmp(pszRaw, "volatile __w64 int") || 0 == strcmp(pszRaw, "volatile __w64 const int") ||
		0 == strcmp(pszRaw, "volatile __w64 long") || 0 == strcmp(pszRaw, "volatile __w64 const long"))
	{
#if (defined _Wp64 && !defined _WIN64)
		pt = PT_INT;
#endif
	}
	else if(0 == strcmp(pszRaw, "__w64 unsigned int") || 0 == strcmp(pszRaw, "__w64 const unsigned int") ||
		0 == strcmp(pszRaw, "__w64 unsigned long") || 0 == strcmp(pszRaw, "__w64 const unsigned long") ||
		0 == strcmp(pszRaw, "volatile __w64 unsigned int") || 0 == strcmp(pszRaw, "volatile __w64 const unsigned int") ||
		0 == strcmp(pszRaw, "volatile __w64 unsigned long") || 0 == strcmp(pszRaw, "volatile __w64 const unsigned long"))
	{
#if (defined _Wp64 && !defined _WIN64)
		pt = PT_UINT;
#endif
	}
	else if(0 == strcmp(pszRaw, "unsigned char &") || 0 == strcmp(pszRaw, "const unsigned char &") ||
		0 == strcmp(pszRaw, "volatile unsigned char &") || 0 == strcmp(pszRaw, "volatile const unsigned char &"))
		pt = PT_UINT1REF;
	else if( 0 == strcmp(pszRaw, "short &")|| 0 == strcmp(pszRaw, "const short &") ||
		0 == strcmp(pszRaw, "volatile short &")|| 0 == strcmp(pszRaw, "volatile const short &"))
		pt = PT_INT2REF;
	else if( 0 == strcmp(pszRaw, "unsigned short &") || 0 == strcmp(pszRaw, "const unsigned short &") ||
		0 == strcmp(pszRaw, "volatile unsigned short &") || 0 == strcmp(pszRaw, "volatile const unsigned short &"))
		pt = PT_UINT2REF;
	else if(0 == strcmp(pszRaw, "int &") || 0 == strcmp(pszRaw, "const int &") ||
		0 == strcmp(pszRaw, "volatile int &") || 0 == strcmp(pszRaw, "volatile const int &"))
		pt = PT_INTREF;
	else if( 0 == strcmp(pszRaw, "unsigned int &") || 0 == strcmp(pszRaw, "const unsigned int &") ||
		0 == strcmp(pszRaw, "volatile unsigned int &") || 0 == strcmp(pszRaw, "volatile const unsigned int &"))
		pt = PT_UINTREF;
	else if(0 == strcmp(pszRaw, "long &") || 0 == strcmp(pszRaw, "const long &") ||
		0 == strcmp(pszRaw, "volatile long &") || 0 == strcmp(pszRaw, "volatile const long &"))
		pt = PT_INTREF;
	else if( 0 == strcmp(pszRaw, "unsigned long &") || 0 == strcmp(pszRaw, "const unsigned long &") ||
		0 == strcmp(pszRaw, "volatile unsigned long &") || 0 == strcmp(pszRaw, "volatile const unsigned long &"))
		pt = PT_UINTREF;
	else if(0 == strcmp(pszRaw, "__w64 int &") || 0 == strcmp(pszRaw, "__w64 const int &") ||
		0 == strcmp(pszRaw, "__w64 long &") || 0 == strcmp(pszRaw, "__w64 const long &") ||
		0 == strcmp(pszRaw, "volatile __w64 int &") || 0 == strcmp(pszRaw, "volatile __w64 const int &") ||
		0 == strcmp(pszRaw, "volatile __w64 long &") || 0 == strcmp(pszRaw, "volatile __w64 const long &"))
	{
#if (defined _Wp64 && !defined _WIN64)
		pt = PT_INTREF;
#endif
	}
	else if(0 == strcmp(pszRaw, "__w64 unsigned int &") || 0 == strcmp(pszRaw, "__w64 const unsigned int &") ||
		0 == strcmp(pszRaw, "__w64 unsigned long &") || 0 == strcmp(pszRaw, "__w64 const unsigned long &") ||
		0 == strcmp(pszRaw, "volatile __w64 unsigned int &") || 0 == strcmp(pszRaw, "volatile __w64 const unsigned int &") ||
		0 == strcmp(pszRaw, "volatile __w64 unsigned long &") || 0 == strcmp(pszRaw, "volatile __w64 const unsigned long &"))
	{
#if (defined _Wp64 && !defined _WIN64)
		pt = PT_UINTREF;
#endif        	
	}
	else if(0 == strcmp(pszRaw, "__int64") || 0 == strcmp(pszRaw, "const __int64") ||
		0 == strcmp(pszRaw, "volatile __int64") || 0 == strcmp(pszRaw, "volatile const __int64"))
		pt = PT_INT64;
	else if(0 == strcmp(pszRaw, "unsigned __int64") || 0 == strcmp(pszRaw, "const unsigned __int64") ||
		0 == strcmp(pszRaw, "volatile unsigned __int64") || 0 == strcmp(pszRaw, "volatile const unsigned __int64"))
		pt = PT_UINT64;
	else if(0 == strcmp(pszRaw, "__int64 &") || 0 == strcmp(pszRaw, "const __int64 &") ||
		0 == strcmp(pszRaw, "volatile __int64 &") || 0 == strcmp(pszRaw, "volatile const __int64 &"))
		pt = PT_INT64REF;
	else if( 0 == strcmp(pszRaw, "unsigned __int64 &") || 0 == strcmp(pszRaw, "const unsigned __int64 &") ||
		0 == strcmp(pszRaw, "volatile unsigned __int64 &") || 0 == strcmp(pszRaw, "volatile const unsigned __int64 &"))
		pt = PT_UINT64REF;
	else if(0 == strcmp(pszRaw, "struct tagVARIANT") || 0 == strcmp(pszRaw, "const struct tagVARIANT") ||
		0 == strcmp(pszRaw, "volatile struct tagVARIANT") || 0 == strcmp(pszRaw, "volatile const struct tagVARIANT"))
	{
		pt = PT_VARIANT;
		nLen = sizeof(tagVARIANT);
	}
	else if(0 == strcmp(pszRaw, "struct tagVARIANT &") || 0 == strcmp(pszRaw, "const struct tagVARIANT &") ||
		0 == strcmp(pszRaw, "volatile struct tagVARIANT &") || 0 == strcmp(pszRaw, "volatile const struct tagVARIANT &"))	
		pt = PT_VARIANTREF;
	else if(0 == strcmp(pszRaw, "struct _GUID") || 0 == strcmp(pszRaw, "const struct _GUID") ||
		0 == strcmp(pszRaw, "volatile struct _GUID") || 0 == strcmp(pszRaw, "volatile const struct _GUID"))
	{
		pt = PT_GUID;
		nLen = sizeof(_GUID);
	}
	else if(0 == strcmp(pszRaw, "struct _GUID &") || 0 == strcmp(pszRaw, "const struct _GUID &") ||
		0 == strcmp(pszRaw, "volatile struct _GUID &") || 0 == strcmp(pszRaw, "volatile const struct _GUID &"))	
		pt = PT_GUIDREF;
	else if(0 == strncmp(pszRaw, "enum ", 5))
		pt = PT_INT;
	else
	{
		//* &
		if(0 == strcmp(pszRaw, "char *") || 0 == strcmp(pszRaw, "char *const ") || 0 == strcmp(pszRaw, "const char *") || 0 == strcmp(pszRaw, "const char *const ") ||
			0 == strcmp(pszRaw, "volatile char *") || 0 == strcmp(pszRaw, "volatile char *const ") || 0 == strcmp(pszRaw, "volatile const char *") || 0 == strcmp(pszRaw, "volatile const char *const "))
			pt = PT_CHARPOINT;
		else if(0 == strcmp(pszRaw, "char *& ") || 0 == strcmp(pszRaw, "char *const & ") || 0 == strcmp(pszRaw, "const char *& ")  ||  0 == strcmp(pszRaw, "const char *const & ") || 
			0 == strcmp(pszRaw, "volatile char *& ") || 0 == strcmp(pszRaw, "volatile char *const & ") || 0 == strcmp(pszRaw, "volatile const char *& ")  ||  0 == strcmp(pszRaw, "volatile const char *const & "))
			pt = PT_CHARPOINTREF;
		else if(0 == strcmp(pszRaw, "wchar_t *") || 0 == strcmp(pszRaw, "const wchar_t *") ||
			0 == strcmp(pszRaw, "const wchar_t *const ") || 0 == strcmp(pszRaw, "wchar_t *const ") ||
			0 == strcmp(pszRaw, "volatile wchar_t *") || 0 == strcmp(pszRaw, "volatile const wchar_t *") ||
			0 == strcmp(pszRaw, "volatile const wchar_t *const ") || 0 == strcmp(pszRaw, "volatile wchar_t *const ")) //wchar_t*
			pt = PT_WCHARPOINT;
		else if(0 == strcmp(pszRaw, "const wchar_t *& ") ||  0 == strcmp(pszRaw, "wchar_t *& ")  ||
			0 == strcmp(pszRaw, "const wchar_t *const & ") || 0 == strcmp(pszRaw, "wchar_t *const & ") ||
			0 == strcmp(pszRaw, "volatile const wchar_t *& ") ||  0 == strcmp(pszRaw, "volatile wchar_t *& ")  ||
			0 == strcmp(pszRaw, "volatile const wchar_t *const & ") || 0 == strcmp(pszRaw, "volatile wchar_t *const & ")) //const wchar_t*
			pt = PT_WCHARPOINTREF;
		else if( 0 == strcmp(pszRaw, "short *")|| 0 == strcmp(pszRaw, "const short *") ||
			0 == strcmp(pszRaw, "volatile short *")|| 0 == strcmp(pszRaw, "volatile const short *"))
			pt = PT_INT2REF;
		else if(0 == strcmp(pszRaw, "unsigned short *") || 0 == strcmp(pszRaw, "const unsigned short *") ||
			0 == strcmp(pszRaw, "const unsigned short *const ") || 0 == strcmp(pszRaw, "unsigned short *const ") ||
			0 == strcmp(pszRaw, "volatile unsigned short *") || 0 == strcmp(pszRaw, "volatile const unsigned short *") ||
			0 == strcmp(pszRaw, "volatile const unsigned short *const ") || 0 == strcmp(pszRaw, "volatile unsigned short *const ")) //wchar_t*
		{
#if (!(defined _WCHAR_T_DEFINED && defined _NATIVE_WCHAR_T_DEFINED))
			pt = PT_WCHARPOINT;
#else
			pt = PT_INT2REF;
#endif
		}
		else if(0 == strcmp(pszRaw, "const unsigned short *& ") ||  0 == strcmp(pszRaw, "unsigned short *& ")  ||
			0 == strcmp(pszRaw, "const unsigned short *const & ") || 0 == strcmp(pszRaw, "unsigned short *const & ") ||
			0 == strcmp(pszRaw, "volatile const unsigned short *& ") ||  0 == strcmp(pszRaw, "volatile unsigned short *& ")  ||
			0 == strcmp(pszRaw, "volatile const unsigned short *const & ") || 0 == strcmp(pszRaw, "volatile unsigned short *const & ") ) //const wchar_t*
		{
#if (!(defined _WCHAR_T_DEFINED && defined _NATIVE_WCHAR_T_DEFINED))
			pt = PT_WCHARPOINTREF;
#endif
		}
		else if(0 == strcmp(pszRaw, "bool *" ) || 0 == strcmp(pszRaw, "const bool *") ||
			0 == strcmp(pszRaw, "volatile bool *") || 0 == strcmp(pszRaw, "volatile const bool *"))
			pt = PT_boolREF;
		else if(0 == strcmp(pszRaw, "int *") || 0 == strcmp(pszRaw, "const int *")||
			0 == strcmp(pszRaw, "volatile int *") || 0 == strcmp(pszRaw, "volatile const int *"))
			pt = PT_INTREF;
		else if( 0 == strcmp(pszRaw, "unsigned int *") || 0 == strcmp(pszRaw, "const unsigned int *") ||
			0 == strcmp(pszRaw, "volatile unsigned int *") || 0 == strcmp(pszRaw, "volatile const unsigned int *"))
			pt = PT_INTREF;
		else if(0 == strcmp(pszRaw, "long *") || 0 == strcmp(pszRaw, "const long *") ||
			0 == strcmp(pszRaw, "volatile long *") || 0 == strcmp(pszRaw, "volatile const long*"))
			pt = PT_INTREF;
		else if( 0 == strcmp(pszRaw, "unsigned long *") || 0 == strcmp(pszRaw, "const unsigned long *") ||
			0 == strcmp(pszRaw, "volatile unsigned long *") || 0 == strcmp(pszRaw, "volatile const unsigned long *"))
			pt = PT_INTREF;
		else if(0 == strcmp(pszRaw, "__int64 *") || 0 == strcmp(pszRaw, "const __int64 *") ||
			0 == strcmp(pszRaw, "volatile __int64 *") || 0 == strcmp(pszRaw, "volatile const __int64 *"))
			pt = PT_INT64REF;
		else if( 0 == strcmp(pszRaw, "unsigned __int64 *") || 0 == strcmp(pszRaw, "const unsigned __int64 *") ||
			0 == strcmp(pszRaw, "volatile unsigned __int64 *") || 0 == strcmp(pszRaw, "volatile const unsigned __int64 *"))
			pt = PT_INT64REF;
		else if(0 == strcmp(pszRaw, "__int64 *& ") || 0 == strcmp(pszRaw, "const __int64 *& ") ||
			0 == strcmp(pszRaw, "volatile __int64 *& ") || 0 == strcmp(pszRaw, "volatile const __int64 *& "))
			pt = PT_INT64REF;
		else if( 0 == strcmp(pszRaw, "unsigned __int64 *& ") || 0 == strcmp(pszRaw, "const unsigned __int64 *& ") ||
			0 == strcmp(pszRaw, "volatile unsigned __int64 *& ") || 0 == strcmp(pszRaw, "volatile const unsigned __int64 *& "))
			pt = PT_INT64REF;
		else if(0 == strcmp(pszRaw, "struct tagVARIANT *") || 0 == strcmp(pszRaw, "const struct tagVARIANT *") ||
			0 == strcmp(pszRaw, "volatile struct tagVARIANT *") || 0 == strcmp(pszRaw, "volatile const struct tagVARIANT *"))		
			pt = PT_VARIANTREF;
		else if(0 == strcmp(pszRaw, "struct _GUID *") || 0 == strcmp(pszRaw, "const struct _GUID *") ||
			0 == strcmp(pszRaw, "volatile struct _GUID *") || 0 == strcmp(pszRaw, "volatile const struct _GUID *"))		
			pt = PT_GUIDREF;		
		else
		{
			pt = PT_PENDPOINT;
			while('\0' != *(++pData) && ('*' != *pData)) ; 
			if('\0' == *pData) //不是指针
			{
				nLen = 4;
				pt = PT_PEND;
			}	
		}
	}
	return TRUE;
}
BOOL CTSLog::GetParamInfo(LPCSTR pszFuncSig, BOOL bIsStatic, CALLTYPE_TSLOG& ct, std::vector<stParamTypeInfo>& vecIndex2TypeName, BOOL& bRetClass)
{
	//保护
#define MAX_FUNCSIG_LEN (2*1024)
	if(NULL == pszFuncSig)
		return FALSE;
	CHAR szFuncSig[MAX_FUNCSIG_LEN] = {0}; //strtok 不能用 常量区 参数 因为里面会更改值
	LPSTR pszCallType = NULL;
	CHAR seps[] = "(,)";
	LPSTR ptoken = NULL; //
	BOOL bStartPend = FALSE;
	INT nIndex = 0;
	pszCallType = (LPSTR)strstr((const char *)pszFuncSig, "__"); 
	if(NULL == pszCallType)
		return FALSE;
	if(0 == memcmp((const void*)pszCallType, (const void*)"__stdcall", 9))
		ct = CT_STDCALL;
	else if(0 == memcmp((const void*)pszCallType, (const void*)"__cdecl", 7))
		ct = CT_CDECL_CALL;
	else if(0 == memcmp((const void*)pszCallType, (const void*)"__thiscall", 10))
		ct = CT_THISCALL;
	else
	{
		ct = CT_OTHERCALL;
		while(NULL != (pszCallType = (LPSTR)strstr((const char *)&pszCallType[2], "__")))
		{
			if(0 == memcmp((const void*)"__stdcall", (const void*)pszCallType, 9))
			{
				ct = CT_STDCALL;
				break;
			}
			else if(0 == memcmp((const void*)"__cdecl", (const void*)pszCallType, 7))
			{
				ct = CT_CDECL_CALL;
				break;
			}
			else if(0 == memcmp((const void*)"__thiscall", (const void*)pszCallType, 10))
			{
				ct = CT_THISCALL;
				break;
			}
		}
	}
	if(CT_OTHERCALL == ct)
		return FALSE;
	bRetClass = FALSE;
	if(0 == memcmp(pszFuncSig, "class ", 6))
		if(' ' == *(pszCallType - 1))
			bRetClass = TRUE;
	pszCallType += 9;
	ptoken = strstr(pszCallType, "(");
	if(CT_THISCALL != ct && !bIsStatic)
	{
		while (ptoken-- > pszCallType)
		{
			if(':' == *ptoken)
			{
				if(ct == CT_STDCALL)
					ct = CT_STDTHISCALL;
				else if(ct == CT_CDECL_CALL)
					ct = CT_CDECLTHISCALL;
				break;
			}
		}
	}
	INT nValidCommaPos = 0;
	nValidCommaPos = (INT)(pszCallType - pszFuncSig) - 1;//"("位置前
	strncpy(szFuncSig, pszFuncSig + nValidCommaPos,MAX_FUNCSIG_LEN);
	ptoken = strtok(szFuncSig , seps);
	if(ptoken)
		nValidCommaPos += (INT)strlen(ptoken);
	ptoken = strtok( NULL, seps ); //过第一次
	if(ptoken)
		nValidCommaPos += (int)strlen(ptoken) + 1;
	int cbLAB = 0;
	BOOL bpend = FALSE;
	while(NULL != ptoken && 0 != strcmp(" throw", ptoken))
	{
		LPSTR pfindLAB = NULL;
		for (pfindLAB = strstr(ptoken, "<"); NULL != pfindLAB; pfindLAB = strstr(pfindLAB + 1, "<"))
			cbLAB++;
		LPSTR pfindRAB = NULL;
		for (pfindRAB = strstr(ptoken, ">"); NULL != pfindRAB; pfindRAB = strstr(pfindRAB + 1, ">"))
		{
			cbLAB--;
			if(!bpend)
				bpend = TRUE;
		}
		if(0 == cbLAB)
		{
			INT nLen = 0;
			PARAM_TYPE eParamType = PT_PEND;
			eParamType = PT_PEND;
			if(!bpend && !bStartPend && !GetParamTypeAndLen(ptoken, eParamType,nLen))
			{
				vecIndex2TypeName.clear();
				return FALSE;
			}
			if(PT_VOID == eParamType)
				return TRUE;
			switch (eParamType)
			{
			case PT_PEND:
				if(!bStartPend)
					bStartPend = TRUE;
				break;
			default:
				break;
			}
			stParamTypeInfo ptinfo = {PT_PEND, 0};
			if(bStartPend)
				ptinfo.pt = PT_PEND;
			else
				ptinfo.pt = eParamType;
			ptinfo.nValidCommaPos = nValidCommaPos;
			vecIndex2TypeName.push_back(ptinfo);
			nIndex++;
			nValidCommaPos = 0;
		}
		ptoken = strtok(NULL, seps);
		if (ptoken)
			nValidCommaPos += (INT)strlen(ptoken) + 1;
	}
	return TRUE;
}
BOOL CTSLog::TranslateCodePage(char ** ppszValA)
{
	UINT code_page = GetStringCodePage(*ppszValA);
	int  nLen = MultiByteToWideChar(code_page  , 0, *ppszValA, -1,NULL,0);
	if(0 == nLen)
	{
		code_page= 0;
		nLen = MultiByteToWideChar(code_page  , 0, *ppszValA, -1,NULL,0);
	}
	wchar_t szBufferW[4096] = {0};
	static char szBufferA[4096] = {0};
	if(nLen > 0 && nLen <2048)
	{
		BOOL b = TRUE;
		nLen = MultiByteToWideChar(code_page, 0, *ppszValA, -1, szBufferW, nLen);
		szBufferW[nLen] = '\0';		
		nLen = WideCharToMultiByte(936, 0, szBufferW, nLen, szBufferA, 2048, "_", &b);
		if(nLen > 0)
		{
			szBufferA[nLen] = '\0';
			*ppszValA = szBufferA;
			return TRUE;
		}	
	}
	return FALSE;
}
DWORD CTSLog::GetSafeValidValue(const CTSLog::PARAM_TYPE &tslog_eParamType,LPSTR tslog_szRegValue, DWORD tslog_nRegValue, DWORD tslog_nRegValue2 /*= 0*/,
				   LPINT tslog_pnRegValue /*= NULL*/, LPINT tslog_pnRegValue2 /*= NULL*/ , LPSTR tslog_pRegValueA /*= NULL */, LPWSTR tslog_pRegValueW/* = NULL*/, LPINT pnRet/* = 0*/)
{
	DWORD dwRet = 0;
 	__try
	{
#if(defined W2A || defined OLE2A)
		USES_CONVERSION;
#endif
		switch(tslog_eParamType)
		{
		case CTSLog::PT_INVALID:
#pragma warning(push)
//#pragma warning(disable : 4312 )
			{
				if(0 < tslog_nRegValue)
					dwRet = *(LPDWORD)(ULONG_PTR)tslog_nRegValue;
				else if( 0 < tslog_nRegValue2)
					dwRet = *(LPDWORD)(ULONG_PTR)tslog_nRegValue2;
				if(NULL != pnRet)
					*pnRet = 0;
				return dwRet;
			}
//#pragma warning(pop)
			break;
		case CTSLog::PT_CHARREF:
			memcpy (&tslog_pRegValueA, &tslog_nRegValue, sizeof(tslog_nRegValue));			
			TranslateCodePage(&tslog_pRegValueA);
			dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE, " (0x%08x) = 0x%08hx('%c')", tslog_nRegValue,(UINT)(CHAR)(*tslog_pRegValueA), (CHAR)*tslog_pRegValueA);		 			
			break; 
		case CTSLog::PT_WCHARREF:
			memcpy(&tslog_pRegValueW, &tslog_nRegValue, sizeof(tslog_nRegValue));
			dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," (0x%08x) = 0x%08hx(L'%C')", tslog_nRegValue, (unsigned short)(*tslog_pRegValueW) , (wchar_t)*tslog_pRegValueW);
			break;
		case CTSLog::PT_CHARPOINT:
			memcpy(&tslog_pRegValueA, &tslog_nRegValue, sizeof(tslog_nRegValue));
			TranslateCodePage(&tslog_pRegValueA);
			if(s_release.m_si.lpMinimumApplicationAddress > tslog_pRegValueA || tslog_pRegValueA > s_release.m_si.lpMaximumApplicationAddress)
			{
                dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08Ix(\"%s\")", tslog_pRegValueA,  "Is Invalid Point!!!");
			}
			else
			{
				if(NULL != tslog_pRegValueA && MAX_USERDATA_SIZE/2 < strlen((LPSTR)tslog_pRegValueA))
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08Ix(\"%s\")", tslog_pRegValueA,  "Length TOO LONG");
				else
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08Ix(\"%s\")", tslog_pRegValueA,  (LPSTR)tslog_pRegValueA);
			}
			break;
		case CTSLog::PT_WCHARPOINT:
			memcpy(&tslog_pRegValueW, &tslog_nRegValue, sizeof(tslog_nRegValue));
			if(s_release.m_si.lpMinimumApplicationAddress > tslog_pRegValueW || tslog_pRegValueW > s_release.m_si.lpMaximumApplicationAddress)
			{
				dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08Ix(\"%s\")", tslog_pRegValueW,  "Is Invalid Point!!!");
			}
			else
			{
				if(NULL != tslog_pRegValueW && MAX_USERDATA_SIZE/2 < wcslen((LPWSTR)tslog_pRegValueW))
					dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE , " = 0x%08Ix(L\"%s\")", tslog_pRegValueW , "Length TOO LONG");
				else
				{
					char szBufferA[2048] = {0};
					int nLen = (int)wcslen(tslog_pRegValueW);
					BOOL b = TRUE;
					WideCharToMultiByte(936, 0, tslog_pRegValueW, nLen, szBufferA, 2048, "_", &b);
					dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE , " = 0x%08Ix(L\"%s\")", tslog_pRegValueW , szBufferA);
				}				
			}
			break;
		case CTSLog::PT_CHARPOINTREF:
			memcpy(&tslog_pRegValueA, &tslog_nRegValue, sizeof(tslog_nRegValue));
			memcpy(&tslog_pRegValueA, tslog_pRegValueA, sizeof(tslog_nRegValue));
			TranslateCodePage(&tslog_pRegValueA);
			if(s_release.m_si.lpMinimumApplicationAddress > tslog_pRegValueA || tslog_pRegValueA > s_release.m_si.lpMaximumApplicationAddress)
			{
				dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08Ix(\"%s\")", tslog_pRegValueA,  "Is Invalid Point!!!");
			}
			else
			{
				if(NULL != tslog_pRegValueA && MAX_USERDATA_SIZE/2 < strlen((LPSTR)tslog_pRegValueA))
					dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE ," (0x%08x) = 0x%08Ix(\"%s\")", tslog_nRegValue, tslog_pRegValueA, "Length TOO LONG");
				else
					dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE ," (0x%08x) = 0x%08Ix(\"%s\")", tslog_nRegValue, tslog_pRegValueA, (LPSTR)tslog_pRegValueA);
			}
			break;
		case CTSLog::PT_WCHARPOINTREF:
			memcpy(&tslog_pRegValueW, &tslog_nRegValue, sizeof(tslog_nRegValue));
			memcpy(&tslog_pRegValueW, tslog_pRegValueW, sizeof(tslog_nRegValue));
			if(s_release.m_si.lpMinimumApplicationAddress > tslog_pRegValueW || tslog_pRegValueW > s_release.m_si.lpMaximumApplicationAddress)
			{
				dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08Ix(\"%s\")", tslog_pRegValueW,  "Is Invalid Point!!!");
			}
			else
			{
				if(NULL != tslog_pRegValueW && MAX_USERDATA_SIZE/2 < wcslen((LPWSTR)tslog_pRegValueW))
					dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE , " (0x%08x) = 0x%08Ix(L\"%s\")", tslog_nRegValue, tslog_pRegValueW, "Length TOO LONG");
				else
				{
					char szBufferA[2048] = {0};
					int nLen = (int)wcslen(tslog_pRegValueW);
					BOOL b = TRUE;
					WideCharToMultiByte(936, 0, tslog_pRegValueW, nLen, szBufferA, 2048, "_", &b);
					dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE , " (0x%08x) = 0x%08Ix(L\"%s\")", tslog_nRegValue, tslog_pRegValueW, szBufferA);					
				}
				/*
#ifdef W2A			
					dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE , " (0x%08x) = 0x%08Ix(L\"%s\")", tslog_nRegValue, tslog_pRegValueW, W2A(tslog_pRegValueW));
#else
					dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE , " (0x%08x) = 0x%08Ix(L\"%S\"?)", tslog_nRegValue, tslog_pRegValueW, tslog_pRegValueW);
#endif			*/
			}
			break;
		case CTSLog::PT_boolREF:
			memcpy(&tslog_pnRegValue ,&tslog_nRegValue, sizeof(tslog_nRegValue));
			if(0 == tslog_pnRegValue)
			{
				dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE ," (0x%08x) = (null)", tslog_nRegValue);
			}
			else
			{
				if(0 == *tslog_pnRegValue)
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " (0x%08x) = 0x%08x(false)", tslog_nRegValue ,  *tslog_pnRegValue);
				else
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " (0x%08x) = 0x%08x(true)", tslog_nRegValue , *tslog_pnRegValue);
			}
			break;		
		case CTSLog::PT_UINT1REF:
			memcpy(&tslog_pnRegValue ,&tslog_nRegValue, sizeof(tslog_nRegValue));
			dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " (0x%08x) = 0x%08x(%u)",tslog_nRegValue , (UINT)(CHAR)*tslog_pnRegValue,(UINT)(unsigned char)*tslog_pnRegValue);
			break;
		case CTSLog::PT_INT2REF:
			memcpy(&tslog_pnRegValue ,&tslog_nRegValue, sizeof(tslog_nRegValue));
			dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," (0x%08x) = 0x%08x(%hd)",tslog_nRegValue , (UINT)(unsigned short)*tslog_pnRegValue,(short)*tslog_pnRegValue);
			break;
		case CTSLog::PT_UINT2REF:
			memcpy(&tslog_pnRegValue ,&tslog_nRegValue, sizeof(tslog_nRegValue));
			dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," (0x%08x) = 0x%08x(%hu)",tslog_nRegValue , (UINT)(unsigned short)*tslog_pnRegValue,(unsigned short)*tslog_pnRegValue);
			break;
		case CTSLog::PT_INTREF:
			memcpy(&tslog_pnRegValue ,&tslog_nRegValue, sizeof(tslog_nRegValue));
			if(0 == tslog_pnRegValue)
				dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " (0x%08x) = (null)", tslog_nRegValue);
			else
				dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " (0x%08x) = 0x%08x(%d)", tslog_nRegValue, *tslog_pnRegValue,*tslog_pnRegValue);
			break;
		case CTSLog::PT_UINTREF:
			memcpy(&tslog_pnRegValue ,&tslog_nRegValue, sizeof(tslog_nRegValue));
			if(0 == tslog_pnRegValue)
				dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " (0x%08x) = (null)", tslog_nRegValue);
			else
				dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " (0x%08x) = 0x%08x(%u)", tslog_nRegValue, *tslog_pnRegValue,*tslog_pnRegValue);
			break;
		case CTSLog::PT_INT64REF:
			{
				memcpy(&tslog_pnRegValue ,&tslog_nRegValue, sizeof(tslog_nRegValue));
				memcpy(&tslog_pnRegValue2 ,&tslog_nRegValue2, sizeof(tslog_nRegValue2));
				DWORD nRegValue = 0;
				DWORD nRegValue2 = 0;
				nRegValue =  tslog_pnRegValue ?  *tslog_pnRegValue : 0;
				nRegValue2 = tslog_pnRegValue2 ?  *tslog_pnRegValue2 : 0;
				UINT64 ullRegValue=(UINT64)nRegValue2<<32;
				ullRegValue+=nRegValue;
				dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " (0x%08x)(0x%08x) = 0x%016I64x(%I64d)", tslog_nRegValue2, tslog_nRegValue,
					(UINT64)ullRegValue, (INT64)ullRegValue);
			}
			break;
		case CTSLog::PT_UINT64REF:
			{
				memcpy(&tslog_pnRegValue ,&tslog_nRegValue, sizeof(tslog_nRegValue));
				memcpy(&tslog_pnRegValue2 ,&tslog_nRegValue2, sizeof(tslog_nRegValue2));
				DWORD nRegValue = 0;
				DWORD nRegValue2 = 0;
				nRegValue =  tslog_pnRegValue ?  *tslog_pnRegValue : 0;
				nRegValue2 = tslog_pnRegValue2 ?  *tslog_pnRegValue2 : 0;
				UINT64 ullRegValue=(UINT64)nRegValue2<<32;
				ullRegValue+=nRegValue;
				dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " (0x%08x)(0x%08x) = 0x%016I64x(%I64u)", tslog_nRegValue2, tslog_nRegValue,
					(UINT64)ullRegValue, (INT64)ullRegValue);
			}
			break;
		case CTSLog::PT_VARIANT:
		case CTSLog::PT_VARIANTREF:
			{
				tagVARIANT * pVar = NULL;
				if(PT_VARIANT == tslog_eParamType)
					pVar = (tagVARIANT *)tslog_pnRegValue;
				else
					memcpy(&pVar, &tslog_nRegValue, sizeof(tslog_nRegValue));
				if(NULL == pVar)
				{
					dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE, " = NULL");
					break;
				}
				switch (pVar->vt)
				{
				case VT_EMPTY:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {VT_EMPTY}");
					break;
				case VT_NULL:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {VT_NULL}");
					break;
				case VT_ERROR:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%d) VT_ERROR}", pVar->scode, pVar->scode);
					break;
				case VT_I1:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%d) VT_I1}", (INT)pVar->bVal, (INT)pVar->bVal);
                    break;
				case VT_I2:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%d) VT_I2}", (INT)pVar->iVal, (INT)pVar->iVal);
					break;
				case VT_I4:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%d) VT_I4}", (INT)pVar->intVal, (INT)pVar->intVal);
					break;
				case VT_I8:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%I64x(%I64d) VT_I8}", (ULONGLONG)pVar->ullVal , (LONGLONG)pVar->llVal);
					break;
				case VT_UI1:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%u) VT_UI1}", (UINT)pVar->bVal, (UINT)pVar->bVal);
					break;
				case VT_UI2:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%u) VT_UI2}", (UINT)pVar->iVal, (UINT)pVar->iVal);
					break;
				case VT_UI4:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%u) VT_UI4}", (UINT)pVar->intVal, (UINT)pVar->intVal);
					break;
				case VT_UI8:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%I64x(%I64u) VT_UI8}", (ULONGLONG)pVar->ullVal , (LONGLONG)pVar->llVal);
				case VT_BOOL:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%d) VT_BOOL}", (INT)pVar->intVal, (INT)pVar->intVal);
					break;
				case VT_INT:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%d) VT_INT}", (INT)pVar->intVal, (INT)pVar->intVal);
				case VT_UINT:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%u) VT_UINT}", (UINT)pVar->intVal, (UINT)pVar->intVal);
				case VT_HRESULT:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%08x(%d) VT_HRESULT}", (UINT)pVar->intVal, (INT)pVar->intVal);
					break;
				case VT_R8:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {%f VT_R8}", pVar->dblVal);
					break;
#ifdef OLE2A 
				case VT_BSTR:
					if(pVar->bstrVal)
					{
						if(MAX_USERDATA_SIZE/2 < wcslen(pVar->bstrVal))
							dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {L\"%s\" VT_BSTR}", "Length TOO LONG");
						else
						{
							char szBufferA[2048] = {0};
							int nLen = (int)wcslen(pVar->bstrVal);
							BOOL b = TRUE;
							WideCharToMultiByte(936, 0, pVar->bstrVal, nLen, szBufferA, 2048, "_", &b);
							dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = {L\"%s\" VT_BSTR}", szBufferA);
						}							
					}
					else
						dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {L\"%s\" VT_BSTR}", "NULL");
					break; 
#else
				case VT_BSTR:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {L\"%S\"? VT_BSTR}", pVar->bstrVal);
					break; 
#endif
#ifdef W2A
				case VT_LPWSTR:
					if(pVar->bstrVal)
					{
						if(MAX_USERDATA_SIZE/2 < wcslen(pVar->bstrVal))
							dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {L\"%s\" VT_BSTR}", "Length TOO LONG");
						else
							dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {L\"%s\" VT_LPWSTR}", OLE2A(pVar->bstrVal));
					}
					else
						dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {L\"%s\" VT_BSTR}", "NULL");
					break;
#else
				case VT_LPWSTR:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {L\"%S\"? VT_LPWSTR}", pVar->bstrVal);
					break;
#endif
				case VT_LPSTR:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {\"%s\" VT_LPSTR}", pVar->pcVal);
					break;
				case VT_DISPATCH:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%p VT_DISPATCH}", (void*)pVar->pdispVal);
					break;
				default:
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {0x%I64x(%I64d) VT_?[0x%08x(%d)]}", (ULONGLONG)pVar->llVal,(LONGLONG)pVar->llVal, pVar->vt, pVar->vt);
					break;
				}
			}
			break;
		case CTSLog::PT_GUID:
		case CTSLog::PT_GUIDREF:
			{
				_GUID * pGUID = NULL;
				LPOLESTR lpGUIDString = NULL;
				if(PT_GUID == tslog_eParamType)
					pGUID = (_GUID *)tslog_pnRegValue;
				else
					memcpy(&pGUID, &tslog_nRegValue, sizeof(tslog_nRegValue));
				if(NULL == pGUID)
				{
					dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE, " = NULL");
					break;
				}
				if(S_OK == StringFromIID(*pGUID, &lpGUIDString))
				{
#ifdef OLE2A
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = %s", W2A(lpGUIDString));
#else
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = %S", lpGUIDString);
#endif
					CoTaskMemFree(lpGUIDString);
				}
				else
					dwRet = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = {StringFromIID ERROR}");
			}
			break;
		default:
			break;
		}
		if((DWORD)-1 == dwRet)
			dwRet = (int)strlen(tslog_szRegValue);
		if(NULL != pnRet)
			*pnRet = 0;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		dwRet = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE, " = 0x%08x(!!!)", tslog_nRegValue);
		if(NULL != pnRet)
			*pnRet = -1;
	}
	return dwRet;
}
LPCTSTR CTSLog::TraceAuto(LPCSTR pszFuncSig, DWORD dwEBP, BOOL bIsStatic)
{	
	if(s_release.m_bIsModuleUnloading)
	{
		return NULL;
	}
	EnterCriticalSection(&s_pThis-> _cs);
	static std::string tslog_strDestFuncSig;			//保存返回结果必须静态
	LPCTSTR tslog_ptszDestFuncSig = NULL;		//保存返回结果
	tslog_strDestFuncSig = pszFuncSig;
	if(CTSLog::IsTRACEValid())
	{
		std::vector<stParamTypeInfo> tslog_vecIndex2TypeName;//第一个存绝对位置, 第二个开始都是相对第一个的位置
		CTSLog::CALLTYPE_TSLOG tslog_ct = CT_OTHERCALL;
		BOOL tslog_bRet = FALSE;
		int tslog_nSize = 0;
		BOOL tslog_bRetClass = FALSE;
		tslog_vecIndex2TypeName.clear();
		tslog_bRet = CTSLog::GetParamInfo(pszFuncSig,bIsStatic, tslog_ct, tslog_vecIndex2TypeName, tslog_bRetClass);
		tslog_nSize = (INT)tslog_vecIndex2TypeName.size();
		if(tslog_bRet)
		{
			INT tslog_nIndex = 0;		  
			INT tslog_nPos	= 0;		 
			INT tslog_nLen	= 0;		  
			DWORD tslog_dwRegValue  = 0;   
			DWORD tslog_nRegValue2 = 0;	  
			INT tslog_nRegOffset	  = 8;    
			CHAR tslog_szRegValue[MAX_USERDATA_SIZE + 1] = {'\0'};  //memset(tslog_szRegValue, 0x00, sizeof(tslog_szRegValue));
//			LPSTR tslog_pszRegValue = NULL;
			CTSLog::PARAM_TYPE tslog_eParamType = PT_INVALID;
			if(CTSLog::CT_STDTHISCALL == tslog_ct || CTSLog::CT_CDECLTHISCALL == tslog_ct)
				tslog_nRegOffset += 4;
			if(tslog_bRetClass)
				tslog_nRegOffset += 4;
			while(tslog_nIndex < tslog_nSize)
			{
				tslog_vecIndex2TypeName[tslog_nIndex].nValidCommaPos += (INT)tslog_nLen;
				tslog_nPos = tslog_nIndex ? (tslog_nPos + tslog_vecIndex2TypeName[tslog_nIndex].nValidCommaPos) : tslog_vecIndex2TypeName[tslog_nIndex].nValidCommaPos;
				tslog_nPos = (INT)tslog_strDestFuncSig.find_first_of(",", tslog_nPos);
				if((INT)std::string::npos  == tslog_nPos)
				{
 					tslog_nPos = (INT)tslog_strDestFuncSig.rfind(" throw(",std::string::npos);
					if((INT)std::string::npos == tslog_nPos)
						tslog_nPos = (INT)tslog_strDestFuncSig.find_last_of(")",std::string::npos);
					else
						tslog_nPos -= 1;
				}
 				tslog_eParamType = tslog_vecIndex2TypeName[tslog_nIndex].pt;
				INT nRet = 0;
				DWORD dwpParam = 0;
				LPINT tslog_pnRegValue =NULL;
				LPINT tslog_pnRegValue2 = NULL;
				dwpParam = dwEBP + tslog_nRegOffset; 
				tslog_dwRegValue = GetSafeValidValue(PT_INVALID, 0, dwpParam, 0,0,0,0,0,&nRet);
				if(nRet < 0)
					tslog_dwRegValue = 0xcccccccd;
				switch (tslog_eParamType)
				{
				case CTSLog::PT_INT64:
					tslog_nRegValue2 = GetSafeValidValue(PT_INVALID, 0, 0, dwpParam+4, 0,0,0,0,&nRet);
					if(nRet < 0)
						tslog_dwRegValue = 0xcccccccd;
					tslog_nRegOffset += 8;
					break;
				case CTSLog::PT_VARIANT:
					memcpy(&tslog_pnRegValue, &dwpParam, sizeof(dwpParam));
					tslog_nRegOffset +=sizeof(tagVARIANT);
					break;
				case CTSLog::PT_GUID:
					memcpy(&tslog_pnRegValue, &dwpParam, sizeof(dwpParam));
					tslog_nRegOffset +=sizeof(_GUID);
					break;
				default:
					tslog_nRegOffset += 4;
					break;
				}
				LPSTR tslog_pRegValueA = NULL;
				LPWSTR tslog_pRegValueW = NULL;
				switch(tslog_eParamType)
				{
				case CTSLog::PT_VOID:
					break;
				case CTSLog::PT_bool:
					if((DWORD)0 == tslog_dwRegValue)
						tslog_nLen = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE," = 0x%08x(false)", tslog_dwRegValue);
					else
						tslog_nLen = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08x(true)", tslog_dwRegValue);
					break;				
				case CTSLog::PT_UINT1:
					tslog_nLen = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08x(%u)", tslog_dwRegValue,(unsigned int)(unsigned char)tslog_dwRegValue);
					break;
				case CTSLog::PT_INT2:
					tslog_nLen = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08x(%hd)", tslog_dwRegValue,(short)tslog_dwRegValue);
					break;
				case CTSLog::PT_UINT2:
					tslog_nLen = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08x(%hu)", tslog_dwRegValue,(unsigned short)tslog_dwRegValue);
					break;
				case CTSLog::PT_INT:
					tslog_nLen = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08x(%d)", tslog_dwRegValue,(int)tslog_dwRegValue);
					break;
				case CTSLog::PT_UINT:
					tslog_nLen = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08x(%u)", tslog_dwRegValue,tslog_dwRegValue);
					break;
				case CTSLog::PT_CHAR:
					tslog_nLen = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = 0x%08x('%c')",tslog_dwRegValue,(char) tslog_dwRegValue);
					tslog_strDestFuncSig.insert(tslog_nPos, tslog_szRegValue);
					break;
				case CTSLog::PT_WCHAR:
					tslog_nLen = _snprintf(tslog_szRegValue,  MAX_USERDATA_SIZE," = 0x%08x(L'%C')",tslog_dwRegValue,(wchar_t) tslog_dwRegValue);
					break;
				case CTSLog::PT_INT64:
					{
						UINT64 ullRegValue=(UINT64)tslog_nRegValue2<<32;
						ullRegValue+=tslog_dwRegValue;
						tslog_nLen = _snprintf(tslog_szRegValue,  MAX_USERDATA_SIZE," = 0x%I64x(%I64d)", (UINT64)ullRegValue,(INT64)ullRegValue);
					}
					break;
				case CTSLog::PT_UINT64:
					{
						UINT64 ullRegValue=(UINT64)tslog_nRegValue2<<32;
						ullRegValue+=tslog_dwRegValue;
						tslog_nLen = _snprintf(tslog_szRegValue,  MAX_USERDATA_SIZE," = 0x%I64x(%I64u)", (UINT64)ullRegValue,(UINT64)ullRegValue);
					}
					break;
				case CTSLog::PT_PEND:
					{
						DWORD tslog_dwRegValue_b1 = 0;
						tslog_dwRegValue_b1 = tslog_dwRegValue & 0x000000FF;
						DWORD tslog_dwRegValue_b2 = 0;
						tslog_dwRegValue_b2 = (tslog_dwRegValue & 0x0000FF00) >> 8;
						DWORD tslog_dwRegValue_b3;
						tslog_dwRegValue_b3 = (tslog_dwRegValue & 0x00FF0000) >> 16;
						DWORD tslog_dwRegValue_b4;
						tslog_dwRegValue_b4 = (tslog_dwRegValue & 0xFF000000) >> 24;
						tslog_nLen = _snprintf(tslog_szRegValue, MAX_USERDATA_SIZE, " = ??? [%02x %02x %02x %02x]", 
							tslog_dwRegValue_b1, tslog_dwRegValue_b2,  tslog_dwRegValue_b3, tslog_dwRegValue_b4 );
					}
					break;
				//.........follow, need safe value....................
				case CTSLog::PT_UINT1REF:
				case CTSLog::PT_INT2REF:
				case CTSLog::PT_UINT2REF:
				case CTSLog::PT_INTREF:
				case CTSLog::PT_UINTREF:
				case CTSLog::PT_boolREF:
				case CTSLog::PT_CHARREF: 
				case CTSLog::PT_WCHARREF: 
				case CTSLog::PT_INT64REF:
				case CTSLog::PT_UINT64REF:
				case CTSLog::PT_CHARPOINT:
				case CTSLog::PT_WCHARPOINT:
				case CTSLog::PT_CHARPOINTREF:
				case CTSLog::PT_WCHARPOINTREF:
				case CTSLog::PT_VARIANT:
				case CTSLog::PT_VARIANTREF:
				case CTSLog::PT_GUID:
				case CTSLog::PT_GUIDREF:
					tslog_nLen = GetSafeValidValue(tslog_eParamType, tslog_szRegValue,  tslog_dwRegValue, tslog_nRegValue2, tslog_pnRegValue, tslog_pnRegValue2, tslog_pRegValueA, tslog_pRegValueW);
					break;
				default:
					tslog_nLen = _snprintf(tslog_szRegValue,MAX_USERDATA_SIZE, " = 0x%08x", tslog_dwRegValue);
					break;
				}
				tslog_nIndex++;
				tslog_strDestFuncSig.insert(tslog_nPos, tslog_szRegValue);
			}
			if(MAX_USERDATA_SIZE <= tslog_nPos)
				tslog_strDestFuncSig = pszFuncSig;
		}
	}
#if (defined(UNICODE) || defined(_UNICODE))
 	static std::basic_string<WCHAR>tslog_strDestFuncSigW;
 	CTSLog::MultiByteToUnicode(tslog_strDestFuncSig,tslog_strDestFuncSigW );
 	tslog_ptszDestFuncSig = tslog_strDestFuncSigW.c_str();
#else
	tslog_ptszDestFuncSig = tslog_strDestFuncSig.c_str();
#endif
	return tslog_ptszDestFuncSig;
}
#endif //end #if (!defined TSLOG_SIMPLE)
//TSTRACE, TSDEBUG, TSINFO, TSWARN, TSERROR, TSFATAL, TSPROMPT, TSHEXDUMP, INSTALLTSLOGINI
#define TSTRACE	 __if_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__)) CTSLog::GetInstance(__TFUNCTION__ ,(LPVOID)this).TSTrace} __if_not_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__).TSTrace}
#define TSDEBUG  __if_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__)) CTSLog::GetInstance(__TFUNCTION__ ,(LPVOID)this).TSDebug} __if_not_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__).TSDebug}
#define TSINFO 	 __if_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__ ,(LPVOID)this).TSInfo } __if_not_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__).TSInfo }
#define TSWARN	 __if_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__ ,(LPVOID)this).TSWarn } __if_not_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__).TSWarn }
#define TSERROR  __if_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__ ,(LPVOID)this).TSError} __if_not_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__).TSError}
#define TSFATAL	 __if_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__ ,(LPVOID)this).TSFatal} __if_not_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__).TSFatal}
#define TSPROMPT __if_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__ ,(LPVOID)this).TSLog  } __if_not_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__)) CTSLog::GetInstance(__TFUNCTION__).TSLog  }
#define TSHEXDUMP __if_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__))  CTSLog::GetInstance(__TFUNCTION__ ,(LPVOID)this).TSHexDump } __if_not_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__)) CTSLog::GetInstance(__TFUNCTION__).TSHexDump }
#define INSTALLTSLOGINI CTSLog::GetInstance(NULL).InstallTSLogIni
/////////////////TSTRACEAUTO/////////////////
#pragma warning( disable : 4731 ) //"pointer": 框架指针寄存器“register”被内联程序集代码修改
#define  _TSTRACEAUTO() \
	DWORD tslog_dwEBP = 0; \
	LPCTSTR tslog_lpctszFuncSig = NULL; \
	{ __asm mov dword ptr[tslog_dwEBP], ebp } \
	__if_exists(this){ tslog_lpctszFuncSig = CTSLog::TraceAuto(__FUNCSIG__,tslog_dwEBP,FALSE); if(!tslog_lpctszFuncSig) tslog_lpctszFuncSig = __TFUNCSIG__; CTSLog::CEnterLeaveFunCallBack callback(tslog_lpctszFuncSig,ENTER_FUNCTION_FLAG __TFUNCTION__ , __TFUNCSIG__,(LPCVOID)this); } \
	__if_not_exists(this){tslog_lpctszFuncSig = CTSLog::TraceAuto(__FUNCSIG__,tslog_dwEBP,TRUE);if(!tslog_lpctszFuncSig) tslog_lpctszFuncSig = __TFUNCSIG__; CTSLog::CEnterLeaveFunCallBack callback(tslog_lpctszFuncSig,ENTER_FUNCTION_FLAG __TFUNCTION__, __TFUNCSIG__); } \
	if(!tslog_lpctszFuncSig){{__asm push ebp }{__asm pop ebp  }} //no reach, but absolutely necessarily. 

#define  _TSTRACEAUTO2() \
	DWORD tslog_dwEBP; \
	LPCTSTR tslog_lpctszFuncSig; \
	{ __asm mov dword ptr[tslog_dwEBP], ebp } \
	__if_exists(this){ if(CTSLog::IsTRACEValid()) { tslog_lpctszFuncSig = CTSLog::TraceAuto(__FUNCSIG__,tslog_dwEBP,FALSE); if(!tslog_lpctszFuncSig) tslog_lpctszFuncSig = __TFUNCSIG__; CTSLog::CEnterLeaveFunCallBack callback(tslog_lpctszFuncSig,ENTER_FUNCTION_FLAG __TFUNCTION__ , __TFUNCSIG__,(LPCVOID)this); } } \
	__if_not_exists(this){ if(CTSLog::IsTRACEValid()) { tslog_lpctszFuncSig = CTSLog::TraceAuto(__FUNCSIG__,tslog_dwEBP,TRUE);if(!tslog_lpctszFuncSig) tslog_lpctszFuncSig = __TFUNCSIG__; CTSLog::CEnterLeaveFunCallBack callback(tslog_lpctszFuncSig,ENTER_FUNCTION_FLAG __TFUNCTION__, __TFUNCSIG__); } } \
	if(!tslog_lpctszFuncSig){{__asm push ebp }{__asm pop ebp  }} //no reach, but absolutely necessarily. 

#define _TSTRACEAUTO_SIMPLE() \
	__if_exists(this) { CTSLog::CEnterLeaveFunCallBack callback( __TFUNCSIG__ , ENTER_FUNCTION_FLAG __TFUNCTION__ ,__TFUNCSIG__, (LPCVOID)this, TRUE); } \
	__if_not_exists(this){ CTSLog::CEnterLeaveFunCallBack callback(__TFUNCSIG__ ,ENTER_FUNCTION_FLAG __TFUNCTION__,__TFUNCSIG__, NULL, TRUE); } 

#define _TSTRACEAUTO_SIMPLE2() \
	__if_exists(this) { if(CTSLog::IsTRACEValid()) { CTSLog::CEnterLeaveFunCallBack callback( __TFUNCSIG__ , ENTER_FUNCTION_FLAG __TFUNCTION__ ,__TFUNCSIG__, (LPCVOID)this, TRUE); } } \
	__if_not_exists(this){ if(CTSLog::IsTRACEValid()) { CTSLog::CEnterLeaveFunCallBack callback(__TFUNCSIG__ ,ENTER_FUNCTION_FLAG __TFUNCTION__,__TFUNCSIG__, NULL, TRUE); } }

#define _TSTRACEAUTO_COMPLEX() \
	DWORD tslog_dwEBP; \
	LPCTSTR tslog_lpctszFuncSig; \
	{ __asm mov dword ptr[tslog_dwEBP], ebp } \
	__if_exists(this){tslog_lpctszFuncSig = CTSLog::TraceAuto(__FUNCSIG__,tslog_dwEBP,FALSE);if(!tslog_lpctszFuncSig) tslog_lpctszFuncSig = __TFUNCSIG__; CTSLog::CEnterLeaveFunCallBack callback(tslog_lpctszFuncSig,ENTER_FUNCTION_FLAG __TFUNCTION__ , __TFUNCSIG__,(LPCVOID)this, FALSE, tslog_dwEBP, __FUNCSIG__); } \
	__if_not_exists(this){tslog_lpctszFuncSig = CTSLog::TraceAuto(__FUNCSIG__,tslog_dwEBP,TRUE);if(!tslog_lpctszFuncSig) tslog_lpctszFuncSig = __TFUNCSIG__;CTSLog::CEnterLeaveFunCallBack callback(tslog_lpctszFuncSig,ENTER_FUNCTION_FLAG __TFUNCTION__, __TFUNCSIG__, NULL, FALSE, tslog_dwEBP, __FUNCSIG__); } \
	if(!tslog_lpctszFuncSig){{__asm push ebp }{__asm pop ebp  }} //no reach, but absolutely necessarily. 

#define _TSTRACEAUTO_COMPLEX2() \
	DWORD tslog_dwEBP; \
	LPCTSTR tslog_lpctszFuncSig; \
	{ __asm mov dword ptr[tslog_dwEBP], ebp } \
	__if_exists(this){ if(CTSLog::IsTRACEValid()) { tslog_lpctszFuncSig = CTSLog::TraceAuto(__FUNCSIG__,tslog_dwEBP,FALSE);if(!tslog_lpctszFuncSig) tslog_lpctszFuncSig = __TFUNCSIG__; CTSLog::CEnterLeaveFunCallBack callback(tslog_lpctszFuncSig,ENTER_FUNCTION_FLAG __TFUNCTION__ , __TFUNCSIG__,(LPCVOID)this, FALSE, tslog_dwEBP, __FUNCSIG__); } } \
	__if_not_exists(this){ if(CTSLog::IsTRACEValid()) { tslog_lpctszFuncSig = CTSLog::TraceAuto(__FUNCSIG__,tslog_dwEBP,TRUE);if(!tslog_lpctszFuncSig) tslog_lpctszFuncSig = __TFUNCSIG__;CTSLog::CEnterLeaveFunCallBack callback(tslog_lpctszFuncSig,ENTER_FUNCTION_FLAG __TFUNCTION__, __TFUNCSIG__, NULL, FALSE, tslog_dwEBP, __FUNCSIG__); } }\
	if(!tslog_lpctszFuncSig){{__asm push ebp }{__asm pop ebp  }} //no reach, but absolutely necessarily. 

#ifdef TSLOG_SIMPLE  //#define TSLOG or TSLOG_COMPLEX
#define TSTRACEAUTO _TSTRACEAUTO_SIMPLE
#define TSTRACEAUTO2 _TSTRACEAUTO_SIMPLE2
#elif defined TSLOG
#define TSTRACEAUTO _TSTRACEAUTO
#define TSTRACEAUTO2 _TSTRACEAUTO2
#else
#define TSTRACEAUTO _TSTRACEAUTO_COMPLEX
#define TSTRACEAUTO2 _TSTRACEAUTO_COMPLEX2
#endif
/////////////////END TSTRACEAUTO/////////////

//V2.0
#define __TS4CXX(level, nop, LogData) __if_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__) ) (CTSLog::GetInstance(__TFUNCTION__, this)<<LogData).TSLog4CXX_Flush(CTSLog::LEVEL_##level)} \
								  __if_not_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__) ) (CTSLog::GetInstance(__TFUNCTION__, NULL)<<LogData).TSLog4CXX_Flush(CTSLog::LEVEL_##level)}

#define __TS4CXX2(level, nop, LogData) __if_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__) && CTSLog::IsLogValid(CTSLog::LEVEL_##level)) (CTSLog::GetInstance(__TFUNCTION__, this)<<LogData).TSLog4CXX_Flush(CTSLog::LEVEL_##level)} \
								  __if_not_exists(this){if(!CTSLog::IsModuleUnloading(__TFUNCTION__) && CTSLog::IsLogValid(CTSLog::LEVEL_##level)) (CTSLog::GetInstance(__TFUNCTION__, NULL)<<LogData).TSLog4CXX_Flush(CTSLog::LEVEL_##level)}

//debug
#define  _TSDEBUG4CXX(nop, LogData)  __TS4CXX(DEBUG, nop, LogData)
#define  TSDEBUG4CXX(LogData) _TSDEBUG4CXX(0, LogData)
//trace
#define  _TSTRACE4CXX(nop, LogData)   __TS4CXX(TRACE, nop, LogData)
#define  TSTRACE4CXX(LogData) _TSTRACE4CXX(0, LogData)
//info
#define  _TSINFO4CXX(nop, LogData)    __TS4CXX(INFO, nop, LogData)
#define  TSINFO4CXX(LogData) _TSINFO4CXX(0, LogData)
//warn
#define  _TSWARN4CXX(nop, LogData)	  __TS4CXX(WARN, nop, LogData)
#define  TSWARN4CXX(LogData) _TSWARN4CXX(0, LogData)
//error
#define  _TSERROR4CXX(nop, LogData)  __TS4CXX(ERROR, nop, LogData)
#define  TSERROR4CXX(LogData) _TSERROR4CXX(0, LogData)
//fatal
#define  _TSFATAL4CXX(nop, LogData)  __TS4CXX(FATAL, nop, LogData)
#define  TSFATAL4CXX(LogData) _TSFATAL4CXX(0, LogData)

//debug
#define  _TSDEBUG4CXX2(nop, LogData)  __TS4CXX2(DEBUG, nop, LogData)
#define  TSDEBUG4CXX2(LogData) _TSDEBUG4CXX2(0, LogData)
//trace
#define  _TSTRACE4CXX2(nop, LogData)   __TS4CXX2(TRACE, nop, LogData)
#define  TSTRACE4CXX2(LogData) _TSTRACE4CXX2(0, LogData)
//info
#define  _TSINFO4CXX2(nop, LogData)    __TS4CXX2(INFO, nop, LogData)
#define  TSINFO4CXX2(LogData) _TSINFO4CXX2(0, LogData)
//warn
#define  _TSWARN4CXX2(nop, LogData)	  __TS4CXX2(WARN, nop, LogData)
#define  TSWARN4CXX2(LogData) _TSWARN4CXX2(0, LogData)
//error
#define  _TSERROR4CXX2(nop, LogData)  __TS4CXX2(ERROR, nop, LogData)
#define  TSERROR4CXX2(LogData) _TSERROR4CXX2(0, LogData)
//fatal
#define  _TSFATAL4CXX2(nop, LogData)  __TS4CXX2(FATAL, nop, LogData)
#define  TSFATAL4CXX2(LogData) _TSFATAL4CXX2(0, LogData)

//end V2.0
#else //end #ifdef TSLOG

#ifdef _TSLOG
#pragma message("warning: undefine macro 'TSLOG', 'TSLOG_SIMPLE', 'TSLOG2'/'TSLOG_COMPLEX', "   "so [#include \""  __FILE__  "\"] will be ignored." "However 'TSERROR/TSERROR4CXX, TSFATAL/TSFATAL4CXX' is valid all the same, and these log is dumped into $(CSIDL_COMMON_APPDATA)\\TSLOG or %TEMP%\\TSLOG\\.  "  __TIMESTAMP__)
#else
#pragma message("warning: undefine macro 'TSLOG', 'TSLOG_SIMPLE', 'TSLOG2'/'TSLOG_COMPLEX', "   "so [#include \""  __FILE__  "\"] will be ignored. " __TIMESTAMP__)
#endif

#define __TTSLOG_GROUP _T("TSLOG")

#if _MSC_VER >= 1400 //只有VC8才支持变参宏
#define TSTRACE(x, ...)
#define TSDEBUG(x, ...)
#define TSINFO(x, ...)
#define TSWARN(x, ...)
#define TSPROMPT __noop
#define TSTRACEAUTO __noop
#define TSHEXDUMP __noop
#define INSTALLTSLOGINI __noop
#else //vc6, vc7
#if _MSC_VER <1300		//VC6.0 以前版本
#define TSTRACE 1 ? (void) 0 : 0
#define TSDEBUG	1 ? (void) 0 : 0
#define TSINFO	1 ? (void) 0 : 0
#define TSWARN	1 ? (void) 0 : 0
#define INSTALLTSLOGINI 1 ? (void) 0 : 0
#define TSTRACEAUTO 1 ? (void) 0 : 0
#define TSTRACEAUTO2 1 ? (void) 0 : 0
#define TSPROMPT 1 ? (void) 0 : 0
#define TSTRACE4CXX 1 ? (void) 0 : 0
#define TSDEBUG4CXX 1 ? (void) 0 : 0
#define TSINFO4CXX 1 ? (void) 0 : 0
#define TSWARN4CXX 1 ? (void) 0 : 0
#define TSERROR4CXX 1 ? (void) 0 : 0
#define TSFATAL4CXX 1 ? (void) 0 : 0
#define TSTRACE4CXX2 1 ? (void) 0 : 0
#define TSDEBUG4CXX2 1 ? (void) 0 : 0
#define TSINFO4CXX2 1 ? (void) 0 : 0
#define TSWARN4CXX2 1 ? (void) 0 : 0
#define TSERROR4CXX2 1 ? (void) 0 : 0
#define TSFATAL4CXX2 1 ? (void) 0 : 0
#else	//VC.net以后版本
#define TSTRACE __noop
#define TSDEBUG	__noop
#define TSINFO	__noop
#define TSWARN	__noop
#define TSPROMPT __noop
#define TSTRACEAUTO __noop
#define TSTRACEAUTO2 __noop
#define TSHEXDUMP __noop
#define INSTALLTSLOGINI __noop
#endif
#endif //_MSC_VER >= 1400

//V2.0 //无论是何种vc环境
//兼容 LOG4CPLUS_DEBUG(logger,b)
#define _TSTRACE4CXX(a,b)
#define _TSDEBUG4CXX(a,b)
#define _TSINFO4CXX(a,b)
#define _TSWARN4CXX(a,b)

#define _TSTRACE4CXX2(a,b)
#define _TSDEBUG4CXX2(a,b)
#define _TSINFO4CXX2(a,b)
#define _TSWARN4CXX2(a,b)
//#define _TSERROR4CXX(a,b) (CTSLog::GetInstance()<<b).TSLog4CXX_Flush()
//#define _TSFATAL4CXX(a,b) (CTSLog::GetInstance()<<b).TSLog4CXX_Flush()
#define TSTRACE4CXX(b)  _TSTRACE4CXX(0,b)
#define TSDEBUG4CXX(b)  _TSDEBUG4CXX(0,b)
#define TSINFO4CXX(b)  _TSINFO4CXX(0,b)
#define TSWARN4CXX(b)  _TSWARN4CXX(0,b)
#define TSERROR4CXX(b)  _TSERROR4CXX(0,b)
#define TSFATAL4CXX(b)  _TSFATAL4CXX(0,b)

#define TSTRACE4CXX2(b)  _TSTRACE4CXX2(0,b)
#define TSDEBUG4CXX2(b)  _TSDEBUG4CXX2(0,b)
#define TSINFO4CXX2(b)  _TSINFO4CXX2(0,b)
#define TSWARN4CXX2(b)  _TSWARN4CXX2(0,b)
#define TSERROR4CXX2(b)  _TSERROR4CXX2(0,b)
#define TSFATAL4CXX2(b)  _TSFATAL4CXX2(0,b)
//end V2.0

#ifdef _TSLOG ////定义_TSLOG时,TSERROR,TSFATAL可输出日志
#define TSFATAL	 __if_not_exists(this){TSFatal} __if_exists(this){TSFatal}
#define _TSERROR4CXX(a,b) (CTSLog::GetInstance()<<b).TSLog4CXX_Flush()
#define _TSFATAL4CXX(a,b) (CTSLog::GetInstance()<<b).TSLog4CXX_Flush()
#define _TSERROR4CXX2(a,b) (CTSLog::GetInstance()<<b).TSLog4CXX_Flush()
#define _TSFATAL4CXX2(a,b) (CTSLog::GetInstance()<<b).TSLog4CXX_Flush()
#else
#define TSFATAL __noop
#define _TSERROR4CXX(a,b)
#define _TSFATAL4CXX(a,b)
#define _TSERROR4CXX2(a,b)
#define _TSFATAL4CXX2(a,b)
#endif
#define TSERROR	TSFATAL

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#if _WIN32_IE >= 0x0500
#include <shlobj.h>
#endif
#include <string> //V2.0 <<std::basic_string
#include <OAIdl.h>
inline void TSFatal(LPCTSTR pFmt, ...)
{
#if (defined(UNICODE) || defined(_UNICODE))
#define TTIMESTAMP2(x) L ## x
#define TTIMESTAMP(x) TTIMESTAMP2(x)
#define __TTIMESTAMP__  TTIMESTAMP(__TIMESTAMP__)
#else
#define __TTIMESTAMP__  __TIMESTAMP__
#endif
#define MAX_USERDATA_SIZE (4096 - 1)
#define MAX_PRIVATEDATA_SIZE 256
	TCHAR szModuleFileName[_MAX_PATH] = {0};
	TCHAR szdrive[_MAX_DRIVE] = {0};
	TCHAR szdir[_MAX_DIR] = {0};
	TCHAR szfname[_MAX_FNAME] = {0};
	TCHAR szext[_MAX_EXT] = {0};
	HANDLE hFile = (HANDLE)INVALID_HANDLE_VALUE;
	GetModuleFileName(NULL,szModuleFileName , _MAX_PATH);
	_tsplitpath(szModuleFileName, szdrive, szdir, szfname, szext );
	static DWORD dwProcessId = 0;
	if((DWORD)0 == dwProcessId)
		dwProcessId = GetCurrentProcessId();
	static TCHAR szMutex[MAX_PRIVATEDATA_SIZE] = {0};
	if('\0' == szMutex[0])
		_sntprintf(szMutex, MAX_PRIVATEDATA_SIZE - 1, _T("#mutex%s_%s"), __TTSLOG_GROUP,szfname);
	HANDLE hMutex = CreateMutex(NULL, true, szMutex);
	if(NULL == hMutex)
		return;
	bool bExist = (ERROR_ALREADY_EXISTS == ::GetLastError());
	if(bExist)
		WaitForSingleObject(hMutex, INFINITE);
	TCHAR szAppFile[_MAX_PATH] = {0}; //logfile path
	TCHAR szDir[_MAX_PATH] = {0};	 //logfile dir
#if _WIN32_IE >= 0x0500
	if(SUCCEEDED(SHGetFolderPath(NULL, 
		CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE, 
		NULL, 
		0, 
		szDir))) 
	{
		_sntprintf(szDir, _MAX_PATH, _T("%s\\TSLOG\\"), szDir);
		DWORD dwAttri = GetFileAttributes(szDir);
		if(dwAttri != INVALID_FILE_ATTRIBUTES && (dwAttri & FILE_ATTRIBUTE_DIRECTORY)) //目录存在
			;
		else if(!CreateDirectory(szDir, NULL))
			return;		
		_sntprintf(szAppFile, _MAX_PATH - 1, _T("%s%s_%s.%s"), szDir, __TTSLOG_GROUP, szfname, _T("log"));
		hFile =	CreateFile(szAppFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0 , NULL);
		FILETIME stCreationTime;
		if(GetFileTime(hFile, &stCreationTime, NULL, NULL))
		{
			SYSTEMTIME stSystemTime;
			if(FileTimeToSystemTime(&stCreationTime, &stSystemTime))
			{
				SYSTEMTIME stCurSystemTime;
				GetSystemTime(&stCurSystemTime);
				if(stCurSystemTime.wYear > stSystemTime.wYear || 1 <= stCurSystemTime.wMonth - stSystemTime.wMonth)
				{
					CloseHandle(hFile);
					DeleteFile(szAppFile);
					hFile =	CreateFile(szAppFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0 , NULL);
				}
			}
		}		
	}
#endif
	if(INVALID_HANDLE_VALUE == hFile)
	{
		GetTempPath(_MAX_PATH, szDir);
		_sntprintf(szDir, _MAX_PATH, _T("%s\\TSLOG\\"), szDir);
		DWORD dwAttri = GetFileAttributes(szDir);
		if(dwAttri != INVALID_FILE_ATTRIBUTES && (dwAttri & FILE_ATTRIBUTE_DIRECTORY)) //目录存在
			;
		else if(!CreateDirectory(szDir, NULL))
			return;
		_sntprintf(szAppFile, _MAX_PATH - 1, _T("%s%s_%s.%s"), szDir, __TTSLOG_GROUP, szfname, _T("log"));
		hFile =	CreateFile(szAppFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0 , NULL);
	}
	if (INVALID_HANDLE_VALUE != hFile)
	{
		DWORD dwPos = SetFilePointer(hFile,0, 0, FILE_END);
		if((DWORD)0 == dwPos)
		{
			LPCSTR pszPrompt = "date       time         processname    processid  threadid  logversion    buildtime                   userdata\r\n";
			DWORD nLen = (DWORD)strlen(pszPrompt);
			WriteFile(hFile, pszPrompt, nLen, &nLen, NULL);
		}
		static HINSTANCE hCurrentModule = (HINSTANCE)INVALID_HANDLE_VALUE;
		MEMORY_BASIC_INFORMATION m = {0};
		//time_t tnow;
		//static struct tm *pnow = NULL;
		//time(&tnow);
		//pnow = localtime(&tnow);
		VirtualQuery(&hCurrentModule, &m, sizeof(MEMORY_BASIC_INFORMATION));
		hCurrentModule = (HINSTANCE) m.AllocationBase;		
		GetModuleFileName(hCurrentModule, szModuleFileName,_MAX_PATH);
		_tsplitpath(szModuleFileName, szdrive, szdir, szfname, szext);
		TCHAR szData[MAX_USERDATA_SIZE + MAX_PRIVATEDATA_SIZE + 1];
		DWORD dwPrivateLen = 0, dwUserDataLen = 0, dwAllLen = 0;
		SYSTEMTIME nowtime = {0};
		GetLocalTime(&nowtime);
		dwPrivateLen = _sntprintf(szData, MAX_PRIVATEDATA_SIZE, _T("%04d-%02d-%02d %02d:%02d:%02d.%03d\t[%s]\t[%lu]\t[%lu]\t[0x%08x]\t[%s]\t"),			
			nowtime.wYear, nowtime.wMonth, nowtime.wDay,
			nowtime.wHour, nowtime.wMinute, nowtime.wSecond, nowtime.wMilliseconds,			
			szfname ,dwProcessId, GetCurrentThreadId(), TSLOG_VERSION, __TTIMESTAMP__);
		__try
		{
			va_list arg;
			va_start(arg, pFmt);
			dwUserDataLen = _vsntprintf(&szData[dwPrivateLen], MAX_USERDATA_SIZE, pFmt, arg);
			va_end(arg);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			dwUserDataLen = _sntprintf(&szData[dwPrivateLen],MAX_USERDATA_SIZE, _T("(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x"), pFmt, GetExceptionCode());
		}
		if((DWORD)-1 == dwUserDataLen)
			dwUserDataLen = (DWORD)_tcslen(&szData[dwPrivateLen]);
		dwAllLen = dwPrivateLen + dwUserDataLen;
		szData[dwAllLen++] = '\r';
		szData[dwAllLen++] = '\n';
		szData[dwAllLen] = '\0';
		CHAR szBufferA[MAX_USERDATA_SIZE + MAX_PRIVATEDATA_SIZE + 1] = {0};
#if (defined(UNICODE) || defined(_UNICODE))
		INT nLen = 0;
		nLen = WideCharToMultiByte(936, 0, szData, -1,NULL, 0 ,NULL, NULL);
		dwAllLen = WideCharToMultiByte(936, 0, szData, -1, szBufferA, sizeof(CHAR) * nLen,NULL,NULL);		
		dwAllLen--;
		OutputDebugStringA(szBufferA);
 		WriteFile(hFile, szBufferA, dwAllLen ,&dwPrivateLen,NULL);
#else
		OutputDebugStringA(szData);
		WriteFile(hFile, szData, dwAllLen,&dwPrivateLen,NULL);
#endif		
		CloseHandle(hFile);
	}
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	hMutex = NULL;
}
class CTSLog
{
public:
	inline	static			CTSLog & CTSLog::GetInstance(void)
	{
		static CTSLog _instance;
		return _instance;
	}
private:
	CTSLog()
	{
		TCHAR szLogData[MAX_USERDATA_SIZE + 1] = {0};
        m_pszLogDataA = &szLogData[0];//故意为保持代码一致
		m_dwUsedSize = 0;
		m_dwMaxLogDataSize = MAX_USERDATA_SIZE + 1;
		InitializeCriticalSection(&_cs);
	}
	~CTSLog()
	{
		m_pszLogDataA = NULL;
		m_dwUsedSize = 0;
		m_dwMaxLogDataSize = MAX_USERDATA_SIZE + 1;
		DeleteCriticalSection(&_cs);
	}
private:
	inline UINT GetStringCodePage(const char* )
	{
		return 936;
	}
private:
	DWORD m_dwUsedSize;
	DWORD m_dwMaxLogDataSize;
	LPTSTR m_pszLogDataA;
	CRITICAL_SECTION	_cs;			// 用于同步对文件的访问
public:
	//V2.0 重载<<
#define OVERFLOW_CHECK()  \
	if((DWORD)-1 == m_dwUsedSize) \
	m_dwUsedSize = m_dwMaxLogDataSize;		
	inline  CTSLog& 		operator << (const bool _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			if(_Val)
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("true"));
			else
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("false"));
			OVERFLOW_CHECK();
		}		
		return *this;
	}
	//字符
	inline  CTSLog& 		operator << (const TCHAR _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			if(isprint((int)_Val))
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%c"), _Val);
			else
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%d"), (INT)_Val);
			OVERFLOW_CHECK();
		}		
		return *this;
	}
#if (defined(UNICODE) || defined(_UNICODE))
	inline  CTSLog& 		operator << (const CHAR _Val)
#else
	inline  CTSLog& 		operator << (const WCHAR _Val)
#endif
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			if(isprint((int)_Val))
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%C"), _Val);
			else
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%d"), (INT)_Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const signed char _Val)
	{
		m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%u"), (INT)_Val);
		if((DWORD)-1 == m_dwUsedSize)
			m_dwUsedSize = m_dwMaxLogDataSize;
		return *this;
	}
	inline  CTSLog& 		operator << (const unsigned char _Val)
	{
		m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%u"), (UINT)_Val);
		if((DWORD)-1 == m_dwUsedSize)
			m_dwUsedSize = m_dwMaxLogDataSize;
		return *this;
	}
	//2字节整数
	inline  CTSLog& 		operator << (const signed short _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%hd"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
#if (defined _WCHAR_T_DEFINED && defined _NATIVE_WCHAR_T_DEFINED)
	//与 operator <<(WCHAR)冲突
	inline  CTSLog& 		operator << (const  unsigned short _Val)
	{		
		m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%hu"), _Val);
		if((DWORD)-1 == m_dwUsedSize)
			m_dwUsedSize = m_dwMaxLogDataSize;
		return *this;
	}
#endif	 
	//四字节整数
	inline  CTSLog& 		operator << (const signed int _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%d"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const unsigned int _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%u"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const signed __int64 _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%I64d"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const unsigned __int64 _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%I64u"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const long _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%ld"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const unsigned long _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%lu"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	inline  CTSLog& 		operator << (const void * _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("0x%p"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//float
	inline  CTSLog& 		operator << (const float _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%f"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//double
	inline  CTSLog& 		operator << (const double _Val)
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%f"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//long double
	inline  CTSLog& 		operator << (const long double _Val)  
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%lf"), _Val);
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//字符串
	inline  CTSLog& 		operator << (LPCTSTR _Val) throw()
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			__try
			{
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%s"), _Val);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				m_dwUsedSize +=_sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("\t(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x\t"), _Val, GetExceptionCode());
			}
			if((DWORD)-1 == m_dwUsedSize)
				m_dwUsedSize = (int)_tcslen(m_pszLogDataA);
			OVERFLOW_CHECK();
		}
		return *this;
	}
#if (defined(UNICODE) || defined(_UNICODE))
	inline  CTSLog& 		operator << (const char* _Val) throw()
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			WCHAR szBufferW[MAX_USERDATA_SIZE + 1] = {0};
			INT nLen = 0;
			UINT code_page = GetStringCodePage(_Val);
			__try
			{
				nLen = MultiByteToWideChar(code_page, 0,_Val,-1,NULL,0);
				if(0 == nLen)
				{
					code_page = 0;
					nLen = MultiByteToWideChar(code_page, 0,_Val,-1,NULL,0);
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{	
				nLen = 0;
				m_dwUsedSize +=_sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("\t(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x\t"), _Val, GetExceptionCode());				
				if((DWORD)-1 == m_dwUsedSize)
					m_dwUsedSize = (int)_tcslen(m_pszLogDataA);
				OVERFLOW_CHECK();
			}
			if(0 < nLen)
			{
				if(MAX_USERDATA_SIZE + 1 < nLen)
				{
					CHAR szLogDataW[MAX_USERDATA_SIZE + 1] = {0};
					memcpy(szLogDataW, _Val, MAX_USERDATA_SIZE * sizeof(CHAR));
					_Val = szLogDataW;
					nLen = MAX_USERDATA_SIZE + 1;
					nLen = MultiByteToWideChar(code_page, 0,_Val, -1, szBufferW,nLen);
					if(0 == nLen)
					{
						code_page = 0;
						nLen = MultiByteToWideChar(code_page, 0,_Val, -1, szBufferW,nLen);
					}
				}
				else
				{
					nLen = MultiByteToWideChar(code_page, 0,_Val, -1, szBufferW,nLen);
					if(0 == nLen)
					{
						code_page = 0;
						nLen = MultiByteToWideChar(code_page, 0,_Val, -1, szBufferW,nLen);
					}
				}
				if(0 < nLen)
				{
					__try
					{
						m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%s"), szBufferW);
					}
					__except(EXCEPTION_EXECUTE_HANDLER)
					{
						m_dwUsedSize +=_sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("\t(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x\t"), szBufferW, GetExceptionCode());
					}
					if((DWORD)-1 == m_dwUsedSize)
						m_dwUsedSize = (int)_tcslen(m_pszLogDataA);
					OVERFLOW_CHECK();
				}
			}
		}
		return *this;
	}
#else
	inline  CTSLog& 		operator << (const wchar_t*  _Val) throw()
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			CHAR szBufferA[MAX_USERDATA_SIZE + 1] = {0};	//调用频繁
			INT nLen = 0;
			__try
			{
				nLen = WideCharToMultiByte(936, 0, _Val, -1,NULL, 0 ,NULL, NULL);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				nLen = 0;
				m_dwUsedSize +=_sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("\t(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x\t"), _Val, GetExceptionCode());
				if((DWORD)-1 == m_dwUsedSize)
					m_dwUsedSize = (int)_tcslen(m_pszLogDataA);
				OVERFLOW_CHECK();
			}
			if(0 < nLen)
			{
				if(MAX_USERDATA_SIZE + 1 < nLen)
				{
					WCHAR szLogDataW[MAX_USERDATA_SIZE + 1] = {0};
					memcpy(szLogDataW, _Val, MAX_USERDATA_SIZE * sizeof(WCHAR));
					_Val = szLogDataW;
					nLen = MAX_USERDATA_SIZE + 1;
					nLen = WideCharToMultiByte(936, 0, _Val, -1, szBufferA, nLen,NULL,NULL);
				}
				else
					nLen = WideCharToMultiByte(936, 0, _Val, -1, szBufferA, nLen,NULL,NULL);
				if(0 < nLen)
				{
					__try
					{
						m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("%s"), szBufferA);
					}
					__except(EXCEPTION_EXECUTE_HANDLER)
					{
						m_dwUsedSize +=_sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("\t(!!!)Access Error, Invalid Memory Address : 0x%p, ExceptionCode : 0x%08x\t"), szBufferA, GetExceptionCode());
					}
					if((DWORD)-1 == m_dwUsedSize)
						m_dwUsedSize = (int)_tcslen(m_pszLogDataA);
					OVERFLOW_CHECK();
				}
			}
		}
		return *this;
	}
#endif
	//IID
	inline  CTSLog& 		operator << (REFIID _Val)  throw()
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			OLECHAR szIID[40];
			HRESULT hr = S_OK;
			hr = StringFromGUID2(_Val, szIID, 40);
			if(SUCCEEDED(hr))
				this->operator << (szIID);
			else
			{
				this->operator << ("{StringFromIID ERROR}");                                
			}
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//tagVARIANT
	inline  CTSLog& 		operator << (const tagVARIANT _Val)  throw()
	{
		EnterCriticalSection(&_cs);
		if(0 < m_dwUsedSize)
			LeaveCriticalSection(&_cs);
		if(m_dwUsedSize < m_dwMaxLogDataSize)
		{
			const tagVARIANT* pVar = &_Val;
			__try
			{
				switch (pVar->vt)
				{
				case VT_EMPTY:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{VT_EMPTY}"));
					break;
				case VT_NULL:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{VT_NULL}"));
					break;
				case VT_ERROR:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_ERROR}"), pVar->scode, pVar->scode);
					break;
				case VT_I1:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_I1}"), (INT)pVar->bVal, (INT)pVar->bVal);
					break;
				case VT_I2:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_I2}"), (INT)pVar->iVal, (INT)pVar->iVal);
					break;
				case VT_I4:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_I4}"), (INT)pVar->intVal, (INT)pVar->intVal);
					break;
				case VT_I8:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%I64x(%I64d) VT_I8}"), (ULONGLONG)pVar->ullVal, (LONGLONG)pVar->llVal);
					break;
				case VT_UI1:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%u) VT_UI1}"), (UINT)pVar->bVal, (UINT)pVar->bVal);
					break;
				case VT_UI2:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%u) VT_UI2}"), (UINT)pVar->iVal, (UINT)pVar->iVal);
					break;
				case VT_UI4:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%u) VT_UI4}"), (UINT)pVar->intVal, (UINT)pVar->intVal);
					break;
				case VT_UI8:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%I64x(%I64u) VT_UI8}"), (ULONGLONG)pVar->ullVal, (LONGLONG)pVar->llVal);
				case VT_BOOL:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_BOOL}"), (INT)pVar->intVal, (INT)pVar->intVal);
					break;
				case VT_INT:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_INT}"), (INT)pVar->intVal, (INT)pVar->intVal);
				case VT_UINT:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%u) VT_UINT}"), (UINT)pVar->intVal, (UINT)pVar->intVal);
				case VT_HRESULT:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%08x(%d) VT_HRESULT}"), (UINT)pVar->intVal, (INT)pVar->intVal);
					break;
				case VT_R8:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{%f VT_R8}"), pVar->dblVal);
					break;
				case VT_BSTR:
					(*this)<<"{L\""<<(pVar->bstrVal)<<"\" VT_BSTR}";
					break; 
				case VT_LPWSTR:
					(*this)<<"{L\""<<(pVar->bstrVal)<<"\" VT_LPWSTR}";
					break;
				case VT_LPSTR:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{\"%s\" VT_LPSTR}"), pVar->pcVal);
					break;
				case VT_DISPATCH:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%p VT_DISPATCH}"), pVar->pdispVal);
					break;
				default:
					m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize, _T("{0x%I64x(%I64d) VT_?[0x%08x(%d)]}"), (ULONGLONG)pVar->ullVal , (LONGLONG)pVar->llVal, pVar->vt, pVar->vt);
					break;
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				m_dwUsedSize += _sntprintf(&m_pszLogDataA[m_dwUsedSize], MAX_USERDATA_SIZE - m_dwUsedSize,_T("{VT_?[0x%08x(%d)], (!!!)Access Error}"), pVar->vt, pVar->vt);
			}
			OVERFLOW_CHECK();
		}
		return *this;
	}
	//std::string
	inline 	 CTSLog& 		operator << (const std::basic_string<WCHAR> _Val)  throw()	
	{
		this->operator << (_Val.c_str());
		return *this;
	}
	inline 	 CTSLog& 		operator << (const std::basic_string<CHAR> _Val)  throw()	
	{
		this->operator << (_Val.c_str());
		return *this;
	}
	//从缓冲到输出
	inline void				TSLog4CXX_Flush(void)
	{
		m_dwUsedSize = 0;
		TSFATAL(&m_pszLogDataA[0]);
		LeaveCriticalSection(&_cs);
	}
};
#endif //TSLOG
#endif //__TSLOG_H_2008_03_13
