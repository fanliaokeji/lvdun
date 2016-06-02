#pragma once
#include "imageloaderimpl.h"

class CGifLoaderImpl :
	public CImageLoaderImpl
{
public:
	CGifLoaderImpl(void);
	~CGifLoaderImpl(void);
	virtual int LoadImage(const wstring& wstrFilePath, bool* pbStop = NULL, bool bScale = false, int nWidth = 0, int nHeight = 0);
	virtual void GetSrcBitmapSize(int& nWidth, int& nHeight);
	virtual XLGP_GIF_HANDLE GetXLGifObj();
private:
	XLGP_GIF_HANDLE m_hGifHandle;
	int	m_nWidth;
	int m_nHeight;
};
