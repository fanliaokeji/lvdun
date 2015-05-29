/********************************************************************
*
* =-----------------------------------------------------------------=
* =                                                                 =
* =             Copyright (c) Xunlei, Ltd. 2004-2009                =
* =                                                                 =
* =-----------------------------------------------------------------=
* 
*   FileName    :   XLUE.h
*   Author      :   xlue group(xlue@xunlei.com)
*   Create      :   2009-12-04
*   LastChange  :   
*   History     :	
*
*   Description :  XLUE.dll��ͷ�ļ�����������صĽӿ�
*
********************************************************************/ 
#ifndef _XUNLEI_XLUE_API_H_
#define _XUNLEI_XLUE_API_H_

#include "./XLUEHandle.h"
#include <XLGraphic.h>

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include <lua/lua.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#ifndef XLUE_EXTERN_C
	#if defined(__cplusplus)	
		#define XLUE_EXTERN_C extern "C"
	#else
		#define XLUE_EXTERN_C 
	#endif // __cplusplus
#endif //XLUE_EXTERN_C

#ifndef XLUE_STDCALL
	#if defined(_MSC_VER)
		#define XLUE_STDCALL __stdcall
	#elif defined(__GNUC__)
		#define XLUE_STDCALL __attribute__((__stdcall__))
	#endif
#endif //XLUE_STDCALL

#if defined(_MSC_VER)
	#if defined(XLUE_UNIONLIB)
			#define XLUE_API(x) XLUE_EXTERN_C  x __stdcall 
	#elif defined(XLUE_EXPORTS)
			#define XLUE_API(x) XLUE_EXTERN_C __declspec(dllexport) x __stdcall 
	#elif defined (XLUE_UNION)
			#define XLUE_API(x) XLUE_EXTERN_C  x __stdcall 
	#else // XLUE_EXPORTS
			#define XLUE_API(x) XLUE_EXTERN_C __declspec(dllimport) x __stdcall 
	#endif // XLUE_EXPORTS
#elif defined(__GNUC__)
	#if defined(XLUE_UNIONLIB)
			#define XLUE_API(x) XLUE_EXTERN_C  __attribute__((__stdcall__)) x
	#elif defined(XLUE_EXPORTS)
			#define XLUE_API(x) XLUE_EXTERN_C __attribute__((__visibility__("default"), __stdcall__)) x
	#elif defined (XLUE_UNION)
			#define XLUE_API(x) XLUE_EXTERN_C  __attribute__((__stdcall__)) x
	#else // XLUE_EXPORTS
			#define XLUE_API(x) XLUE_EXTERN_C __attribute__((__visibility__("default"), __stdcall__)) x 
	#endif // XLUE_EXPORTS
#endif

#if !defined(WIN32) && !defined(XLUE_WIN32)
#include <XLUESysPreDefine.h>
#endif // WIN32 && XLUE_WIN32

#include "./XLUEDefine.h"

//Kernel

XLUE_API(long) XLUE_SetDC(const void* lpBuffer, size_t size);

// У��XAR
#define XARVERIFY_RESULT_SUCCESS		0
#define XARVERIFY_RESULT_INVALIDXAR		1
#define XARVERIFY_RESULT_LOADXARFAILED	2
#define XARVERIFY_RESULT_INVALIDKEY		3
#define XARVERIFY_RESULT_XARNOSIGN		4
#define XARVERIFY_RESULT_DECRYPTFAILED	5
#define XARVERIFY_RESULT_INVALIDDATA	6
#define XARVERIFY_RESULT_INVALIDXARSIGN	7
#define XARVERIFY_RESULT_INVALIDSIGN	8

XLUE_API(long) XLUE_VerifyXAR(const wchar_t* lpXARFile);
XLUE_API(long) XLUE_VerifyXAR2(const wchar_t* lpXARFile, const char* lpKey);

XLUE_API(long) XLUE_Init(void*);
XLUE_API(long) XLUE_InitLoader(void*);
XLUE_API(long) XLUE_InitLuaHost(void*);

XLUE_API(long) XLUE_RegisterStandardObjects(void*);
XLUE_API(long) XLUE_RegisterAnimation(void*);
XLUE_API(long) XLUE_RegisterResource(void*);
XLUE_API(long) XLUE_RegisterHostWnd(void*);

XLUE_API(long) XLUE_Uninit(void*);
XLUE_API(long) XLUE_UninitLuaHost(void*);
XLUE_API(long) XLUE_UninitLoader(void*);
XLUE_API(long) XLUE_UninitHandleMap(void*);

XLUE_API(long) XLUE_ClearLuaObj(void*);

#if defined(XLUE_UNIONLIB)
// LIBģʽ������ִ��XLUEOPS�Ĺ��ܣ��������ָ����XLUEOPS����ô�÷�����ִ��ֱ���÷�����Ҫ�˳�����������̷���
// �������̿��Ը��ݷ���ֵ��������ǰ�������ͺ���Ҫ��ʲô���飺
// 1. ����0��˵���ý�������ͨ���̣���ô�����������Լ����̼���
// 2. ���ط�0��˵���ý�����XLUEOPS���̣���ôXLUE_OPSMain���غ�ֱ���˳�����
//     a) ����1��˵��ִ�гɹ���ִ�������
//	   b) ����-1��˵����ʼ������ʧ��
XLUE_API(int)  XLUE_OPSMain(const wchar_t *lpCmdLine);
#endif // XLUE_UNIONLIB

// ����textobjectʹ�õ�Ĭ���ı���Ⱦ����
XLUE_API(unsigned long) XLUE_SetTextObjectDefaultTextType(unsigned long textType);

/*------------------------��������ؽӿ�--------------------------------------*/

XLUE_API(XLUE_OBJTREE_HANDLE) XLUE_CreateObjTree(const char* id);
XLUE_API(long) XLUE_DestroyObjTree(XLUE_OBJTREE_HANDLE hObjTree);
XLUE_API(XLUE_OBJTREE_HANDLE) XLUE_GetObjTree(const char* id);

XLUE_API(BOOL) XLUE_IsObjTreeValid(XLUE_OBJTREE_HANDLE hObjTree);

XLUE_API(void) XLUE_SetTreeMaxClosure(XLUE_OBJTREE_HANDLE hObjTree,LPCRECT pClosure);
XLUE_API(const char*) XLUE_GetObjTreeID(XLUE_OBJTREE_HANDLE hObjTree);

XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetUIObject(XLUE_OBJTREE_HANDLE hObjTree,const char* id);
XLUE_API(size_t) XLUE_GetObjTreeObjectCount(XLUE_OBJTREE_HANDLE hObjTree);

XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetObjTreeRootObj(XLUE_OBJTREE_HANDLE hObjTree);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetObjTreeFocusObj(XLUE_OBJTREE_HANDLE hObjTree);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetObjTreeCaptureMouseObj(XLUE_OBJTREE_HANDLE hObjTree);

XLUE_API(long) XLUE_SetObjTreeRootObj(XLUE_OBJTREE_HANDLE hObjTree,XLUE_LAYOUTOBJ_HANDLE hRootObj);
XLUE_API(XLUE_HOSTWND_HANDLE) XLUE_GetObjTreeBindHostWnd(XLUE_OBJTREE_HANDLE hObjTree);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_HitTest(XLUE_OBJTREE_HANDLE hObjTree, long x, long y);

XLUE_API(void) XLUE_PushObjTreeDirtyRect(XLUE_OBJTREE_HANDLE hObjTree, const RECT* lpDirtyRect);

/*------------------ Ԫ������ؽӿ�------------------------*/

XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_CreateObj(const char* id, const char* className, const char* package);
XLUE_API(long) XLUE_DestoryObj(XLUE_LAYOUTOBJ_HANDLE hObj);

// �жϸ����ľ���Ƿ���һ����Ч��Ԫ����
XLUE_API(BOOL) XLUE_IsObjValid(XLUE_LAYOUTOBJ_HANDLE hObj);

XLUE_API(const char*) XLUE_GetObjID(XLUE_LAYOUTOBJ_HANDLE hObj);
//SetID�ڴ󲿷�ʱ���ǲ�Ӧ���õ�!
XLUE_API(BOOL) XLUE_SetObjID(XLUE_LAYOUTOBJ_HANDLE hObj,const char* id);
XLUE_API(const char*) XLUE_GetObjClassName(XLUE_LAYOUTOBJ_HANDLE hObj);

XLUE_API(XLUE_OBJTREE_HANDLE) XLUE_GetObjOwner(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetObjFather(XLUE_LAYOUTOBJ_HANDLE hObj);

//�����ѭ��Ӧ���Լ�ȥ�أ���ʹ���߱�֤
XLUE_API(long) XLUE_AddObjChild(XLUE_LAYOUTOBJ_HANDLE hObj,XLUE_LAYOUTOBJ_HANDLE hChild);
XLUE_API(long) XLUE_RemoveObjChild(XLUE_LAYOUTOBJ_HANDLE hObj,XLUE_LAYOUTOBJ_HANDLE hChild);
XLUE_API(long) XLUE_RemoveObjAllChildren(XLUE_LAYOUTOBJ_HANDLE hObj);

XLUE_API(size_t) XLUE_GetObjChildCount(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetObjChild(XLUE_LAYOUTOBJ_HANDLE hObj,size_t index);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetObjChildByID(XLUE_LAYOUTOBJ_HANDLE hObj, const char* id);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetObjChildByCmd(XLUE_LAYOUTOBJ_HANDLE hObj, const char* lpObjectCmd);

XLUE_API(BOOL) XLUE_IsChild(XLUE_LAYOUTOBJ_HANDLE hObj, XLUE_LAYOUTOBJ_HANDLE hChild);

//ֻ���Լ���control������
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetControlObject(XLUE_LAYOUTOBJ_HANDLE hObj, const char* id);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetControlObjectEx(XLUE_LAYOUTOBJ_HANDLE hObj,const char* id);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_GetOwnerControl(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(BOOL) XLUE_IsControl(XLUE_LAYOUTOBJ_HANDLE hObj);

// ���úͻ�ȡλ����غ���
XLUE_API(const RECT*) XLUE_GetObjPos(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(long) XLUE_SetObjPos(XLUE_LAYOUTOBJ_HANDLE hObj, const RECT* pNewPos);
XLUE_API(long) XLUE_SetObjPosExp(XLUE_LAYOUTOBJ_HANDLE hObj, const char* lpLeft, const char* lpTop, const char* lpWidth, const char* lpHeight);
XLUE_API(long) XLUE_SetObjPosExp2(XLUE_LAYOUTOBJ_HANDLE hObj, const char* lpLeft, const char* lpTop, const char* lpRight, const char* lpBottom);
XLUE_API(const RECT*) XLUE_GetObjAbsPos(XLUE_LAYOUTOBJ_HANDLE hObj);

// limit������غ���
XLUE_API(BOOL) XLUE_IsObjLimitChild(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjLimitChild(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL isLimit);

XLUE_API(BOOL) XLUE_IsObjLimit(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjLimit(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL isLimit);

XLUE_API(BOOL) XLUE_GetObjLimitRect(XLUE_LAYOUTOBJ_HANDLE hObj, RECT* lpLimitRect);
XLUE_API(BOOL) XLUE_GetObjCaretLimitRect(XLUE_LAYOUTOBJ_HANDLE hObj, RECT* lpLimitRect);

XLUE_API(BOOL) XLUE_ObjHitTest(XLUE_LAYOUTOBJ_HANDLE hObj, short x, short y);

// ���pRect = NULL���ʾ��������������Ϊ��Ч
// pDirty��Ԫ��������ϵ�������Ԫ��������Ͻ�
XLUE_API(void) XLUE_ObjPushDirtyRect(XLUE_LAYOUTOBJ_HANDLE hObj,const RECT* pDirty);

// ����Ԫ�����lpSrcClipRect����Ŀ��λͼ��lpDestClipRect����Ҫע�⼸�㣺
// lpDestClipRect��lpSrcClipRect�Ĵ�С����һ�£������߾�����Ϊ��
// lpDestClipRect�������hDestBitmap�����Ͻ�(0,0)λ�õģ����Ҳ��ɳ���hDestBitmap�Ĵ�С
// lpSrcClipRect�������hObj�����Ͻ�(0,0)λ�õģ����Ҵ�С���ɳ����������С
// alphaָ���˴˴λ���Ҫʹ�õ�alphaֵ(Ĭ��Ӧ��ʹ��Ԫ�������õ�alpha����)
XLUE_API(BOOL) XLUE_ObjPaint(XLUE_LAYOUTOBJ_HANDLE hObj, XL_BITMAP_HANDLE hDestBitmap, LPCRECT lpDestClipRect,
							 LPCRECT lpSrcClipRect, unsigned char alpha);

XLUE_API(void) XLUE_EnableObjInputTarget(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL isEnable);
XLUE_API(BOOL) XLUE_CanObjHandleInput(XLUE_LAYOUTOBJ_HANDLE hObj);

// ��ʽ����Ԫ���������ͼ��������¼�
XLUE_API(long) XLUE_ObjProcessInput(XLUE_LAYOUTOBJ_HANDLE hObj, unsigned long actionType, unsigned long wParam,unsigned long lParam, BOOL* lpHandled);

XLUE_API(void) XLUE_SetObjCaptureMouse(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL isCapture);
XLUE_API(BOOL) XLUE_GetObjCaptureMouse(XLUE_LAYOUTOBJ_HANDLE hObj);

// ���󽹵���غ���
XLUE_API(void) XLUE_SetObjFocus(XLUE_LAYOUTOBJ_HANDLE hObj, BOOL isFocus);
XLUE_API(void) XLUE_SetObjFocusEx(XLUE_LAYOUTOBJ_HANDLE hObj, BOOL isFocus, FocusReason reason);
XLUE_API(BOOL) XLUE_GetObjFocus(XLUE_LAYOUTOBJ_HANDLE hObj);

XLUE_API(void) XLUE_SetObjFocusStrategy(XLUE_LAYOUTOBJ_HANDLE hObj, FocusStrategy strategy);
XLUE_API(FocusStrategy) XLUE_GetObjFocusStrategy(XLUE_LAYOUTOBJ_HANDLE hObj);

XLUE_API(long) XLUE_QueryObjFocus(XLUE_LAYOUTOBJ_HANDLE hObj);

// ����zorder��غ���
XLUE_API(long) XLUE_GetObjZorder(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjZorder(XLUE_LAYOUTOBJ_HANDLE hObj,long zorder);
XLUE_API(long) XLUE_GetObjAbsZorder(XLUE_LAYOUTOBJ_HANDLE hObj);

// tab������غ���
XLUE_API(long) XLUE_GetObjTabOrder(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjTabOrder(XLUE_LAYOUTOBJ_HANDLE hObj,long newTabOrder);

XLUE_API(long) XLUE_GetObjTabGroup(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjTabGroup(XLUE_LAYOUTOBJ_HANDLE hObj,long newTabGroup);

XLUE_API(BOOL) XLUE_GetObjTabStop(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjTabStop(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL newTabStop);

// ·�ɵ�ǰ�����¼���������
XLUE_API(void) XLUE_ObjRouteToFather(XLUE_LAYOUTOBJ_HANDLE hObj);

// ����Ĭ�ϻ���ָ���¼����ض������
XLUE_API(BOOL) XLUE_ObjSetRediretorByEvent(XLUE_LAYOUTOBJ_HANDLE hObj, const char* eventName, const char* lpCommand);
XLUE_API(BOOL) XLUE_ObjSetDefaultRediretorByEvent(XLUE_LAYOUTOBJ_HANDLE hObj, const char* lpCommand);

// Ԫ�����cursor������ؽӿ�
XLUE_API(void) XLUE_SetObjCursorID(XLUE_LAYOUTOBJ_HANDLE hObj,const char* pid);
XLUE_API(const char*) XLUE_GetObjCursorID(XLUE_LAYOUTOBJ_HANDLE hObj, long x, long y);

// Ԫ�����visible������ؽӿ�
XLUE_API(BOOL) XLUE_GetObjVisible(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjVisible(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL isVisible,BOOL isRecursive);
XLUE_API(BOOL) XLUE_GetObjChildrenVisible(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjChildrenVisible(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL isVisible);

XLUE_API(BOOL) XLUE_GetObjFatherVisible(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(BOOL) XLUE_GetObjPrivateVisible(XLUE_LAYOUTOBJ_HANDLE hObj);

// Ԫ�����enable������ؽӿ�
XLUE_API(BOOL) XLUE_GetObjEnable(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjEnable(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL bEnable);
XLUE_API(void) XLUE_SetObjEnableEx(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL bEnable, BOOL isRecursive);
XLUE_API(BOOL) XLUE_GetObjChildrenEnable(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjChildrenEnable(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL bEnable);

XLUE_API(BOOL) XLUE_GetObjFatherEnable(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(BOOL) XLUE_GetObjPrivateEnable(XLUE_LAYOUTOBJ_HANDLE hObj);

// ��ȡ������Ԫ�����alpha
XLUE_API(unsigned char) XLUE_GetRenderObjAlpha(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetRenderObjAlpha(XLUE_LAYOUTOBJ_HANDLE hObj,unsigned char alpha);
XLUE_API(void) XLUE_SetRenderObjAlphaEx(XLUE_LAYOUTOBJ_HANDLE hObj,unsigned char alpha, BOOL isRecursive);

// Ԫ����mask��ؽӿ�
XLUE_API(BOOL) XLUE_GetObjInheritMask(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetObjInheritMask(XLUE_LAYOUTOBJ_HANDLE hObj,BOOL isVisible,BOOL isRecursive);

XLUE_API(const RECT*) XLUE_GetObjMaskPos(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(long) XLUE_SetObjMaskPos(XLUE_LAYOUTOBJ_HANDLE hObj,RECT* pNewPos);
XLUE_API(long) XLUE_SetObjMaskPosExp(XLUE_LAYOUTOBJ_HANDLE hObj, const char* lpLeft, const char* lpTop, const char* lpWidth, const char* lpHeight);
XLUE_API(long) XLUE_SetObjMaskPosExp2(XLUE_LAYOUTOBJ_HANDLE hObj, const char* lpLeft, const char* lpTop, const char* lpRight, const char* lpBottom);

XLUE_API(void) XLUE_SetObjMaskBlendType(XLUE_LAYOUTOBJ_HANDLE hObj, unsigned long blendtype);
XLUE_API(unsigned long) XLUE_GetObjMaskBlendType(XLUE_LAYOUTOBJ_HANDLE hObj);

// Ԫ�����ResProvider��ؽӿ�
XLUE_API(BOOL) XLUE_SetObjResProvider(XLUE_LAYOUTOBJ_HANDLE hObj, XLUE_RESPROVIDER_HANDLE hResProvider);
XLUE_API(XLUE_RESPROVIDER_HANDLE) XLUE_GetObjResProvider(XLUE_LAYOUTOBJ_HANDLE hObj);

// Ԫ�����Ϸ���ؽӿ�
XLUE_API(void) XLUE_SetObjDropEnable(XLUE_LAYOUTOBJ_HANDLE hObj, BOOL bEnableDrop);
XLUE_API(BOOL) XLUE_GetObjDropEnable(XLUE_LAYOUTOBJ_HANDLE hObj);

// ����Ⱦ��ؽӿ�

// ��ʾ�󶨵�һ�������֧�ֵݹ�
// ����ö���֮ǰ�Ѿ����˷�autoģʽ�Ĳ������ô�Ḳ��
XLUE_API(BOOL) XLUE_ObjBindLayer(XLUE_LAYOUTOBJ_HANDLE hObj, XLUE_LAYOUTOBJ_HANDLE hLayerObj, BOOL isRecursive);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_ObjGetLayer(XLUE_LAYOUTOBJ_HANDLE hObj);

// ImageObject����ؽӿ�
XLUE_API(BOOL) XLUE_SetImageObjResID(XLUE_LAYOUTOBJ_HANDLE hObj, const char* id);
XLUE_API(const char*) XLUE_GetImageObjResID(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(BOOL) XLUE_SetImageObjBitmap(XLUE_LAYOUTOBJ_HANDLE hObj, XL_BITMAP_HANDLE hBitmap);
XLUE_API(XL_BITMAP_HANDLE) XLUE_GetImageObjBitmap(XLUE_LAYOUTOBJ_HANDLE hObj);

// RealObject����ؽӿ�
XLUE_API(OS_HOSTWND_HANDLE) XLUE_SetRealObjWindow(XLUE_LAYOUTOBJ_HANDLE hObj, OS_HOSTWND_HANDLE hWnd);
XLUE_API(OS_HOSTWND_HANDLE) XLUE_GetRealObjWindow(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(OS_HOSTWND_HANDLE) XLUE_GetRealObjHostWindow(XLUE_LAYOUTOBJ_HANDLE hObj);

// LayerObject����ؽӿ�
XLUE_API(BOOL) XLUE_GetLayerObjClipSens(XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(void) XLUE_SetLayerObjClipSens(XLUE_LAYOUTOBJ_HANDLE hObj, BOOL clipSens);

// �Ѷ���push��luaջ�������ʧ�ܵĻ�������ջ������һ��nil
XLUE_API(BOOL) XLUE_PushObject(lua_State* luaState, XLUE_LAYOUTOBJ_HANDLE hObj);

// �ӵ�ǰluaջ��indexλ��ȡ��Ԫ���󣬲���lua���ͼ��
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_CheckObject(lua_State* luaState, int index);

// �ӵ�ǰluaջ��indexλ��ȡ�����Ϊclass��Ԫ���󣬲���lua���ͼ��
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_CheckObjectEx(lua_State* luaState, int index, const char* className);

// Ԥע����չ����
XLUE_API(BOOL) XLUE_PreRegisterExtType(const ExtPreRegisterInfo* lpPreRegisterInfo);

/*------------------------��չԪ�������ض���-------------------------------*/

// ע���ⲿ��չԪ����
XLUE_API(BOOL) XLUE_RegisterExtObj(const ExtObjRegisterInfo* lpRegisterInfo);

// ����paramCount���ǲ�������n��retCount�Ƿ���ֵ����m(�¼�����������ķ���ֵ������m+3!)
// lua�����ķ���ֵ����Ϊresult, ret1, ret2,...,retm, handled, callNext������ret1-retm�Ǹ��¼������m������ֵ
// resultΪXLUE_FireExtObjEvent�ķ���ֵ��lpHandled��handled����ֵ
// fire�¼�֮ǰ����Ҫ��n������push��luaջ����������ɺ�ջ����m������ֵ��lpHandled��־���¼��Ƿ񱻴������¼�����������Ĭ�Ϸ���ֵ��ȷ��
XLUE_API(long) XLUE_FireExtObjEvent(XLUE_LAYOUTOBJ_HANDLE hObj, const char* eventName, lua_State* luaState, int paramCount, int retCount, BOOL* lpHandled);

// ���hObj��һ����չ������ô���Ի�ȡ���Ӧ���Զ���handle
XLUE_API(void*) XLUE_GetExtHandle(XLUE_LAYOUTOBJ_HANDLE hObj);


/*--------------------��Ⱦ������ط���--------------------------*/
XLUE_API(XL_BITMAP_HANDLE) XLUE_RenderTree(XLUE_OBJTREE_HANDLE hTree,LPCRECT pViewRect);
XLUE_API(BOOL) XLUE_RenderObject(XLUE_LAYOUTOBJ_HANDLE hObj,XL_BITMAP_HANDLE hDstBmp,BOOL isRenderChild,LPCRECT pViewRect);


/*---------------------��ʱ����ط���---------------------------*/
typedef void (XLUE_STDCALL *LPFNONTIMER)(void* userData, unsigned int timerID);

XLUE_API(unsigned int) XLUE_SetTimer(LPFNONTIMER lpTimerProc, unsigned int elapse, void* userData);
XLUE_API(unsigned int) XLUE_SetOnceTimer(LPFNONTIMER lpTimerProc, unsigned int elapse, void* userData);
XLUE_API(unsigned int) XLUE_SetIDTimer(LPFNONTIMER lpTimerProc, unsigned int timerID, unsigned int elapse, void* userData);
XLUE_API(unsigned int) XLUE_SetOnceIDTimer(LPFNONTIMER lpTimerProc, unsigned int timerID, unsigned int elapse, void* userData);

XLUE_API(BOOL) XLUE_QueryTimer(unsigned int timerID);

XLUE_API(BOOL) XLUE_KillTimer(unsigned int timerID, void** lplpUserData);

/*--------------------������ط���--------------------------*/
XLUE_API(BOOL) XLUE_IsAnimationValid(XLUE_ANIMATION_HANDLE hAnimation);

XLUE_API(XLUE_ANIMATION_HANDLE) XLUE_CreateAnimation(const char* className,const char* lpID, const char* package);
XLUE_API(long) XLUE_AddRefAnimation(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(long) XLUE_ReleaseAnimation(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(const char*) XLUE_GetAnimationClassName(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(long) XLUE_GetAnimationState(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(long) XLUE_ResumeAnimation(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(long) XLUE_StopAnimation(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(long) XLUE_UpdateAnimationRunningTime(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(long) XLUE_GetAnimationRunningTime(XLUE_ANIMATION_HANDLE hAnimation);

XLUE_API(long) XLUE_SetKeyAnimationForceStop(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(long) XLUE_SetKeyAnimationTotalTime(XLUE_ANIMATION_HANDLE hAnimation,unsigned long totalTime);
XLUE_API(unsigned long) XLUE_GetKeyAnimationTotalTime(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(long) XLUE_SetKeyAnimationLoop(XLUE_ANIMATION_HANDLE hAnimation, BOOL bloop);
XLUE_API(long) XLUE_PosChangAniBindLayoutObj(XLUE_ANIMATION_HANDLE hAnimation,XLUE_LAYOUTOBJ_HANDLE hObj, BOOL isRender/* = FALSE*/);
XLUE_API(long) XLUE_PosChangeAniSetKeyPos(XLUE_ANIMATION_HANDLE hAnimation,long startLeft,long startTop,long endLeft,long endTop);
XLUE_API(long) XLUE_PosChangeAniSetKeyRect(XLUE_ANIMATION_HANDLE hAnimation,const RECT* pStart,const RECT* pEnd);

XLUE_API(long) XLUE_SetKeyAnimationBindObj(XLUE_ANIMATION_HANDLE hAnimation,XLUE_LAYOUTOBJ_HANDLE hObj, BOOL isRender/* = FALSE*/);

// ���߼������������
XLUE_API(long) XLUE_AniBindCurveID(XLUE_ANIMATION_HANDLE hAnimation, const char* id);
XLUE_API(long) XLUE_AniBindCurve(XLUE_ANIMATION_HANDLE hAnimation, XL_CURVE_HANDLE hCurve);
XLUE_API(const char*) XLUE_AniGetCurveID(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(XL_CURVE_HANDLE) XLUE_AniGetCurve(XLUE_ANIMATION_HANDLE hAnimation);

XLUE_API(long) XLUE_AlphaChangAniBindRenderObj(XLUE_ANIMATION_HANDLE hAnimation,XLUE_LAYOUTOBJ_HANDLE hObj, BOOL isRender/* = FALSE*/);
XLUE_API(long) XLUE_AlphaChagneAniSetKeyAlpha(XLUE_ANIMATION_HANDLE hAnimatio,unsigned char startAlpha,unsigned char endAlpha);

XLUE_API(long) XLUE_MaskChangeAniBindMaskObj(XLUE_ANIMATION_HANDLE hAnimation, XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(XLUE_LAYOUTOBJ_HANDLE) XLUE_MaskChangeAniGetBindMaskObj(XLUE_ANIMATION_HANDLE hAnimation);
XLUE_API(long) XLUE_MaskChangeAniSetMaskKeyFrame(XLUE_ANIMATION_HANDLE hAnimation, 
                                                 POINT beginOrg, SIZE beginSize,POINT endOrg, SIZE endSize);

XLUE_API(long) XLUE_B3DAniSetCentrePoint(XLUE_ANIMATION_HANDLE hAnimation,POINT centre);
XLUE_API(long) XLUE_B3DAniSetZPlane(XLUE_ANIMATION_HANDLE hAnimation,int ZPlaneCoordinate);
XLUE_API(long) XLUE_B3DAniSetCentrePointMode(XLUE_ANIMATION_HANDLE hAnimation,AngleChangeAniCentreMode mode);
XLUE_API(long) XLUE_B3DAniSetDistanceDisp2Obs(XLUE_ANIMATION_HANDLE hAnimation,int distDisp2obs);
XLUE_API(long) XLUE_B3DAniSetPositionMode(XLUE_ANIMATION_HANDLE hAnimation,AngleChangeAniPositionMode mode);
XLUE_API(long) XLUE_B3DAniSetSizeLimitMode(XLUE_ANIMATION_HANDLE hAnimation,AngleChangeAniSizeLimitMode mode);
XLUE_API(long) XLUE_B3DAniSetBlendMode(XLUE_ANIMATION_HANDLE hAnimation,DWORD mode);

XLUE_API(long) XLUE_AngleChangeAniBindRenderObj(XLUE_ANIMATION_HANDLE hAnimation,XLUE_LAYOUTOBJ_HANDLE hObj);
XLUE_API(long) XLUE_AngleChangeAniSetKeyAngle(XLUE_ANIMATION_HANDLE hAnimation,double startAngle[3],double endAngle[3]);
XLUE_API(long) XLUE_AngleChangeAniSetKeyRange(XLUE_ANIMATION_HANDLE hAnimation,double startRange[3],double endRange[3]);

XLUE_API(long) XLUE_TurnObjectAniBindRenderObj(XLUE_ANIMATION_HANDLE hAnimation,XLUE_LAYOUTOBJ_HANDLE hFrontObj,XLUE_LAYOUTOBJ_HANDLE hBackObj);
XLUE_API(long) XLUE_TurnObjectAniBindFrontObj(XLUE_ANIMATION_HANDLE hAnimation,XLUE_LAYOUTOBJ_HANDLE hFrontObj,LPCRECT pFrontView);
XLUE_API(long) XLUE_TurnObjectAniBindBackObj(XLUE_ANIMATION_HANDLE hAnimation,XLUE_LAYOUTOBJ_HANDLE hBackObj);
XLUE_API(long) XLUE_TurnObjectAniSetFlag(XLUE_ANIMATION_HANDLE hAnimation,TurnObjectFlag flag);


/*--------------------hostwnd��ط���---------------------------------------------*/

XLUE_API(BOOL) XLUE_IsHostWndValid(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(const char*) XLUE_GetHostWndID(XLUE_HOSTWND_HANDLE hHostWnd);
XLUE_API(const char*) XLUE_GetHostWndClassName(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_BindUIObjectTree(XLUE_HOSTWND_HANDLE hHostWnd, XLUE_OBJTREE_HANDLE hObjTree);
XLUE_API(XLUE_OBJTREE_HANDLE) XLUE_UnbindUIObjectTree(XLUE_HOSTWND_HANDLE hHostWnd);
XLUE_API(XLUE_OBJTREE_HANDLE) XLUE_GetBindUIObjectTree(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_SetUpdateFPS(XLUE_HOSTWND_HANDLE hHostWnd, unsigned long ulFPS);
XLUE_API(unsigned long) XLUE_GetUpdateFPS(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_SetHostWndVisible(XLUE_HOSTWND_HANDLE hHostWnd, BOOL bVisible);
XLUE_API(BOOL) XLUE_GetHostWndVisible(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_SetHostWndLayered(XLUE_HOSTWND_HANDLE hHostWnd, BOOL bLayered);
XLUE_API(BOOL) XLUE_GetHostWndLayered(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_MoveHostWnd(XLUE_HOSTWND_HANDLE hHostWnd, LPCRECT lpRect);
XLUE_API(BOOL) XLUE_GetHostWndRect(XLUE_HOSTWND_HANDLE hHostWnd, LPRECT lpRect);

XLUE_API(BOOL) XLUE_SetHostWndCacheRect(XLUE_HOSTWND_HANDLE hHostWnd, LPCRECT lpRect);
XLUE_API(BOOL) XLUE_GetHostWndCacheRect(XLUE_HOSTWND_HANDLE hHostWnd, LPRECT lpRect);

XLUE_API(BOOL) XLUE_SetHostWndParent(XLUE_HOSTWND_HANDLE hHostWnd, OS_HOSTWND_HANDLE hParentHostWnd);
XLUE_API(OS_HOSTWND_HANDLE) XLUE_GetHostWndParent(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(OS_HOSTWND_HANDLE) XLUE_GetHostWndWindowHandle(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(const char*) XLUE_GetHostWndCursorID(XLUE_HOSTWND_HANDLE hHostWnd);
XLUE_API(void) XLUE_SetHostWndCursorID(XLUE_HOSTWND_HANDLE hHostWnd,const char* pid);

XLUE_API(BOOL) XLUE_SetHostWndTitle(XLUE_HOSTWND_HANDLE hHostWnd, const char* lpTitle);
XLUE_API(const char*) XLUE_GetHostWndTitle(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_SetHostWndEnable(XLUE_HOSTWND_HANDLE hHostWnd, BOOL bEndable);
XLUE_API(BOOL) XLUE_GetHostWndEnable(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_SetHostWndTopMost(XLUE_HOSTWND_HANDLE hHostWnd, BOOL bTopMost);
XLUE_API(BOOL) XLUE_GetHostWndTopMost(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_HostWndPtToScreenPt(XLUE_HOSTWND_HANDLE hHostWnd, POINT* lpPT);
XLUE_API(BOOL) XLUE_ScreenPtToHostWndPt(XLUE_HOSTWND_HANDLE hHostWnd, POINT* lpPT);
XLUE_API(BOOL) XLUE_HostWndRectToScreenRect(XLUE_HOSTWND_HANDLE hHostWnd, RECT* lpRect);
XLUE_API(BOOL) XLUE_ScreenRectToHostWndRect(XLUE_HOSTWND_HANDLE hHostWnd, RECT* lpRect);

XLUE_API(BOOL) XLUE_TreePtToHostWndPt(XLUE_HOSTWND_HANDLE hHostWnd, POINT* lpPT);
XLUE_API(BOOL) XLUE_HostWndPtToTreePt(XLUE_HOSTWND_HANDLE hHostWnd, POINT* lpPT);
XLUE_API(BOOL) XLUE_TreeRectToHostWndRect(XLUE_HOSTWND_HANDLE hHostWnd, RECT* lpRect);
XLUE_API(BOOL) XLUE_HostWndRectToTreeRect(XLUE_HOSTWND_HANDLE hHostWnd, RECT* lpRect);

XLUE_API(XL_BITMAP_HANDLE) XLUE_GetWindowBitmap(XLUE_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_ShowHostWnd(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd, CMDSHOW cmdShow);

XLUE_API(BOOL) XLUE_SetHostWndAppWindow(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd, BOOL bAppWindow);
XLUE_API(BOOL) XLUE_GetHostWndAppWindow(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_SetHostWndToolWindow(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd, BOOL bToolWindow);
XLUE_API(BOOL) XLUE_GetHostWndToolWindow(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_MaxHostWnd(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd);
XLUE_API(BOOL) XLUE_MinHostWnd(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd);
XLUE_API(BOOL) XLUE_RestoreHostWnd(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd);
XLUE_API(BOOL) XLUE_CenterHostWnd(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd, XLUE_HOSTWND_HANDLE hWndCenter);
XLUE_API(BOOL) XLUE_SetHostWndMinTrackSize(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd, const SIZE* lpSize);
XLUE_API(BOOL) XLUE_SetHostWndMaxTrackSize(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd, const SIZE* lpSize);
XLUE_API(BOOL) XLUE_GetHostWndMinTrackSize(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd, LPSIZE lpSize);
XLUE_API(BOOL) XLUE_GetHostWndMaxTrackSize(XLUE_NORMAL_HOSTWND_HANDLE hHostWnd, LPSIZE lpSize);

XLUE_API(int) XLUE_HostWndDoModal(XLUE_MODAL_HOSTWND_HANDLE hHostWnd, OS_HOSTWND_HANDLE hParent);
XLUE_API(BOOL) XLUE_HostWndEndDialog(XLUE_MODAL_HOSTWND_HANDLE hHostWnd, int nRetCode);

XLUE_API(BOOL) XLUE_HostWndCreateWnd(XLUE_FRAME_HOSTWND_HANDLE hHostWnd, OS_HOSTWND_HANDLE hParentWnd);
XLUE_API(BOOL) XLUE_HostWndDestroyWnd(XLUE_FRAME_HOSTWND_HANDLE hHostWnd);

XLUE_API(BOOL) XLUE_DeleteHostWnd(XLUE_HOSTWND_HANDLE hHostWnd);
XLUE_API(XLUE_HOSTWND_HANDLE) XLUE_CreateHostWnd(const char* lpID, const char* lpType);

XLUE_API(BOOL) XLUE_BeginEnumHostWnd();
XLUE_API(XLUE_HOSTWND_HANDLE) XLUE_GetNextHostWnd();

XLUE_API(XLUE_HOSTWND_HANDLE) XLUE_GetHostWndByHandle(OS_HOSTWND_HANDLE hWnd);
XLUE_API(XLUE_HOSTWND_HANDLE) XLUE_GetHostWndByID(const char* id);

/*------------------------XAR����Դ����ط���----------------------------------*/

typedef void (XLUE_STDCALL* ASYNLOADXARCALLBACKPROC)(char const*, BOOL);

#define XLUE_LOADSTRATEGY_DOM	0
#define XLUE_LOADSTRATEGY_SAX	1

XLUE_API(long) XLUE_SetLoadStrategy(long strategy);

XLUE_API(long) XLUE_LoadXAR(const char* xarName);
XLUE_API(long) XLUE_AsynLoadXAR(const char* xarName, ASYNLOADXARCALLBACKPROC lpCallBack, int flag);

XLUE_API(long) XLUE_IsXARLoaded(const char* xarName);
XLUE_API(long) XLUE_IsXARInAsynLoading(const char* xarName);

XLUE_API(BOOL) XLUE_XARExist(const char* xarName);
XLUE_API(long) XLUE_UnloadAllXAR();
XLUE_API(long) XLUE_AddXARSearchPath(const wchar_t* xarSearhPath);
XLUE_API(long) XLUE_RemoveXARSearchPath(const wchar_t* xarSearhPath);
XLUE_API(long) XLUE_ClearAllXARSearchPath();
XLUE_API(long) XLUE_LoadXLUEApp(const wchar_t* xlueAppXMLPath);

XLUE_API(long) XLUE_SelectResPackage(const char *resPackageName);

// �ӵ�ǰ���м��ص�xar����(����xar���Ⱥ���ش���)����ָ��id����Դ������0��ʾ�ɹ�����0��ʾʧ��
// ���صľ�������Ϊ�գ�ʹ����Ϻ���Ҫ������Ӧ��Release�����ͷ�
XLUE_API(long) XLUE_GetBitmap(const char *id, XL_BITMAP_HANDLE *phBitmap);
XLUE_API(long) XLUE_GetFont(const char *id, XL_FONT_HANDLE *phFont);
XLUE_API(long) XLUE_GetTexture(const char *id, XL_TEXTURE_HANDLE *phTexture);
XLUE_API(long) XLUE_GetImageList(const char *id, XL_IMAGELIST_HANDLE *phImageList);
XLUE_API(long) XLUE_GetColor(const char *id, XL_Color *pColor);
XLUE_API(long) XLUE_GetCurve(const char* id, XL_CURVE_HANDLE* phCurve);
XLUE_API(long) XLUE_GetImageSeq(const char* id, XL_IMAGESEQ_HANDLE* lpImageSeq);
XLUE_API(long) XLUE_GetPen(const char *id, XL_PEN_HANDLE *phPen);
XLUE_API(long) XLUE_GetBrush(const char *id, XL_BRUSH_HANDLE *phBrush);
XLUE_API(long) XLUE_GetRes(const char* resType, const char *id, XLUE_RESOURCE_HANDLE *lphResHandle);

/*------------------------------ResProvider����غ���--------------------------------*/

XLUE_API(BOOL) XLUE_IsResProviderValid(XLUE_RESPROVIDER_HANDLE hResProvider);

XLUE_API(XLUE_RESPROVIDER_HANDLE) XLUE_GetGlobalResProvider();
XLUE_API(XLUE_RESPROVIDER_HANDLE) XLUE_GetResProviderFromXAR(const char* xarName);

XLUE_API(long) XLUE_GetBitmapFromProvider(XLUE_RESPROVIDER_HANDLE hResProvider, const char *id, XL_BITMAP_HANDLE *lphBitmap, XLUE_RESPROVIDER_HANDLE *lphFromResProvider);
XLUE_API(long) XLUE_GetFontFromProvider(XLUE_RESPROVIDER_HANDLE hResProvider, const char *id, XL_FONT_HANDLE *lphFont, XLUE_RESPROVIDER_HANDLE *lphFromResProvider);
XLUE_API(long) XLUE_GetTextureFromProvider(XLUE_RESPROVIDER_HANDLE hResProvider, const char *id, XL_TEXTURE_HANDLE *lphTexture, XLUE_RESPROVIDER_HANDLE *lphFromResProvider);
XLUE_API(long) XLUE_GetImageListFromProvider(XLUE_RESPROVIDER_HANDLE hResProvider, const char *id, XL_IMAGELIST_HANDLE *lphImageList, XLUE_RESPROVIDER_HANDLE *lphFromResProvider);
XLUE_API(long) XLUE_GetColorFromProvider(XLUE_RESPROVIDER_HANDLE hResProvider, const char *id, XL_Color *lpColor, XLUE_RESPROVIDER_HANDLE *lphFromResProvider);
XLUE_API(long) XLUE_GetCurveFromProvider(XLUE_RESPROVIDER_HANDLE hResProvider, const char* id, XL_CURVE_HANDLE* lphCurve, XLUE_RESPROVIDER_HANDLE *lphFromResProvider);
XLUE_API(long) XLUE_GetImageSeqFromProvider(XLUE_RESPROVIDER_HANDLE hResProvider, const char* id, XL_IMAGESEQ_HANDLE* llpImageSeq, XLUE_RESPROVIDER_HANDLE *lphFromResProvider);
XLUE_API(long) XLUE_GetPenFromProvider(XLUE_RESPROVIDER_HANDLE hResProvider, const char *id, XL_PEN_HANDLE *lphPen, XLUE_RESPROVIDER_HANDLE *lphFromResProvider);
XLUE_API(long) XLUE_GetBrushFromProvider(XLUE_RESPROVIDER_HANDLE hResProvider, const char *id, XL_BRUSH_HANDLE *lphBrush, XLUE_RESPROVIDER_HANDLE *lphFromResProvider);
XLUE_API(long) XLUE_GetResFromProvider(XLUE_RESPROVIDER_HANDLE hResProvider, const char* resType, const char* id, XLUE_RESOURCE_HANDLE* lphResHandle, XLUE_RESPROVIDER_HANDLE *lphFromResProvider);

// ResProvider����Դ�¼�����ָ��id��Դ���غ͸���ʱ�򴥷�
typedef BOOL (XLUE_STDCALL* LPFNRESPROVIDERONRESEVENT)(void* userData, const char* id, ResEventFlag flag);

XLUE_API(unsigned long) XLUE_ResProviderAttachResEvent(XLUE_RESPROVIDER_HANDLE hResProvider, const char* resId, const char* resType, LPFNRESPROVIDERONRESEVENT handler, void* userData);
XLUE_API(BOOL) XLUE_ResProviderDetachResEvent(XLUE_RESPROVIDER_HANDLE hResProvider, const char* resId, const char* resType, unsigned long eventCookie);

/*-----------------------------��չ��Դ����غ���------------------------------------*/

// ע����չ��Դ����
XLUE_API(BOOL) XLUE_RegisterExtRes(const ExtResourceRegisterInfo* lpRegisterInfo);

// ���º�����ֻ�����չ��Դ���ͣ�������Դ���Ͳ���ʹ��XLUE_RESOURCE_HANDLE��������Ҳ����ͨ����������������

XLUE_API(long) XLUE_AddRefResource(XLUE_RESOURCE_HANDLE hResHandle);
XLUE_API(long) XLUE_ReleaseResource(XLUE_RESOURCE_HANDLE hResHandle);

XLUE_API(const char*) XLUE_GetResType(XLUE_RESOURCE_HANDLE hResHandle);
XLUE_API(const char*) XLUE_GetResID(XLUE_RESOURCE_HANDLE hResHandle);

// ��ȡ��չ��Դ���͵��ⲿ�����Ҳ����ExtResourceCreator���ص��Զ�����
XLUE_API(void*) XLUE_GetResExtHandle(XLUE_RESOURCE_HANDLE hResHandle);

// ��ȡ��չ��Դ���͵�������Դ����������������XGP����չicon������˵��void*�ȼ���XLGP_ICON_HANDLE
// ����bitmap������˵��void*�ȼ���XL_BITMAP_HANDLE
// ��Դ����������ü��������������ֵ��Ϊnil����ô��Ҫ����XLUE_ReleaseResRealHandle�ͷţ�
XLUE_API(void*) XLUE_GetResRealHandle(XLUE_RESOURCE_HANDLE hResHandle);
XLUE_API(long) XLUE_AddRefResRealHandle(XLUE_RESOURCE_HANDLE hResHandle, void* lpResRealHandle);
XLUE_API(long) XLUE_ReleaseResRealHandle(XLUE_RESOURCE_HANDLE hResHandle, void* lpResRealHandle);

/*------------------------------ȫ�ָ���API--------------------------------------------*/

// XLUE_GC flags
#define XLUE_GC_RES	0x01

/* eg: unsigned long flag = 0; XLUE_GC(&flag);*/
XLUE_API(long) XLUE_GC(void* lpvReserved);
XLUE_API(long) XLUE_Stat(long lType);
XLUE_API(long) XLUE_GetLuaStack(lua_State* luaState,char* lpStackBuffer, int bufferSize);

// hook
// hook type define
#define XLUE_HOOK_TEXTCHANGE 0x01

typedef struct tagXLUE_HOOK_DEF
{
	void* userData;
	XL_HANDLE obj;
	const char* lpObjType;
	const char* lpEventName;

}XLUE_HOOK_DESC;

typedef long (XLUE_STDCALL* LPFNTEXTCHANGEHOOK)(const XLUE_HOOK_DESC* lpHookDesc,	// ���ڸô�hook�ĸ�����Ϣ
											 wchar_t* lpstrTextBuffer,			// �ı�buffer���ⲿ���Զ�̬�޸�
											 long textLen,						// �ı��ĳ��ȣ�Ҳ���ַ�����
											 long* lpTextBufferLen,				// �ı��������Ĵ�С��Ҳ���ֽ���
											 BOOL* lpCallNext);					// �Ƿ������һ��hook��Ĭ��Ϊtrue

XLUE_API(long) XLUE_SetGlobalHook(unsigned long type, void* lpHookProc, void* userData, unsigned long flags, BOOL hookBack);
XLUE_API(BOOL) XLUE_RemoveGlobalHook(long cookie, void** lplpUserData);

/*--------------------��Ϣѭ����غ���-------------------------------------*/
// ִ��һ����Ϣѭ������Ҫ���ƺõ������ȣ��ڸ������ڣ�ÿ����Ϣѭ��listener���ᱻ����
XLUE_API(long) XLUE_DoMessageLoopWork(void* reserved);

typedef long (XLUE_STDCALL *LPFNDOMESSAGEWORKPROC)(void* userData);
XLUE_API(long) XLUE_RegisterMessageLoopListener(LPFNDOMESSAGEWORKPROC lpfnWorkProc, void* userData);
XLUE_API(BOOL) XLUE_UnregisterMessageLoopListener(long cookie, void** lpUserData);

/*---------------------����xml�򵥷�����غ���-----------------------------*/

// ��Ҫע�⣬XLUE_XML_HANDLE�ⲿ���ɳ��У�ֻ��������Ļص���������ʹ��

// ��ȡԪ�ص����ƺ�ֵ
XLUE_API(const char*) XLUE_XML_GetName(XLUE_XML_HANDLE hXML);
XLUE_API(const char*) XLUE_XML_GetValue(XLUE_XML_HANDLE hXML);

// ö��Ԫ�ص������б�
XLUE_API(BOOL) XLUE_XML_BeginGetAttribute(XLUE_XML_HANDLE hXML);
XLUE_API(BOOL) XLUE_XML_GetNextAttribute(XLUE_XML_HANDLE hXML, const char** lplpName, const char** lplpValue);

// ö����Ԫ�أ����ص���Ԫ�ؾ��������Ҫ����XLUE_XML_Free�ͷ�
XLUE_API(size_t) XLUE_XML_GetChildCount(XLUE_XML_HANDLE hXML);
XLUE_API(XLUE_XML_HANDLE) XLUE_XML_GetChildByIndex(XLUE_XML_HANDLE hXML, size_t index);

XLUE_API(BOOL) XLUE_XML_Free(XLUE_XML_HANDLE hXML);


/*--------------------������Դ���͵�lua������������----------------------------------------------*/

// �ھ��Ϊnull����pushʧ�ܵ�����£���pushһ��nil��ջ��
// ���push�ɹ�����Ӧ�ľ�����Զ��������ü���
XLUE_API(BOOL) XLUE_PushBitmap(lua_State* luaState, XL_BITMAP_HANDLE hBitmap);
XLUE_API(BOOL) XLUE_PushMask(lua_State* luaState, XL_MASK_HANDLE hMask);
XLUE_API(BOOL) XLUE_PushTexture(lua_State* luaState, XL_TEXTURE_HANDLE hTexture);
XLUE_API(BOOL) XLUE_PushFont(lua_State* luaState, XL_FONT_HANDLE hFont);
XLUE_API(BOOL) XLUE_PushColor(lua_State* luaState, XL_Color* lpColor);
XLUE_API(BOOL) XLUE_PushColor2(lua_State* luaState, XL_Color color);
XLUE_API(BOOL) XLUE_PushPen(lua_State* luaState, XL_PEN_HANDLE hPen);
XLUE_API(BOOL) XLUE_PushBrush(lua_State* luaState, XL_BRUSH_HANDLE hBrush);
XLUE_API(BOOL) XLUE_PushImageList(lua_State* luaState, XL_IMAGELIST_HANDLE hImageList);
XLUE_API(BOOL) XLUE_PushImageSeq(lua_State* luaState, XL_IMAGESEQ_HANDLE hImageSeq);
XLUE_API(BOOL) XLUE_PushCurve(lua_State* luaState, XL_CURVE_HANDLE hCurve);
XLUE_API(BOOL) XLUE_PushRes(lua_State* luaState, XLUE_RESOURCE_HANDLE hResHandle);

// ���漸��check��������������Ϊ�գ���ô������֮����Ҫ�Ծ��������Ӧ��Release(color����)
XLUE_API(BOOL) XLUE_CheckBitmap(lua_State* luaState, int index, XL_BITMAP_HANDLE *lpBitmap);
XLUE_API(BOOL) XLUE_CheckMask(lua_State* luaState, int index, XL_MASK_HANDLE *lpMask);
XLUE_API(BOOL) XLUE_CheckTexture(lua_State* luaState, int index, XL_TEXTURE_HANDLE *lpTexture);
XLUE_API(BOOL) XLUE_CheckFont(lua_State* luaState, int index, XL_FONT_HANDLE *lpFont);
XLUE_API(BOOL) XLUE_CheckColor(lua_State* luaState, int index, XL_Color** lplpColor);
XLUE_API(BOOL) XLUE_CheckColor2(lua_State* luaState, int index, XL_Color* lpColor);
XLUE_API(BOOL) XLUE_CheckPen(lua_State* luaState, int index, XL_PEN_HANDLE* lpPen);
XLUE_API(BOOL) XLUE_CheckBrush(lua_State* luaState, int index, XL_BRUSH_HANDLE* lpBrush);
XLUE_API(BOOL) XLUE_CheckImageList(lua_State* luaState, int index, XL_IMAGELIST_HANDLE* lpImageList);
XLUE_API(BOOL) XLUE_CheckImageSeq(lua_State* luaState, int index, XL_IMAGESEQ_HANDLE* lpImageSeq);
XLUE_API(BOOL) XLUE_CheckCurve(lua_State* luaState, int index, XL_CURVE_HANDLE* lpCurve);
XLUE_API(BOOL) XLUE_CheckRes(lua_State* luaState, int index, XLUE_RESOURCE_HANDLE *lphResHandle);
XLUE_API(BOOL) XLUE_CheckResEx(lua_State* luaState, int index, const char* lpResType, XLUE_RESOURCE_HANDLE *lphResHandle);

#endif //_XUNLEI_XLUE_API_H_
