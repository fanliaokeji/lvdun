#include "StdAfx.h"
#include "DefultLoaderImpl.h"
#include <cmath>
#include <strsafe.h>
#include "..\ImageHelper\exif_thumbnail_length.h"
#include "..\ImageHelper\ImageProcessor.h"
#include "..\ImageHelper\ImageUtility.h"

CDefaultLoaderImpl::CDefaultLoaderImpl(void)
{
	m_hXLBitmap = NULL;
	m_nSrcBitmapWidth = 0;
	m_nSrcBitmapHeight = 0;
	m_nExifInfoStatus = 0;
}

CDefaultLoaderImpl::~CDefaultLoaderImpl(void)
{
}

BOOL CDefaultLoaderImpl::AutoRotate()
{
	int nOrientationValue = m_KKImageEXIF->GetOrientation();
	XL_BITMAP_HANDLE hNewBitmap = NULL;
	XL_BITMAP_HANDLE hTmpBitmap = NULL;
	switch (nOrientationValue)
	{
	case 1:
		//这种情况不需要转正
		return FALSE;
		break;
	case 2: //直接水平翻转
		hNewBitmap = CImageProcessor::ImageLeftRightMirror(m_hXLBitmap);
		if (hNewBitmap)
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hNewBitmap;
		}
		break;
	case 3: //180度旋转
		hNewBitmap = CImageProcessor::ImageRotate180(m_hXLBitmap);
		if (hNewBitmap)
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hNewBitmap;
		}
		break;
	case 4: //直接垂直翻转
		hNewBitmap = CImageProcessor::ImageTopBottomMirror(m_hXLBitmap);
		if (hNewBitmap)
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hNewBitmap;
		}
		break;
	case 5:
		//右转90度，然后左右翻转
		hTmpBitmap = CImageProcessor::ImageRightRotate(m_hXLBitmap);
		if (hTmpBitmap)	// 右转成功
		{
			// 先释放掉这个，免得占内存
			XL_ReleaseBitmap(m_hXLBitmap);
			hNewBitmap = CImageProcessor::ImageLeftRightMirror(hTmpBitmap);
			if (hNewBitmap) // 左右镜像成功
			{
				XL_ReleaseBitmap(hTmpBitmap);
				m_hXLBitmap = hNewBitmap;
			}
			else
			{
				m_hXLBitmap = hTmpBitmap;
			}
			// 修改 尺寸数据
			int nTemp = m_nSrcBitmapWidth;
			m_nSrcBitmapWidth = m_nSrcBitmapHeight;
			m_nSrcBitmapHeight = nTemp;
		}
		break;
	case 6: //右转90度
		hNewBitmap = CImageProcessor::ImageRightRotate(m_hXLBitmap);
		if (hNewBitmap) // 右转成功
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hNewBitmap;
			// 修改 尺寸数据
			int nTemp = m_nSrcBitmapWidth;
			m_nSrcBitmapWidth = m_nSrcBitmapHeight;
			m_nSrcBitmapHeight = nTemp;
		}
		break;
	case 7:
		//左转90度，然后左右翻转
		hTmpBitmap = CImageProcessor::ImageLeftRotate(m_hXLBitmap);
		if (hTmpBitmap)	// 左转成功
		{
			// 先释放掉这个，免得占内存
			XL_ReleaseBitmap(m_hXLBitmap);
			hNewBitmap = CImageProcessor::ImageLeftRightMirror(hTmpBitmap);
			if (hNewBitmap) // 左右镜像成功
			{
				XL_ReleaseBitmap(hTmpBitmap);
				m_hXLBitmap = hNewBitmap;
			}
			else
			{
				m_hXLBitmap = hTmpBitmap;
			}
			// 修改 尺寸数据
			int nTemp = m_nSrcBitmapWidth;
			m_nSrcBitmapWidth = m_nSrcBitmapHeight;
			m_nSrcBitmapHeight = nTemp;
		}
		break;
	case 8: //左转90度
		hNewBitmap = CImageProcessor::ImageLeftRotate(m_hXLBitmap);
		if (hNewBitmap) // 左转成功
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hNewBitmap;
			// 修改 尺寸数据
			int nTemp = m_nSrcBitmapWidth;
			m_nSrcBitmapWidth = m_nSrcBitmapHeight;
			m_nSrcBitmapHeight = nTemp;
		}
		break;
	default:
		return FALSE;
		break;
	}
}

int CDefaultLoaderImpl::LoadImage(const wstring& wstrFilePath, bool* pbStop, bool bScale, int nWidth, int nHeight)
{
	TSAUTO();
	TSINFO4CXX(L"wstrFilePath:"<<wstrFilePath);

	// 初始化Exif信息对象
	if (m_KKImageEXIF)
	{
		delete m_KKImageEXIF;
		m_KKImageEXIF = NULL;
	}
	m_KKImageEXIF = new CKKImageEXIF();

	// 加载文件
	fipWinImage newImage;
	FREE_IMAGE_FORMAT fif = fipImage::identifyFIFU(wstrFilePath.c_str());

	if (fif == FIF_UNKNOWN)
	{
		TSERROR4CXX(_T("不能识别的文件格式！"));
		return 3;
	}
	//处理raw文件
	else if (fif == FIF_RAW)
	{
		TSINFO4CXX("This is a raw format file!");
		if (!LoadRAWImage(wstrFilePath.c_str(), newImage))
		{
			TSERROR4CXX("Load raw image failed!");
			return 4;
		}
		m_nExifInfoStatus = 1;
	}
	//处理JPEG文件
	//else if (fif == FIF_JPEG)
	//{
	//	TSINFO4CXX("This is a jpeg format file!");
	//	m_hXLBitmap = LoadJPEGImage(wstrFilePath.c_str(), TRUE);
	//	assert(m_hXLBitmap);
	//	if (m_hXLBitmap == NULL)
	//	{
	//		return 4;
	//	}
	//	m_nExifInfoStatus = 1;
	//}
	else
	{
		
		if (fif == FIF_PSD)	// PSD文件
		{
			// 加载
			if (!newImage.loadU(wstrFilePath.c_str(), PSD_CMYK))
			{
				TSERROR4CXX(_T("加载图片失败！"));
				return 4;
			}
		}
		else
		{
			if (!newImage.loadU(wstrFilePath.c_str()))
			{
				TSERROR4CXX(_T("加载图片失败！"));
				return 4;
			}
		}

		// 设置是否有EXIF信息状态
		if (fif == FIF_TIFF)
		{
			m_nExifInfoStatus = 1;
		}
		// 做额外处理
		wstring wstrMonitorIccFilePath = CImageUtility::GetDisplayMonitorICCFilePath();

		// 判断是不是CMYK
		FIICCPROFILE* pImageICC = FreeImage_GetICCProfile(newImage);
		if(pImageICC)
		{
			if (pImageICC->flags & FIICC_COLOR_IS_CMYK)
			{
				CMYKtosRGB(newImage, wstrMonitorIccFilePath);
			}
			else	// 不是CMYK，判断有没有ICC，如果有，也做转换
			{
				if(pImageICC->data) // 有ICC
				{
					ApplyICC(newImage, wstrMonitorIccFilePath);
				}
				else	// 没有ICC
				{
					if (PathFileExists(wstrMonitorIccFilePath.c_str()))
					{	
						sRGBtoMonitorRGB(newImage, wstrMonitorIccFilePath);
					}
				}
			}
		}
		else // 判断显示器的ICC
		{
			if (PathFileExists(wstrMonitorIccFilePath.c_str()))
			{
				sRGBtoMonitorRGB(newImage, wstrMonitorIccFilePath);
			}
		}
		
	}
	if (m_hXLBitmap == NULL)	// 只会在Raw格式下才会调用到
	{
		m_hXLBitmap = ConvertFipImagetoXLBitmapHandle(newImage);
	}
	
	assert(m_hXLBitmap);
	if (m_hXLBitmap == NULL)
	{
		return 4;
	}

	if (fif == FIF_BMP)
	{
		//对于32位BMP的情况，这里还是先将它的alpha全部置为255
		XL_ResetAlphaChannel(m_hXLBitmap, 255);
	}
	XLBitmapInfo bmpInfo;
	XL_GetBitmapInfo(m_hXLBitmap, &bmpInfo);
	m_nSrcBitmapWidth = bmpInfo.Width;
	m_nSrcBitmapHeight = bmpInfo.Height;
	// 做缩放
	if (bScale)// 计算缩放率
	{
		double dRatio = 0;
		double nMaxLength = nWidth>nHeight?nWidth:nHeight;
		double nMinLength = nWidth<nHeight?nWidth:nHeight;

		double nImageMaxLength = m_nSrcBitmapWidth>m_nSrcBitmapHeight?m_nSrcBitmapWidth:m_nSrcBitmapHeight;
		double nImageMinLength = m_nSrcBitmapWidth<m_nSrcBitmapHeight?m_nSrcBitmapWidth:m_nSrcBitmapHeight;

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
			TSINFO4CXX("LoadFile by default method, dRatio:" << dRatio);
			XL_BITMAP_HANDLE hNewBitmap = CImageProcessor::RescaleImage(m_hXLBitmap, m_nSrcBitmapWidth*dRatio, m_nSrcBitmapHeight*dRatio);
			if (hNewBitmap)
			{
				XL_ReleaseBitmap(m_hXLBitmap);
				m_hXLBitmap = hNewBitmap;
			}
		}
	}
	return 0;
}

BOOL CDefaultLoaderImpl::LoadRAWImage(LPCTSTR lpszPathName, fipWinImage& newImage)
{
	TSAUTO();

	if (!newImage.loadU(lpszPathName, RAW_PREVIEW))
	{
		TSERROR4CXX(_T("加载raw图片失败！"));
		return FALSE;
	}

	// 获取JPEG文件的EXIF信息
	// FIMD_EXIF_MAIN和FIMD_EXIF_EXIF里面的内容主要用来读取相关的信息（不能写）
	// FIMD_EXIF_RAW里面的信息，字段“ExifRaw”只用来写，在保存图片的时候，必须调用setMetadata把它设置到保存的图片里面去

	fipMetadataFind MetadataFind;
	fipTag tag;
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAIN, newImage, tag))
	{
		do
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_MAIN:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_EXIF, newImage, tag))
	{
		do
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_EXIF:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_GPS, newImage, tag))
	{
		do
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_GPS:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAKERNOTE, newImage, tag))
	{
		do 
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_MAKERNOTE:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_INTEROP, newImage, tag))
	{
		do 
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_INTEROP:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_RAW, newImage, tag))
	{
		do 
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_RAW:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());

			//计算exif信息长度
			DWORD tag_length = tag.getLength();
			DWORD thumbnail_length = get_thumbnail_length((unsigned char*)tag.getValue(), tag_length);
			tag_length += 8;
			tag_length -= thumbnail_length;
			m_KKImageEXIF->SetExifLength(tag_length);

		} while (MetadataFind.findNextMetadata(tag));
	}

	// 获取显示器的ICC文件路径
	wstring wstrMonitorIccFilePath = CImageUtility::GetDisplayMonitorICCFilePath();
	//判断是否是Adobe RGB颜色空间，如果是，则进行颜色空间的转换
	std::string strColorSpace;
	if (m_KKImageEXIF->GetColorSpace(strColorSpace) && strColorSpace == "Adobe RGB")
	{
		TSINFO4CXX("this is a adobe RGB image!");
		//如果是Adobe RGB颜色空间，对其进行转换
		if (!AdobeRGBtosRGB(newImage, wstrMonitorIccFilePath))
		{
			TSERROR4CXX("convert adobe RGB to sRGB failed!");
			return FALSE;
		}
	}
	else
	{
		// 判断显示器有没有ICC
		if (PathFileExists(wstrMonitorIccFilePath.c_str()))
		{
			sRGBtoMonitorRGB(newImage, wstrMonitorIccFilePath);
		}
	}
	return TRUE;

}

//调用阿潘提供的接口，速度快了200ms，该函数基本上不会被调用
XL_BITMAP_HANDLE CDefaultLoaderImpl::LoadJPEGImage(LPCTSTR lpszPathName, BOOL IsLoadExif)
{
	TSAUTO();

	//为了保证各种色彩空间的图在主界面或拼图界面都能显示，强制loadexif
	IsLoadExif = TRUE;

	fipWinImage newImage;

	//加载头部信息
	//使用JPEG_CMYK的目的是：如果jpg是JCS_CMYK, 不要让freeimage来做cmyk-rgb的转换，让freeimage在icc信息中记录一个FIICC_COLOR_IS_CMYK的标志
	if (!newImage.loadU(lpszPathName, JPEG_CMYK))
	{
		TSINFO4CXX("Load Head Failed!");
		return NULL;
	}

	int nWidth = newImage.getWidth();
	int nHeight = newImage.getHeight();

	//加载Exif信息
	if (IsLoadExif == TRUE)
	{
		// 获取JPEG文件的EXIF信息
		// FIMD_EXIF_MAIN和FIMD_EXIF_EXIF里面的内容主要用来读取相关的信息（不能写）
		// FIMD_EXIF_RAW里面的信息，字段“ExifRaw”只用来写，在保存图片的时候，必须调用setMetadata把它设置到保存的图片里面去

		fipMetadataFind MetadataFind;
		fipTag tag;
		if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAIN, newImage, tag))
		{
			do
			{
				std::string strFieldName = tag.getKey();
				m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
				TSINFO4CXX("FIMD_EXIF_MAIN:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
					<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
			} while (MetadataFind.findNextMetadata(tag));
		}
		if (MetadataFind.findFirstMetadata(FIMD_EXIF_EXIF, newImage, tag))
		{
			do
			{
				std::string strFieldName = tag.getKey();
				m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
				TSINFO4CXX("FIMD_EXIF_EXIF:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
					<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
			} while (MetadataFind.findNextMetadata(tag));
		}
		if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAKERNOTE, newImage, tag))
		{
			do 
			{
				std::string strFieldName = tag.getKey();
				m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
				TSINFO4CXX("FIMD_EXIF_MAKERNOTE:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
					<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
			} while (MetadataFind.findNextMetadata(tag));
		}
		/*if (MetadataFind.findFirstMetadata(FIMD_EXIF_GPS, newImage, tag))
		{
		do
		{
		std::string strFieldName = tag.getKey();
		m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
		//TSINFO4CXX("FIMD_EXIF_GPS:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
		//<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
		}		
		if (MetadataFind.findFirstMetadata(FIMD_EXIF_INTEROP, newImage, tag))
		{
		do 
		{
		std::string strFieldName = tag.getKey();
		m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
		TSINFO4CXX("FIMD_EXIF_INTEROP:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
		<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
		}*/
		if (MetadataFind.findFirstMetadata(FIMD_EXIF_RAW, newImage, tag))
		{
			do 
			{
				std::string strFieldName = tag.getKey();
				m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
				TSINFO4CXX("FIMD_EXIF_RAW:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
					<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());

				//计算exif信息长度
				DWORD tag_length = tag.getLength();
				DWORD thumbnail_length = get_thumbnail_length((unsigned char*)tag.getValue(), tag_length);
				tag_length += 8;
				tag_length -= thumbnail_length;
				m_KKImageEXIF->SetExifLength(tag_length);

			} while (MetadataFind.findNextMetadata(tag));
		}
	}

	DWORD time1 = ::GetTickCount();

	XL_BITMAP_HANDLE hNewBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
	assert(hNewBitmap);
	if (hNewBitmap == NULL)
	{
		TSINFO4CXX("XL_CreateBitmap Failed!");
		return NULL;
	}

	DWORD time2 = ::GetTickCount();

	BYTE* BmpBuf = XL_GetBitmapBuffer(hNewBitmap, 0, 0);
	
	int ret = -1;
	//int ret = FreeImage_Jpeg_LoadU(lpszPathName, BmpBuf);
	if (ret != 0)
	{
		TSINFO4CXX("Load Jpeg Failed!");
		return NULL;
	}

	//判断是否是Adobe RGB或cmyk颜色空间，如果是，则进行颜色空间的转换
	if (IsLoadExif == TRUE)
	{
		FIICCPROFILE* pImageICC = FreeImage_GetICCProfile(newImage);
		if(pImageICC && (pImageICC->flags & FIICC_COLOR_IS_CMYK) )
		{
			if (!CMYKtosRGB(newImage,hNewBitmap))
			{
				TSERROR4CXX("convert CMYK to sRGB failed!");
				return FALSE;
			}
		}
		else
		{
			std::string strColorSpace;
			if (m_KKImageEXIF->GetColorSpace(strColorSpace) && strColorSpace == "Adobe RGB")
			{
				TSINFO4CXX("this is a adobe RGB image!");
				//如果是Adobe RGB颜色空间，对其进行转换
				if (!AdobeRGBtosRGB(newImage, hNewBitmap))
				{
					TSERROR4CXX("convert adobe RGB to sRGB failed!");
					return NULL;
				}
			}
		}
	}

	DWORD time3 = ::GetTickCount();

	TSINFO4CXX("time2 - time1:"<<time2 - time1<<" time3 - time2:"<<time3 - time2);

	return hNewBitmap;
}