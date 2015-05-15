/********************************************************************
*
* =-----------------------------------------------------------------=
* =                                                                 =
* =             Copyright (c) Xunlei, Ltd. 2004-2009                =
* =                                                                 =
* =-----------------------------------------------------------------=
* 
*   FileName    :   XLMath.h
*   Author      :   xlue group(xlue@xunlei.com)
*   Create      :   2009-9-3
*   LastChange  :   
*   History     :	
*
*   Description :   ͼ�ο����ѧ��غ�����ͷ�ļ�
*
********************************************************************/ 
#ifndef __XUNLEI_MATH_H__
#define __XUNLEI_MATH_H__

#ifdef WIN32
	#ifdef XLGRAPHIC_EXPORTS
		#ifdef __cplusplus 
			#define XLMATH_API(x) extern "C" __declspec(dllexport) x __stdcall 
		#else //__cplusplus
			#define XLMATH_API(x)  __declspec(dllexport) x __stdcall 
		#endif //__cplusplus
	#elif defined (XLUE_UNION)
		#ifdef __cplusplus 
			#define XLMATH_API(x) extern "C" x __stdcall 
		#else //__cplusplus
			#define XLMATH_API(x) x __stdcall 
		#endif //__cplusplus
	#else //XLGRAPHIC_EXPORTS
		#ifdef __cplusplus 
			#define XLMATH_API(x) extern "C" __declspec(dllimport) x __stdcall 
		#else //__cplusplus
			#define XLMATH_API(x) __declspec(dllimport) x __stdcall 
		#endif //__cplusplus
	#endif
#else
	#ifdef _cplusplus
		#define XLMATH_API(x) extern "C" x
	#else //__cplusplus
		#define XLMATH_API(x) x
	#endif //__cplusplus
#endif

#define XL_RECT_NOT_INTERSECT 0
#define XL_RECT_INTERSET      1
#define XL_RECT_INCLUDE_RECT  2
#define XL_RECT_IN_RECT       3
//********************* ��������㷨 ****************************
//***�����ཻ
XLMATH_API(BOOL) XL_SetRect(LPRECT lprc,int xLeft,int yTop,int xRight,int yBottom);
XLMATH_API(BOOL) XL_OffsetRect(LPRECT lprc,int dx,int dy);
XLMATH_API(BOOL) XL_SetRectEmpty(LPRECT lprc);
XLMATH_API(BOOL) XL_EqualRect(const RECT* lprc1,const RECT* lprc2);
XLMATH_API(BOOL) XL_CopyRect(LPRECT lprcDst,const RECT* lprsSrc);
XLMATH_API(BOOL) XL_IsRectEmpty(const RECT* lprc);
XLMATH_API(BOOL) XL_PtInRect(const RECT *lprc,POINT pt);

XLMATH_API(long) XL_IsRectIntersect(const RECT* pRect1,const RECT* pRect2);
XLMATH_API(BOOL) XL_IntersectRect(RECT* pResult,const RECT* pSrc1,const RECT* pSrc2);
//��һ�������
XLMATH_API(BOOL) XL_IntersectRectEx(RECT* pResult,const RECT* pRectList,size_t listSize);

//***���κϲ�
//�õ�һ������pSrc1,pSrc2����С����
XLMATH_API(BOOL) XL_UnionRect(RECT* pResult,const RECT* pSrc1,const RECT* pSrc2);
//��������μ��Ϻϲ��ɽ�����μ���,������μ��ϰ���������μ��ϣ����ҽ�����μ�����û���ཻ�ľ��� 
XLMATH_API(long) XL_UnionRectEx(const RECT* pInputRectList,size_t listSize,RECT* pOutputRectList,size_t outlistSize);


//***********************���ʽ����㷨************************************

typedef DWORD* XL_EXP_HANDLE;

typedef LPCRECT (*XL_ExpBindProc)(void* lpObject, const char* lpcstrObject);

XLMATH_API(XL_EXP_HANDLE) XL_CreateExp();
XLMATH_API(BOOL) XL_DeleteExp(XL_EXP_HANDLE hExp);
XLMATH_API(XL_EXP_HANDLE) XL_CloneEXP(XL_EXP_HANDLE hSrcExp);

XLMATH_API(BOOL) XL_SetExp(XL_EXP_HANDLE hExp, const char* lpcstrExp, BOOL* lpbPureNum);
XLMATH_API(BOOL) XL_BindExpObject(XL_EXP_HANDLE hExp, void* lpObject, XL_ExpBindProc lpBindProc);
XLMATH_API(BOOL) XL_BindExpRect(XL_EXP_HANDLE hExp, LPCRECT lpRect);

XLMATH_API(BOOL) XL_CaclExp(XL_EXP_HANDLE hExp);
XLMATH_API(long) XL_GetExpValue(XL_EXP_HANDLE hExp);
XLMATH_API(long) XL_GetExpString(XL_EXP_HANDLE hExp, char* lpBuffer, long len);

// ���α��ʽ
typedef DWORD* XL_RECTEXP_HANDLE;

XLMATH_API(XL_RECTEXP_HANDLE) XL_CreateRectExp();
XLMATH_API(BOOL) XL_DeleteRectExp(XL_RECTEXP_HANDLE hExp);

XLMATH_API(BOOL) XL_CaclRectExp(XL_RECTEXP_HANDLE hExp);
XLMATH_API(BOOL) XL_BindRectExpRect(XL_RECTEXP_HANDLE hExp, LPCRECT lpRect);

XLMATH_API(long) XL_GetRectExpLeft(XL_RECTEXP_HANDLE hExp);
XLMATH_API(long) XL_GetRectExpTop(XL_RECTEXP_HANDLE hExp);
XLMATH_API(long) XL_GetRectExpWidth(XL_RECTEXP_HANDLE hExp);
XLMATH_API(long) XL_GetRectExpHeight(XL_RECTEXP_HANDLE hExp);
XLMATH_API(BOOL) XL_GetRectExpRect(XL_RECTEXP_HANDLE hExp, LPRECT lpRect);

XLMATH_API(BOOL) XL_SetRectExpLeft(XL_RECTEXP_HANDLE hExp, LPCSTR lpcstrLeft);
XLMATH_API(BOOL) XL_SetRectExpTop(XL_RECTEXP_HANDLE hExp, LPCSTR lpcstrTop);
XLMATH_API(BOOL) XL_SetRectExpWidth(XL_RECTEXP_HANDLE hExp, LPCSTR lpcstrWidth);
XLMATH_API(BOOL) XL_SetRectExpHeight(XL_RECTEXP_HANDLE hExp, LPCSTR lpcstrHeight);

//*********************������ϵ�����㷨*******************************************************
//����
//typedef DWORD* XL_ANICURVE_HANDLE;


//����ʱ���ǵ����ģ�B���������еĲ��ֵ�ͱ�����������������һ������ֵ9999
//������ֵΪ9999����õ��ǻ��㣬Ӧ����

#define XLMATH_BSPLINE_ERROR      9999

#define	XLMATH_BEZIER   0//������
#define	XLMATH_BSPLINE  1//B����

typedef struct ParamControlPoint 
{
	float t;
	float s;
}_ControlPoint_;

//lpControl -- ���Ƶ�����
//nSize -- ���Ƶ�����
//type -- ��������
//t -- ʱ��ֵ��0~1��

XLMATH_API(float) XL_GetCurvePosition(_ControlPoint_* lpControl, int nSize, int type, float t);


#ifdef WIN32
	#ifdef XLGRAPHIC_THREADSAFE
		#define XL_INCREMENT InterlockedIncrement	
		#define XL_DECREMENT InterlockedDecrement
	#else  //XLGRAPHIC_THREADSAFE
		#define	XL_INCREMENT(x) (++*(x))
		#define XL_DECREMENT(x) (--*(x))
	#endif //XLGRAPHIC_THREADSAFE
#else 
	#define	XL_INCREMENT(x) (++*(x))
	#define XL_DECREMENT(x) (--*(x))
#endif // WIN32

#endif // __XUNLEI_MATH_H__

