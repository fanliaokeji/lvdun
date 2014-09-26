/********************************************************************
*
* =-----------------------------------------------------------------=
* =                                                                 =
* =             Copyright (c) Xunlei, Ltd. 2004-2011              =
* =                                                                 =
* =-----------------------------------------------------------------=
* 
*   FileName    :   XLGraphicPlus
*   Author      :   xlue group(xlue@xunlei.com)
*   Create      :   2011-9-26 15:38
*   LastChange  :   2011-9-26 15:38
*   History     :	
*
*   Description :   XLGraphics�������ͷ�ļ�
*
********************************************************************/ 
#ifndef __XLGRAPHICPLUS_H__
#define __XLGRAPHICPLUS_H__

#ifdef WIN32
	#ifdef XLGRAPHICPLUS_EXPORTS
		#ifdef __cplusplus
			#define XLGRAPHICPLUS_API(x) extern "C" __declspec(dllexport) x __stdcall 
		#else
			#define XLGRAPHICPLUS_API(x) __declspec(dllexport) x __stdcall 
		#endif //__cplusplus
	#elif defined (XLUE_UNION)
		#ifdef __cplusplus
			#define XLGRAPHICPLUS_API(x) extern "C" x __stdcall 
		#else
			#define XLGRAPHICPLUS_API(x) x __stdcall 
		#endif //__cplusplus
	#else //XLGRAPHICPLUS_API
		#ifdef __cplusplus
			#define XLGRAPHICPLUS_API(x) extern "C" __declspec(dllimport) x __stdcall 
		#else
			#define XLGRAPHICPLUS_API(x) __declspec(dllimport) x __stdcall 
		#endif // __cplusplus
	#endif //XLGRAPHICPLUS_EXPORTS
#else
	#ifdef __cplusplus
		#define XLGRAPHICPLUS_API(x) extern "C" x 
	#else // __cplusplus
		#define XLGRAPHICPLUS_API(x) x 
	#endif // __cplusplus
#endif

#include <XLGraphic.h>
#include <XLLuaRuntime.h>


//������XGP����Ҫ�ĸ��ִ�����
#define XLGP_RESULT_SUCCESS			0
#define XLGP_RESULT_FAILED			1   

#define XLGP_RESULT_INVALIDPARAM	2	// ��Ч����

#define XLGP_RESULT_OUT_OF_MEMORY   3   //�ڴ����벻��


//��������չ��Ļ������ݽṹ
typedef void*	XLGP_ICON_HANDLE;
typedef void*	XLGP_GIF_HANDLE;
typedef void*	XLGP_CURSOR_HANDLE;

typedef int  TCurveData[256];

////��ʼ��������ݽṹ�ͽӿ�
typedef struct tagXLGraphicPlusParam
{
	BOOL bInitLua;	// �Ƿ��ʼ��lua��Ĭ��ΪTRUE
	// ���ӳ�ʼ���ֶ�
}XLGraphicPlusParam;

XLGRAPHICPLUS_API(int)	XLGP_InitGraphicPlus(const XLGraphicPlusParam* lpInitParam);
XLGRAPHICPLUS_API(int)	XLGP_UnInitGraphicPlus();
XLGRAPHICPLUS_API(BOOL) XLGP_PrepareGraphicPlusParam(XLGraphicPlusParam* lpInitParam);

XLGRAPHICPLUS_API(BOOL) XLGP_RegisterLuaHost();


//////HBITMAP��XL_BITMAP_HANDLE��ת

//ת�����豸�޹�λͼ������destBitCountָ����Ŀ��λͼ������λ��������ȡֵ32��24��16��8
XLGRAPHICPLUS_API(HBITMAP) XLGP_ConvertXLBitmapToDIB(XL_BITMAP_HANDLE hBitmap, unsigned short destBitCount);
//ת�����豸���λͼ������hDCָ����Ŀ���豸dc
XLGRAPHICPLUS_API(HBITMAP) XLGP_ConvertXLBitmapToDDB(XL_BITMAP_HANDLE hBitmap, HDC hDC);

//ת����XLGraphic֧�ֵ�λͼ��ʽ������destColorTypeָ����Ŀ��λͼ�ĸ�ʽ��Ŀǰ֧��XLGRAPHIC_CT_ARGB32��XLGRAPHIC_CT_RGB24��XLGRAPHIC_CT_GRAY
//��Ҫע�⣬���DIB�������32λ�ģ���ôת����XLBitmap��alphaͨ���ᱣ��ԭDIB�ģ����ԭDIB��alphaͨ�������壬��ô��ʹ��XL_ResetAlphaChannel�Խ������alpha����
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_ConvertDIBToXLBitmap(HBITMAP hBitmap, unsigned long destColorType);
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_ConvertDDBToXLBitmap(HDC hDC, HBITMAP hBitmap, unsigned long destColorType);

//�豸���λͼת�豸�޹�λͼ��Ŀ��λͼ����32bit; ���Դλͼ�Ѿ���DIB����ô����null
XLGRAPHICPLUS_API(HBITMAP) XLGP_ConvertDDBToDIB(HDC hDC, HBITMAP hBitmap);

//////jpeg��ؽӿ�
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_LoadJpegFromStream(XLFS_STREAM_HANDLE hStream);
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_LoadJpegFromFile(const wchar_t* lpFileName);

//////����bmp�Ľӿ�
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_LoadBmpFromStream(XLFS_STREAM_HANDLE hStream);
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_LoadBmpFromFile(const wchar_t* lpFileName);
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_LoadOriginBmpFromStream(XLFS_STREAM_HANDLE hStream);
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_LoadOriginBmpFromFile(const wchar_t* lpFileName);

//////icon��ؽӿ�
XLGRAPHICPLUS_API(XLGP_ICON_HANDLE) XLGP_CreateIconFromBitmap(XL_BITMAP_HANDLE hBitmap);
XLGRAPHICPLUS_API(XLGP_ICON_HANDLE) XLGP_CreateIconFromHandle(HICON hIcon);

//���ļ�����������ͼ��
XLGRAPHICPLUS_API(XLGP_ICON_HANDLE) XLGP_LoadIconFromStream(XLFS_STREAM_HANDLE hStream, int cxDesired, int cyDesired);
XLGRAPHICPLUS_API(XLGP_ICON_HANDLE) XLGP_LoadIconFromFile(const wchar_t* lpFileName, int cxDesired, int cyDesired);

// ��dll����exe�ļ���ȡָ��������ָ����С��ͼ�꣬iconindex��0��ʼ
XLGRAPHICPLUS_API(XLGP_ICON_HANDLE) XLGP_LoadIconFromModuleFile(const wchar_t* lpExeFileName, int iconIndex, int cxDesired, int cyDesired);
// ��dll����exeģ����ȡָ��������ָ����С��ͼ�꣬iconindex��0��ʼ
XLGRAPHICPLUS_API(XLGP_ICON_HANDLE) XLGP_LoadIconFromModule(HMODULE hModule, int iconIndex, int cxDesired, int cyDesired);

// ��ָ�����ļ����ͻ�ȡָ����С��ͼ��
XLGRAPHICPLUS_API(XLGP_ICON_HANDLE) XLGP_LoadIconFromFileExt(const wchar_t* lpExt, int cxDesired, int cyDesired);

XLGRAPHICPLUS_API(long) XLGP_SaveIconToIcoFile(XLGP_ICON_HANDLE hIcon, const wchar_t* lpFileName);

XLGRAPHICPLUS_API(unsigned long) XLGP_AddRefIcon(XLGP_ICON_HANDLE hIcon);
XLGRAPHICPLUS_API(unsigned long) XLGP_ReleaseIcon(XLGP_ICON_HANDLE hIcon);

//��ȡ��ϵͳicon�����������
XLGRAPHICPLUS_API(HICON) XLGP_IconGetHandle(XLGP_ICON_HANDLE hIcon);
XLGRAPHICPLUS_API(BOOL) XLGP_IconGetSize(XLGP_ICON_HANDLE hIcon, int* lpWidth, int* lpHeight);

// �����ȡ��bitmap�Ǿ���Ԥ�˵ģ�����ֱ�����õ�xlue������Ϊbitmap��Դʹ��
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_IconGetBitmap(XLGP_ICON_HANDLE hIcon);

// ��ȡû��Ԥ�˵�bitmap����bitmap���ɽ���xlue����Ⱦ��ϵ
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_IconGetOriginBitmap(XLGP_ICON_HANDLE hIcon);

// cursor��ؽӿ�
XLGRAPHICPLUS_API(XLGP_CURSOR_HANDLE) XLGP_CreateCursorFromHandle(HCURSOR hCursor, bool aniCursor);

//���ļ�����������ͼ��
XLGRAPHICPLUS_API(XLGP_CURSOR_HANDLE) XLGP_LoadCursorFromStream(XLFS_STREAM_HANDLE hStream, int cxDesired, int cyDesired);
XLGRAPHICPLUS_API(XLGP_CURSOR_HANDLE) XLGP_LoadCursorFromStreamEx(XLFS_STREAM_HANDLE hStream, int cxDesired, int cyDesired, BOOL aniCursor);
XLGRAPHICPLUS_API(XLGP_CURSOR_HANDLE) XLGP_LoadCursorFromFile(const wchar_t* lpFileName, int cxDesired, int cyDesired);


XLGRAPHICPLUS_API(unsigned long) XLGP_AddRefCursor(XLGP_CURSOR_HANDLE hCursor);
XLGRAPHICPLUS_API(unsigned long) XLGP_ReleaseCursor(XLGP_CURSOR_HANDLE hCursor);

//��ȡ��ϵͳcursor�����ע�ⲻ������
XLGRAPHICPLUS_API(HCURSOR) XLGP_CursorGetHandle(XLGP_CURSOR_HANDLE hCursor);


//gif��ؽӿ�
XLGRAPHICPLUS_API(XLGP_GIF_HANDLE) XLGP_LoadGifFromFile(const wchar_t* lpFileName);
XLGRAPHICPLUS_API(XLGP_GIF_HANDLE) XLGP_LoadGifFromStream(XLFS_STREAM_HANDLE hStream);
XLGRAPHICPLUS_API(unsigned long) XLGP_AddRefGif(XLGP_GIF_HANDLE hGif);
XLGRAPHICPLUS_API(unsigned long) XLGP_ReleaseGif(XLGP_GIF_HANDLE hGif);

XLGRAPHICPLUS_API(BOOL) XLGP_GifGetSize(XLGP_GIF_HANDLE hGif, int* lpWidth, int* lpHeight);

XLGRAPHICPLUS_API(unsigned int) XLGP_GifGetFrameCount(XLGP_GIF_HANDLE hGif);

//��ȡ��uFrameIndex������һ֡��ʱ����
XLGRAPHICPLUS_API(unsigned long) XLGP_GifGetIntervalAfterFrame(XLGP_GIF_HANDLE hGif, unsigned int uFrameIndex);

//��ȡ��uFrameIndex֡��XLBitmap����0��ʼ��
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_GifGetFrame(XLGP_GIF_HANDLE hGif, unsigned int uFrameIndex);

XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_GifGetFirstFrame(XLGP_GIF_HANDLE hGif);
XLGRAPHICPLUS_API(XL_BITMAP_HANDLE) XLGP_GifGetNextFrame(XLGP_GIF_HANDLE hGif);

//�����ļ��ӿ�
//����xlbmpΪjpg�ļ���qualityȡֵ��0-100��0-ѹ������ߣ�������ͣ�100-�������
XLGRAPHICPLUS_API(BOOL) XLGP_SaveXLBitmapToJpegFile(XL_BITMAP_HANDLE hBmp, const wchar_t* lpFilePath, unsigned long quality);
XLGRAPHICPLUS_API(BOOL) XLGP_SaveXLBitmapToBmpFile(XL_BITMAP_HANDLE hBmp, const wchar_t* lpFilePath, unsigned long quality);
XLGRAPHICPLUS_API(BOOL) XLGP_SaveXLBitmapToPngFile(XL_BITMAP_HANDLE hBmp, const wchar_t* lpFilePath, unsigned long quality);


// lua��ؽӿڣ���ָ���汾��Ч

//���������麯����xlue.dll�����ĺ�����ȫһ��
//�ھ��Ϊnull����pushʧ�ܵ�����£���pushһ��nil��ջ��
XLGRAPHICPLUS_API(BOOL) XLGP_PushBitmap(lua_State* luaState, XL_BITMAP_HANDLE hBitmap);
XLGRAPHICPLUS_API(BOOL) XLGP_PushMask(lua_State* luaState, XL_MASK_HANDLE hMask);
XLGRAPHICPLUS_API(BOOL) XLGP_PushTexture(lua_State* luaState, XL_TEXTURE_HANDLE hTexture);
XLGRAPHICPLUS_API(BOOL) XLGP_PushFont(lua_State* luaState, XL_FONT_HANDLE hFont);
XLGRAPHICPLUS_API(BOOL) XLGP_PushColor(lua_State* luaState, XL_Color* lpColor);
XLGRAPHICPLUS_API(BOOL) XLGP_PushColor2(lua_State* luaState, XL_Color color);
XLGRAPHICPLUS_API(BOOL) XLGP_PushPen(lua_State* luaState, XL_PEN_HANDLE hPen);
XLGRAPHICPLUS_API(BOOL) XLGP_PushBrush(lua_State* luaState, XL_BRUSH_HANDLE hBrush);
XLGRAPHICPLUS_API(BOOL) XLGP_PushImageList(lua_State* luaState, XL_IMAGELIST_HANDLE hImageList);
XLGRAPHICPLUS_API(BOOL) XLGP_PushImageSeq(lua_State* luaState, XL_IMAGESEQ_HANDLE hImageSeq);
XLGRAPHICPLUS_API(BOOL) XLGP_PushCurve(lua_State* luaState, XL_CURVE_HANDLE hCurve);
XLGRAPHICPLUS_API(BOOL) XLGP_PushIcon(lua_State* luaState, XLGP_ICON_HANDLE hIcon);
XLGRAPHICPLUS_API(BOOL) XLGP_PushGif(lua_State* luaState, XLGP_GIF_HANDLE hGif);
XLGRAPHICPLUS_API(BOOL) XLGP_PushCursor(lua_State* luaState, XLGP_CURSOR_HANDLE hCursor);

// ���漸��check��������������Ϊ�գ���ô������֮����Ҫ�Ծ��������Ӧ��Release(color����)
XLGRAPHICPLUS_API(BOOL) XLGP_CheckBitmap(lua_State* luaState, int index, XL_BITMAP_HANDLE *lpBitmap);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckMask(lua_State* luaState, int index, XL_MASK_HANDLE *lpMask);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckTexture(lua_State* luaState, int index, XL_TEXTURE_HANDLE *lpTexture);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckFont(lua_State* luaState, int index, XL_FONT_HANDLE *lpFont);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckColor(lua_State* luaState, int index, XL_Color** lplpColor);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckColor2(lua_State* luaState, int index, XL_Color* lpColor);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckPen(lua_State* luaState, int index, XL_PEN_HANDLE* lpPen);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckBrush(lua_State* luaState, int index, XL_BRUSH_HANDLE* lpBrush);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckImageList(lua_State* luaState, int index, XL_IMAGELIST_HANDLE* lpImageList);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckImageSeq(lua_State* luaState, int index, XL_IMAGESEQ_HANDLE* lpImageSeq);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckCurve(lua_State* luaState, int index, XL_CURVE_HANDLE* lpCurve);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckIcon(lua_State* luaState, int index, XLGP_ICON_HANDLE* lpIcon);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckGif(lua_State* luaState, int index, XLGP_GIF_HANDLE* lpGif);
XLGRAPHICPLUS_API(BOOL) XLGP_CheckCursor(lua_State* luaState, int index, XLGP_CURSOR_HANDLE* lpCursor);

#endif //__XLGRAPHICPLUS_H__