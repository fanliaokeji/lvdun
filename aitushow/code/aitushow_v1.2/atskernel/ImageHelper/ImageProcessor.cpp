#include "StdAfx.h"
#include "ImageProcessor.h"

using namespace Gdiplus;

CImageProcessor::CImageProcessor(void)
{
}

CImageProcessor::~CImageProcessor(void)
{

}

void CImageProcessor::GetThumbnailSize(const int nSrcWidth, const int nSrcHeight, const int nDstWidth, const int nDstHeight, int& nThumbnailWidth, int& nThumbnailHeight)
{
	TSAUTO();

	if (nSrcWidth <= nDstWidth && nSrcHeight <= nDstHeight)
	{
		nThumbnailWidth = nSrcWidth;
		nThumbnailHeight = nSrcHeight;
		return;
	}

	double dWidthZoom = (double)nSrcWidth/nDstWidth;
	double dHeightZoom = (double)nSrcHeight/nDstHeight;

	if (dWidthZoom > dHeightZoom)
	{
		nThumbnailWidth = nDstWidth;
		nThumbnailHeight = std::floor(nSrcHeight/dWidthZoom + 0.5);
		nThumbnailHeight = nThumbnailHeight>0?nThumbnailHeight:1;
	}
	else
	{
		nThumbnailWidth = std::floor(nSrcWidth/dHeightZoom + 0.5);
		nThumbnailWidth = nThumbnailWidth>0?nThumbnailWidth:1;
		nThumbnailHeight = nDstHeight;
	}
}
XL_BITMAP_HANDLE CImageProcessor::RescaleImage(XL_BITMAP_HANDLE hXLBitmap, int nWidth, int nHeight, BOOL IsKeepPercent, InterpolationMode eInterpolationMode)
{
	TSAUTO();

	TSDEBUG4CXX("nWidth:"<<nWidth<<" nHeight:"<<nHeight);
	assert(nWidth > 0 && nHeight > 0);

	DWORD time1 = ::GetTickCount();

	XLBitmapInfo oldBitmapInfo;
	XL_GetBitmapInfo(hXLBitmap, &oldBitmapInfo);
	if (oldBitmapInfo.ColorType != XLGRAPHIC_CT_ARGB32)
	{
		TSERROR4CXX("Error Bitmap Type, must be ARGB32!");
		return NULL;
	}
	Bitmap oldBitmap(oldBitmapInfo.Width, oldBitmapInfo.Height, oldBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hXLBitmap, 0, 0));

	int nDstWidth, nDstHeight;
	if (IsKeepPercent == TRUE)
	{
		CImageProcessor::GetThumbnailSize(oldBitmapInfo.Width, oldBitmapInfo.Height, nWidth, nHeight, nDstWidth, nDstHeight);
	}
	else
	{
		nDstWidth = nWidth;
		nDstHeight = nHeight;
	}
	assert(nDstWidth >= 1 && nDstHeight >= 1);
	nDstWidth = nDstWidth>=1?nDstWidth:1;
	nDstHeight = nDstHeight>=1?nDstHeight:1;

	TSDEBUG4CXX("nDstWidth:"<<nDstWidth<<" nDstHeight:"<<nDstHeight);

	XL_BITMAP_HANDLE hNewXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nDstWidth, nDstHeight);
	assert(hNewXLBitmap);
	if (hNewXLBitmap == NULL)
	{
		return NULL;
	}

	XLBitmapInfo newBitmapInfo;
	XL_GetBitmapInfo(hNewXLBitmap, &newBitmapInfo);
	Bitmap newBitmap(newBitmapInfo.Width, newBitmapInfo.Height, newBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hNewXLBitmap, 0, 0));

	Graphics graphics(&newBitmap);
	graphics.SetInterpolationMode(eInterpolationMode);
	graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	ImageAttributes RescaleAttributes;
	RescaleAttributes.SetWrapMode(WrapModeTileFlipXY);

	Rect DstRect(0, 0, newBitmapInfo.Width, newBitmapInfo.Height);
	graphics.DrawImage(&oldBitmap, DstRect, 0, 0, oldBitmapInfo.Width, oldBitmapInfo.Height, UnitPixel, &RescaleAttributes);

	DWORD time2 = ::GetTickCount();

	TSDEBUG4CXX("SrcWidth:"<<oldBitmapInfo.Width<<" SrcHeight:"<<oldBitmapInfo.Height<<" NewWidth:"<<nWidth<<" NewHeight:"<<nHeight<<" RescaleTime:"<<time2 - time1);

	return hNewXLBitmap;
}
XL_BITMAP_HANDLE CImageProcessor::ClipSubBindBitmap(XL_BITMAP_HANDLE hXLBitmap, int nLeft, int nTop, int nWidth, int nHeight)
{
	CRect rect = CRect(nLeft, nTop, nLeft+nWidth, nTop+nHeight);
	return XL_ClipSubBindBitmap(hXLBitmap, &rect);
}

XL_BITMAP_HANDLE CImageProcessor::ConvertImageToBitmap( Gdiplus::Image* lpImage, UINT width /*= 0*/, UINT height /*= 0*/ )
{
	assert(lpImage);

	CClientDC dc(NULL);
	CDC MemoryDC;
	MemoryDC.CreateCompatibleDC(dc);

	assert(MemoryDC);

	if (height == 0)
	{
		height = lpImage->GetHeight();
	}
	if (width == 0)
	{
		width = lpImage->GetWidth();
	}

	assert(height > 0 && width > 0);
	if (height <= 0 || width <= 0)
	{
		return NULL;
	}

	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biHeight =  -(LONG)height;
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;
	BYTE* pbase = NULL;
	CBitmap memoryBmp = ::CreateDIBSection(MemoryDC, &bmpInfo, DIB_RGB_COLORS, (void**)&pbase, 0, 0);
	assert(memoryBmp);
	if (memoryBmp.IsNull())
	{
		return NULL;
	}

	HBITMAP hOldBmp = MemoryDC.SelectBitmap(memoryBmp);

	Gdiplus::Graphics graphics(MemoryDC);

	Gdiplus::Rect rect(0, 0, width, height);
	graphics.DrawImage(lpImage, rect);

	assert(graphics.GetLastStatus() == Gdiplus::Ok);

	MemoryDC.SelectBitmap(hOldBmp);

	return ConvertDIBToXLBitmap32(memoryBmp);
}

XL_BITMAP_HANDLE CImageProcessor::ConvertDIBToXLBitmap32(HBITMAP hBitmap)
{
	assert(hBitmap);

	CBitmapHandle srcBitmap(hBitmap);
	BITMAP bmp;
	srcBitmap.GetBitmap(bmp);
	if (bmp.bmHeight <= 0 || bmp.bmWidth <= 0)
	{
		assert(false);
		return NULL;
	}

	XL_BITMAP_HANDLE hXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, bmp.bmWidth, bmp.bmHeight);
	assert(hXLBitmap);
	if (hXLBitmap == NULL)
	{
		return NULL;
	}

	BYTE* lpbyDest = XL_GetBitmapBuffer(hXLBitmap, 0, 0);
	assert(lpbyDest);

	if (bmp.bmBits != NULL && bmp.bmBitsPixel == 32)
	{
		::memcpy(lpbyDest, bmp.bmBits, bmp.bmWidthBytes * bmp.bmHeight);
	}
	else
	{
		CDC MemoryDC, srcDC;
		srcDC.CreateCompatibleDC(NULL);
		assert(srcDC);

		MemoryDC.CreateCompatibleDC(srcDC);
		assert(MemoryDC);

		BITMAPINFO bmpInfo;
		bmpInfo.bmiHeader.biHeight = -bmp.bmHeight;
		bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biSizeImage = 0;
		bmpInfo.bmiHeader.biXPelsPerMeter = 0;
		bmpInfo.bmiHeader.biYPelsPerMeter = 0;
		bmpInfo.bmiHeader.biClrUsed = 0;
		bmpInfo.bmiHeader.biClrImportant = 0;


		BYTE* pbase = NULL;
		CBitmap memoryBmp = ::CreateDIBSection(MemoryDC, &bmpInfo, DIB_RGB_COLORS, (void**)&pbase, 0, 0);
		assert(memoryBmp);

		HBITMAP hOldSrcBmp = srcDC.SelectBitmap(srcBitmap);
		HBITMAP hOldBmp = MemoryDC.SelectBitmap(memoryBmp);

		MemoryDC.BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, srcDC, 0, 0, SRCCOPY);

		srcDC.SelectBitmap(hOldSrcBmp);
		MemoryDC.SelectBitmap(hOldBmp);

		XLBitmapInfo info;
		XL_GetBitmapInfo(hXLBitmap, &info);

		::memcpy(lpbyDest, pbase, info.ScanLineLength * info.Height);

		// 重置alpha通道
		XL_ResetAlphaChannel(hXLBitmap, 255);
	}

	return hXLBitmap;
}

XL_BITMAP_HANDLE CImageProcessor::ImageLeftRotate(XL_BITMAP_HANDLE hXLBitmap)
{
	TSAUTO();

	XLBitmapInfo oldBitmapInfo;
	XL_GetBitmapInfo(hXLBitmap, &oldBitmapInfo);
	Bitmap oldBitmap(oldBitmapInfo.Width, oldBitmapInfo.Height, oldBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hXLBitmap, 0, 0));

	oldBitmap.RotateFlip(Rotate270FlipNone);

	XL_BITMAP_HANDLE hNewXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, oldBitmapInfo.Height, oldBitmapInfo.Width);
	if (!hNewXLBitmap)
	{
		return NULL;
	}

	XLBitmapInfo newBitmapInfo;
	XL_GetBitmapInfo(hNewXLBitmap, &newBitmapInfo);
	Bitmap newBitmap(newBitmapInfo.Width, newBitmapInfo.Height, newBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hNewXLBitmap, 0, 0));

	Graphics graphics(&newBitmap);
	graphics.DrawImage(&oldBitmap, 0, 0, newBitmapInfo.Width, newBitmapInfo.Height);

	return hNewXLBitmap;
}

XL_BITMAP_HANDLE CImageProcessor::ImageRightRotate(XL_BITMAP_HANDLE hXLBitmap)
{
	TSAUTO();

	XLBitmapInfo oldBitmapInfo;
	XL_GetBitmapInfo(hXLBitmap, &oldBitmapInfo);
	Bitmap oldBitmap(oldBitmapInfo.Width, oldBitmapInfo.Height, oldBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hXLBitmap, 0, 0));

	oldBitmap.RotateFlip(Rotate90FlipNone);

	XL_BITMAP_HANDLE hNewXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, oldBitmapInfo.Height, oldBitmapInfo.Width);
	if (!hNewXLBitmap)
	{
		return NULL;
	}
	XLBitmapInfo newBitmapInfo;
	XL_GetBitmapInfo(hNewXLBitmap, &newBitmapInfo);
	Bitmap newBitmap(newBitmapInfo.Width, newBitmapInfo.Height, newBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hNewXLBitmap, 0, 0));

	Graphics graphics(&newBitmap);
	graphics.DrawImage(&oldBitmap, 0, 0, newBitmapInfo.Width, newBitmapInfo.Height);

	return hNewXLBitmap;
}

XL_BITMAP_HANDLE CImageProcessor::ImageRotate180(XL_BITMAP_HANDLE hXLBitmap)
{
	TSAUTO();

	XLBitmapInfo oldBitmapInfo;
	XL_GetBitmapInfo(hXLBitmap, &oldBitmapInfo);
	Bitmap oldBitmap(oldBitmapInfo.Width, oldBitmapInfo.Height, oldBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hXLBitmap, 0, 0));

	oldBitmap.RotateFlip(Rotate180FlipNone);

	XL_BITMAP_HANDLE hNewXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, oldBitmapInfo.Width, oldBitmapInfo.Height);
	if (!hNewXLBitmap)
	{
		return NULL;
	}
	XLBitmapInfo newBitmapInfo;
	XL_GetBitmapInfo(hNewXLBitmap, &newBitmapInfo);
	Bitmap newBitmap(newBitmapInfo.Width, newBitmapInfo.Height, newBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hNewXLBitmap, 0, 0));

	Graphics graphics(&newBitmap);
	graphics.DrawImage(&oldBitmap, 0, 0, newBitmapInfo.Width, newBitmapInfo.Height);

	return hNewXLBitmap;
}  

XL_BITMAP_HANDLE CImageProcessor::RotateImage(XL_BITMAP_HANDLE hXLBitmap, double dAngle, int alpha, int r, int g, int b)
{
	TSAUTO();

	int nAngle = (int)dAngle%360;
	if (nAngle < 0)
	{
		nAngle = nAngle + 360;
	}
	TSDEBUG4CXX("nAngle:"<<nAngle);

	if (nAngle%360 == 270)
	{
		TSDEBUG4CXX("LeftRotate!");
		return CImageProcessor::ImageLeftRotate(hXLBitmap);
	}
	else if (nAngle%360 == 90)
	{
		TSDEBUG4CXX("RightRotate!");
		return CImageProcessor::ImageRightRotate(hXLBitmap);
	}
	else if (nAngle%360 == 180)
	{
		TSDEBUG4CXX("Rotate 180!");
		return CImageProcessor::ImageRotate180(hXLBitmap);
	}

	double pi = 3.14159265359;
	double arg = dAngle/180*pi;

	XLBitmapInfo oldBitmapInfo;
	XL_GetBitmapInfo(hXLBitmap, &oldBitmapInfo);
	if (oldBitmapInfo.ColorType != XLGRAPHIC_CT_ARGB32)
	{
		TSERROR4CXX("Error Bitmap Type, must be ARGB32!");
		return NULL;
	}
	Bitmap oldBitmap(oldBitmapInfo.Width, oldBitmapInfo.Height, oldBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hXLBitmap, 0, 0));

	//计算旋转后图像的大小
	int nOldWidth = oldBitmapInfo.Width;
	int nOldHeight = oldBitmapInfo.Height;

	int nNewWidth = std::floor(nOldWidth * std::abs(std::cos(arg)) + nOldHeight * std::abs(std::sin(arg)) + 0.5f);
	int nNewHeight = std::floor(nOldWidth * std::abs(std::sin(arg)) + nOldHeight * std::abs(std::cos(arg)) + 0.5f);

	XL_BITMAP_HANDLE hNewXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nNewWidth, nNewHeight);
	assert(hNewXLBitmap);
	if (hNewXLBitmap == NULL)
	{
		return NULL;
	}

	XLBitmapInfo newBitmapInfo;
	XL_GetBitmapInfo(hNewXLBitmap, &newBitmapInfo);
	Bitmap newBitmap(nNewWidth, nNewHeight, newBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hNewXLBitmap, 0, 0));

	Graphics graphics(&newBitmap);
	graphics.SetInterpolationMode(InterpolationModeHighQuality);
	graphics.SetCompositingQuality(CompositingQualityHighQuality);
	graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	PointF ImageCenter = PointF((float)nNewWidth/2, (float)nNewHeight/2);
	RectF ImageRect = RectF(ImageCenter.X - (float)nOldWidth/2, ImageCenter.Y - (float)nOldHeight/2, nOldWidth, nOldHeight);

	//填充背景色
	SolidBrush UserBrush(Color(alpha, r, g, b));
	Rect rect(0, 0, nNewWidth, nNewHeight);
	graphics.FillRectangle(&UserBrush, rect);

	//以图像中心点为中心，将坐标系旋转
	graphics.TranslateTransform(ImageCenter.X, ImageCenter.Y);
	graphics.RotateTransform(dAngle);
	graphics.TranslateTransform(-ImageCenter.X, -ImageCenter.Y);


	graphics.DrawImage(&oldBitmap, ImageRect, -1, -1, oldBitmapInfo.Width + 1, oldBitmapInfo.Height + 1, UnitPixel);

	graphics.ResetTransform();

	return hNewXLBitmap;
}

XL_BITMAP_HANDLE CImageProcessor::ImageLeftRightMirror(XL_BITMAP_HANDLE hXLBitmap)
{
	TSAUTO();

	XLBitmapInfo bmpInfo;
	XL_GetBitmapInfo(hXLBitmap, &bmpInfo);
	if (bmpInfo.ColorType != XLGRAPHIC_CT_ARGB32)
	{
		TSERROR4CXX("Error Bitmap Type, must be ARGB32!");
		return NULL;
	}

	XL_BITMAP_HANDLE hNewXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, bmpInfo.Width, bmpInfo.Height);
	assert(hNewXLBitmap);
	if (hNewXLBitmap == NULL)
	{
		return NULL;
	}

	BYTE* SrcBmpBuf = XL_GetBitmapBuffer(hXLBitmap, 0, 0);
	BYTE* DstBmpBuf = XL_GetBitmapBuffer(hNewXLBitmap, 0, 0);

	for (int j = 0; j < bmpInfo.Height; j++)
	{
		BYTE* SrcBmpLine = SrcBmpBuf + bmpInfo.ScanLineLength*j;
		BYTE* DstBmpLine = DstBmpBuf + bmpInfo.ScanLineLength*j;

		for (int i = 0; i < bmpInfo.Width; i++)
		{
			memcpy(DstBmpLine + i*4, SrcBmpLine + (bmpInfo.Width - i - 1)*4, 4);
		}
	}

	return hNewXLBitmap;
}

XL_BITMAP_HANDLE CImageProcessor::ImageTopBottomMirror(XL_BITMAP_HANDLE hXLBitmap)
{
	TSAUTO();

	XLBitmapInfo bmpInfo;
	XL_GetBitmapInfo(hXLBitmap, &bmpInfo);
	if (bmpInfo.ColorType != XLGRAPHIC_CT_ARGB32)
	{
		TSERROR4CXX("Error Bitmap Type, must be ARGB32!");
		return NULL;
	}

	XL_BITMAP_HANDLE hNewXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, bmpInfo.Width, bmpInfo.Height);
	assert(hNewXLBitmap);
	if (hNewXLBitmap == NULL)
	{
		return NULL;
	}

	BYTE* SrcBmpBuf = XL_GetBitmapBuffer(hXLBitmap, 0, 0);
	BYTE* DstBmpBuf = XL_GetBitmapBuffer(hNewXLBitmap, 0, 0);

	for (int j = 0; j < bmpInfo.Height; j++)
	{
		BYTE* DstBmpLine = DstBmpBuf + bmpInfo.ScanLineLength*j;
		BYTE* SrcBmpLine = SrcBmpBuf + bmpInfo.ScanLineLength*(bmpInfo.Height - j - 1);

		memcpy(DstBmpLine, SrcBmpLine, bmpInfo.ScanLineLength);
	}

	return hNewXLBitmap;
}

XL_BITMAP_HANDLE CImageProcessor::DrawThumbnailViewLayer(int nTotalWidth, int nTotalHeight, int nLeftPos, int nTopPos, int nRectWidth, int nRectHeight)
{
	if (nTotalWidth < 1 || nTotalHeight < 1)
	{
		return NULL;
	}
	if (nLeftPos < 0 || nLeftPos >= nTotalWidth || nTopPos < 0 || nTopPos >= nTotalHeight)
	{
		return NULL;
	}
	if (nLeftPos + nRectWidth > nTotalWidth)
	{
		nRectWidth = nTotalWidth - nLeftPos;
	}
	if (nTopPos + nRectHeight > nTotalHeight)
	{
		nRectHeight = nTotalHeight - nRectHeight;
	}
	RectF rect(0, 0, nTotalWidth, nTotalHeight);
	RectF clipRect(nLeftPos, nTopPos, nRectWidth, nRectHeight);
	// 创建一个位图
	XL_BITMAP_HANDLE hBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nTotalWidth, nTotalHeight);
	if (hBitmap == NULL)
	{
		return NULL;
	}
	XLBitmapInfo bmpInfo;
	XL_GetBitmapInfo(hBitmap, &bmpInfo);
	Bitmap newBitmap(bmpInfo.Width, bmpInfo.Height, bmpInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hBitmap, 0, 0));
	Gdiplus::Graphics graphics(&newBitmap);

	// 全部透明填充
	SolidBrush TransparentBrush(Color(1, 0, 0, 0));
	graphics.FillRectangle(&TransparentBrush, 0, 0, nTotalWidth, nTotalHeight);

	// 填充非透明区域
	Region ClipArea(clipRect);
	Region defaultRegion(rect);
	defaultRegion.Exclude(&ClipArea);
	graphics.SetClip(&defaultRegion);
	SolidBrush HalfTransparentBrush(Color(150, 0, 0, 0));
	graphics.FillRectangle(&HalfTransparentBrush, 0, 0, nTotalWidth, nTotalHeight);


	graphics.SetClip(rect);
	Pen pen(Color(255, 255, 255, 255));
	Pen pen1(Color(255, 255, 255, 255));
	pen.SetAlignment(PenAlignmentInset);
	pen1.SetDashStyle(DashStyleDash );
	// 画4条线
	graphics.DrawLine(&pen1, nLeftPos, nTopPos, nLeftPos, nTopPos+nRectHeight-1);
	graphics.DrawLine(&pen1, nLeftPos, nTopPos, nLeftPos+nRectWidth-1, nTopPos);
	graphics.DrawLine(&pen1, nLeftPos+nRectWidth-1, nTopPos, nLeftPos+nRectWidth-1, nTopPos+nRectHeight-1);
	graphics.DrawLine(&pen1, nLeftPos, nTopPos+nRectHeight-1, nLeftPos+nRectWidth-1, nTopPos+nRectHeight-1);
	// 画4个角

	//graphics.SetSmoothingMode(SmoothingModeHighQuality); //防直线锯齿
	//graphics.SetInterpolationMode(InterpolationModeHighQuality);
	//graphics.DrawArc(&pen, nLeftPos-4, nTopPos-4, 8, 8, 0, 360);
	//graphics.DrawArc(&pen, nLeftPos+nRectWidth-5, nTopPos-3, 8,8, 0, 360);
	//graphics.DrawArc(&pen, nLeftPos-4, nTopPos+nRectHeight-5, 8, 8, 0, 360);
	//graphics.DrawArc(&pen, nLeftPos+nRectWidth-5, nTopPos+nRectHeight-5, 8, 8, 0, 360);
	return hBitmap;
}
XL_BITMAP_HANDLE CImageProcessor::GetImageByARGB(int nWidth, int nHeight, int nAlpha, int nRed, int nGreen, int nBlue)
{
	TSAUTO();

	assert(nWidth);
	assert(nHeight);
	if (nWidth <= 0 || nHeight <= 0)
	{
		TSERROR4CXX("nWidth or nHeight is wrong value!");
		return NULL;
	}

	TSDEBUG4CXX("nAlpha:"<<nAlpha<<" nRed:"<<nRed<<" nGreeen:"<<nGreen<<" nBlue:"<<nBlue);

	XL_BITMAP_HANDLE hXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
	assert(hXLBitmap);
	if (hXLBitmap == NULL)
	{
		TSDEBUG4CXX("Create hXLBitmap Failed!");
		return NULL;
	}

	XLBitmapInfo bmpInfo;
	XL_GetBitmapInfo(hXLBitmap, &bmpInfo);
	Bitmap bmp(bmpInfo.Width, bmpInfo.Height, bmpInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(hXLBitmap, 0, 0));

	Gdiplus::Graphics graphics(&bmp);
	graphics.SetSmoothingMode(SmoothingModeHighQuality); //防直线锯齿
	graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	SolidBrush brush(Color(nAlpha, nRed, nGreen, nBlue));

	graphics.FillRectangle(&brush, 0, 0, nWidth, nHeight);

	//预乘
	XL_PreMultiplyBitmap(hXLBitmap);

	return hXLBitmap;
}