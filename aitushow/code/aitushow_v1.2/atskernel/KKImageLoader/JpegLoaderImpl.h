#pragma once
#include "imageloaderimpl.h"
#include "libjpeg9/jpeglib.h"
#include <setjmp.h>
#include <stdio.h>
#include "..\ImageHelper\ImageProcessor.h"

#define EXIF_MARKER		(JPEG_APP0+1)
#define ICC_MARKER		(JPEG_APP0+2)
#define ICC_HEADER_SIZE 14



class CJpegLoaderImpl :
	public CImageLoaderImpl
{
public:
	CJpegLoaderImpl(void);
	~CJpegLoaderImpl(void);
	virtual int LoadImage(const wstring& wstrFilePath, bool* pbStop = NULL, bool bScale = false, int nWidth = 0, int nHeight = 0);
	virtual XL_BITMAP_HANDLE GetXLBitmap()
	{
		return m_hBitmap;
	}
	virtual void GetSrcBitmapSize(int& nWidth, int& nHeight);
	virtual int GetExifInfoStatus()
	{
		return m_nExifInfoStatus;
	}
	virtual BOOL AutoRotate();
private:
	BOOL MarkerIsIcc(jpeg_saved_marker_ptr marker);
	BOOL ReadIccProfile(j_decompress_ptr cinfo, JOCTET **icc_data_ptr, unsigned *icc_data_len);
	BOOL ReadExifProfile(j_decompress_ptr cinfo, JOCTET **icc_data_ptr, unsigned *icc_data_len);
	XL_BITMAP_HANDLE m_hBitmap;
	int m_nBitmapWidth;
	int m_nBitmapHeight;
	bool m_nExifInfoStatus;
	int m_nOriention;
private:
	//
};
