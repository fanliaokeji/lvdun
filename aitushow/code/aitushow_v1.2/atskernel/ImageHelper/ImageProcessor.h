#pragma once

#include <GdiPlus.h>
#include <cmath>

class CImageProcessor
{
public:
	CImageProcessor(void);
	~CImageProcessor(void);
	
	static void GetThumbnailSize(const int nSrcWidth, const int nSrcHeight, const int nDstWidth, const int nDstHeight, int& nThumbnailWidth, int& nThumbnailHeight);

	// 缩放
	static XL_BITMAP_HANDLE RescaleImage(XL_BITMAP_HANDLE hXLBitmap, int nWidth, int nHeight, BOOL IsKeepPercent = FALSE, Gdiplus::InterpolationMode eInterpolationMode = Gdiplus::InterpolationModeBilinear);
	// 在原图上裁剪一个小区域
	static XL_BITMAP_HANDLE ClipSubBindBitmap(XL_BITMAP_HANDLE hXLBitmap, int nLeft, int nTop, int nWidth, int nHeight);
	// 将GDI+中的Image转换成XLBitmap
	static XL_BITMAP_HANDLE ConvertImageToBitmap( Gdiplus::Image* lpImage, UINT width = 0, UINT height = 0 );
	// 将HBitmap转成XLBitmap
	static XL_BITMAP_HANDLE ConvertDIBToXLBitmap32( HBITMAP hBitmap );

	static XL_BITMAP_HANDLE ImageRotate180(XL_BITMAP_HANDLE hXLBitmap); //旋转180度
	static XL_BITMAP_HANDLE ImageLeftRotate(XL_BITMAP_HANDLE hXLBitmap); //左旋
	static XL_BITMAP_HANDLE GetImageByARGB(int nWidth, int nHeight, int nAlpha, int nRed, int nGreen, int nBlue); //获得指定颜色的指定大小的图片
	static XL_BITMAP_HANDLE ImageRightRotate(XL_BITMAP_HANDLE hXLBitmap); //右旋
	static XL_BITMAP_HANDLE ImageLeftRightMirror(XL_BITMAP_HANDLE hXLBitmap); //左右翻转
	static XL_BITMAP_HANDLE ImageTopBottomMirror(XL_BITMAP_HANDLE hXLBitmap); //上下翻转
	static XL_BITMAP_HANDLE RotateImage(XL_BITMAP_HANDLE hXLBitmap, double dAngle, int alpha = 0, int r = 0, int g = 0, int b = 0); //旋转任意角度
	static XL_BITMAP_HANDLE DrawThumbnailViewLayer(int nTotalWidth, int nTotalHeight, int nLeftPos, int nTopPos, int nRectWidth, int nRectHeight);
private:	
	//
};
