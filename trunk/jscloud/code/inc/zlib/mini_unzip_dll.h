#ifndef mini_unzip_dll_D5AEC4E4_5DC4_46eb_BAA2_6DE02315B088
#define mini_unzip_dll_D5AEC4E4_5DC4_46eb_BAA2_6DE02315B088

// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
//��ı�׼�������� DLL �е������ļ��������������϶���� MINI_UNZIP_DLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
//�κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ 
// MINI_UNZIP_DLL_API ������Ϊ�ǴӴ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef MINI_UNZIP_DLL_EXPORTS
#define MINI_UNZIP_DLL_API __declspec(dllexport)
#else
#define MINI_UNZIP_DLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// this function is exported from mini_unzip_dll.dll.
// unzip a .zip file to an existing folder. return 0 when succeeded, but non zero when failed.   
MINI_UNZIP_DLL_API int mini_unzip_dll( const char * zipfilename, const char * directroy );

#ifdef __cplusplus
};
#endif
    
#endif