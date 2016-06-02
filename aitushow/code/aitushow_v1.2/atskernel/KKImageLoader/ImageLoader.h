#pragma once
#include "ImageLoaderImpl.h"

typedef enum{
	KKImg_Type_InValid = 0,
	KKImg_Type_Jpeg,
	KKImg_Type_Gif,
	KKImg_Type_Png,
	KKImg_Type_Psd,
	KKImg_Type_Default

}ImageLoaderImplType;

// 图片文件加载类，对外提供接口，内部根据文件类型调用相应的实际加载类
class CImageLoader
{
public:
	CImageLoader(void);
	~CImageLoader(void);

	// 通用

	// 返回值说明：0 成功，可以调用GetXLBitmap获取加载所得位图， 1 路径为空 2 文件不存在 3 不能识别的格式 4 加载失败
	int LoadImage(const wstring& wstrFilePath, bool* pbStop = NULL, bool bScale = false, int nWidth = 0, int nHeight = 0, bool bAutoRotate = false);
	CKKImageEXIF* LoadImageFileOnlyExif(LPCTSTR lpszPathName); //仅加载图片的exif信息
	ImageLoaderImplType GetLoaderType();				// 获取图片文件加载类类型
	CKKImageEXIF* GetExifInfo(bool bReset = true);		// 获取Exif信息
	CKKImageInfo* GetImageInfo(bool bReset = true);	// 获取图片文件基本信息
	BOOL SaveImage(XL_BITMAP_HANDLE hSaveBitmap, LPCTSTR lpszPathName, CKKImageEXIF* pKKImageEXIF = NULL, BOOL IsDelExif = FALSE,	int nJPEGQuality = 100, BOOL IsHighQuality = FALSE, int nDPI = -1);

	
	// 普通图片文件相关
	XL_BITMAP_HANDLE GetXLBitmap();
	XLGP_GIF_HANDLE GetXLGifObj();
	
private:
	ImageLoaderImplType m_nType;	// 具体加载类的类型
	CImageLoaderImpl* m_LoaderImpl;	// 具体的加载类
	CKKImageInfo* m_KKImageInfo;
	//
};
