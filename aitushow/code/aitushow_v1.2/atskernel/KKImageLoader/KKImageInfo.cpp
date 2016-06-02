#include "stdafx.h"
#include "KKImageInfo.h"

CKKImageInfo::CKKImageInfo()
{
	m_strFilePath = _T("");
	m_nFileSize = 0;
	m_strFileName = _T("Î´ÃüÃû.JPG");
	m_strCreateTime = _T("");
	m_strLastEditTime = _T("");
	m_nWidth = 1;
	m_nHeight = 1;
	m_nExifInfoStatus = -1;
	m_nFlashStatus = -1;
}

CKKImageInfo::~CKKImageInfo()
{
}

void CKKImageInfo::Init()
{
	m_strFilePath = _T("");
	m_nFileSize = 0;
	m_strFileName = _T("Î´ÃüÃû.JPG");
	m_strCreateTime = _T("");
	m_strLastEditTime = _T("");
}

void CKKImageInfo::SetFilePath(std::wstring strFilePath)
{
	m_strFilePath = strFilePath;
}

std::wstring CKKImageInfo::GetFilePath()
{
	return m_strFilePath;
}

void CKKImageInfo::SetFileSize(DWORD nFileSize)
{
	m_nFileSize = nFileSize;
}

DWORD CKKImageInfo::GetFileSize()
{
	return m_nFileSize;
}

void CKKImageInfo::SetFileName(std::wstring strFileName)
{
	m_strFileName = strFileName;
}

std::wstring CKKImageInfo::GetFileName()
{
	return m_strFileName;
}

void CKKImageInfo::SetCreateTime(std::wstring strCreateTime)
{
	m_strCreateTime = strCreateTime;
}

std::wstring CKKImageInfo::GetCreateTime()
{
	return m_strCreateTime;
}

void CKKImageInfo::SetLastEditTime(std::wstring strLastEditTime)
{
	m_strLastEditTime = strLastEditTime;
}

std::wstring CKKImageInfo::GetLastEditTime()
{
	return m_strLastEditTime;
}

void CKKImageInfo::SetImageSize(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}
void CKKImageInfo::GetImageSize(int& nWidth, int& nHeight)
{
	nWidth = m_nWidth;
	nHeight = m_nHeight;
}
void CKKImageInfo::SetExifInfoStatus(int nExifInfoStatus)
{
	m_nExifInfoStatus = nExifInfoStatus;
}
int CKKImageInfo::GetExifInfoStatus()
{
	return m_nExifInfoStatus;
}

void CKKImageInfo::SetDateTimeOriginal(wstring& wstrDateTimeOriginal)
{
	m_wstrDateTimeOriginal = wstrDateTimeOriginal;
}
wstring CKKImageInfo::GetDateTimeOriginal()
{
	return m_wstrDateTimeOriginal;
}

void CKKImageInfo::SetMake(wstring& wstrMake)
{
	m_wstrMake = wstrMake;
}
wstring CKKImageInfo::GetMake()
{
	return m_wstrMake;
}

void CKKImageInfo::SetModel(wstring& wstrModel)
{
	m_wstrModel = wstrModel;
}
wstring CKKImageInfo::GetModel()
{
	return m_wstrModel;
}

void CKKImageInfo::SetLensType(wstring& wstrLensType)
{
	m_wstrLensType = wstrLensType;
}
wstring CKKImageInfo::GetLensType()
{
	return m_wstrLensType;
}

void CKKImageInfo::SetShutterCount(wstring& wstrShutterCount)
{
	m_wstrShutterCount = wstrShutterCount;
}
wstring CKKImageInfo::GetShutterCount()
{
	return m_wstrShutterCount;
}

void CKKImageInfo::SetShutterSpeed(wstring& wstrShutterSpeed)
{
	m_wstrShutterSpeed = wstrShutterSpeed;
}
wstring CKKImageInfo::GetShutterSpeed()
{
	return m_wstrShutterSpeed;
}

void CKKImageInfo::SetFNumber(wstring& wstrFNumber)
{
	m_wstrFNumber = wstrFNumber;
}
wstring CKKImageInfo::GetFNumber()
{
	return m_wstrFNumber;
}

void CKKImageInfo::SetFocalLength(wstring& wstrFocalLength)
{
	m_wstrFocalLength = wstrFocalLength;
}
wstring CKKImageInfo::GetFocalLength()
{
	return m_wstrFocalLength;
}

void CKKImageInfo::SetISO(wstring& wstrISO)
{
	m_wstrISO = wstrISO;
}
wstring CKKImageInfo::GetISO()
{
	return m_wstrISO;
}

void CKKImageInfo::SetExposureCompensation(wstring& wstrExposureCompensation)
{
	m_wstrExposureCompensation = wstrExposureCompensation;
}
wstring CKKImageInfo::GetExposureCompensation()
{
	return m_wstrExposureCompensation;
}

void CKKImageInfo::SetFlashStatus(int nFlashStatus)
{
	m_nFlashStatus = nFlashStatus;
}
int CKKImageInfo::GetFlashStatus()
{
	return m_nFlashStatus;
}


void CKKImageInfo::SetWhiteBalance(wstring& wstrWhiteBalance)
{
	m_wstrWhiteBalance = wstrWhiteBalance;
}
wstring CKKImageInfo::GetWhiteBalance()
{
	return m_wstrWhiteBalance;
}

void CKKImageInfo::SetExposureProgram(wstring& wstrExposureProgram)
{
	m_wstrExposureProgram = wstrExposureProgram;
}
wstring CKKImageInfo::GetExposureProgram()
{
	return m_wstrExposureProgram;
}

void CKKImageInfo::SetMeteringMode(wstring& wstrMeteringMode)
{
	m_wstrMeteringMode = wstrMeteringMode;
}
wstring CKKImageInfo::GetMeteringMode()
{
	return m_wstrMeteringMode;
}