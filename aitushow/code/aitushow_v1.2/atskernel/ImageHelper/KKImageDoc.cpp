#include "StdAfx.h"
#include "KKImageDoc.h"
#include "..\ImageHelper\ImageProcessor.h"

CKKImageDoc::CKKImageDoc(void)
{
	m_hGifObj = NULL;
	m_hSrcBitmap = NULL;
	m_hAdaptedBitmap = NULL;
	m_KKImageEXIF = NULL; 
	m_KKImageInfo = NULL;
	m_nDocType = 0;
	m_nRotateAngle = 0;
	m_nDocId = CreateDocId();
}

CKKImageDoc::~CKKImageDoc(void)
{
	TSAUTO();
	if (m_hSrcBitmap)
	{
		XL_ReleaseBitmap(m_hSrcBitmap);
		m_hSrcBitmap = NULL;
	}

	if (m_hAdaptedBitmap)
	{
		XL_ReleaseBitmap(m_hAdaptedBitmap);
		m_hAdaptedBitmap = NULL;
	}
	if (m_KKImageEXIF)
	{
		delete m_KKImageEXIF;
		m_KKImageEXIF = NULL;
	}
	if (m_KKImageInfo)
	{
		delete m_KKImageInfo;
		m_KKImageInfo = NULL;
	}
	if (m_hGifObj)
	{
		XLGP_ReleaseGif(m_hGifObj);
		m_hGifObj = NULL;
	}
}

int CKKImageDoc::CreateDocId()
{
	static int nIndex = 0;
	return ++nIndex;
}

void CKKImageDoc::SetSrcBitmap(XL_BITMAP_HANDLE hXLBitmap)
{
	if (m_hSrcBitmap)	// 如果之前有，先release掉
	{
		XL_ReleaseBitmap(m_hSrcBitmap);
		m_hSrcBitmap = NULL;
	}
	if (m_nRotateAngle%360 != 0)	// 需要旋转
	{
		XL_BITMAP_HANDLE hNewBitmap = CImageProcessor::RotateImage(hXLBitmap, m_nRotateAngle);
		if (hNewBitmap)	// 旋转成功，做替换
		{
			XL_ReleaseBitmap(hXLBitmap);
			m_hSrcBitmap = hNewBitmap;
			return;
		}
	}
	m_hSrcBitmap = hXLBitmap;
}
XL_BITMAP_HANDLE CKKImageDoc::GetSrcBitmap()
{
	return m_hSrcBitmap;
}
void CKKImageDoc::ReleaseSrcBitmap()
{
	if (m_hSrcBitmap)
	{
		XL_ReleaseBitmap(m_hSrcBitmap);
		m_hSrcBitmap = NULL;
	}
}

void CKKImageDoc::SetGifObj(XLGP_GIF_HANDLE hGifObj)
{
	m_hGifObj = hGifObj;
}
XLGP_GIF_HANDLE CKKImageDoc::GetGifObj()
{
	return m_hGifObj;
}

void CKKImageDoc::SetAdaptedBitmap(XL_BITMAP_HANDLE hXLBitmap)
{
	m_hAdaptedBitmap = hXLBitmap;
}
XL_BITMAP_HANDLE CKKImageDoc::GetAdaptedBitmap()
{
	return m_hAdaptedBitmap;
}
void CKKImageDoc::SetExifInfo(CKKImageEXIF* pExifInfo)
{
	m_KKImageEXIF = pExifInfo;
}
void CKKImageDoc::SetImageInfo(CKKImageInfo* pImageInfo)
{
	m_KKImageInfo = pImageInfo;
}

void CKKImageDoc::GetSrcBitmapSize(int& nWidth, int& nHeight)
{
	if (m_KKImageInfo != NULL)
	{
		m_KKImageInfo->GetImageSize(nWidth, nHeight);
	}
	else
	{
		nWidth = 1;
		nHeight = 1;
	}
}
void CKKImageDoc::SetDocType(int nType)
{
	m_nDocType = nType;
}
int CKKImageDoc::GetDocType()
{
	return m_nDocType;
}
void CKKImageDoc::SetExifInfoStatus(int nExifInfoStatus)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetExifInfoStatus(nExifInfoStatus);
	}
}
int CKKImageDoc::GetExifInfoStatus()
{
	if (m_KKImageInfo)
	{
		return m_KKImageInfo->GetExifInfoStatus();
	}
	return 0;
}

wstring CKKImageDoc::GetFileName()
{
	wstring wstrFileName;
	if (m_KKImageInfo)
	{
		wstrFileName = m_KKImageInfo->GetFileName();
	}
	return wstrFileName;
}
void CKKImageDoc::SetFileName(wstring& wstrFileName)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetFileName(wstrFileName);
	}
}
wstring CKKImageDoc::GetFilePath()
{
	wstring wstrFilePath;
	if (m_KKImageInfo)
	{
		wstrFilePath = m_KKImageInfo->GetFilePath();
	}
	return wstrFilePath;
}
void CKKImageDoc::SetFilePath(wstring& wstrFilePath)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetFilePath(wstrFilePath);
	}
}
wstring CKKImageDoc::GetFileType()
{
	wstring wstrExtension;
	if (m_KKImageInfo)
	{
		wstring wstrFilePath = m_KKImageInfo->GetFilePath();
		wstrExtension = ::PathFindExtension(wstrFilePath.c_str());
		wstrExtension = wstrExtension.substr(1, wstrExtension.size()-1);
	}
	return wstrExtension;
}
DWORD CKKImageDoc::GetFileSize()
{
	DWORD dwFileSize = 0;
	if (m_KKImageInfo)
	{
		dwFileSize = m_KKImageInfo->GetFileSize();
	}
	return dwFileSize;
}
void CKKImageDoc::GetFileImageSize(int& nWidth, int& nHeight)
{
	nWidth = 0;
	nHeight = 0;
	if (m_KKImageInfo)
	{
		m_KKImageInfo->GetImageSize(nWidth, nHeight);
	}
}

void CKKImageDoc::SetDateTimeOriginal(wstring& wstrDateTimeOriginal)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetDateTimeOriginal(wstrDateTimeOriginal);
	}
}
wstring CKKImageDoc::GetDateTimeOriginal()
{
	wstring DateTimeOriginal;
	if (m_KKImageInfo)
	{
		DateTimeOriginal = m_KKImageInfo->GetDateTimeOriginal();
	}
	return DateTimeOriginal;
}

void CKKImageDoc::SetMake(wstring& wstrMake)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetMake(wstrMake);
	}
}
wstring CKKImageDoc::GetMake()
{
	wstring wstrMake;
	if (m_KKImageInfo)
	{
		wstrMake = m_KKImageInfo->GetMake();
	}
	return wstrMake;
}

void CKKImageDoc::SetModel(wstring& wstrModel)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetModel(wstrModel);
	}
}
wstring CKKImageDoc::GetModel()
{
	wstring wstrModel;
	if (m_KKImageInfo)
	{
		wstrModel = m_KKImageInfo->GetModel();
	}
	return wstrModel;
}

void CKKImageDoc::SetLensType(wstring& wstrLensType)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetLensType(wstrLensType);
	} 
}
wstring CKKImageDoc::GetLensType()
{
	wstring wstrLensType;
	if (m_KKImageInfo)
	{
		wstrLensType = m_KKImageInfo->GetLensType();
	}
	return wstrLensType;
}
void CKKImageDoc::SetShutterCount(wstring& wstrShutterCount)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetShutterCount(wstrShutterCount);
	}
}
wstring CKKImageDoc::GetShutterCount()
{
	wstring wstrShutterCount;
	if (m_KKImageInfo)
	{
		wstrShutterCount = m_KKImageInfo->GetShutterCount();
	}
	return wstrShutterCount;
}

void CKKImageDoc::SetShutterSpeed(wstring& wstrLensType)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetShutterSpeed(wstrLensType);
	}
}
wstring CKKImageDoc::GetShutterSpeed()
{
	wstring wstrLensType;
	if (m_KKImageInfo)
	{
		wstrLensType = m_KKImageInfo->GetShutterSpeed();
	}
	return wstrLensType;
}
void CKKImageDoc::SetFNumber(wstring& wstrFNumber)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetFNumber(wstrFNumber);
	}
}
wstring CKKImageDoc::GetFNumber()
{
	wstring wstrFNumber;
	if (m_KKImageInfo)
	{
		wstrFNumber = m_KKImageInfo->GetFNumber();
	}
	return wstrFNumber;
}

void CKKImageDoc::SetFocalLength(wstring& wstrFocalLength)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetFocalLength(wstrFocalLength);
	}
}
wstring CKKImageDoc::GetFocalLength()
{
	wstring wstrFocalLength;
	if (m_KKImageInfo)
	{
		wstrFocalLength = m_KKImageInfo->GetFocalLength();
	}
	return wstrFocalLength;
}

void CKKImageDoc::SetISO(wstring& wstrISO)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetISO(wstrISO);
	}
}
wstring CKKImageDoc::GetISO()
{
	wstring wstrISO;
	if (m_KKImageInfo)
	{
		wstrISO = m_KKImageInfo->GetISO();
	}
	return wstrISO;
}

void CKKImageDoc::SetExposureCompensation(wstring& wstrExposureCompensation)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetExposureCompensation(wstrExposureCompensation);
	}
}
wstring CKKImageDoc::GetExposureCompensation()
{
	wstring wstrExposureCompensation;
	if (m_KKImageInfo)
	{
		wstrExposureCompensation = m_KKImageInfo->GetExposureCompensation();
	}
	return wstrExposureCompensation;
}

void CKKImageDoc::SetFlashStatus(int nFlashStatus)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetFlashStatus(nFlashStatus);
	}
}
int CKKImageDoc::GetFlashStatus()
{
	int nFlashStatus = -1;
	if (m_KKImageInfo)
	{
		nFlashStatus = m_KKImageInfo->GetFlashStatus();
	}
	return nFlashStatus;
}

void CKKImageDoc::SetWhiteBalance(wstring& wstrWhiteBalance)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetWhiteBalance(wstrWhiteBalance);
	}
}
wstring CKKImageDoc::GetWhiteBalance()
{
	wstring wstrWhiteBalance;
	if (m_KKImageInfo)
	{
		wstrWhiteBalance = m_KKImageInfo->GetWhiteBalance();
	}
	return wstrWhiteBalance;
}

void CKKImageDoc::SetExposureProgram(wstring& wstrExposureProgram)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetExposureProgram(wstrExposureProgram);
	}
}
wstring CKKImageDoc::GetExposureProgram()
{
	wstring wstrExposureProgram;
	if (m_KKImageInfo)
	{
		wstrExposureProgram = m_KKImageInfo->GetExposureProgram();
	}
	return wstrExposureProgram;
}

void CKKImageDoc::SetMeteringMode(wstring& wstrMeteringMode)
{
	if (m_KKImageInfo)
	{
		m_KKImageInfo->SetMeteringMode(wstrMeteringMode);
	}
}
wstring CKKImageDoc::GetMeteringMode()
{
	wstring wstrMeteringMode;
	if (m_KKImageInfo)
	{
		wstrMeteringMode = m_KKImageInfo->GetMeteringMode();
	}
	return wstrMeteringMode;
}

void CKKImageDoc::ResetRotateAngle()
{
	m_nRotateAngle = 0;
}

int CKKImageDoc::GetRotateAngle()
{
	return m_nRotateAngle;
}

void CKKImageDoc::ResetRotate()
{
	if (m_hAdaptedBitmap)
	{
		m_nRotateAngle = 0 - m_nRotateAngle;
		if (m_nRotateAngle%360 != 0)	// 需要旋转
		{
			XL_BITMAP_HANDLE hNewBitmap = CImageProcessor::RotateImage(m_hAdaptedBitmap, m_nRotateAngle);
			if (hNewBitmap)	// 旋转成功，做替换
			{
				XL_ReleaseBitmap(m_hAdaptedBitmap);
				m_hAdaptedBitmap = hNewBitmap;
				
				if (m_nRotateAngle%180 != 0)
				{
					// 交换长短边
					int nWidth, nHeight;
					GetSrcBitmapSize(nWidth, nHeight);
					m_KKImageInfo->SetImageSize(nHeight, nWidth);
				}
				m_nRotateAngle = 0;
			}
		}
	}
}

void CKKImageDoc::RotateBitmap(int nOffsetAngle)
{
	if (m_hAdaptedBitmap)
	{
		int nWidth, nHeight;
		GetSrcBitmapSize(nWidth, nHeight);
		if (nOffsetAngle < 0)	// 左旋
		{
			XL_BITMAP_HANDLE hNewBitmap = CImageProcessor::ImageLeftRotate(m_hAdaptedBitmap);
			if (hNewBitmap)	// 旋转成功，做替换
			{
				XL_ReleaseBitmap(m_hAdaptedBitmap);
				m_hAdaptedBitmap = hNewBitmap;
				m_KKImageInfo->SetImageSize(nHeight, nWidth);
				m_nRotateAngle += nOffsetAngle;
			}
		}
		else	// 右旋
		{
			XL_BITMAP_HANDLE hNewBitmap = CImageProcessor::ImageRightRotate(m_hAdaptedBitmap);
			if (hNewBitmap)	// 旋转成功，做替换
			{
				XL_ReleaseBitmap(m_hAdaptedBitmap);
				m_hAdaptedBitmap = hNewBitmap;
				m_KKImageInfo->SetImageSize(nHeight, nWidth);
				m_nRotateAngle += nOffsetAngle;
			}
		}
		
	}
	if (m_hSrcBitmap)
	{
		if (nOffsetAngle < 0)	// 左旋
		{
			XL_BITMAP_HANDLE hNewBitmap = CImageProcessor::ImageLeftRotate(m_hSrcBitmap);
			if (hNewBitmap)	// 旋转成功，做替换
			{
				XL_ReleaseBitmap(m_hSrcBitmap);
				m_hSrcBitmap = hNewBitmap;
			}
		}
		else	// 右旋
		{
			XL_BITMAP_HANDLE hNewBitmap = CImageProcessor::ImageRightRotate(m_hSrcBitmap);
			if (hNewBitmap)	// 旋转成功，做替换
			{
				XL_ReleaseBitmap(m_hSrcBitmap);
				m_hSrcBitmap = hNewBitmap;
			}
		}
	}
}