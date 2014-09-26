/********************************************************************
*
* =-----------------------------------------------------------------=
* =                                                                 =
* =             Copyright (c) Xunlei, Ltd. 2004-2011                =
* =                                                                 =
* =-----------------------------------------------------------------=
* 
*   FileName    :   XLGraphic.h 
*   Author      :   xlue group(xlue@xunlei.com)
*   Create      :   2007��4��3��
*   LastChange  :   
*   History     :	
*
*   Description :   XLGraphic.dll��ͷ�ļ�����������صĽӿ�
*
********************************************************************/ 

#ifndef _XUNLEI_GRAPHIC_H_
#define _XUNLEI_GRAPHIC_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XLGRAPHIC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XLGRAPHIC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef WIN32
	#ifdef XLGRAPHIC_EXPORTS
		#ifdef __cplusplus
			#define XLGRAPHIC_API(x) extern "C" __declspec(dllexport) x __stdcall 
		#else
			#define XLGRAPHIC_API(x) __declspec(dllexport) x __stdcall 
		#endif //__cplusplus
	#elif defined (XLUE_UNION)
		#ifdef __cplusplus
			#define XLGRAPHIC_API(x) extern "C" x __stdcall 
		#else
			#define XLGRAPHIC_API(x) x __stdcall 
		#endif //__cplusplus
	#else//XLGRAPHIC_EXPORTS
		#ifdef __cplusplus
			#define XLGRAPHIC_API(x) extern "C" __declspec(dllimport) x __stdcall 
		#else
			#define XLGRAPHIC_API(x) __declspec(dllimport) x __stdcall 
		#endif // __cplusplus
	#endif//XLGRAPHIC_EXPORTS
#else
	#ifdef __cplusplus
		#define XLGRAPHIC_API(x) extern "C" x 
	#else
		#define XLGRAPHIC_API(x) x 
	#endif
#endif

#ifndef __XLFS_H__
	#include <XLFS.h>
#endif //__XLFS_H__

//////////////////����///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
#define XLGRAPHIC_INVALID_HANDLE		NULL
// �������
#define XLGRAPHIC_FILL_SINGLE_COLOR		0	//��ɫ���
#define XLGRAPHIC_FILL_LINE_GRADIENT	1	//ֱ�߽������
#define XLGRAPHIC_FILL_CIRCLE_GRADIENT	2	//Բ���������
// ��ɫ���Ͷ���
#define	XLGRAPHIC_CT_ARGB32		0
#define XLGRAPHIC_CT_RGB24		1
#define XLGRAPHIC_CT_RGB565		2
#define XLGRAPHIC_CT_RGB555		3
#define XLGRAPHIC_CT_GRAY		4
#define XLGRAPHIC_CT_HSV655		5
#define XLGRAPHIC_CT_PARGB32	6
#define XLGRAPHIC_CT_HSV978		7
#define XLGRAPHIC_CT_HSL978		8
#define XLGRAPHIC_CT_UNKNOWN	0xCDCDCDCD

// ��Ⱦ����
#define XLGRAPHIC_BLEND_CONST_ALPHA		0
#define XLGRAPHIC_BLEND_SRC_ALPHA		1
#define XLGRAPHIC_BLEND_DIRECTOVERLAP	2
#define XLGRAPHIC_BLEND_KEYVALUEOVERLAP	3

// �ı�����
#define XLTEXT_TYPE_GDI				0
#define XLTEXT_TYPE_FREETYPE		1
#define XLTEXT_TYPE_FIXEDGDI		2

// �����hint���ԣ�����freetype������Ч
#define XLTEXT_FONTHINT_DEFAULT				0
#define XLTEXT_FONTHINT_NOHINT				1
#define XLTEXT_FONTHINT_FORCEAUTOHINT		2
#define XLTEXT_FONTHINT_DISABLEAUTOHINT		3

//����Ч��������freetype������Ч
#define XLTEXT_EFFECT_NONE			0
#define XLTEXT_EFFECT_BRIGHT		1
#define XLTEXT_EFFECT_BORDER		2

// ��ʽ����
#define			XLTEXT_DT_TOP			0x00000000
#define			XLTEXT_DT_VCENTER		0x00000004
#define			XLTEXT_DT_BOTTOM		0x00000008

#define			XLTEXT_DT_LEFT			0x00000000
#define			XLTEXT_DT_CENTER		0x00000001
#define			XLTEXT_DT_RIGHT			0x00000002

#define			XLTEXT_DT_WORD_ELLIPSIS	0x00000040
#define         XLTEXT_DT_END_ELLIPSIS  0x00000080
#define			XLTEXT_DT_WORDBREAK		0x00000100

// ��ȡ���е�ȫ����ȣ�������ʾ�������Ű�
#define			XLTEXT_DT_VISIBLEWIDTH	0x00000200

// �߼�����������ṹ
#define XLTEXT_LF_FACESIZE 32

// ����ķ��������壬Ĭ��ΪXLAntiAliasMode_Default
// ��ʱ�ڲ�ʹ�õ���LCD��MONO�Ļ�ϲ���
#define XLTEXT_ANTIALIASMODE_DEFAULT	0
#define XLTEXT_ANTIALIASMODE_MONO		1
#define XLTEXT_ANTIALIASMODE_LIGHT		2
#define XLTEXT_ANTIALIASMODE_LCD		3


// �ַ�������
#define XLTEXT_CHAR_SET_ALL			0		//ȫ�ַ�
#define XLTEXT_CHAR_SET_UNCH		1		//������
#define XLTEXT_CHAR_SET_CH			2		//������

// XLTextEnv�ı���ģʽ
#define		XLTEXT_BK_TRANSPARENT	0x0000
#define		XLTEXT_BK_OPAQUE		0x0001

#define XLADJUST_MODE_NORMAL			0x00000001
#define XLADJUST_MODE_BEPAINT			0x00000002
#define XLADJUST_MODE_SPLITTERPOINT		0x00000004

// transform
#define XLTRANS_MODE_NORMAL				1
#define XLTRANS_MODE_ANTIALIAS			2
#define XLTRANS_MODE_SHADOW				4

// region limit defination
#define XLGRAPHIC_INTEX_NORMAL		0x00000000		
#define XLGRAPHIC_INTEX_INFINITY	0x00000001
#define XLGRAPHIC_INTEX_NINFINITY	0x00000002

// mask type
#define XLMASK_SOURCE_NULL		0
#define XLMASK_SOURCE_BITMAP	1
#define XLMASK_SOURCE_TEXTURE	2
#define XLMASK_SOURCE_FILL		3

//instance chanel 
#define XLGRAPHIC_COLORCHANEL_NULL	0
#define XLGRAPHIC_COLORCHANEL_B		0
#define XLGRAPHIC_COLORCHANEL_G		1
#define XLGRAPHIC_COLORCHANEL_R		2
#define XLGRAPHIC_COLORCHANEL_A		3

//mask blend type
#define XLMASK_BLEND_INSTEAD		1
#define XLMASK_BLEND_ATTENUNATE		2

// ��������
#define XLTEXTURE_TYPE_STRETCH			0
#define XLTEXTURE_TYPE_NORMAL			1
#define XLTEXTURE_TYPE_NINEINONE		2
#define XLTEXTURE_TYPE_THREEINONEH		3
#define XLTEXTURE_TYPE_THREEINONEV		4
#define XLTEXTURE_TYPE_FIVEINONEH		5
#define XLTEXTURE_TYPE_FIVEINONEV		6
#define XLTEXTURE_TYPE_TILE				7

// ��������һ�����м䲿������
#define XLTEXTURE_CENTERSTRETCH    0   
// ��������һ������Ե��������
#define XLTEXTURE_SIDESTRETCH      1 


//////////////////����///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// �ڲ���ɫֵ��bgra���ֽ�˳���б���COLORREF���ֽ�˳��
typedef DWORD           XL_Color;

// ������Ͷ���
typedef void* XL_BITMAP_HANDLE;
typedef void* XL_BITMAPLIST_HANDLE;
typedef void* XL_TEXTURE_HANDLE;
typedef void* XL_TEXTENV_HANDLE;
typedef void* XL_FONT_HANDLE; 
typedef void* XL_IMAGELIST_HANDLE;
typedef void* XL_CURVE_HANDLE;
typedef void* XL_MASK_HANDLE;
typedef void* XL_IMAGESEQ_HANDLE;
typedef void* XL_TRANSFORM_HANDLE;
typedef void* XL_BRUSH_HANDLE;
typedef void* XL_PEN_HANDLE;

// λͼ�ṹ
typedef struct tagXLBitmapInfo
{
	DWORD ColorType;
	unsigned long Width;
	unsigned long Height;
	long ScanLineLength; //���scan line�Ǹ�������ʾλͼ�ǵ���洢��

}XLBitmapInfo;

typedef struct tagXLGraphicHint
{
	LPCRECT pClipRect;
	XL_MASK_HANDLE Mask;
	LPCRECT pMaskRect;
	DWORD MaskBlendType;
}XLGraphicHint;

//----------------��ʼ����-------------------------------
typedef struct tagXLGraphicParam
{
	// ָ����Ҫ���ı���Ⱦ���棬����gdi��freetype����
	DWORD			textType;
	// ���������ֶ�ֻ��freetype�ı���Ⱦ��Ч
	unsigned int	uCacheMaxFaces;
	unsigned int	uCacheMaxSize;
	unsigned int	uCacheMaxBytes;
	DWORD			fontHint;
	BOOL			bUseKerning;
	int				nItalicSlant;
	int				nXPixelsPerInch;
	int				nYPixelsPerInch;
}XLGraphicParam;

XLGRAPHIC_API(int) XL_InitGraphicLib(void* initParam);
XLGRAPHIC_API(int) XL_UnInitGraphicLib();
XLGRAPHIC_API(BOOL) XL_PrepareGraphicParam(XLGraphicParam* lpParam);
XLGRAPHIC_API(XLGraphicHint) XL_DefaultGraphicHint();

////////��ɫ�ռ�ת��  XL_Color����
//RGBA  PRGBA
#define XLCOLOR_BGR(b,g,r)          ((XL_Color)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#define XLCOLOR_BGRA(b,g,r,a)		((XL_Color)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)((BYTE)(r)|((WORD)((BYTE)(a))<<8))<<16))))
#define XLCOLOR_BGRA_A(bgra)		((BYTE)((bgra)>>24))
#define XLCOLOR_BGRA_R(bgra)		((BYTE)((bgra)>>16))
#define XLCOLOR_BGRA_G(bgra)		((BYTE)((bgra)>>8))
#define XLCOLOR_BGRA_B(bgra)		((BYTE)(bgra))
XLGRAPHIC_API(XL_Color) XL_ARGB2PARGB(const XL_Color clr);
XLGRAPHIC_API(XL_Color) XL_PARGB2ARGB(const XL_Color clr); 

// rgba(colorref)  bgra
#define XLColorToCOLORREF(bgra)     (XLCOLOR_BGRA(XLCOLOR_BGRA_R(bgra), XLCOLOR_BGRA_G(bgra), \
	XLCOLOR_BGRA_B(bgra),XLCOLOR_BGRA_A(bgra)))

//HSL  HSV
#define XLCOLOR_HSV(h,s,v) \
	(XL_Color)((((DWORD)(h)) << 23) | (((DWORD)(s)) << 16) | ((WORD)(v)) << 8)
#define XLCOLOR_HSV_H(clr) ((short)((clr) >> 23))
#define XLCOLOR_HSV_S(clr) ((unsigned char)(((clr) << 9) >> 25))
#define XLCOLOR_HSV_V(clr) ((unsigned char)(((clr) << 16) >> 24))
XLGRAPHIC_API(XL_Color) XL_RGB2HSV(const XL_Color clrSource);
XLGRAPHIC_API(XL_Color) XL_HSV2RGB(const XL_Color hsv,const unsigned char alpha);

#define XLCOLOR_HSL(h,s,l) \
	(XL_Color)((((DWORD)(h)) << 23) | (((DWORD)(s)) << 16) | ((WORD)(l)) << 8)
#define XLCOLOR_HSL_H(clr) ((short)((clr) >> 23) )
#define XLCOLOR_HSL_S(clr) ((unsigned char)(((clr) << 9) >> 25))
#define XLCOLOR_HSL_L(clr) ((unsigned char)(((clr) << 16) >> 24))

XLGRAPHIC_API(XL_Color) XL_HSL2RGB(const XL_Color hsl,const unsigned char alpha);
XLGRAPHIC_API(XL_Color) XL_RGB2HSL(const XL_Color clrSource);

//XYZ  Lab
#define XLCOLOR_XYZ(x,y,z) \
	(XL_Color)(((DWORD)(x)) << 24 | ((DWORD)(y)) << 16 | ((WORD)(z)) <<8)
#define XLCOLOR_XYZ_X(clr) ((unsigned char)((clr) >> 24))
#define XLCOLOR_XYZ_Y(clr) ((unsigned char)(((clr) << 8) >> 24))
#define XLCOLOR_XYZ_Z(clr) ((unsigned char)(((clr) << 16) >> 24))

//L�� 0 �� 120 
//a�� -120 �� 120 
//b�� -120 �� 120
#define XLCOLOR_LAB(l,a,b) \
	(XL_Color)(((DWORD)(x)) << 24 | ((DWORD)(y)) << 16 | ((WORD)(z)) <<8)
#define XLCOLOR_LAB_L(clr) ((unsigned char)((clr) >> 24))
#define XLCOLOR_LAB_A(clr) ((char)(((clr) << 8) >> 24))
#define XLCOLOR_LAB_B(clr) ((char)(((clr) << 16) >> 24))

//-----------Bitmap���ݽṹ����---------------------------
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_CreateBitmap(DWORD BitmapColorType,unsigned long Width,unsigned long Height);
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_CreateBindBitmap(XLBitmapInfo* pBitmapInfo,BYTE* pBuffer);

XLGRAPHIC_API(unsigned long) XL_AddRefBitmap(XL_BITMAP_HANDLE hBitmap);
XLGRAPHIC_API(unsigned long) XL_ReleaseBitmap(XL_BITMAP_HANDLE hBitmap);

// �õ�һ��BindBuffer����λͼ,��λͼ����Ч��ȡ����hSrcBitmap����Ч��
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_ClipSubBindBitmap(XL_BITMAP_HANDLE hSrcBitmap,LPCRECT pClipRect);

// �õ�һ��OwnerBuffer����λͼ
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_CloneBitmap(XL_BITMAP_HANDLE hSrcBitmap);

// ��ָ����ɫ�����λͼ
XLGRAPHIC_API(void) XL_FillBitmap(XL_BITMAP_HANDLE hBitmap,DWORD newColor);

// �õ�Bitmap����Ϣ
XLGRAPHIC_API(int) XL_GetBitmapInfo(XL_BITMAP_HANDLE hBitmap,XLBitmapInfo* pResult);

// ��ȡͼƬ�������ֽ���
XLGRAPHIC_API(unsigned char) XL_GetBitmapPixelBytes(XL_BITMAP_HANDLE hBitmap);

// �ж�ͼƬ��ԭʼͼ�����Ǽ��õ�����ͼ
XLGRAPHIC_API(BOOL) XL_IsBitmapOwnerBuffer(XL_BITMAP_HANDLE hBitmap);

// ��ȡͼƬ���ڴ滺�壬��Ҫ����С�Ĳ�������ֹ�����
XLGRAPHIC_API(BYTE*) XL_GetBitmapBuffer(XL_BITMAP_HANDLE hBitmap,unsigned long x,unsigned long y);

// �ж�һ��ͼƬ�Ƿ񾭹�����clip������Ҳ��ͨ��XL_ClipSubBindBitmap������һ����С��ͼƬ����
XLGRAPHIC_API(BOOL) XL_IsBitmapLossyCliped(XL_BITMAP_HANDLE hBitmap);

// �ж�һ��ͼƬ�Ƿ񾭹�Ԥ�˴���
XLGRAPHIC_API(BOOL) XL_IsBitmapPreMultiplied(XL_BITMAP_HANDLE hBitmap);

// ��ͼƬ����Ԥ�����ù��̲����棻������Ⱦ��ϵ֮ǰ������32λ��bitmap��Ҫ����Ԥ����
XLGRAPHIC_API(BOOL) XL_PreMultiplyBitmap(XL_BITMAP_HANDLE hBitmap);

// ��������alphaͨ����Ŀ��λͼ����32λ
XLGRAPHIC_API(BOOL) XL_ResetAlphaChannel(XL_BITMAP_HANDLE hBitmap, unsigned char alpha);

// ��ȡͼƬ��ɫ��
XLGRAPHIC_API(XL_Color) XL_GetBitmapMainColor(XL_BITMAP_HANDLE hSrcBmp,LPCRECT lpRegions,
											  unsigned int nRegionCnt);

// ��ͼƬɫ�����ת�������Դλͼɫ���Ѿ���Ŀ��λͼɫ��һ�£���ô��ֱ�ӷ���
// ��Ҫ���ؾ��ֻҪ��Ϊnull������Ҫrelease
// Ŀǰ֧��32λ��24λ��ת��32λת8λ��24λת8λ
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_ConvertBitmap(XL_BITMAP_HANDLE hBitmap, DWORD destColorType);

// ����pgn��ʽ��ͼƬ��ͼƬ������Ԥ��
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_LoadPngFromStream(XLFS_STREAM_HANDLE hStream);
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_LoadPngFromFile(const wchar_t* lpFile);

//----------Bitmap��һЩBlend����
// Blend����
typedef struct tagXLBlendHint
{
	XLGraphicHint BaseHint;
	DWORD BlendType;
	union 
	{                 
		unsigned char ConstAlpha;	// alphaֵ
		unsigned long KeyValue;		// �ؼ�ɫ��ֵ
	};   
}XLBlendHint;

// ClipRect���DestBitmap��0,0�㣬MaskRect�����ClipRect��0,0��
XLGRAPHIC_API(int) XL_Blend(XL_BITMAP_HANDLE hDesBitmap,long DstX,long DstY,XL_BITMAP_HANDLE hSrcBitmap, 
							const XLBlendHint* pHint);

typedef struct tagXLFillBlendHint
{
	XLBlendHint BaseHint;
	DWORD FillType;				//����
	union 
	{
		XL_Color SingleColor;
		struct {
			XL_Color SrcColor;	//�����ɫ
			XL_Color DstColor;	//Ŀ�����ɫ
			long SrcX;			//���X����ֵ
			long SrcY;			//���Y����ֵ
			long DstX;			//Ŀ���X����ֵ
			long DstY;			//Ŀ���Y����ֵ
		}Gradient;
	};
}XLFillBlendHint;

XLGRAPHIC_API(int) XL_FillBlend(XL_BITMAP_HANDLE hDesBitmap, const XLFillBlendHint* pHint);


//�µ�Blend������֧�������ͬʱ���
//���pSrcRectΪNULL,��SrcRect=src�Ĵ�С,���pDestRect=NULL,��pDestRect=(0,0,srcRect.width,srcRect.height)
//StretchBlend�ṹ
#define XLGRAPHIC_STRETCHTYPE_MULTISAMPLE_1X	0x00
#define XLGRAPHIC_STRETCHTYPE_MULTISAMPLE_2X	0x01
typedef struct tagXLStretchBlendHint
{
	XLBlendHint BaseHint;
	unsigned char StretchType;
}XLStretchBlendHint;

XLGRAPHIC_API(int) XL_StretchBlend(XL_BITMAP_HANDLE hDestBitmap,const RECT* pDestRect,
								   XL_BITMAP_HANDLE hSrcBitmap,const XLStretchBlendHint* pHint);
//---------Bitmap���������
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_StretchBitmap(XL_BITMAP_HANDLE hBitmap,unsigned long newWidth,unsigned long newHeight);

//---------Bitmap loader ��һ��ӿڻ��޸�,ʹ�ÿ�Դͨ�õ������巽ʽ
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_LoadBitmapFromMemory(const void *buffer, long size, unsigned long colorType);
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_LoadBitmapFromMemoryEx(const void *buffer, long size, unsigned long colorType, const wchar_t* lpFileName);
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_LoadBitmapFromFile(const wchar_t* filePath,unsigned long colorType);


//-----��ɫ����-------
typedef struct tagHSLAdjust
{
	short HAdjust; //-180--180
	short SAdjust; //-100--100
	short LAdjust; //-255--255

	DWORD Mode;
	XL_Color KeyColor;
}XL_HSLAdjust;
XLGRAPHIC_API(BOOL) XL_AdjustBitmapColor(XL_BITMAP_HANDLE hSrcBmp,XL_HSLAdjust* pModify);
XLGRAPHIC_API(XL_Color) XL_AdjustColor(XL_Color clrSource,XL_HSLAdjust* pModify);
XLGRAPHIC_API(XL_HSLAdjust) XL_GetHSVModify(const XL_Color clrSource,DWORD mode);

///////����
XLGRAPHIC_API(XL_HSLAdjust) XL_GetHSLAdjust(const XL_Color clrSource,DWORD mode);
XLGRAPHIC_API(int) XL_GetColorVariance(const XL_Color clr1,const XL_Color clr2);


//////////�任////////////////////////
XLGRAPHIC_API(unsigned long) XL_AddRefTrans(XL_TRANSFORM_HANDLE hTrans);
XLGRAPHIC_API(unsigned long) XL_ReleaseTrans(XL_TRANSFORM_HANDLE hTrans);
XLGRAPHIC_API(int) XL_GetSubTransCount(XL_TRANSFORM_HANDLE hTrans);
XLGRAPHIC_API(XL_TRANSFORM_HANDLE) XL_GetSubTrans(XL_TRANSFORM_HANDLE hTrans,int nIndex);
XLGRAPHIC_API(BOOL) XL_AddSubTrans(XL_TRANSFORM_HANDLE hTrans,XL_TRANSFORM_HANDLE hSubTrans);
XLGRAPHIC_API(XL_TRANSFORM_HANDLE) XL_CloneTrans(XL_TRANSFORM_HANDLE hLNT);
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_DoTrans(XL_BITMAP_HANDLE hSrc,XL_BITMAP_HANDLE hDest,
										   XL_TRANSFORM_HANDLE hTrans,DWORD flag);
XLGRAPHIC_API(void) XL_TransPoint(XL_TRANSFORM_HANDLE hTrans,const POINT *lpSrcPt,POINT* lpDstPt);
XLGRAPHIC_API(void) XL_TransRect(XL_TRANSFORM_HANDLE hTrans,const RECT* lpSrcRect,RECT* lpDstRect);
XLGRAPHIC_API(void) XL_TransGetDestBmpSize(XL_TRANSFORM_HANDLE hTrans,XL_BITMAP_HANDLE hSrc,
										   int*pWidth,int* pHeight);
XLGRAPHIC_API(void) XL_TransKeepCentreRect(XL_TRANSFORM_HANDLE hTrans,const RECT* lpSrcRect,
										   RECT* lpDstRect,const POINT* lpCentre);

typedef struct tagXLIntEx
{	
	DWORD m_flag;
	int m_value;
}XL_IntEx;

typedef struct tagIntLimit
{
	XL_IntEx m_min;
	XL_IntEx m_max;
}XL_IntLimit;

typedef struct tagRectRegionLimit
{
	//DWORD m_sign;
	XL_IntLimit m_limits[3];
}XL_RectRegion;

// bogus 3d linearity transform
typedef struct tagTransInfo
{
	double a[3][3];
	double b[3][1];
	int m_nRgnCnt;
	XL_RectRegion m_rgns[1];
}XLTransInfo;

XLGRAPHIC_API(BOOL) XL_GetTransInfo(XL_TRANSFORM_HANDLE hLNT,XLTransInfo* pInfo);
XLGRAPHIC_API(XL_TRANSFORM_HANDLE) XL_CreateTrans();
XLGRAPHIC_API(XL_TRANSFORM_HANDLE) XL_CreateRotateTrans(double dAnglex,double dAngley,double dAnglez);
XLGRAPHIC_API(XL_TRANSFORM_HANDLE) XL_CreateTransferTrans(int nDeltax,int nDeltay,int nDeltaz);
XLGRAPHIC_API(XL_TRANSFORM_HANDLE) XL_CreateStretchTrans(double dRatex,double dRatey,double dRatez);
XLGRAPHIC_API(BOOL) XL_SetTransParam(XL_TRANSFORM_HANDLE hLNT,double a[3][3],double b[3][1]);
XLGRAPHIC_API(BOOL) XL_SetTransDistDisp2obs(XL_TRANSFORM_HANDLE hLNT,int newDist);
XLGRAPHIC_API(BOOL) XL_SetTransRegion(XL_TRANSFORM_HANDLE hLNT,XL_RectRegion* pRegions,int nRegionCnt);


/////////////////////////////////////////////////////
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_GetBitmapChanel(XL_BITMAP_HANDLE hSrcBitmap,DWORD chanel,LPCRECT pRegion);
XLGRAPHIC_API(unsigned long) XL_AddRefMask(XL_MASK_HANDLE hMask);
XLGRAPHIC_API(unsigned long) XL_ReleaseMask(XL_MASK_HANDLE hMask);
XLGRAPHIC_API(XL_MASK_HANDLE) XL_NewMask();
XLGRAPHIC_API(XL_MASK_HANDLE) XL_CreateSubMask(XL_MASK_HANDLE hMask,LPCRECT lprcRect);
XLGRAPHIC_API(DWORD) XL_GetMaskSourceType(XL_MASK_HANDLE hMask);
XLGRAPHIC_API(void*) XL_GetMaskSource(XL_MASK_HANDLE hMask);
XLGRAPHIC_API(void) XL_BindMaskSource(XL_MASK_HANDLE hMask,DWORD type,void* pInstance,BOOL cache,DWORD chanel);
XLGRAPHIC_API(BOOL) XL_BuildMaskCache(XL_MASK_HANDLE hMask,int nWidth,int nHeight,LPCRECT pRegion,BOOL useDuelSize);
XLGRAPHIC_API(BOOL) XL_SetMaskSize(XL_MASK_HANDLE hMask,int nWidth,int nHeight,LPCRECT lpRegion);
XLGRAPHIC_API(void) XL_GetMaskSize(XL_MASK_HANDLE pMask,int *pWidth,int* pHeight,LPRECT lpRegion);
XLGRAPHIC_API(DWORD) XL_GetMaskSourceChanel(XL_MASK_HANDLE hMask);
XLGRAPHIC_API(XL_MASK_HANDLE) XL_CloneMask(XL_MASK_HANDLE hMask);
XLGRAPHIC_API(void) XL_SetMaskBmpStretch(XL_MASK_HANDLE hMask,BOOL bmpStretch);
XLGRAPHIC_API(void) XL_ClearMaskCache(XL_MASK_HANDLE hMask);


//---�����ı�����ӿ�----

#ifdef WIN32
//-------------- һЩ��Windows�йص�OS����--------------
XLGRAPHIC_API(int) XL_PaintBitmap(HDC hDC,XL_BITMAP_HANDLE hBitmap,const RECT* pDestRect,const RECT* pSrcRect);
XLGRAPHIC_API(int) XL_AlphaPaintBitmap(HDC hdcDest, int nXOriginDest, int nYOriginDest,
									   int nWidthDest, int nHeightDest,
									   XL_BITMAP_HANDLE hSrcBitmap, int nXOriginSrc, int nYOriginSrc,
									   int nWidthSrc, int nHeightSrc,
									   BLENDFUNCTION blendFunction);
#endif


// --------------�������ؽṹ�ͺ�������------------------------------------------------------

// ����ķ��������壬Ĭ��ΪXLAntiAliasMode_Normal
typedef struct tagXLLogFontInfo
{
	LONG  lfHeight;
	LONG  lfWidth;
	LONG  lfEscapement;
	LONG  lfOrientation;
	LONG  lfWeight;
	BYTE  lfItalic;
	BYTE  lfUnderline;
	BYTE  lfStrikeOut;
	BYTE  lfCharSet;
	BYTE  lfOutPrecision;
	BYTE  lfClipPrecision;
	BYTE  lfQuality;
	BYTE  lfPitchAndFamily;
	WCHAR lfFaceName[XLTEXT_LF_FACESIZE];

	BOOL    bUseCache;

	// ��������ز���
	DWORD antialiasMode;
	BOOL bIsBGROrder;        // ��LCD������ģʽ��Ч����ʶ����LCD��Ļ��RGB����˳��Ĭ����RGB����

	BOOL bBorder;

}XLLogFontInfo, *LPXLLogFontInfo;

XLGRAPHIC_API(XL_FONT_HANDLE) XL_CreateFont(const LPXLLogFontInfo lpLogFontInfo);
XLGRAPHIC_API(BOOL) XL_GetLogFont(XL_FONT_HANDLE hFont, LPXLLogFontInfo lpLogFontInfo);
XLGRAPHIC_API(BOOL) XL_DeleteFont(XL_FONT_HANDLE hFont);
XLGRAPHIC_API(unsigned long) XL_AddRefFont(XL_FONT_HANDLE hFont);
XLGRAPHIC_API(unsigned long) XL_ReleaseFont(XL_FONT_HANDLE hFont);

// ----------------�ı�����ؽṹ�ͺ�������-----------------------------------------------------
//----------- �ı������Ĵ���������----------------------------------

// ����һ���µ�TextEnv�����ؾ����hSrc��Ϊ�գ����µ�TextEnv��hSrc��ͬ���������Ĭ��ֵ
XLGRAPHIC_API(XL_TEXTENV_HANDLE) XL_CreateTextEnv(XL_TEXTENV_HANDLE hSrc);

// ����ָ���ı���Ⱦ�����textenv
XLGRAPHIC_API(XL_TEXTENV_HANDLE) XL_CreateTextEnv2(DWORD type);

// ɾ��һ�����е�TextEnv����ɾ���ɹ�������true�����򷵻�false�����ܸ�TextEnv����ʹ���У����ü�������1
XLGRAPHIC_API(BOOL) XL_DeleteTextEvn(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(unsigned long) XL_AddRefTextEnv(XL_TEXTENV_HANDLE hTextEnv);
XLGRAPHIC_API(unsigned long) XL_ReleaseTextEnv(XL_TEXTENV_HANDLE hTextEnv);

//------------ ���ı����������úͶ�ȡ ---------------------

// ���Ĭ�ϵ�facename
XLGRAPHIC_API(BOOL) XL_GetDefaultFaceName(wchar_t* szFaceName, UINT nLen);
XLGRAPHIC_API(BOOL) XL_IsSupportFont(const wchar_t* szFaceName);
XLGRAPHIC_API(BOOL) XL_SetConfigFontName(const wchar_t* szFaceName);

XLGRAPHIC_API(BOOL) XL_SetFreeTypeEnabled(BOOL bEnabled);
XLGRAPHIC_API(BOOL) XL_IsFreeTypeEnabled();

// �趨LCDģʽ�µ������������ӣ�gdi��freetype�ɶ�������
XLGRAPHIC_API(BOOL) XL_SetLCDModeFactor(BOOL gdi, double prim, double second, double tert);

//�����ı�ÿ�����ҵļ��
XLGRAPHIC_API(int) XL_SetTextLRGap(XL_TEXTENV_HANDLE hTextEnv, int nGap);
XLGRAPHIC_API(int) XL_GetTextLRGap(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(BOOL) XL_EnableRenderMode(BOOL bEnabled);
XLGRAPHIC_API(BOOL) XL_IsRenderModeEnabled();
XLGRAPHIC_API(BOOL) XL_SetLoadNoBitmap(BOOL bLoadNoBitmap);
XLGRAPHIC_API(DWORD) XL_SetHintMode(DWORD dwHintMode);
XLGRAPHIC_API(DWORD) XL_SetAntialiasMode(DWORD dwAntialiasMode);

XLGRAPHIC_API(XL_FONT_HANDLE) XL_GetDefaultFont();

XLGRAPHIC_API(BOOL)  XL_SetTextType(XL_TEXTENV_HANDLE hTextEnv, DWORD type);
XLGRAPHIC_API(DWORD)  XL_GetTextType(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(XL_Color) XL_SetTextColor(XL_TEXTENV_HANDLE hTextEnv, XL_Color crText);
XLGRAPHIC_API(XL_Color) XL_GetTextColor(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(XL_Color) XL_GetTextEffectColor(XL_TEXTENV_HANDLE hTextEnv);
XLGRAPHIC_API(XL_Color) XL_SetTextEffectColor(XL_TEXTENV_HANDLE hTextEnv, XL_Color crEffect);

XLGRAPHIC_API(DWORD) XL_SetTextEffectType(XL_TEXTENV_HANDLE hTextEnv, DWORD dwEffectType);
XLGRAPHIC_API(DWORD) XL_GetTextEffectType(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(float) XL_SetTextBorderWidth(XL_TEXTENV_HANDLE hTextEnv,float fWidth);
XLGRAPHIC_API(float) XL_GetTextBorderWidth(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(XL_Color) XL_SetBknColor(XL_TEXTENV_HANDLE hTextEnv, XL_Color crBkn);
XLGRAPHIC_API(XL_Color) XL_GetBknColor(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(int)	XL_SetBknMode(XL_TEXTENV_HANDLE hTextEnv,int nMode);
XLGRAPHIC_API(int)	XL_GetBknMode(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(XL_FONT_HANDLE) XL_SetTextFont(XL_TEXTENV_HANDLE hTextEnv, XL_FONT_HANDLE hFont);
XLGRAPHIC_API(XL_FONT_HANDLE) XL_GetTextFont(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(unsigned long) XL_SetTextAlignment(XL_TEXTENV_HANDLE hTextEnv, unsigned long ulTextAlignment);
XLGRAPHIC_API(unsigned long) XL_GetTextAlignment(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(int) XL_SetTextCharacterExtra(XL_TEXTENV_HANDLE hTextEnv, int nTextCharacterExtra);
XLGRAPHIC_API(int) XL_GetTextCharacterExtra(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(BOOL) XL_SetTextForceUnderline(XL_TEXTENV_HANDLE hTextEnv, BOOL bForceUnderline);
XLGRAPHIC_API(BOOL) XL_GetTextForceUnderline(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(int) XL_SetLineGap(XL_TEXTENV_HANDLE hTextEnv, int nLineGap);
XLGRAPHIC_API(int) XL_GetLineGap(XL_TEXTENV_HANDLE hTextEnv);

XLGRAPHIC_API(BOOL) XL_EnableShadow(XL_TEXTENV_HANDLE hTextEnv, BOOL bEnable);
XLGRAPHIC_API(XL_Color) XL_SetShadowColor(XL_TEXTENV_HANDLE hTextEnv, XL_Color crShadow);
XLGRAPHIC_API(BOOL) XL_SetShadowOffset(XL_TEXTENV_HANDLE hTextEnv, char xOffset, char yOffset);


// ��ȡָ������nCount�ı�lpcstrText�ĸ߶ȺͿ����Ϣ
XLGRAPHIC_API(BOOL) XL_GetSinglelineTextExtent(XL_TEXTENV_HANDLE hTextEnv, const wchar_t* lpcstrText,INT nCount, LPSIZE lpSize);

//ΪrichEditд��һ����ͬʱ�����͵㵽baseline�ľ���
XLGRAPHIC_API(BOOL) XL_GetTextExtentAndBaseLine(XL_TEXTENV_HANDLE hTextEnv, const wchar_t* lpcstrText,
												INT nCount, LPSIZE lpSize, int* lpnFontDescender);

XLGRAPHIC_API(BOOL) XL_GetMultilineTextExtent(XL_TEXTENV_HANDLE hTextEnv, const wchar_t* lpcstrText,INT nCount,INT nWidthLimit, LPSIZE lpSize);

// ����ָ����ʽ����ı������һ��������ʾ��Ч����
XLGRAPHIC_API(int)	XL_DrawSinglelineText(XL_TEXTENV_HANDLE hTextEnv, XL_BITMAP_HANDLE hBmp, const wchar_t* lpcstrText,INT nCount, LPCRECT lpcRect, DWORD dwFormat, XLGraphicHint* pHint);
XLGRAPHIC_API(int)	XL_DrawMultilineText(XL_TEXTENV_HANDLE hTextEnv, XL_BITMAP_HANDLE hBmp, const wchar_t* lpcstrText,INT nCount, LPCRECT lpcRect, DWORD dwFormat, XLGraphicHint* pHint);

XLGRAPHIC_API(BOOL) XL_TabbedTextOut(XL_TEXTENV_HANDLE hTextEnv, XL_BITMAP_HANDLE hBmp, const wchar_t* lpcstrText, INT nCount, 
										INT nXPos, INT nYPos, INT nTabPostions, const INT* lpnTabPostions, INT nTabOrigin, LPSIZE lpTextDimension, XLGraphicHint* pHint);

//-------------�����߶����֧��---------------------
#define XLCURVE_BEZIER		0 //������
#define XLCURVE_BSPLINE     1 //B��������


typedef struct tagXLAniPoint 
{
	float t;
	float s;
}XLAniPoint;

typedef struct tagXLAniCurveInfo
{
	DWORD type;
	unsigned long nsize;//���Ƶ�����
    XLAniPoint* lpPoint;
}XLAniCurveInfo;

XLGRAPHIC_API(XL_CURVE_HANDLE) XL_CreateCurve(DWORD type);
XLGRAPHIC_API(unsigned long) XL_AddRefCurve(XL_CURVE_HANDLE hCurve);
XLGRAPHIC_API(unsigned long) XL_ReleaseCurve(XL_CURVE_HANDLE hCurve);
XLGRAPHIC_API(unsigned long) XL_AddCurveControlPoint(XL_CURVE_HANDLE hCurve, float t, float s);
XLGRAPHIC_API(BOOL) XL_RemoveCurveControlPoint(XL_CURVE_HANDLE hCurve, long lControlPoint);
XLGRAPHIC_API(unsigned long) XL_GetCurveControlPointCount(XL_CURVE_HANDLE hCurve);
XLGRAPHIC_API(BOOL) XL_GetCurveInfo(XL_CURVE_HANDLE hCurve, XLAniCurveInfo* info);
XLGRAPHIC_API(BOOL) XL_GetCurveControlPoint(XL_CURVE_HANDLE hCurve, float* point, int nIndex);
XLGRAPHIC_API(DWORD) XL_GetCurveType(XL_CURVE_HANDLE hCurve);
XLGRAPHIC_API(float) XL_GetCurveProgress(XL_CURVE_HANDLE hCurve, float t);


//------------ ����������֧�� ---------------------
XLGRAPHIC_API(XL_TEXTURE_HANDLE) XL_CreateTexture(DWORD type);
XLGRAPHIC_API(unsigned long) XL_AddRefTexture(XL_TEXTURE_HANDLE hTexture);
XLGRAPHIC_API(unsigned long) XL_ReleaseTexture(XL_TEXTURE_HANDLE hTexture);

XLGRAPHIC_API(BOOL) XL_SetTextureBitmap(XL_TEXTURE_HANDLE hTexture, XL_BITMAP_HANDLE hBitmap, unsigned long uFlags);
XLGRAPHIC_API(BOOL) XL_SetTextureRect(XL_TEXTURE_HANDLE hTexture, LPCRECT lpRect);
XLGRAPHIC_API(BOOL) XL_SetTextureOrigin(XL_TEXTURE_HANDLE hTexture, const char* lpstrX, const char* lpstrY);

XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_GetTextureBitmap(XL_TEXTURE_HANDLE hTexture);
XLGRAPHIC_API(DWORD) XL_GetTextureType(XL_TEXTURE_HANDLE hTexture);

XLGRAPHIC_API(long) XL_AddTextureBlock(XL_TEXTURE_HANDLE hTexture, LPCRECT lpSrc, 
									   const char* lpstrDestLeft, const char* lpstrDestTop, 
									   const char* lpstrDestWidth, const char* lpstrDestHeight);
XLGRAPHIC_API(BOOL) XL_RemoveTextureBlock(XL_TEXTURE_HANDLE hTexture, long lBlock);

XLGRAPHIC_API(BOOL) XL_PaintTexture(XL_TEXTURE_HANDLE hTexture, long DstX, long DstY,
									XL_BITMAP_HANDLE hBitmap, const XLBlendHint* pHint);

//--------------��ImageList��֧��-------------------------

#define IMAGELIST_FLAG_HOR  0       // ImageList��ˮƽ��
#define IMAGELIST_FLAG_VER  1       // ImageList�Ǵ�ֱ��

#define IMAGELIST_FLAG_NOSPLITTER   0   // ImageList����û�зָ���
#define IMAGELIST_FLAG_SPLITTER     2   // ImageList�����зָ���

#define IMAGELIST_FLAG_EUQALBLOCK   0   // ImageList����ÿ��Block��size����ȵ�
#define IMAGELIST_FLAG_VARYBLOCK    4   // ImageList����ÿ��Block��size�ǲ��ȵģ���Ҫ�÷ָ�����ȷ��


XLGRAPHIC_API(XL_IMAGELIST_HANDLE) XL_CreateImageList();
XLGRAPHIC_API(unsigned long) XL_AddRefImageList(XL_IMAGELIST_HANDLE hImageList);
XLGRAPHIC_API(unsigned long) XL_ReleaseImageList(XL_IMAGELIST_HANDLE hImageList);

XLGRAPHIC_API(BOOL) XL_SetImageListBitmap(XL_IMAGELIST_HANDLE hImageList, XL_BITMAP_HANDLE hBitmap, unsigned long uFlags, int nBlockSize, XL_Color crSplitter);
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_GetImageListBitmap(XL_IMAGELIST_HANDLE hImageList);

XLGRAPHIC_API(BOOL) XL_PaintImageList(XL_IMAGELIST_HANDLE hImageList, XL_BITMAP_HANDLE hBitmap, int nIndex, LPCRECT lprcDest, unsigned char uAlpha);
XLGRAPHIC_API(int) XL_GetImageCount(XL_IMAGELIST_HANDLE hImageList);
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_ExtractBitmap(XL_IMAGELIST_HANDLE hImageList, int nIndex);

//���ʶ���
/* Pen Styles */
#define XLVECTOR_PS_SOLID            0
#define XLVECTOR_PS_DASH             1       /* -------  */
#define XLVECTOR_PS_DOT              2       /* .......  */
#define XLVECTOR_PS_DASHDOT          3       /* _._._._  */
#define XLVECTOR_PS_DASHDOTDOT       4       /* _.._.._  */
#define XLVECTOR_PS_NULL             5
#define XLVECTOR_PS_INSIDEFRAME      6

typedef struct tagXLLogPen
{
	int nPenStyle;
	int nWidth;
	XL_Color crColor;
}XLLOGPEN, *LPXLLOGPEN;

XLGRAPHIC_API(XL_PEN_HANDLE) XL_CreatePen(const LPXLLOGPEN lpLogPen);
XLGRAPHIC_API(BOOL) XL_GetLogPen(XL_PEN_HANDLE hPen, LPXLLOGPEN lpLogPen);
XLGRAPHIC_API(void) XL_SetPenColor(XL_PEN_HANDLE hPen, XL_Color color);
XLGRAPHIC_API(unsigned long) XL_AddRefPen(XL_PEN_HANDLE hPen);
XLGRAPHIC_API(unsigned long) XL_ReleasePen(XL_PEN_HANDLE hPen);

// ��ˢ����
/* Brush Styles */
#define XLVECTOR_BS_SOLID            0
// ������ݲ�֧��
#define XLVECTOR_BS_NULL             1
#define XLVECTOR_BS_HOLLOW           XLVECTOR_BS_NULL
#define XLVECTOR_BS_HATCHED          2
#define XLVECTOR_BS_PATTERN          3
#define XLVECTOR_BS_INDEXED          4
#define XLVECTOR_BS_DIBPATTERN       5
#define XLVECTOR_BS_DIBPATTERNPT     6
#define XLVECTOR_BS_PATTERN8X8       7

/* DIB color table identifiers */
#define XLVECTOR_DIB_RGB_COLORS      0 /* color table in RGBs */
#define XLVECTOR_DIB_PAL_COLORS      1 /* color table in palette indices */

/* Hatch Styles */
#define XLVECTOR_HS_HORIZONTAL       0       /* ----- */
#define XLVECTOR_HS_VERTICAL         1       /* ||||| */
#define XLVECTOR_HS_FDIAGONAL        2       /* \\\\\ */
#define XLVECTOR_HS_BDIAGONAL        3       /* ///// */
#define XLVECTOR_HS_CROSS            4       /* +++++ */
#define XLVECTOR_HS_DIAGCROSS        5       /* xxxxx */

typedef struct tagXLLOGBRUSH { 
	UINT     lbStyle; 
	XL_Color lbColor; 
	LONG     lbHatch; 
} XLLOGBRUSH, *LPXLLOGBRUSH;

XLGRAPHIC_API(XL_BRUSH_HANDLE) XL_CreateBrush(const LPXLLOGBRUSH lpLogBrush);
XLGRAPHIC_API(BOOL) XL_GetLogBrush(XL_BRUSH_HANDLE hBrush, LPXLLOGBRUSH lpLogBrush);
XLGRAPHIC_API(void) XL_SetBrushColor(XL_BRUSH_HANDLE hBrush, XL_Color color);
XLGRAPHIC_API(unsigned long) XL_AddRefBrush(XL_BRUSH_HANDLE hBrush);
XLGRAPHIC_API(unsigned long) XL_ReleaseBrush(XL_BRUSH_HANDLE hBrush);


XLGRAPHIC_API(BOOL) XL_DrawLine(XL_BITMAP_HANDLE hBmp, XL_PEN_HANDLE hPen, int nX1, int nY1,int nX2, int nY2, const XLGraphicHint* pHint);
XLGRAPHIC_API(BOOL) XL_DrawRectangle(XL_BITMAP_HANDLE hBmp, XL_BRUSH_HANDLE hBrush, XL_PEN_HANDLE hPen, int nX, int nY,int nWidth,  int nHeight, const XLGraphicHint* pHint);
XLGRAPHIC_API(BOOL) XL_DrawRectangle2(XL_BITMAP_HANDLE hBmp, XL_BRUSH_HANDLE hBrush, XL_PEN_HANDLE hPen, int nX, int nY,int nWidth,  int nHeight, const XLBlendHint* pBlendHint);
XLGRAPHIC_API(BOOL) XL_DrawPolygon(XL_BITMAP_HANDLE hBmp, XL_BRUSH_HANDLE hBrush,  XL_PEN_HANDLE hPen, POINT *lpPt, int nCount, const XLGraphicHint* pHint);
XLGRAPHIC_API(BOOL) XL_DrawEllipse(XL_BITMAP_HANDLE hBmp, XL_BRUSH_HANDLE hBrush, XL_PEN_HANDLE hPen, int nX, int nY,int nWidth, int nHeight, const XLGraphicHint* pHint);						   
XLGRAPHIC_API(BOOL) XL_DrawCurve(XL_BITMAP_HANDLE hBmp, XL_PEN_HANDLE hPen, int nX,  int nY,XL_CURVE_HANDLE hCurve, const XLGraphicHint* pHint);

XLGRAPHIC_API(XL_IMAGESEQ_HANDLE) XL_CreateImageSeq(int nCapacity);
XLGRAPHIC_API(unsigned long) XL_AddRefImageSeq(XL_IMAGESEQ_HANDLE hImageSeq);
XLGRAPHIC_API(unsigned long) XL_ReleaseImageSeq(XL_IMAGESEQ_HANDLE hImageSeq);

XLGRAPHIC_API(BOOL) XL_ImageSeqBeginAddImage(XL_IMAGESEQ_HANDLE hImageSeq);
XLGRAPHIC_API(BOOL) XL_ImageSeqEndAddImage(XL_IMAGESEQ_HANDLE hImageSeq);
XLGRAPHIC_API(BOOL) XL_ImageSeqAddImage(XL_IMAGESEQ_HANDLE hImageSeq, XL_BITMAP_HANDLE hBitmap, unsigned long long pos);

XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_ImageSeqGetImage(XL_IMAGESEQ_HANDLE hImageSeq, unsigned long long pos);

XLGRAPHIC_API(int) XL_ImageSeqGetCount(XL_IMAGESEQ_HANDLE hImageSeq);
XLGRAPHIC_API(XL_BITMAP_HANDLE) XL_ImageSeqGetImageByIndex(XL_IMAGESEQ_HANDLE hImageSeq, int nIndex);

XLGRAPHIC_API(long) XL_StatObject(long lType);

// �������սӿڣ����ڵ��ÿ��Լ����ڴ�ռ��
XLGRAPHIC_API(long) XL_GC(void* lpReserved);


/////image loader��չ

// ע��loader��Ҫ�Ļص�����
typedef XL_BITMAP_HANDLE (*LPFNLOADIMAGEFROMSTREAM)(XLFS_STREAM_HANDLE hStream, unsigned long resultColorType);
typedef BOOL (*LPFNCHECKIMAGEFILEPATH)(const wchar_t* filePath, const wchar_t* extName);

XLGRAPHIC_API(int) XL_RegisterImageLoader(LPFNCHECKIMAGEFILEPATH lpFileCheck, LPFNLOADIMAGEFROMSTREAM lpLoaderFunction);
XLGRAPHIC_API(BOOL) XL_RemoveImageloader(int cookie);

#endif // _XUNLEI_GRAPHIC_H_

