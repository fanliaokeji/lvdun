#include "StdAfx.h"
#include "ImageLoaderImpl.h"


CImageLoaderImpl::CImageLoaderImpl(void)
{
	m_KKImageEXIF = NULL;
}

CImageLoaderImpl::~CImageLoaderImpl(void)
{
	if (m_KKImageEXIF)
	{
		delete m_KKImageEXIF;
		m_KKImageEXIF = NULL;
	}
}

int CImageLoaderImpl::GetExifInfoStatus()
{
	return 0;
}

XL_BITMAP_HANDLE CImageLoaderImpl::GetXLBitmap()
{
	return NULL;
}
XLGP_GIF_HANDLE CImageLoaderImpl::GetXLGifObj()
{
	return NULL;
}

BOOL CImageLoaderImpl::AutoRotate()
{
	return TRUE;
}

BOOL CImageLoaderImpl::AdobeRGBtosRGB(fipWinImage& _fipWinImage, wstring wstrMonitorICCFilePath)
{
	TSAUTO();

	BYTE* pixels = _fipWinImage.accessPixels();

	unsigned int nWidth = _fipWinImage.getWidth();
	unsigned int nHeight = _fipWinImage.getHeight();
	unsigned int nScanWidth = _fipWinImage.getScanWidth();
	unsigned int nBPP = _fipWinImage.getBitsPerPixel();
	if (nBPP != 24)
	{
		TSERROR4CXX("the image file must be 24 bit!");
		return FALSE;
	}
	TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);

	BYTE* CurPixel = pixels;

	//第二种方法，调用开源库lcms，利用不同颜色空间的icc文件来实现转换，速度较快
	cmsHPROFILE hInProfile, hOutProfile;
	cmsHTRANSFORM hTransfrom;
	FILE * fInProfile = NULL;
	

	//先尝试读取图片自带的icc文件
	FIICCPROFILE* pImageICC = FreeImage_GetICCProfile(_fipWinImage);
	if (pImageICC->data)
	{
		TSINFO4CXX("Load ICC successful!");
		hInProfile = cmsOpenProfileFromMem(pImageICC->data, pImageICC->size);
	}
	else
	{
		std::wstring ICCPath;
		if (!GetICCPath(ICCPath))
		{
			TSERROR4CXX("Get icc path failed!");
			return FALSE;
		}
		std::wstring AdobeRGB1998IccPath = ICCPath + _T("\\AdobeRGB1998.icc");
		TSINFO4CXX("AdobeRGB1998IccPath:"<<AdobeRGB1998IccPath);

		fInProfile = _wfopen(AdobeRGB1998IccPath.c_str(), _T("r"));
		if (!fInProfile)
		{
			TSERROR4CXX("Open AdobeRGB1998.icc failed!");
			return FALSE;
		}
		hInProfile = cmsOpenProfileFromStream(fInProfile, "r");
	}

	if (!hInProfile)
	{

		TSERROR4CXX("Open AdobeRGB ICC Failed!");
		if (fInProfile)
		{
			fclose(fInProfile);
		}
		return FALSE;
	}
	
	// 读取显示器的或者创建默认hOutProfile
	FILE * fMonitorProfile = NULL;
	if (PathFileExists(wstrMonitorICCFilePath.c_str()))
	{
		fMonitorProfile = _wfopen(wstrMonitorICCFilePath.c_str(), _T("rb"));
		if (fMonitorProfile)
		{
			hOutProfile = cmsOpenProfileFromStream(fMonitorProfile, "r");
		}
		else
		{
			hOutProfile = cmsCreate_sRGBProfile();
		}
	}
	else
	{
		hOutProfile = cmsCreate_sRGBProfile();
	}
	if (!hOutProfile)
	{
		TSERROR4CXX("Open sRGB ICC Failed!");
		cmsCloseProfile(hInProfile);
		if (fInProfile)
		{
			fclose(fInProfile);
		}
		return FALSE;
	}

	hTransfrom = cmsCreateTransform(hInProfile, TYPE_BGR_8, hOutProfile, TYPE_BGR_8, INTENT_ABSOLUTE_COLORIMETRIC, 0);
	// 先关闭这些打开的句柄
	cmsCloseProfile(hInProfile);
	cmsCloseProfile(hOutProfile);
	if (fInProfile)
	{
		fclose(fInProfile);
	}
	if (fMonitorProfile)
	{
		fclose(fMonitorProfile);
	}

	if (!hTransfrom)
	{
		TSERROR4CXX("Create Transfrom Failed!");
		return FALSE;
	}

	

	for (unsigned int i = 0; i < nHeight; i++)
	{
		CurPixel = pixels + nScanWidth*i;
		cmsDoTransform(hTransfrom, CurPixel, CurPixel, nWidth);
	}

	cmsDeleteTransform(hTransfrom);

	return TRUE;
}

BOOL CImageLoaderImpl::AdobeRGBtosRGB(fipWinImage& _fipWinImage, XL_BITMAP_HANDLE hXLBitmap)
{
	TSAUTO();

	assert(hXLBitmap);
	if (hXLBitmap == NULL)
	{
		TSERROR4CXX("hXLBitmap is NULL!");
		return FALSE;
	}

	XLBitmapInfo BmpInfo;
	XL_GetBitmapInfo(hXLBitmap, &BmpInfo);

	BYTE* pixels = XL_GetBitmapBuffer(hXLBitmap, 0, 0);

	unsigned int nWidth = BmpInfo.Width;
	unsigned int nHeight = BmpInfo.Height;
	unsigned int nScanWidth = BmpInfo.ScanLineLength;
	unsigned int nBPP = _fipWinImage.getBitsPerPixel();
	TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);

	BYTE* CurPixel = pixels;

	//调用开源库lcms，利用不同颜色空间的icc文件来实现转换，速度较快
	cmsHPROFILE hInProfile, hOutProfile;
	cmsHTRANSFORM hTransfrom;
	FILE * fInProfile = NULL;

	//先尝试读取图片自带的icc文件
	FIICCPROFILE* pImageICC = FreeImage_GetICCProfile(_fipWinImage);
	if (pImageICC->data)
	{
		TSINFO4CXX("Load ICC successful!");
		hInProfile = cmsOpenProfileFromMem(pImageICC->data, pImageICC->size);
	}
	else
	{
		std::wstring ICCPath;
		if (!GetICCPath(ICCPath))
		{
			TSERROR4CXX("Get icc path failed!");
			return FALSE;
		}
		std::wstring AdobeRGB1998IccPath = ICCPath + _T("\\AdobeRGB1998.icc");
		TSINFO4CXX("AdobeRGB1998IccPath:"<<AdobeRGB1998IccPath);

		fInProfile = _wfopen(AdobeRGB1998IccPath.c_str(), _T("r"));
		if (!fInProfile)
		{
			TSERROR4CXX("Open AdobeRGB1998.icc failed!");
			return FALSE;
		}

		hInProfile = cmsOpenProfileFromStream(fInProfile, "r");
	}

	if (!hInProfile)
	{
		if (fInProfile)
		{
			fclose(fInProfile);
		}
		TSERROR4CXX("Open AdobeRGB ICC Failed!");
		return FALSE;
	}

	hOutProfile = cmsCreate_sRGBProfile();
	if (!hOutProfile)
	{
		cmsCloseProfile(hInProfile);
		if (fInProfile)
		{
			fclose(fInProfile);
		}
		TSERROR4CXX("Open sRGB ICC Failed!");
		return FALSE;
	}

	hTransfrom = cmsCreateTransform(hInProfile, TYPE_BGRA_8, hOutProfile, TYPE_BGRA_8, INTENT_ABSOLUTE_COLORIMETRIC, 0);
	if (!hTransfrom)
	{
		cmsCloseProfile(hInProfile);
		cmsCloseProfile(hOutProfile);
		if (fInProfile)
		{
			fclose(fInProfile);
		}
		TSERROR4CXX("Create Transfrom Failed!");
		return FALSE;
	}

	cmsCloseProfile(hInProfile);
	cmsCloseProfile(hOutProfile);
	if (fInProfile)
	{
		fclose(fInProfile);
	}

	for (unsigned int i = 0; i < nHeight; i++)
	{
		CurPixel = pixels + nScanWidth*i;
		cmsDoTransform(hTransfrom, CurPixel, CurPixel, nWidth);
	}

	cmsDeleteTransform(hTransfrom);

	return TRUE;
}

BOOL CImageLoaderImpl::AdobeRGBtosRGB(const void * MemPtr, cmsUInt32Number dwSize, XL_BITMAP_HANDLE hXLBitmap, wstring wstrMonitorICCFilePath)
{
	TSAUTO();

	assert(hXLBitmap);
	if (hXLBitmap == NULL)
	{
		TSERROR4CXX("hXLBitmap is NULL!");
		return FALSE;
	}

	XLBitmapInfo BmpInfo;
	XL_GetBitmapInfo(hXLBitmap, &BmpInfo);

	BYTE* pixels = XL_GetBitmapBuffer(hXLBitmap, 0, 0);

	unsigned int nWidth = BmpInfo.Width;
	unsigned int nHeight = BmpInfo.Height;
	unsigned int nScanWidth = BmpInfo.ScanLineLength;
	TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);

	BYTE* CurPixel = pixels;

	//调用开源库lcms，利用不同颜色空间的icc文件来实现转换，速度较快
	cmsHPROFILE hInProfile, hOutProfile;
	cmsHTRANSFORM hTransfrom;
	FILE * fInProfile = NULL;

	if (MemPtr && dwSize > 0)
	{
		hInProfile = cmsOpenProfileFromMem(MemPtr, dwSize);
	}
	else
	{
		std::wstring ICCPath;
		if (!GetICCPath(ICCPath))
		{
			TSERROR4CXX("Get icc path failed!");
			return FALSE;
		}

		std::wstring AdobeRGB1998IccPath = ICCPath + _T("\\AdobeRGB1998.icc");
		fInProfile = _wfopen(AdobeRGB1998IccPath.c_str(), _T("r"));
		if (!fInProfile)
		{
			TSERROR4CXX("Open AdobeRGB1998.icc failed!");
			return FALSE;
		}
		hInProfile = cmsOpenProfileFromStream(fInProfile, "r");
	}

	if (!hInProfile)
	{
		TSERROR4CXX("Open AdobeRGB ICC Failed!");
		if (fInProfile)
		{
			fclose(fInProfile);
		}
		return FALSE;
	}

	// 读取显示器的或者创建默认hOutProfile
	FILE * fMonitorProfile = NULL;
	if (PathFileExists(wstrMonitorICCFilePath.c_str()))
	{
		fMonitorProfile = _wfopen(wstrMonitorICCFilePath.c_str(), _T("rb"));
		if (fMonitorProfile)
		{
			hOutProfile = cmsOpenProfileFromStream(fMonitorProfile, "r");
		}
		else
		{
			hOutProfile = cmsCreate_sRGBProfile();
		}
	}
	else
	{
		hOutProfile = cmsCreate_sRGBProfile();
	}
	if (!hOutProfile)
	{
		cmsCloseProfile(hInProfile);
		if (fInProfile)
		{
			fclose(fInProfile);
		}
		if (fMonitorProfile)
		{
			fclose(fMonitorProfile);
		}
		TSERROR4CXX("Open sRGB ICC Failed!");
		return FALSE;
	}

	hTransfrom = cmsCreateTransform(hInProfile, TYPE_BGRA_8, hOutProfile, TYPE_BGRA_8, INTENT_ABSOLUTE_COLORIMETRIC, 0);
	cmsCloseProfile(hInProfile);
	cmsCloseProfile(hOutProfile);
	if (fInProfile)
	{
		fclose(fInProfile);
	}
	if (fMonitorProfile)
	{
		fclose(fMonitorProfile);
	}
	if (!hTransfrom)
	{
		TSERROR4CXX("Create Transfrom Failed!");
		return FALSE;
	}
	for (unsigned int i = 0; i < nHeight; i++)
	{
		CurPixel = pixels + nScanWidth*i;
		cmsDoTransform(hTransfrom, CurPixel, CurPixel, nWidth);
	}

	cmsDeleteTransform(hTransfrom);

	return TRUE;
}

BOOL CImageLoaderImpl::GetICCPath(std::wstring& ICCPath)
{
	TSAUTO();

	wchar_t wszModulePath[MAX_PATH];
	::GetModuleFileNameW(NULL, wszModulePath, MAX_PATH);
	TSINFO4CXX("ModulePath:"<<wszModulePath);

	::PathAppend(wszModulePath, L"..\\icc");
	TSINFO4CXX("icc Path:"<<wszModulePath);

	if (!::PathFileExists(wszModulePath))
	{
		TSINFO4CXX("icc directory is not exist!");
		return FALSE;
	}

	ICCPath = wszModulePath;
	return TRUE;
}

XL_BITMAP_HANDLE CImageLoaderImpl::ConvertFipImagetoXLBitmapHandle(fipWinImage& FipImage)
{
	TSAUTO();

	long ImageWidth = FipImage.getWidth();
	long ImageHeight = FipImage.getHeight();
	TSINFO4CXX("ImageWidth:"<<ImageWidth<<" ImageHeight:"<<ImageHeight);

	TSINFO4CXX("FipImage BPP Width:"<<FipImage.getBitsPerPixel());

	XL_BITMAP_HANDLE hXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, ImageWidth, ImageHeight);
	assert(hXLBitmap);
	if (hXLBitmap == NULL)
	{
		TSINFO4CXX("XL_CreateBitmap Failed!");
		return NULL;
	}

	BYTE* lpbyDest = XL_GetBitmapBuffer(hXLBitmap, 0, 0);
	assert(lpbyDest);

	XLBitmapInfo BmpInfo;
	XL_GetBitmapInfo(hXLBitmap, &BmpInfo);
	if (FipImage.getBitsPerPixel() != 32)
	{
		FipImage.convertTo32Bits();
	}	
	for (long i = 0; i < ImageHeight; i++)
	{
		memcpy(lpbyDest + BmpInfo.ScanLineLength * i, FipImage.accessPixels() + BmpInfo.ScanLineLength*(ImageHeight - i - 1), BmpInfo.ScanLineLength);
	}

	return hXLBitmap;
}


BOOL CImageLoaderImpl::sRGBtoMonitorRGB(XL_BITMAP_HANDLE hXLBitmap, wstring wstrMonitorICCFilePath)
{
	TSAUTO();

	assert(hXLBitmap);
	if (hXLBitmap == NULL)
	{
		TSERROR4CXX("hXLBitmap is NULL!");
		return FALSE;
	}
	if (!PathFileExists(wstrMonitorICCFilePath.c_str()))
	{
		TSERROR4CXX("wstrMonitorICCFilePath is invalid!");
		return FALSE;
	}

	XLBitmapInfo BmpInfo;
	XL_GetBitmapInfo(hXLBitmap, &BmpInfo);

	BYTE* pixels = XL_GetBitmapBuffer(hXLBitmap, 0, 0);

	unsigned int nWidth = BmpInfo.Width;
	unsigned int nHeight = BmpInfo.Height;
	unsigned int nScanWidth = BmpInfo.ScanLineLength;
	TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);

	BYTE* CurPixel = pixels;

	//调用开源库lcms，利用不同颜色空间的icc文件来实现转换，速度较快
	cmsHPROFILE hInProfile, hOutProfile;
	cmsHTRANSFORM hTransfrom;
	
	hInProfile = cmsCreate_sRGBProfile();

	if (!hInProfile)
	{
		TSERROR4CXX("Open AdobeRGB ICC Failed!");
		return FALSE;
	}

	// 读取显示器的或者创建默认hOutProfile
	FILE * fMonitorProfile = NULL;
	if (PathFileExists(wstrMonitorICCFilePath.c_str()))
	{
		fMonitorProfile = _wfopen(wstrMonitorICCFilePath.c_str(), _T("rb"));
		if (fMonitorProfile)
		{
			hOutProfile = cmsOpenProfileFromStream(fMonitorProfile, "r");
		}
		else
		{
			hOutProfile = cmsCreate_sRGBProfile();
		}
	}
	else
	{
		hOutProfile = cmsCreate_sRGBProfile();
	}
	if (!hOutProfile)
	{
		if (fMonitorProfile)
		{
			fclose(fMonitorProfile);
		}
		TSERROR4CXX("Open sRGB ICC Failed!");
		return FALSE;
	}

	hTransfrom = cmsCreateTransform(hInProfile, TYPE_BGRA_8, hOutProfile, TYPE_BGRA_8, INTENT_ABSOLUTE_COLORIMETRIC, 0);
	cmsCloseProfile(hInProfile);
	cmsCloseProfile(hOutProfile);
	if (fMonitorProfile)
	{
		fclose(fMonitorProfile);
	}
	if (!hTransfrom)
	{
		TSERROR4CXX("Create Transfrom Failed!");
		return FALSE;
	}
	for (unsigned int i = 0; i < nHeight; i++)
	{
		CurPixel = pixels + nScanWidth*i;
		cmsDoTransform(hTransfrom, CurPixel, CurPixel, nWidth);
	}

	cmsDeleteTransform(hTransfrom);

	return TRUE;
}

void CImageLoaderImpl::sRGBtoMonitorRGB(fipWinImage& _fipWinImage, wstring wstrMonitorICCFilePath)
{
	if (!PathFileExists(wstrMonitorICCFilePath.c_str()))
	{
		return;
	}
	cmsHPROFILE hInProfile, hOutProfile;
	cmsHTRANSFORM hTransfrom;

	hInProfile = cmsCreate_sRGBProfile();
	if (!hInProfile)
	{
		TSERROR4CXX("Open Embedded ICC Failed!");
		return ;
	}

	// 读取显示器的或者创建默认hOutProfile
	FILE * fMonitorProfile = NULL;
	if (PathFileExists(wstrMonitorICCFilePath.c_str()))
	{
		fMonitorProfile = _wfopen(wstrMonitorICCFilePath.c_str(), _T("rb"));
		if (fMonitorProfile)
		{
			hOutProfile = cmsOpenProfileFromStream(fMonitorProfile, "r");
		}
		else
		{
			hOutProfile = cmsCreate_sRGBProfile();
		}
	}
	else
	{
		hOutProfile = cmsCreate_sRGBProfile();
	}

	if (!hOutProfile)
	{
		cmsCloseProfile(hInProfile);
		if (fMonitorProfile)
		{
			fclose(fMonitorProfile);
		}
		TSERROR4CXX("Open sRGB ICC Failed!");
		return ;
	}

	unsigned int nBPP = _fipWinImage.getBitsPerPixel();
	if( nBPP == 24 )
	{
		hTransfrom = cmsCreateTransform(hInProfile, TYPE_BGR_8, hOutProfile, TYPE_BGR_8, INTENT_ABSOLUTE_COLORIMETRIC, 0);
		cmsCloseProfile(hInProfile);
		cmsCloseProfile(hOutProfile);
		if (fMonitorProfile)
		{
			fclose(fMonitorProfile);
		}
		if (!hTransfrom)
		{
			TSERROR4CXX("Create Transfrom Failed!");
			return ;
		}
		BYTE* pixels = _fipWinImage.accessPixels();
		unsigned int nWidth = _fipWinImage.getWidth();
		unsigned int nHeight = _fipWinImage.getHeight();
		unsigned int nScanWidth = _fipWinImage.getScanWidth();
		unsigned int nBPP = _fipWinImage.getBitsPerPixel();
		TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);
		BYTE* CurPixel = pixels;
		for (unsigned int i = 0; i < nHeight; i++)
		{
			CurPixel = pixels + nScanWidth*i;
			cmsDoTransform(hTransfrom, CurPixel, CurPixel, nWidth);
		}
		cmsDeleteTransform(hTransfrom);
	}
	else if( nBPP == 48 )
	{
		_fipWinImage.convertTo32Bits();
		hTransfrom = cmsCreateTransform(hInProfile, TYPE_BGRA_8, hOutProfile, TYPE_BGRA_8, INTENT_ABSOLUTE_COLORIMETRIC, 0);
		cmsCloseProfile(hInProfile);
		cmsCloseProfile(hOutProfile);
		if (fMonitorProfile)
		{
			fclose(fMonitorProfile);
		}
		if (!hTransfrom)
		{
			TSERROR4CXX("Create Transfrom Failed!");
			return ;
		}

		BYTE* pixels = _fipWinImage.accessPixels();
		unsigned int nWidth = _fipWinImage.getWidth();
		unsigned int nHeight = _fipWinImage.getHeight();
		unsigned int nScanWidth = _fipWinImage.getScanWidth();
		unsigned int nBPP = _fipWinImage.getBitsPerPixel();
		TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);
		BYTE* CurPixel = pixels;
		for (unsigned int i = 0; i < nHeight; i++)
		{
			CurPixel = pixels + nScanWidth*i;
			cmsDoTransform(hTransfrom, CurPixel, CurPixel, nWidth);
		}
		cmsDeleteTransform(hTransfrom);
	}
}


BOOL CImageLoaderImpl::CMYKtosRGB(fipWinImage& _fipWinImage, wstring wstrMonitorICCFilePath)
{
	TSAUTO();

	BYTE* pixels = _fipWinImage.accessPixels();

	unsigned int nWidth = _fipWinImage.getWidth();
	unsigned int nHeight = _fipWinImage.getHeight();
	unsigned int nScanWidth = _fipWinImage.getScanWidth();
	unsigned int nBPP = _fipWinImage.getBitsPerPixel();
	if (nBPP != 32)
	{
		TSERROR4CXX("the image file must be 32 bit!");
		return FALSE;
	}
	TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);

	//调用开源库lcms，利用不同颜色空间的icc文件来实现转换，速度较快
	cmsHPROFILE hInProfile, hOutProfile;
	cmsHTRANSFORM hTransfrom;

	std::wstring ICCPath;
	if (!GetICCPath(ICCPath))
	{
		TSERROR4CXX("Get icc path failed!");
		return FALSE;
	}
	FILE * fInProfile = NULL;
	FIICCPROFILE* pImageICC = FreeImage_GetICCProfile(_fipWinImage);
	if (pImageICC->data)
	{
		TSINFO4CXX("Load ICC successful!");
		hInProfile = cmsOpenProfileFromMem(pImageICC->data, pImageICC->size);
	}
	else
	{
		std::wstring CMYKPath = ICCPath + _T("\\USWebCoatedSWOP.icc");
		TSINFO4CXX("CMYKPath:"<<CMYKPath);

		fInProfile = _wfopen(CMYKPath.c_str(), _T("rb"));
		if (!fInProfile)
		{
			TSERROR4CXX("Open USWebCoatedSWOP.icc failed!");
			return FALSE;
		}
		hInProfile = cmsOpenProfileFromStream(fInProfile, "r");
		if (!hInProfile)
		{
			fclose(fInProfile);
		}
	}

	if (!hInProfile)
	{
		TSERROR4CXX("Open Embedded ICC Failed!");
		return FALSE;
	}
	
	// 读取显示器的或者创建默认hOutProfile
	FILE * fMonitorProfile = NULL;
	if (PathFileExists(wstrMonitorICCFilePath.c_str()))
	{
		fMonitorProfile = _wfopen(wstrMonitorICCFilePath.c_str(), _T("rb"));
		if (fMonitorProfile)
		{
			hOutProfile = cmsOpenProfileFromStream(fMonitorProfile, "r");
		}
		else
		{
			hOutProfile = cmsCreate_sRGBProfile();
		}
	}
	else
	{
		hOutProfile = cmsCreate_sRGBProfile();
	}
	
	if (!hOutProfile)
	{
		cmsCloseProfile(hInProfile);
		if (fMonitorProfile)
		{
			fclose(fMonitorProfile);
		}
		TSERROR4CXX("Create hOutProfile Failed!");
		return FALSE;
	}

	hTransfrom = cmsCreateTransform(hInProfile, TYPE_CMYK_8, hOutProfile, TYPE_BGRA_8, INTENT_PERCEPTUAL, 0);
	cmsCloseProfile(hInProfile);
	cmsCloseProfile(hOutProfile);
	if( fInProfile != NULL )
		fclose(fInProfile);
	if( fMonitorProfile != NULL )
		fclose(fMonitorProfile);
	if (!hTransfrom)
	{
		TSERROR4CXX("Create Transfrom Failed!");
		return FALSE;
	}

	//BYTE* CurPixel = new BYTE[nScanWidth];
	FIBITMAP * new_fbitmap = FreeImage_Allocate( nWidth, nHeight, 32, 0X00FF0000, 0X0000FF00, 0X000000FF );
	if( new_fbitmap == NULL )
		return FALSE;
	BYTE* obj_pixels = FreeImage_GetBits( new_fbitmap );
	unsigned int obj_nScanWidth = FreeImage_GetPitch(new_fbitmap);
	for (unsigned int i = 0; i < nHeight; i++)
	{
		BYTE* cur_line = pixels + nScanWidth*i;
		BYTE* cur_obj_line = obj_pixels + obj_nScanWidth*i;
		cmsDoTransform(hTransfrom, cur_line, cur_obj_line , nWidth);


		for( int j=3; j<nScanWidth; j+=4 )
		{
			cur_obj_line[j] = 255;
		}
	}


	//delete []CurPixel;
	cmsDeleteTransform(hTransfrom);
	(fipImage&)(_fipWinImage) = new_fbitmap;

	return TRUE;
}
BOOL CImageLoaderImpl::CMYKtosRGB(fipWinImage& _fipWinImage, XL_BITMAP_HANDLE hXLBitmap)
{
	TSAUTO();

	assert(hXLBitmap);
	if (hXLBitmap == NULL)
	{
		TSERROR4CXX("hXLBitmap is NULL!");
		return FALSE;
	}

	XLBitmapInfo BmpInfo;
	XL_GetBitmapInfo(hXLBitmap, &BmpInfo);

	BYTE* pixels = XL_GetBitmapBuffer(hXLBitmap, 0, 0);

	unsigned int nWidth = BmpInfo.Width;
	unsigned int nHeight = BmpInfo.Height;
	unsigned int nScanWidth = BmpInfo.ScanLineLength;
	//unsigned int nBPP = _fipWinImage.getBitsPerPixel();
	TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);

	//调用开源库lcms，利用不同颜色空间的icc文件来实现转换，速度较快
	cmsHPROFILE hInProfile, hOutProfile;
	cmsHTRANSFORM hTransfrom;

	std::wstring ICCPath;
	if (!GetICCPath(ICCPath))
	{
		TSERROR4CXX("Get icc path failed!");
		return FALSE;
	}
	FILE * fInProfile = NULL;
	FIICCPROFILE* pImageICC = FreeImage_GetICCProfile(_fipWinImage);
	if (pImageICC->data)
	{
		TSINFO4CXX("Load ICC successful!");
		hInProfile = cmsOpenProfileFromMem(pImageICC->data, pImageICC->size);
	}
	else
	{
		std::wstring CMYKPath = ICCPath + _T("\\USWebCoatedSWOP.icc");
		TSINFO4CXX("CMYKPath:"<<CMYKPath);

		fInProfile = _wfopen(CMYKPath.c_str(), _T("rb"));
		if (!fInProfile)
		{
			TSERROR4CXX("Open USWebCoatedSWOP.icc failed!");
			return FALSE;
		}
		hInProfile = cmsOpenProfileFromStream(fInProfile, "r");
	}

	if (!hInProfile)
	{
		TSERROR4CXX("Open Embedded ICC Failed!");
		return FALSE;
	}

	hOutProfile = cmsCreate_sRGBProfile();
	if (!hOutProfile)
	{
		TSERROR4CXX("Open sRGB ICC Failed!");
		return FALSE;
	}

	hTransfrom = cmsCreateTransform(hInProfile, TYPE_CMYK_8, hOutProfile, TYPE_BGRA_8, INTENT_PERCEPTUAL, 0);
	if (!hTransfrom)
	{
		TSERROR4CXX("Create Transfrom Failed!");
		return FALSE;
	}

	cmsCloseProfile(hInProfile);
	cmsCloseProfile(hOutProfile);
	if( fInProfile != NULL )
		fclose(fInProfile);

	BYTE* CurPixel = new BYTE[nScanWidth];
	for (unsigned int i = 0; i < nHeight; i++)
	{
		BYTE* cur_line = pixels + nScanWidth*i;
		::memcpy( CurPixel,cur_line, nScanWidth);
		cmsDoTransform(hTransfrom, CurPixel, cur_line, nWidth);
		for( int j=3; j<nScanWidth; j+=4 )
		{
			cur_line[j] = 255;
		}
	}
	delete []CurPixel;
	cmsDeleteTransform(hTransfrom);

	return TRUE;
}

BOOL CImageLoaderImpl::CMYKtosRGB(const void * IccDataPtr, cmsUInt32Number dwIccDataSize, XL_BITMAP_HANDLE hXLBitmap, wstring wstrMonitorICCFilePath)
{
	DWORD nStartTime = GetTickCount();
	assert(hXLBitmap);
	if (hXLBitmap == NULL)
	{
		TSERROR4CXX("hXLBitmap or IccDataPtr is NULL!");
		return FALSE;
	}

	XLBitmapInfo BmpInfo;
	XL_GetBitmapInfo(hXLBitmap, &BmpInfo);

	BYTE* pixels = XL_GetBitmapBuffer(hXLBitmap, 0, 0);

	unsigned int nWidth = BmpInfo.Width;
	unsigned int nHeight = BmpInfo.Height;
	unsigned int nScanWidth = BmpInfo.ScanLineLength;
	TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);

	//调用开源库lcms，利用不同颜色空间的icc文件来实现转换，速度较快
	cmsHPROFILE hInProfile, hOutProfile;
	cmsHTRANSFORM hTransfrom;
	FILE * fInProfile = NULL;
	
	if (IccDataPtr && dwIccDataSize != 0)	// 用内嵌的
	{
		TSINFO4CXX("Load ICC from buffer successful!");
		hInProfile = cmsOpenProfileFromMem(IccDataPtr, dwIccDataSize);
	}
	else	// 用程序默认的
	{
		std::wstring ICCPath;
		if (!GetICCPath(ICCPath))
		{
			TSERROR4CXX("Get icc path failed!");
			return FALSE;
		}

		std::wstring CMYKPath = ICCPath + _T("\\USWebCoatedSWOP.icc");
		TSINFO4CXX("CMYKPath:"<<CMYKPath);

		fInProfile = _wfopen(CMYKPath.c_str(), _T("rb"));
		if (!fInProfile)
		{
			TSERROR4CXX("Open USWebCoatedSWOP.icc failed!");
			return FALSE;
		}
		hInProfile = cmsOpenProfileFromStream(fInProfile, "r");
	}

	if (!hInProfile)
	{
		if (fInProfile)
		{
			fclose(fInProfile);
		}
		TSERROR4CXX("Open Embedded ICC Failed!");
		return FALSE;
	}
	
	// 读取显示器的或者创建默认hOutProfile
	FILE * fMonitorProfile = NULL;
	if (PathFileExists(wstrMonitorICCFilePath.c_str()))
	{
		fMonitorProfile = _wfopen(wstrMonitorICCFilePath.c_str(), _T("rb"));
		if (fMonitorProfile)
		{
			hOutProfile = cmsOpenProfileFromStream(fMonitorProfile, "r");
		}
		else
		{
			hOutProfile = cmsCreate_sRGBProfile();
		}
	}
	else
	{
		hOutProfile = cmsCreate_sRGBProfile();
	}
	if (!hOutProfile)
	{
		TSERROR4CXX("Open sRGB ICC Failed!");
		if (fInProfile)
		{
			fclose(fInProfile);
		}
		if (fMonitorProfile)
		{
			fclose(fMonitorProfile);
		}
		cmsCloseProfile(hInProfile);
		return FALSE;
	}

	hTransfrom = cmsCreateTransform(hInProfile, TYPE_CMYK_8, hOutProfile, TYPE_BGRA_8, INTENT_PERCEPTUAL, 0);
	cmsCloseProfile(hInProfile);
	cmsCloseProfile(hOutProfile);
	if (fInProfile)
	{
		fclose(fInProfile);
	}
	if (fMonitorProfile)
	{
		fclose(fMonitorProfile);
	}
	if (!hTransfrom)
	{
		return FALSE;
	}

	BYTE* CurPixel = new BYTE[nScanWidth];
	for (unsigned int i = 0; i < nHeight; i++)
	{
		BYTE* cur_line = pixels + nScanWidth*i;
		::memcpy( CurPixel,cur_line, nScanWidth);
		cmsDoTransform(hTransfrom, CurPixel, cur_line, nWidth);
		for( int j=3; j<nScanWidth; j+=4 )
		{
			cur_line[j] = 255;
		}
	}
	delete []CurPixel;
	cmsDeleteTransform(hTransfrom);
	DWORD nEndTime = GetTickCount();
	TSINFO4CXX("CMYKtosRGB consume time:" << nEndTime-nStartTime);
	return TRUE;
}

void CImageLoaderImpl::ApplyICC(fipWinImage& _fipWinImage, wstring wstrMonitorICCFilePath)
{
	TSAUTO();

	cmsHPROFILE hInProfile, hOutProfile;
	cmsHTRANSFORM hTransfrom;

	FILE * fInProfile = NULL;
	FIICCPROFILE* pImageICC = FreeImage_GetICCProfile(_fipWinImage);
	if (pImageICC->data)
	{
		TSINFO4CXX("Load ICC successful!");
		hInProfile = cmsOpenProfileFromMem(pImageICC->data, pImageICC->size);
	}
	else
	{
		return;
	}

	if (!hInProfile)
	{
		TSERROR4CXX("Open Embedded ICC Failed!");
		return ;
	}
	
	// 读取显示器的或者创建默认hOutProfile
	FILE * fMonitorProfile = NULL;
	if (PathFileExists(wstrMonitorICCFilePath.c_str()))
	{
		fMonitorProfile = _wfopen(wstrMonitorICCFilePath.c_str(), _T("rb"));
		if (fMonitorProfile)
		{
			hOutProfile = cmsOpenProfileFromStream(fMonitorProfile, "r");
		}
		else
		{
			hOutProfile = cmsCreate_sRGBProfile();
		}
	}
	else
	{
		hOutProfile = cmsCreate_sRGBProfile();
	}

	if (!hOutProfile)
	{
		cmsCloseProfile(hInProfile);
		if (fMonitorProfile)
		{
			fclose(fMonitorProfile);
		}
		TSERROR4CXX("Open sRGB ICC Failed!");
		return ;
	}

	unsigned int nBPP = _fipWinImage.getBitsPerPixel();
	if( nBPP == 24 )
	{
		hTransfrom = cmsCreateTransform(hInProfile, TYPE_BGR_8, hOutProfile, TYPE_BGR_8, INTENT_SATURATION, 0);
		cmsCloseProfile(hInProfile);
		cmsCloseProfile(hOutProfile);
		if (fMonitorProfile)
		{
			fclose(fMonitorProfile);
		}
		if (!hTransfrom)
		{
			TSERROR4CXX("Create Transfrom Failed!");
			return ;
		}
		BYTE* pixels = _fipWinImage.accessPixels();
		unsigned int nWidth = _fipWinImage.getWidth();
		unsigned int nHeight = _fipWinImage.getHeight();
		unsigned int nScanWidth = _fipWinImage.getScanWidth();
		unsigned int nBPP = _fipWinImage.getBitsPerPixel();
		TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);
		BYTE* CurPixel = pixels;
		for (unsigned int i = 0; i < nHeight; i++)
		{
			CurPixel = pixels + nScanWidth*i;
			cmsDoTransform(hTransfrom, CurPixel, CurPixel, nWidth);
		}
		cmsDeleteTransform(hTransfrom);
	}
	else if( nBPP == 48 )
	{
		_fipWinImage.convertTo32Bits();
		hTransfrom = cmsCreateTransform(hInProfile, TYPE_BGRA_8, hOutProfile, TYPE_BGRA_8, INTENT_PERCEPTUAL, 0);
		cmsCloseProfile(hInProfile);
		cmsCloseProfile(hOutProfile);
		if (fMonitorProfile)
		{
			fclose(fMonitorProfile);
		}
		if (!hTransfrom)
		{
			TSERROR4CXX("Create Transfrom Failed!");
			return ;
		}

		BYTE* pixels = _fipWinImage.accessPixels();
		unsigned int nWidth = _fipWinImage.getWidth();
		unsigned int nHeight = _fipWinImage.getHeight();
		unsigned int nScanWidth = _fipWinImage.getScanWidth();
		unsigned int nBPP = _fipWinImage.getBitsPerPixel();
		TSINFO4CXX("Width:"<<nWidth<<", Height:"<<nHeight<<", ScanWidth:"<<nScanWidth);
		BYTE* CurPixel = pixels;
		for (unsigned int i = 0; i < nHeight; i++)
		{
			CurPixel = pixels + nScanWidth*i;
			cmsDoTransform(hTransfrom, CurPixel, CurPixel, nWidth);
		}
		cmsDeleteTransform(hTransfrom);
	}
	else
		return ;
}
