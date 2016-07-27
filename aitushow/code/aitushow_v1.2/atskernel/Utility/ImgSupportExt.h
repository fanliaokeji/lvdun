#ifndef IMG_SUPPORT_EXT_H
#define IMG_SUPPORT_EXT_H
#include "freeimage/FreeImage.h"

#ifndef _countof
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

//support img ext
static char *szImgExt[] = {
	//"unknow",
	"jpg",
	"jpeg",
	"jpe",
	"bmp",
	"png",
	"gif",
	"tiff",
	"tif",
	"psd",
	"ico",
	"pcx",
	"tga",
	"wbm",
	"ras",
	"mng",
	"cr2",
	"nef",
	"arw",
	"dng",
	"srf",
	"raf",
	"wmf",
	"ras"
};

inline bool IsSupportExt(const char *szExt)
{
	static int len = _countof(szImgExt);
	
	for (int i = 0; i < len; ++i)
	{
		if (strcmp(szExt,szImgExt[i]) == 0)
		{
			return true;
		}
	}
	return false;
}


inline FREE_IMAGE_FORMAT ConverStrExtToFIF(const char *szExt)
{
	if (strcmp(szExt,"bmp") == 0)
		return FIF_BMP;
	if (strcmp(szExt,"ico") == 0)
		return FIF_ICO;
	if (strcmp(szExt,"jpg") == 0)
		return FIF_JPEG;
	if (strcmp(szExt,"jpeg") == 0)
		return FIF_JPEG;
	if (strcmp(szExt,"jpe") == 0)
		return FIF_JPEG;
	if (strcmp(szExt,"jng") == 0)
		return FIF_JNG;
	if (strcmp(szExt,"koala") == 0)
		return FIF_KOALA;
	if (strcmp(szExt,"lbm") == 0)
		return FIF_LBM;
	if (strcmp(szExt,"iff") == 0)
		return FIF_IFF;
	if (strcmp(szExt,"mng") == 0)
		return FIF_MNG;
	if (strcmp(szExt,"pbm") == 0)
		return FIF_PBM;
	if (strcmp(szExt,"pbmraw") == 0)
		return FIF_PBMRAW;
	if (strcmp(szExt,"pcd") == 0)
		return FIF_PCD;
	if (strcmp(szExt,"pcx") == 0)
		return FIF_PCX;
	if (strcmp(szExt,"pgm") == 0)
		return FIF_PGM;
	if (strcmp(szExt,"pgmraw") == 0)
		return FIF_PGMRAW;
	if (strcmp(szExt,"png") == 0)
		return FIF_PNG;
	if (strcmp(szExt,"ppm") == 0)
		return FIF_PPM;
	if (strcmp(szExt,"ppmraw") == 0)
		return FIF_PPMRAW;
	if (strcmp(szExt,"ras") == 0)
		return FIF_RAS;
	if (strcmp(szExt,"targa") == 0)
		return FIF_TARGA;
	if (strcmp(szExt,"tiff") == 0)
		return FIF_TIFF;
	if (strcmp(szExt,"wbmp") == 0)
		return FIF_WBMP;
	if (strcmp(szExt,"psd") == 0)
		return FIF_PSD;
	if (strcmp(szExt,"cut") == 0)
		return FIF_CUT;
	if (strcmp(szExt,"xbm") == 0)
		return FIF_XBM;
	if (strcmp(szExt,"xpm") == 0)
		return FIF_XPM;
	if (strcmp(szExt,"dds") == 0)
		return FIF_DDS;
	if (strcmp(szExt,"gif") == 0)
		return FIF_GIF;
	//if (strcmp(szExt,"hdr") == 0)
	//	return FIF_HDR;
	if (strcmp(szExt,"faxg3") == 0)
		return FIF_FAXG3;
	if (strcmp(szExt,"sgi") == 0)
		return FIF_SGI;
	if (strcmp(szExt,"exr") == 0)
		return FIF_EXR;
	if (strcmp(szExt,"j2k") == 0)
		return FIF_J2K;
	if (strcmp(szExt,"jp2") == 0)
		return FIF_JP2;
	if (strcmp(szExt,"pfm") == 0)
		return FIF_PFM;
	if (strcmp(szExt,"pict") == 0)
		return FIF_PICT;
	//if (strcmp(szExt,"raw") == 0)
	//	return FIF_RAW;
	if (strcmp(szExt,"webp") == 0)
		return FIF_WEBP;
	if (strcmp(szExt,"jxr") == 0)
		return FIF_JXR;
	return FIF_UNKNOWN;
}

inline std::string ConverFIFToStrExt(const FREE_IMAGE_FORMAT fif)
{
	if (fif == FIF_UNKNOWN)
		return  "unknow";
	if (fif == FIF_BMP)
		return  "bmp";
	if (fif == FIF_ICO)
		return  "ico";
	if (fif == FIF_JPEG)
		return  "jpeg";
	if (fif == FIF_JNG)
		return  "jng";
	if (fif == FIF_KOALA)
		return  "koala";
	if (fif == FIF_LBM)
		return  "lbm";
	if (fif == FIF_IFF)
		return  "iff";
	if (fif == FIF_MNG)
		return  "mng";
	if (fif == FIF_PBM)
		return  "pbm";
	if (fif == FIF_PBMRAW)
		return  "pbmraw";
	if (fif == FIF_PCD)
		return  "pcd";
	if (fif == FIF_PCX)
		return  "pcx";
	if (fif == FIF_PGM)
		return  "pgm";
	if (fif == FIF_PGMRAW)
		return  "pgmraw";
	if (fif == FIF_PNG)
		return  "png";
	if (fif == FIF_PPM)
		return  "ppm";
	if (fif == FIF_PPMRAW)
		return  "ppmraw";
	if (fif == FIF_RAS)
		return  "ras";
	if (fif == FIF_TARGA)
		return  "targa";
	if (fif == FIF_TIFF)
		return  "tiff";
	if (fif == FIF_WBMP)
		return  "wbmp";
	if (fif == FIF_PSD)
		return  "psd";
	if (fif == FIF_CUT)
		return  "cut";
	if (fif == FIF_XBM)
		return  "xbm";
	if (fif == FIF_XPM)
		return  "xpm";
	if (fif == FIF_DDS)
		return  "dds";
	if (fif == FIF_GIF)
		return  "gif";
	//if (fif == FIF_HDR)
	//	return  "hdr";
	if (fif == FIF_FAXG3)
		return  "faxg3";
	if (fif == FIF_SGI)
		return  "sgi";
	if (fif == FIF_EXR)
		return  "exr";
	if (fif == FIF_J2K)
		return  "j2k";
	if (fif == FIF_JP2)
		return  "jp2";
	if (fif == FIF_PFM)
		return  "pfm";
	if (fif == FIF_PICT)
		return  "pict";
	//if (fif == FIF_RAW)
	//	return  "raw";
	if (fif == FIF_WEBP)
		return  "webp";
	if (fif == FIF_JXR)
		return  "jxr";
	return "unknow";
}

#endif //IMG_SUPPORT_EXT_H