
/********************************************************************
*
* =-----------------------------------------------------------------=
* =                                                                 =
* =             Copyright (c) Xunlei, Ltd. 2004-2011                =
* =                                                                 =
* =-----------------------------------------------------------------=
* 
*   FileName    :   XLFS.h
*   Author      :   xlue group(xlue@xunlei.com)
*   Create      :   2010-1-28
*   LastChange  :   
*   History     :	
*
*   Description :   Ѹ�������ļ�ϵͳ�ӿڶ���
*                   ���ǵ�ϵͳ���ݵ����⣬XLFS���õ���·�����Ǵ�Сд���е� 
*                   ������·�����뾡��ʹ��Linux��/
*                   ע���̰߳�ȫ����,XLFS�ṩ�Ľӿڶ�Ӧ�����̰߳�ȫ��
*                   �мƻ�֧���첽IO,���Խӿڶ��б���
*
********************************************************************/ 

#ifndef __XLFS_H__
#define __XLFS_H__

#ifdef XLFS_EXPORTS
	#ifdef __cplusplus
		#define XLFS_API(x) extern "C" __declspec(dllexport) x __stdcall 
	#else
		#define XLFS_API(x) __declspec(dllexport) x __stdcall 
	#endif //__cplusplus
#elif defined (XLUE_UNION)
	#ifdef __cplusplus
		#define XLFS_API(x) extern "C"  x __stdcall 
	#else
		#define XLFS_API(x) x __stdcall 
	#endif //__cplusplus
#else // XLFS_EXPORTS
	#ifdef __cplusplus
		#define XLFS_API(x) extern "C" __declspec(dllimport) x __stdcall 
	#else
		#define XLFS_API(x) __declspec(dllimport) x __stdcall 
	#endif //__cplusplus
#endif // XLFS_EXPORTS

// xlfs����Ķ���
typedef void * XLFS_FILE_HANDLE;
typedef void * XLFS_DIR_HANDLE;
typedef void * XLFS_PROTOCOLLOADER_HANDLE;
typedef void * XLFS_MEDIUMLOADER_HANDLE;
typedef void * XLFS_STREAM_HANDLE;


#define MAX_ABSPATH_LENGTH 512


typedef int (*pfnFileOpenCallBack)(void *pParam,XLFS_FILE_HANDLE hFile,int result);
typedef int (*pfnDirOpenCallBack)(void *pParam,XLFS_DIR_HANDLE hDir,int result);
typedef int (*pfnFileUpdateCallBack)(void *pParam);

// �Զ���Ľ��ܻص�����
typedef int (*pfnDecryptCallBack)(void* userData, const void* lpContext, __int64 contextLen, void* lpDecryptBuffer, __int64 decryptBufferOffset, __int64 decryptBufferLen); 

typedef struct tagFileOpenCallBackData
{
	pfnFileOpenCallBack callBackFunc; //�ص�����
	void * pParam; //��������thisָ��
}FileOpenCallBackData, *LPFileOpenCallBackData;

typedef struct tagFileUpdateCallBackData
{
	pfnFileUpdateCallBack callBackFunc; //�ص�����
	void * pParam; //��������thisָ��
}FileUpdateCallBackData, *LPFileUpdateCallBackData;

typedef struct tagDirOpenCallBackData
{
	pfnDirOpenCallBack callBackFunc; //�ص�����
	void * pParam; //��������thisָ��
}DirOpenCallBackData;

typedef struct tagXLFSDirEntry
{  
	unsigned long Attributes; //��ʱδ�������Ե����� 
	__int64 CreationTime;  
	__int64 LastAccessTime;  
	__int64 LastWriteTime;  
	__int64 FileSize;//DIRû��size
	wchar_t Name[MAX_PATH];  
} XLFSDirEntry;

// xlfs������Ķ���
#define XLFS_RESULT_SUCCESS        0
#define XLFS_RESULT_ASYN_OPENING   1
#define XLFS_RESULT_FILE_NOT_FOUND 2
#define XLFS_RESULT_NOT_OPEN       3
#define XLFS_RESULT_PATH_ERROR     4
#define XLFS_RESULT_PROTOCOL_ERROR 5
#define XLFS_RESULT_OPENMODE_NOT_SUPPORTED 6

#define XLFS_RESULT_DIR_NOT_FOUND 7
#define XLFS_RESULT_INVALID_PARAM 8
#define XLFS_RESULT_NOT_XAR_FILE 9
#define XLFS_RESULT_BLOCK_MEDIA 10
#define XLFS_RESULT_NOT_SUPPORTED	11
#define XLFS_RESULT_NOT_FILE	12
#define XLFS_RESULT_NOT_DIR		13
#define XLFS_RESULT_MEDIUM_NOT_SUPPORTED	14
#define XLFS_RESULT_LOAD_MODULE_FAILED	15
#define XLFS_RESULT_LOAD_RESOURCE_NOT_FOUND	16
#define XLFS_RESULT_LOAD_LOAD_RESOURCE_FAILED	17
#define XLFS_RESULT_LOAD_INVALID_PEFILE	18
#define XLFS_RESULT_LOAD_INVALID_SECITON	19

#define XLFS_RESULT_INVLAID_HANDLE -1

// ��ʼ���ͷ���ʼ����غ���
XLFS_API(long) XLFS_Init();
XLFS_API(long) XLFS_Uninit();

// �����Զ���Ľ��ܻص�����
XLFS_API(long) XLFS_SetXARDecryptCallBack(int flags, pfnDecryptCallBack lpCallBack, void* userData);

// �ļ�ϵͳ���
XLFS_API(long) XLFS_FileExists(const wchar_t * filePath);
XLFS_API(long) XLFS_DirExists(const wchar_t* dirPath);

// ·��������
XLFS_API(long) XLFS_FixPath(const wchar_t* pSrcPath,wchar_t** ppDestPath);

//
// �ļ���ز���,����¼�����
// ע�⣬ֻ���ļ������첽��(�첽��δʵ��)
//
//openMode�̳���C,����r,w,r+��
XLFS_API(long) XLFS_OpenFile(const wchar_t * filePath, const wchar_t * openMode, XLFS_FILE_HANDLE *phFile,LPFileOpenCallBackData pCallBack);
XLFS_API(__int64) XLFS_ReadFile(XLFS_FILE_HANDLE hFile, unsigned char *pBuffer, __int64 bufferLen);
XLFS_API(__int64) XLFS_WriteFile(XLFS_FILE_HANDLE hFile, const unsigned char *pBuffer, __int64 bufferLen);
XLFS_API(__int64) XLFS_SeekFile(XLFS_FILE_HANDLE hFile,__int64 newPosition,int origin);
XLFS_API(__int64) XLFS_GetFileSize(XLFS_FILE_HANDLE hFile);
XLFS_API(__int64) XLFS_GetFilePosition(XLFS_FILE_HANDLE hFile);
XLFS_API(long) XLFS_DeleteFile(const wchar_t * filePath);
XLFS_API(long) XLFS_IsEOF(XLFS_FILE_HANDLE hFile);
XLFS_API(long) XLFS_CloseFile(XLFS_FILE_HANDLE hFile);
XLFS_API(long) XLFS_GetFileAbsPath(XLFS_FILE_HANDLE hFile,const wchar_t** ppResult);

XLFS_API(long) XLFS_GetXARFileType(XLFS_FILE_HANDLE hFile, unsigned long* lpType);

//
// Ŀ¼��ز���,��Ҫ���ڱ���
//
XLFS_API(long) XLFS_OpenDirectory(const wchar_t * dirPath, XLFS_DIR_HANDLE *phDir,DirOpenCallBackData* pCallback);
XLFS_API(long) XLFS_ReadDirectory(XLFS_DIR_HANDLE hDir,XLFSDirEntry* pResult);
XLFS_API(long) XLFS_CloseDirectory(XLFS_DIR_HANDLE hDir);

//
//Mount��ز���
//
XLFS_API(long) XLFS_MountDir(const wchar_t* pDestPath,const wchar_t* pSrcPath,long mountOption,const wchar_t* mountCmd);
XLFS_API(long) XLFS_MountFile(const wchar_t* pDestPath,const wchar_t* pSrcPath,long mountOption,const wchar_t* mountCmd);
XLFS_API(long) XLFS_UnMount(const wchar_t* pDestPath,long unmountOption);
XLFS_API(long) XLFS_GetRealAbsPath(const wchar_t* pVirtualPath, int nIndex, wchar_t** ppResult);
XLFS_API(long) XLFS_GetRealAbsPathCount(const wchar_t* pVirtualPath, int *pnCount);

//
//�ļ�Cache��ز���
//

//vaildTime˵���˸�cache����Чʱ��(��λms)������Ϊ0ʹ��ϵͳĬ��ֵ��-1Ϊ��Զ��Ч
XLFS_API(XLFS_FILE_HANDLE) XLFS_UpdateCacheFile(const wchar_t * filePath,long vaildTime);
XLFS_API(long) XLFS_GetCacheFile(const wchar_t * fileAbsPath, XLFS_FILE_HANDLE *phFile);

// �ͷ�һ��ʱ���ڲ�ʹ�õ���Դ�������ڴ�ռ��
XLFS_API(long) XLFS_CacheGC(); 

//������һ��cachefile��Ч,�Ը�cachefile����һ��Update�������ᴥ��һ���ļ�IO
XLFS_API(long) XLFS_RemoveCacheFile(const wchar_t * filePath);

//�趨xarʹ�õļ���Э��
XLFS_API(long) XLFS_SetXARProtocol(short version, unsigned long mask);

// ͳ����Ϣ�ӿ�
XLFS_API(long) XLFS_Stat(long type);

// ��������ؽӿ�
XLFS_API(XLFS_STREAM_HANDLE) XLFS_CreateStreamFromFile(const wchar_t* fileName, const wchar_t* lpMode);
XLFS_API(XLFS_STREAM_HANDLE) XLFS_CreateStreamFromMountFile(const wchar_t* fileName, const wchar_t* lpMode);
XLFS_API(XLFS_STREAM_HANDLE) XLFS_CreateStreamFromMemory(void* lpBuffer, unsigned long size);
XLFS_API(XLFS_STREAM_HANDLE) XLFS_CreateStreamFromResource(HMODULE hModule, const wchar_t* resName, const wchar_t* resType);

XLFS_API(unsigned long) XLFS_AddRefStream(XLFS_STREAM_HANDLE hStream);
XLFS_API(unsigned long) XLFS_ReleaseStream(XLFS_STREAM_HANDLE hStream);

XLFS_API(BOOL) XLFS_StreamSeek(XLFS_STREAM_HANDLE hStream, int offset, int origin);
XLFS_API(unsigned int) XLFS_StreamRead(XLFS_STREAM_HANDLE hStream, void* buffer, unsigned int size);
XLFS_API(unsigned int) XLFS_StreamWrite(XLFS_STREAM_HANDLE hStream, const void* buffer, unsigned int size);
XLFS_API(unsigned int) XLFS_StreamGetPosition(XLFS_STREAM_HANDLE hStream);
XLFS_API(unsigned int) XLFS_StreamSetPosition(XLFS_STREAM_HANDLE hStream, unsigned int pos);
XLFS_API(unsigned int) XLFS_StreamGetSize(XLFS_STREAM_HANDLE hStream);
XLFS_API(unsigned int) XLFS_StreamSetSize(XLFS_STREAM_HANDLE hStream, unsigned int size);

#endif // __XLFS_H__