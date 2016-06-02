#pragma once
#include "lcms2/lcms2.h"
#include "freeimage/FreeImagePlus.h"
#include "KKImageEXIF.h"
#include "KKImageInfo.h"
#include <Gdiplus.h>
// 实际加载类的父类，不要直接使用该类，要加载图片的时候用ImageLoader类
class CImageLoaderImpl
{
public:
	CImageLoaderImpl(void);
	virtual ~CImageLoaderImpl(void);
	virtual int LoadImage(const wstring& wstrFilePath, bool* pbStop = NULL, bool bScale = false, int nWidth = 0, int nHeight = 0) = 0;
	virtual XL_BITMAP_HANDLE GetXLBitmap();
	virtual XLGP_GIF_HANDLE GetXLGifObj();

	virtual void GetSrcBitmapSize(int& nWidth, int& nHeight) = 0;
	virtual int GetExifInfoStatus();


	virtual BOOL AdobeRGBtosRGB(fipWinImage& _fipWinImage, wstring wstrMonitorICCFilePath);
	virtual BOOL AdobeRGBtosRGB(fipWinImage& _fipWinImage, XL_BITMAP_HANDLE hXLBitmap);
	virtual BOOL AdobeRGBtosRGB(const void * MemPtr, cmsUInt32Number dwSize, XL_BITMAP_HANDLE hXLBitmap, wstring wstrMonitorICCFilePath);
	
	virtual BOOL CMYKtosRGB(const void * MemPtr, cmsUInt32Number dwSize, XL_BITMAP_HANDLE hBitmap, wstring wstrMonitorICCFilePath);
	virtual BOOL CMYKtosRGB(fipWinImage& _fipWinImage, XL_BITMAP_HANDLE hXLBitmap);
	virtual BOOL CMYKtosRGB(fipWinImage& _fipWinImage, wstring wstrMonitorICCFilePath);
	virtual XL_BITMAP_HANDLE ConvertFipImagetoXLBitmapHandle(fipWinImage& FipImage);

	
	void ApplyICC(fipWinImage& _fipWinImage, wstring wstrMonitorICCFilePath);
	void sRGBtoMonitorRGB(fipWinImage& _fipWinImage, wstring wstrMonitorICCFilePath);
	BOOL sRGBtoMonitorRGB(XL_BITMAP_HANDLE hXLBitmap, wstring wstrMonitorICCFilePath);
	virtual BOOL GetICCPath(std::wstring& ICCPath);
	virtual BOOL AutoRotate();
	CKKImageEXIF* m_KKImageEXIF;
private:
	
	//
};
