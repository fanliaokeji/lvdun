#include "StdAfx.h"
#include "GifLoaderImpl.h"
#include "..\ImageHelper\ImageProcessor.h"

CGifLoaderImpl::CGifLoaderImpl(void)
{
	m_nWidth = 0;
	m_nHeight = 0;

	m_hGifHandle = NULL;
}

CGifLoaderImpl::~CGifLoaderImpl(void)
{
	m_nWidth = 0;
	m_nHeight = 0;
}
XLGP_GIF_HANDLE CGifLoaderImpl::GetXLGifObj()
{
	return m_hGifHandle;
}
int CGifLoaderImpl::LoadImage(const wstring& wstrFilePath, bool* pbStop, bool bScale, int nWidth, int nHeight)
{
	m_hGifHandle = XLGP_LoadGifFromFile(wstrFilePath.c_str());
	if (m_hGifHandle)
	{
		 XLGP_GifGetSize(m_hGifHandle, &m_nWidth, &m_nHeight);
		return 0;
	}
	return 4;
}
void CGifLoaderImpl::GetSrcBitmapSize(int& nWidth, int& nHeight)
{	
	nWidth = m_nWidth;
	nHeight = m_nHeight;
}
