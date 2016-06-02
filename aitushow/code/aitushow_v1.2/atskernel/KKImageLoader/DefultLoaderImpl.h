#pragma once
#include "imageloaderimpl.h"


class CDefaultLoaderImpl :
	public CImageLoaderImpl
{
public:
	CDefaultLoaderImpl(void);
	~CDefaultLoaderImpl(void);
	virtual int LoadImage(const wstring& wstrFilePath, bool* pbStop = NULL, bool bScale = false, int nWidth = 0, int nHeight = 0);
	virtual XL_BITMAP_HANDLE GetXLBitmap()
	{
		return m_hXLBitmap;
	}
	virtual void GetSrcBitmapSize(int& nWidth, int& nHeight)
	{
		nWidth = m_nSrcBitmapWidth;
		nHeight = m_nSrcBitmapHeight;
	}
	virtual int GetExifInfoStatus()
	{
		return m_nExifInfoStatus;
	}

	virtual BOOL AutoRotate();
private:
	BOOL LoadRAWImage(LPCTSTR lpszPathName, fipWinImage& newImage);
	XL_BITMAP_HANDLE LoadJPEGImage(LPCTSTR lpszPathName, BOOL IsLoadExif);
private:
	XL_BITMAP_HANDLE m_hXLBitmap;
	int m_nSrcBitmapWidth;
	int m_nSrcBitmapHeight;
	int m_nExifInfoStatus;

	//
};
