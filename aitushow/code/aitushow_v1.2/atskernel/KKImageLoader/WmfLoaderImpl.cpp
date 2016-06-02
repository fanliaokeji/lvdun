#include "StdAfx.h"
#include "WmfLoaderImpl.h"
#include "..\ImageHelper\ImageProcessor.h"

CWmfLoaderImpl::CWmfLoaderImpl(void)
{
	m_hBitmap = NULL;
	m_nWidth = 1;
	m_nHeight = 1;
}

CWmfLoaderImpl::~CWmfLoaderImpl(void)
{
}

XL_BITMAP_HANDLE CWmfLoaderImpl::GetXLBitmap()
{
	return m_hBitmap;
}
void CWmfLoaderImpl::GetSrcBitmapSize(int& nWidth, int& nHeight)
{	
	nWidth = m_nWidth;
	nHeight = m_nHeight;
}

int CWmfLoaderImpl::LoadImage(const wstring& wstrFilePath, bool* pbStop, bool bScale, int nWidth, int nHeight)
{
	Gdiplus::Metafile metafile(wstrFilePath.c_str());
	if (metafile.GetLastStatus() != Gdiplus::Ok)
	{
		return 4;
	}
	Gdiplus::MetafileHeader metafileheader;
	metafile.GetMetafileHeader(&metafileheader);
	m_nHeight = metafileheader.Height;
	m_nWidth = metafileheader.Width;
	if ( m_nWidth == 0 || m_nHeight == 0 )
	{
		return 4;
	}
	int nCurWidth = m_nWidth;
	int nCurHeight = m_nHeight;
	// 做缩放
	if (bScale)// 计算缩放率
	{
		double dRatio = 0;
		double nMaxLength = nWidth>nHeight?nWidth:nHeight;
		double nMinLength = nWidth<nHeight?nWidth:nHeight;

		double nImageMaxLength = m_nWidth>m_nHeight?m_nWidth:m_nHeight;
		double nImageMinLength = m_nWidth<m_nHeight?m_nWidth:m_nHeight;

		if (nMaxLength >= nImageMaxLength && nMinLength >= nImageMinLength) // 不缩放
		{
		}
		else	// 缩放
		{
			if (nMaxLength/nImageMaxLength > nMinLength/nImageMinLength)
			{
				dRatio = nMinLength/nImageMinLength;
			}
			else
			{
				dRatio = nMaxLength/nImageMaxLength;
			}
			nCurWidth = nCurWidth * dRatio;
			nCurHeight = nCurHeight * dRatio;
		}
	}

	m_hBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nCurWidth, nCurHeight);
	if (m_hBitmap == NULL)
	{
		return 4;
	}

	XLBitmapInfo newBitmapInfo;
	XL_GetBitmapInfo(m_hBitmap, &newBitmapInfo);
	Gdiplus::Bitmap newBitmap(newBitmapInfo.Width, newBitmapInfo.Height, newBitmapInfo.ScanLineLength, PixelFormat32bppARGB, XL_GetBitmapBuffer(m_hBitmap, 0, 0));
	Gdiplus::Graphics graphics(&newBitmap);
	Gdiplus::Rect rect(0, 0, nCurWidth, nCurHeight);
	graphics.DrawImage(&metafile, rect);

	if (m_hBitmap != NULL)
	{
		return 0;
	}
	return 4;
}
