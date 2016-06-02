#include "StdAfx.h"
#include "ImageLoader.h"
#include "JpegLoaderImpl.h"
#include "GifLoaderImpl.h"
#include "WmfLoaderImpl.h"
#include "PsdLoaderImpl.h"
#include "DefultLoaderImpl.h"
#include "..\ImageHelper\exif_thumbnail_length.h"
#include "..\Utility\StringOperation.h"

CImageLoader::CImageLoader(void)
{
	m_LoaderImpl = NULL;
	m_KKImageInfo = NULL;
	m_nType = KKImg_Type_InValid;

}

CImageLoader::~CImageLoader(void)
{
	if (m_LoaderImpl)
	{
		delete m_LoaderImpl;
		m_LoaderImpl = NULL;
	}
	if (m_KKImageInfo)
	{
		delete m_KKImageInfo;
		m_KKImageInfo = NULL;
	}
}

int CImageLoader::LoadImage(const wstring& wstrFilePath, bool* pbStop, bool bScale, int nWidth, int nHeight, bool bAutoRotate)
{
	TSAUTO();
	TSINFO4CXX("加载的文件路径是" << wstrFilePath.c_str());
	int nRet = 0;
	if (wstrFilePath.empty())
	{
		nRet = 1;
		TSINFO4CXX("wstrFilePath is empty and return " << nRet);
		return nRet;
	}
	if (!PathFileExists(wstrFilePath.c_str()))
	{
		nRet = 2;
		TSINFO4CXX("File is not Exists and return "<<nRet);
		return nRet;
	}
	// 释放掉上次的加载类
	if (m_LoaderImpl)
	{
		delete m_LoaderImpl;
		m_LoaderImpl = NULL;
	}
	// 判断文件类型
	// 获取后缀名
	wstring wstrExtName = PathFindExtensionW(wstrFilePath.c_str());
	// 转换成小写
	wstring lowerExtName = ultra::ToLower(wstrExtName);
	if (lowerExtName == L".jpg" || lowerExtName == L".jpeg" || lowerExtName == L".jpe")
	{
		m_nType = KKImg_Type_Jpeg;
		m_LoaderImpl = new CJpegLoaderImpl();
	}
	else if (lowerExtName == L".gif")
	{
		m_nType = KKImg_Type_Gif;
		m_LoaderImpl = new CGifLoaderImpl();
	}
	else if (lowerExtName == L".wmf")
	{
		m_nType = KKImg_Type_Default;
		m_LoaderImpl = new CWmfLoaderImpl();
	}
	else
	{
		if (lowerExtName == L".png")
		{
			m_nType = KKImg_Type_Png;
		}
		else
		{
			m_nType = KKImg_Type_Default;
		}
		m_LoaderImpl = new CDefaultLoaderImpl();
	}
	// 加载位图
	nRet = m_LoaderImpl->LoadImage(wstrFilePath, pbStop, bScale, nWidth, nHeight);

	if (nRet != 0 && m_nType == KKImg_Type_Jpeg)	// 后缀名不对，重新加载
	{
		m_nType = KKImg_Type_Default;
		delete m_LoaderImpl;
		m_LoaderImpl = new CDefaultLoaderImpl();
		nRet = m_LoaderImpl->LoadImage(wstrFilePath, pbStop, bScale, nWidth, nHeight);
	}
	
	// 获取图像信息
	if (nRet == 0)
	{
		// 判断是否自动转正
		if(bAutoRotate)
		{
			m_LoaderImpl->AutoRotate();
		}
		m_KKImageInfo = new CKKImageInfo();
		// 设置路径
		m_KKImageInfo->SetFilePath(wstrFilePath);
		// 设置文件名
		m_KKImageInfo->SetFileName(::PathFindFileName(wstrFilePath.c_str()));
		// 设置图片尺寸
		int nWidth, nHeight;
		m_LoaderImpl->GetSrcBitmapSize(nWidth, nHeight);
		m_KKImageInfo->SetImageSize(nWidth, nHeight);
		// 设置Exif标志
		m_KKImageInfo->SetExifInfoStatus(m_LoaderImpl->GetExifInfoStatus());
		
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile(wstrFilePath.c_str(), &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			//获取文件大小(字节为单位）
			DWORD nFileSize = (FindFileData.nFileSizeHigh * MAXDWORD + 1) + FindFileData.nFileSizeLow;
			m_KKImageInfo->SetFileSize(nFileSize);

			//获取文件创建时间
			SYSTEMTIME stUTC, stLocal;
			FileTimeToSystemTime(&(FindFileData.ftCreationTime), &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			TCHAR szCreationTime[100] = {0};
			wsprintf(szCreationTime, _T("%d/%d/%d %d:%d"), stLocal.wYear, stLocal.wMonth, stLocal. wDay, stLocal.wHour, stLocal.wMinute);
			m_KKImageInfo->SetCreateTime(szCreationTime);

			//获取文件最后修改时间
			FileTimeToSystemTime(&(FindFileData.ftLastWriteTime), &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			TCHAR szLastWriteTime[100] = {0};
			wsprintf(szLastWriteTime, _T("%d/%d/%d %d:%d"), stLocal.wYear, stLocal.wMonth, stLocal. wDay, stLocal.wHour, stLocal.wMinute);
			m_KKImageInfo->SetLastEditTime(szLastWriteTime);
			FindClose(hFind);
		}
	}
	return nRet;
}
ImageLoaderImplType CImageLoader::GetLoaderType()
{
	return m_nType;
}

XL_BITMAP_HANDLE CImageLoader::GetXLBitmap()
{
	if (m_LoaderImpl)
	{
		return m_LoaderImpl->GetXLBitmap();
	}
	return NULL;
}

CKKImageEXIF* CImageLoader::GetExifInfo(bool bReset)
{
	return NULL;
}
CKKImageInfo* CImageLoader::GetImageInfo(bool bReset)
{
	CKKImageInfo* pTemp = m_KKImageInfo;
	if (bReset)
	{
		m_KKImageInfo = NULL;
	}
	return pTemp;
}

XLGP_GIF_HANDLE CImageLoader::GetXLGifObj()
{
	if (m_nType != KKImg_Type_Gif)
	{
		return NULL;
	}
	if (m_LoaderImpl)
	{
		return m_LoaderImpl->GetXLGifObj();
	}
	return NULL;
}
//SaveImage不负责hSaveBitmap资源的释放
BOOL CImageLoader::SaveImage(XL_BITMAP_HANDLE hSaveBitmap, LPCTSTR lpszPathName, CKKImageEXIF* pKKImageEXIF, BOOL IsDelExif, int nJPEGQuality, BOOL IsHighQuality, int nDPI)
{
	TSAUTO();

	XLBitmapInfo SaveBmpInfo;
	XL_GetBitmapInfo(hSaveBitmap, &SaveBmpInfo);

	XL_BITMAP_HANDLE hDstBitmap = NULL;

	FREE_IMAGE_FORMAT fif = fipImage::identifyFIFU(lpszPathName);
	if (fif == FIF_UNKNOWN)
	{
		TSERROR4CXX(_T("文件保存格式错误！"));
		return FALSE;
	}

	if (fif != FIF_PNG)
	{
		//如果不是PNG，那么先和纯白色的底进行混合，再保存
		//这里的hSaveBitmap是Doc中复制过的，所以是可以修改的		
		hDstBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, SaveBmpInfo.Width, SaveBmpInfo.Height);
		assert(hDstBitmap);
		if (hDstBitmap == NULL)
		{
			TSERROR4CXX("Create hDstBitmap Failed!");
			return FALSE;
		}
		XL_FillBitmap(hDstBitmap, XLCOLOR_BGRA(255, 255, 255, 255));
		XL_PreMultiplyBitmap(hSaveBitmap); //混合前要预乘
		XLGP_SrcAlphaBlend(hDstBitmap, 0, 0, hSaveBitmap, 255);
	}

	fipWinImage _fipWinImage;
	if (fif != FIF_PNG)
	{
		//不是PNG，转化成24位的
		//注意，Freeimage中像素是从下到上，而XL_BITMAP_HANDLE中是从上到下，所以拷贝像素的时候
		_fipWinImage.setSize(FIT_BITMAP, SaveBmpInfo.Width, SaveBmpInfo.Height, 24);
		unsigned nFipScanLength = _fipWinImage.getScanWidth();
		BYTE* FipBuf = _fipWinImage.accessPixels();
		BYTE* SaveBitmapBuf = XL_GetBitmapBuffer(hDstBitmap, 0, 0);
		for (int j = 0; j < SaveBmpInfo.Height; j++)
		{
			BYTE* FipLine = FipBuf + nFipScanLength*j;
			BYTE* SaveBitmapLine = SaveBitmapBuf + SaveBmpInfo.ScanLineLength*(SaveBmpInfo.Height - 1 -j);
			for (int i = 0; i < SaveBmpInfo.Width; i++)
			{
				BYTE* FipPixel = FipLine + 3*i;
				BYTE* SaveBitmapPixel = SaveBitmapLine + 4*i;
				FipPixel[0] = SaveBitmapPixel[0];
				FipPixel[1] = SaveBitmapPixel[1];
				FipPixel[2] = SaveBitmapPixel[2];
			}
		}

		XL_ReleaseBitmap(hDstBitmap);
	}
	else
	{
		//PNG转化成32位的
		_fipWinImage.setSize(FIT_BITMAP, SaveBmpInfo.Width, SaveBmpInfo.Height, 32);
		unsigned nFipScanLength = _fipWinImage.getScanWidth();
		BYTE* FipBuf = _fipWinImage.accessPixels();
		BYTE* SaveBitmapBuf = XL_GetBitmapBuffer(hSaveBitmap, 0, 0);
		for (int j = 0; j < SaveBmpInfo.Height; j++)
		{
			BYTE* FipLine = FipBuf + nFipScanLength*j;
			BYTE* SaveBitmapLine = SaveBitmapBuf + SaveBmpInfo.ScanLineLength*(SaveBmpInfo.Height - 1 -j);
			for (int i = 0; i < SaveBmpInfo.Width; i++)
			{
				BYTE* FipPixel = FipLine + 4*i;
				BYTE* SaveBitmapPixel = SaveBitmapLine + 4*i;
				FipPixel[0] = SaveBitmapPixel[0];
				FipPixel[1] = SaveBitmapPixel[1];
				FipPixel[2] = SaveBitmapPixel[2];
				FipPixel[3] = SaveBitmapPixel[3];
			}
		}
	}

	if (fif == FIF_JPEG)
	{
		TSINFO4CXX("Save Type is JPEG");
		if (IsDelExif == FALSE)
		{
			//这里保存Exif信息
			if (pKKImageEXIF != NULL)
			{
				pKKImageEXIF->SetImageEXIF(&_fipWinImage);
			}
		}
	}
	else if (fif == FIF_GIF)
	{
		TSINFO4CXX("保存类型是GIF！");
		//转换成gif之前，需要先将位图转换成8bit的
		if (!_fipWinImage.colorQuantize(FIQ_WUQUANT))
		{
			TSINFO4CXX(_T("转换成8bit位图失败！"));
			return FALSE;
		}
	}

	// 确保文件目录存在
	wchar_t szFilePath[MAX_PATH];
	swprintf(szFilePath, L"%s", lpszPathName);
	::PathRemoveFileSpec(szFilePath);
	// 如果父目录不存在的话重新创建新的目录
	if (!::PathFileExists(szFilePath))
	{
		int nRet = SHCreateDirectoryEx(NULL,szFilePath,NULL);
		if (nRet != ERROR_SUCCESS && nRet != ERROR_FILE_EXISTS && nRet != ERROR_ALREADY_EXISTS)
		{
			return FALSE;
		}
	}

	if (fif == FIF_JPEG)
	{
		TSINFO4CXX(L"nJPEGQuality = " << nJPEGQuality);

		DWORD flag = 0;
		if (IsHighQuality == TRUE)
		{
			flag = JPEG_SUBSAMPLING_444;
		}
		TSINFO4CXX("lpszPathName:"<<lpszPathName);
		if (!_fipWinImage.saveU(lpszPathName, nJPEGQuality | flag))
		{
			TSERROR4CXX(_T("保存JPEG格式失败！"));
			::DeleteFile(lpszPathName);
			return FALSE;
		}

		//修改相关的exif信息
		//void* handle = create_exif_handle(lpszPathName, true);

		//// 删除缩略图
		//delete_thumb(handle);
		//// 修改Exif中Color值为1
		//update_color_space(handle, 1);
		//// 修改Exif中Orientation值为1
		//update_orientation(handle, 1);
		//// 看看是否要修改DPI
		//if (nDPI != -1)
		//{
		//	update_dpi(handle, nDPI);
		//}

		//close_exif_handle(handle);
		TSINFO4CXX(_T("保存JPEG格式成功！"));

		return TRUE;
	}
	else
	{
		if (!_fipWinImage.saveU(lpszPathName))
		{
			TSINFO4CXX(_T("保存文件失败！"));

			::DeleteFile(lpszPathName);
			return FALSE;
		}
	}

	return TRUE;
}

CKKImageEXIF* CImageLoader::LoadImageFileOnlyExif(LPCTSTR lpszPathName)
{
	TSAUTO();
	CKKImageEXIF* pKKImageEXIF = NULL; 
	


	FREE_IMAGE_FORMAT fif = fipImage::identifyFIFU(lpszPathName);
	if (fif != FIF_JPEG && fif != FIF_RAW)
	{
		//如果不是JPEG和RAW格式，那么直接返回
		TSINFO4CXX("the image is not jpep and raw!");
		return pKKImageEXIF;
	}

	//加载头部信息
	fipWinImage newImage;
	//使用JPEG_CMYK的目的是：如果jpg是JCS_CMYK, 不要让freeimage来做cmyk-rgb的转换，让freeimage在icc信息中记录一个FIICC_COLOR_IS_CMYK的标志
	if (!newImage.loadU(lpszPathName, FIF_LOAD_NOPIXELS|JPEG_CMYK))
	{
		TSINFO4CXX("Load Head Failed!");
		return pKKImageEXIF;
	}

	// 获取JPEG文件的EXIF信息
	// FIMD_EXIF_MAIN和FIMD_EXIF_EXIF里面的内容主要用来读取相关的信息（不能写）
	// FIMD_EXIF_RAW里面的信息，字段“ExifRaw”只用来写，在保存图片的时候，必须调用setMetadata把它设置到保存的图片里面去
	
	pKKImageEXIF = new CKKImageEXIF();

	//先获取特殊的一些Exif信息（即不通过freeimage来获取的）
	/*void* handle = create_exif_handle(lpszPathName, false);
	if (handle != NULL)
	{
		TSINFO4CXX("create_exif_handle successful!");

		//获取快门次数
		wchar_t szShutterCount[20] = {0};
		get_shutter_count_from_maker_note(handle, szShutterCount, 20);
		pKKImageEXIF->SetShutterCount(szShutterCount);

		//获取镜头信息
		wchar_t szLensModel[100] = {0};
		get_lens_model_from_maker_note(handle, szLensModel, 100);
		pKKImageEXIF->SetLensModel(szLensModel);

		//获取拍摄模式
		int nExposureMode = get_exposure_program_from_makenote(handle);
		pKKImageEXIF->SetExposureProgram(nExposureMode);

		close_exif_handle(handle);
	}
	else
	{
		TSINFO4CXX("create_exif_handle failed!");
	}
	*/

	fipMetadataFind MetadataFind;
	fipTag tag;
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAIN, newImage, tag))
	{
		do
		{
			std::string strFieldName = tag.getKey();
			pKKImageEXIF->AddImageEXIF(strFieldName, tag);
			//TSINFO4CXX("FIMD_EXIF_MAIN:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
			//<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_EXIF, newImage, tag))
	{
		do
		{
			std::string strFieldName = tag.getKey();
			pKKImageEXIF->AddImageEXIF(strFieldName, tag);
			//TSINFO4CXX("FIMD_EXIF_EXIF:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
			//<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAKERNOTE, newImage, tag))
	{
		do 
		{
			std::string strFieldName = tag.getKey();
			pKKImageEXIF->AddImageEXIF(strFieldName, tag);
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_RAW, newImage, tag))
	{
		do 
		{
			std::string strFieldName = tag.getKey();
			pKKImageEXIF->AddImageEXIF(strFieldName, tag);


			//计算exif信息长度
			DWORD tag_length = tag.getLength();
			DWORD thumbnail_length = get_thumbnail_length((unsigned char*)tag.getValue(), tag_length);
			tag_length += 8;
			tag_length -= thumbnail_length;
			pKKImageEXIF->SetExifLength(tag_length);

		} while (MetadataFind.findNextMetadata(tag));
	}

	return pKKImageEXIF;
}