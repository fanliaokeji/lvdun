#pragma once

#include "freeimage/FreeImagePlus.h"
#include <string>
using namespace std;
class CKKImageEXIF
{
public:
	CKKImageEXIF();
	~CKKImageEXIF();

public:
	void Init();
	void AddImageEXIF(std::string strFieldName, fipTag& newTag);
	void SetShutterCount(std::wstring wstrShutterCount);
	void SetLensModel(std::wstring wstrLensModel);
	void SetExifLength(int nExifLength);
	int GetExifLength();

	//下面这些用于显示图片信息时
	BOOL GetColorSpace(std::string& strColorSpace); //获得颜色空间,nCloroSpace值为0表示sRGB，0xFFFF为adobe RGB（其实这个不一定）
	BOOL GetMake(std::string& strMake); //获得制造厂商
	BOOL GetModel(std::string& strModel); //获得相机型号
	BOOL GetDateTime(std::string& strDateTime); //获得拍摄日期
	BOOL GetExposureTime(std::string& strExposureTime); //获得曝光时间（即快门速度)
	BOOL GetFNumber(std::string& strFNumber); //获得光圈
	BOOL GetFocalLength(std::string& strFocalLength); //获得焦距
	BOOL GetISO(std::string& strISO); //获得ISO
	BOOL GetLens(std::string& strLens); //获得相机镜头
	BOOL GetExposureBiasValue(std::string& strExposureBiasValue); //获得曝光补偿
	BOOL GetFlash(std::string& strFlash); //获得闪光灯
	BOOL GetWhiteBalance(std::string& strWhiteBalance); //获得白平衡
	BOOL GetExposureProgram(std::string& strExposureProgram); //拍摄模式
	BOOL GetMeteringMode(std::string& strMeteringMode); //测光模式
	BOOL GetShutterCount(std::string& strShutterCount); //获得快门次数
	int GetOrientation();

	//下面这些接口用于获取一些特殊格式的信息
	BOOL GetDateDay(std::string& strDateDay, int nFormatType);
	BOOL GetDateTime(std::string& strDateTime, int nFormatType);

	void SetImageEXIF(fipImage* fImage); //这个用来在保存图像为JPEG的时候，保存EXIF信息

	std::wstring ApplyExif(std::wstring wstrText, BOOL bIsReplaceSpecialChar = FALSE, std::wstring wstrFormatChar = _T("_")); //对一段包含exif信息的文本应用exif信息，返回应用后的文本

private:
	std::string CalculateWeekDay(int nYear, int nMonth, int nDay);

private:
	std::map<std::string, fipTag> m_exifs;

	wchar_t m_szShutterCount[20];
	wchar_t m_szLensModel[100];

	int m_nExifLength;

private:
	//
};
