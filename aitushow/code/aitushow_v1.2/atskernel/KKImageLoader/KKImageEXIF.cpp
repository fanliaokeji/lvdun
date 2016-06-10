#include "stdafx.h"
#include "KKImageEXIF.h"
#include "../Utility/StringOperation.h"
#include "../ImageHelper/ImageUtility.h"
#include <strsafe.h>

using namespace std;
using namespace Gdiplus;

CKKImageEXIF::CKKImageEXIF()
{
	TSAUTO();

	Init();
}

CKKImageEXIF::~CKKImageEXIF()
{

}

void CKKImageEXIF::Init()
{
	TSAUTO();

	m_exifs.clear();

	ZeroMemory(m_szShutterCount, 40);
	ZeroMemory(m_szLensModel, 200);

	m_nExifLength = 0;
}

void CKKImageEXIF::AddImageEXIF(std::string strFieldName, fipTag& newTag)
{
	if (m_exifs.find(strFieldName) == m_exifs.end())
	{
		m_exifs[strFieldName] = newTag;
	}
}

void CKKImageEXIF::SetShutterCount(std::wstring wstrShutterCount)
{
	TSAUTO();

	TSINFO4CXX("wstrShutterCount:"<<wstrShutterCount);

	ZeroMemory(m_szShutterCount, 40); //40个字节

	StringCchCopy(m_szShutterCount, 20, wstrShutterCount.c_str());

	TSINFO4CXX("m_szShutterCount:"<<m_szShutterCount);
}

void CKKImageEXIF::SetLensModel(std::wstring wstrLensModel)
{
	TSAUTO();

	TSINFO4CXX("wstrLensModel:"<<wstrLensModel);

	ZeroMemory(m_szLensModel, 200); //40个字节

	StringCchCopy(m_szLensModel, 100, wstrLensModel.c_str());

	TSINFO4CXX("m_szLensModel:"<<m_szLensModel);	
}

void CKKImageEXIF::SetExifLength(int nExifLength)
{
	TSAUTO();

	m_nExifLength = nExifLength + 8;
}

int CKKImageEXIF::GetExifLength()
{
	TSAUTO();

	return m_nExifLength;
}

BOOL CKKImageEXIF::GetColorSpace(std::string& strColorSpace)
{
	TSAUTO();

	if (m_exifs.find("ColorSpace") == m_exifs.end())
	{
		TSINFO4CXX("There is no ColorSpace Info!");
		return FALSE;
	}

	UINT16 nColorSpace;
	memcpy(&nColorSpace,  m_exifs["ColorSpace"].getValue(),  m_exifs["ColorSpace"].getLength());

	TSINFO4CXX("nColorSpace:"<<nColorSpace);

	if (nColorSpace == 0x0001)
	{
		TSINFO4CXX("This is a sRGB JPEG!");
		strColorSpace = "sRGB";
	}
	else if (nColorSpace = 0xFFFF)
	{
		//如果ColorSpace的值为65535，并且有相机型号信息，我们才认为是Adobe RGB
		if (m_exifs.find("Make") != m_exifs.end())
		{
			char szMake[100] = {0};
			memcpy(szMake, m_exifs["Make"].getValue(),  m_exifs["Make"].getLength());
			std::string strMake = szMake;
			TSINFO4CXX("Make:"<<strMake);

			if (strMake.size() != 0)
			{
				TSINFO4CXX("This is a Adobe RGB JPEG!");
				strColorSpace = "Adobe RGB";
			}
			else
			{
				TSINFO4CXX("Length of Make is 0!");
				strColorSpace = "unknow";
			}
		}
		else
		{
			strColorSpace = "unknow";
			TSINFO4CXX("There is no Make Info, it is not a adobe RGB image!");
		}		
	}
	else
	{
		TSINFO4CXX("The ColorSpace of this JPEG is unknow");
		strColorSpace = "unknow";
	}

	return TRUE;
}

BOOL CKKImageEXIF::GetMake(std::string& strMake)
{
	TSAUTO();

	if (m_exifs.find("Make") == m_exifs.end())
	{
		TSINFO4CXX("There is no Make Info!");
		return FALSE;
	}

	strMake = (char*)m_exifs["Make"].getValue();

	return TRUE;
}

BOOL CKKImageEXIF::GetModel(std::string& strModel)
{
	TSAUTO();

	if (m_exifs.find("Model") == m_exifs.end())
	{
		TSINFO4CXX("There is no Model Info!");
		return FALSE;
	}

	strModel = (char*)m_exifs["Model"].getValue();

	return TRUE;
}

BOOL CKKImageEXIF::GetDateTime(std::string& strDateTime)
{
	TSAUTO();

	if (m_exifs.find("DateTimeOriginal") == m_exifs.end())
	{
		//如果没有DateTimeOriginal信息，就读DateTime信息（主要是对raw格式）
		if (m_exifs.find("DateTime") == m_exifs.end())
		{
			TSINFO4CXX("There is no DateTime Info!");
			return FALSE;
		}
		strDateTime = (char*)m_exifs["DateTime"].getValue();
	}
	else
	{
		strDateTime = (char*)m_exifs["DateTimeOriginal"].getValue();
	}

	return TRUE;
}

/*
	这个函数用来获取特殊格式的拍摄日期（用于添加文字的插入Exif信息）
	nType的值决定格式类型，最终的输出结果跟这个有关
	nType:1 YYYY-MM-DD
	nType:2 YY-MM-DD
	nType:3 MM/DD/YYYY
	nType:4 MMM.DD.YY
	nType:5 YYYY
	nType:6 YY
	nType:7 M
	nType:8 MM
	nType:9 MMM
	nType:10 D
	nType:11 DD
	nType:12 DDD
*/

BOOL CKKImageEXIF::GetDateDay(std::string& strDateDay, int nFormatType)
{
	TSAUTO();

	std::string strSrcDateTime;
	if (m_exifs.find("DateTimeOriginal") == m_exifs.end())
	{
		if (m_exifs.find("DateTime") == m_exifs.end())
		{
			TSINFO4CXX("There is no DateTime Info!");
			return FALSE;
		}
		strSrcDateTime = (char*)m_exifs["DateTime"].getValue(); //最原始的拍摄时间
	}
	else
	{
		strSrcDateTime = (char*)m_exifs["DateTimeOriginal"].getValue();
	}

	std::string strYear = strSrcDateTime.substr(0, 4);
	std::string strMonth = strSrcDateTime.substr(5, 2);
	std::string strDay = strSrcDateTime.substr(8, 2);
	TSINFO4CXX("Year:"<<strYear<<" Month:"<<strMonth<<" Day:"<<strDay);

	//年份缩写
	std::string strYearYY = strYear.substr(2, 2);

	//月份缩写
	std::string strMonthM; //一位月份，即去掉月份前面的0
	if (strMonth.find("0") == 0)
	{
		strMonthM = strMonth.substr(1, 1);
	}
	else
	{
		strMonthM = strMonth;
	}

	std::string strMonthMMM;
	if (strMonth == "01")
	{
		strMonthMMM = "Jan";
	}
	else if (strMonth == "02")
	{
		strMonthMMM = "Feb";
	}
	else if (strMonth == "03")
	{
		strMonthMMM = "Mar";
	}
	else if (strMonth == "04")
	{
		strMonthMMM = "Apr";
	}
	else if (strMonth == "05")
	{
		strMonthMMM = "May";
	}
	else if (strMonth == "06")
	{
		strMonthMMM = "Jun";
	}
	else if (strMonth == "07")
	{
		strMonthMMM = "Jul";
	}
	else if (strMonth == "08")
	{
		strMonthMMM = "Aug";
	}
	else if (strMonth == "09")
	{
		strMonthMMM = "Sep";
	}
	else if (strMonth == "10")
	{
		strMonthMMM = "Oct";
	}
	else if (strMonth == "11")
	{
		strMonthMMM = "Nov";
	}
	else if (strMonth == "12")
	{
		strMonthMMM = "Dec";
	}


	//日期缩写
	std::string strDayD;
	if (strDay.find("0") == 0)
	{
		strDayD = strDay.substr(1, 1);
	}
	else
	{
		strDayD = strDay;
	}

	std::string strDayDDD;
	int nYear, nMonth, nDay;
	nYear = atoi(strYear.c_str());
	nMonth = atoi(strMonth.c_str());
	nDay = atoi(strDay.c_str());

	std::string WeekDay = CalculateWeekDay(nYear, nMonth, nDay);
	strDayDDD = WeekDay;

	if (nFormatType == 1)
	{
		strDateDay = strYear + "-" + strMonth + "-" + strDay;
	}
	else if (nFormatType == 2)
	{
		strDateDay = strYearYY + "-" + strMonth + "-" + strDay;
	}
	else if (nFormatType == 3)
	{
		strDateDay = strMonth + "/" + strDay + "/" + strYear;
	}
	else if (nFormatType == 4)
	{
		strDateDay = strMonthMMM + "." + strDay + "." + strYear;
	}
	else if (nFormatType == 5)
	{
		strDateDay = strYear;
	}
	else if (nFormatType == 6)
	{
		strDateDay = strYearYY;
	}
	else if (nFormatType == 7)
	{
		strDateDay = strMonthM;
	}
	else if (nFormatType == 8)
	{
		strDateDay = strMonth;
	}
	else if (nFormatType == 9)
	{
		strDateDay = strMonthMMM;
	}
	else if (nFormatType == 10)
	{
		strDateDay = strDayD;
	}
	else if (nFormatType == 11)
	{
		strDateDay = strDay;
	}
	else if (nFormatType == 12)
	{
		strDateDay = strDayDDD;
	}

	return TRUE;
}

/*
	这个函数用来获取特殊格式的拍摄时间（用于添加文字的插入Exif信息）
	nType的值决定格式类型，最终的输出结果跟这个有关
	nType:1 HH:MM AP/PM
	nType:2 HH:MM:SS
	nType:3 HH
	nType:4 MM
	nType:5 SS
*/
BOOL CKKImageEXIF::GetDateTime(std::string& strDateTime, int nFormatType)
{
	TSAUTO();

	std::string strSrcDateTime;
	if (m_exifs.find("DateTimeOriginal") == m_exifs.end())
	{
		if (m_exifs.find("DateTime") == m_exifs.end())
		{
			TSINFO4CXX("There is no DateTime Info!");
			return FALSE;
		}
		strSrcDateTime = (char*)m_exifs["DateTime"].getValue(); //最原始的拍摄时间
	}
	else
	{
		strSrcDateTime = (char*)m_exifs["DateTimeOriginal"].getValue();
	}

	std::string strHour = strSrcDateTime.substr(11, 2);
	std::string strMinute = strSrcDateTime.substr(14, 2);
	std::string strSecond = strSrcDateTime.substr(17, 2);
	TSINFO4CXX("Hour:"<<strHour<<" Minute:"<<strMinute<<" Second:"<<strSecond);

	int nHour = atoi(strHour.c_str());
	int nMinute = atoi(strMinute.c_str());
	int nSecond = atoi(strSecond.c_str());

	int nNewHour; //这个是表示把24小时制换成12小时制之后的
	std::string strAMPM = "AM";
	if (nHour > 12)
	{
		nNewHour = nHour - 12;
		strAMPM = "PM";
	}
	else if (nHour == 12)
	{
		nNewHour = 12;
		strAMPM = "PM";
	}
	else if (nHour == 0)
	{
		nNewHour = 12;
	}
	else
	{
		nNewHour = nHour;
	}
	char szNewHour[100] = {0};
	sprintf(szNewHour, "%2d", nNewHour);
	std::string strNewHour = szNewHour;

	if (nFormatType == 1)
	{
		strDateTime = strNewHour + ":" + strMinute + " " + strAMPM;
	}
	else if (nFormatType == 2)
	{
		strDateTime = strHour + ":" + strMinute + ":" + strSecond;
	}
	else if (nFormatType == 3)
	{
		strDateTime = strHour;
	}
	else if (nFormatType == 4)
	{
		strDateTime = strMinute;
	}
	else if (nFormatType == 5)
	{
		strDateTime = strSecond;
	}

	return TRUE;
}

BOOL CKKImageEXIF::GetExposureTime(std::string& strExposureTime)
{
	TSAUTO();

	if (m_exifs.find("ExposureTime") == m_exifs.end())
	{
		TSINFO4CXX("There is no ExposureTime Info!");
		return FALSE;
	}

	UINT32 nExposureTime[2];
	memcpy(nExposureTime, m_exifs["ExposureTime"].getValue(), m_exifs["ExposureTime"].getLength());

	if (nExposureTime[0] == 0)
	{
		strExposureTime = "0s";
		return TRUE;
	}

	UINT32 nDenominator = nExposureTime[1]/nExposureTime[0];

	char szExposureTime[100] = {0};
	::StringCchPrintfA(szExposureTime, 100, "1/%d", nDenominator);

	strExposureTime = szExposureTime;

	strExposureTime = strExposureTime + "s";

	TSINFO4CXX("ExposureTime:"<<strExposureTime);

	return TRUE;
}

BOOL CKKImageEXIF::GetFNumber(std::string& strFNumber)
{
	TSAUTO();

	if (m_exifs.find("FNumber") == m_exifs.end())
	{
		TSINFO4CXX("There is no FNumber Info!");
		return FALSE;
	}

	UINT32 nFNumber[2];
	memcpy(nFNumber, m_exifs["FNumber"].getValue(), m_exifs["FNumber"].getLength());

	float fFNumber = (float)nFNumber[0]/nFNumber[1];

	char szFNumber[100] = {0};
	::StringCchPrintfA(szFNumber, 100, "%.1f", fFNumber);

	strFNumber = szFNumber;

	strFNumber = "F" + strFNumber;

	TSINFO4CXX("FNumber:"<<strFNumber);

	return TRUE;
}

BOOL CKKImageEXIF::GetFocalLength(std::string& strFocalLength)
{
	TSAUTO();

	if (m_exifs.find("FocalLength") == m_exifs.end())
	{
		TSINFO4CXX("There is no FocalLength Info!");
		return FALSE;
	}

	UINT32 nFocalLength[2];
	memcpy(nFocalLength, m_exifs["FocalLength"].getValue(), m_exifs["FocalLength"].getLength());

	float fFocalLength = (float)nFocalLength[0]/nFocalLength[1];

	char szFocalLength[100] = {0};
	::StringCchPrintfA(szFocalLength, 100, "%.2f", fFocalLength);

	strFocalLength = szFocalLength;

	TSINFO4CXX("FocalLength:"<<strFocalLength);

	return TRUE;
}

BOOL CKKImageEXIF::GetISO(std::string& strISO)
{
	TSAUTO();

	if (m_exifs.find("ISOSpeedRatings") == m_exifs.end())
	{
		TSINFO4CXX("There is no ISOSpeedRatings Info!");
		return FALSE;
	}

	UINT16 nISO;
	memcpy(&nISO, m_exifs["ISOSpeedRatings"].getValue(), m_exifs["ISOSpeedRatings"].getLength());

	char szISO[100] = {0};
	::StringCchPrintfA(szISO, 100, "ISO%d", nISO);

	strISO = szISO;

	TSINFO4CXX("ISO:"<<strISO);

	return TRUE;
}

BOOL CKKImageEXIF::GetLens(std::string& strLens)
{
	TSAUTO();

	std::wstring wstrLens = m_szLensModel;
	
	strLens = ultra::_T2UTF(wstrLens);
	TSINFO4CXX("strLens:"<<strLens);

	return TRUE;
}

BOOL CKKImageEXIF::GetFlash(std::string& strFlash)
{
	TSAUTO();

	if (m_exifs.find("Flash") == m_exifs.end())
	{
		TSINFO4CXX("There is no Flash Info!");
		return FALSE;
	}

	UINT16 nFlash;
	memcpy(&nFlash, m_exifs["Flash"].getValue(), m_exifs["Flash"].getLength());

	std::wstring wstrFlash;
	if ((nFlash & 0x0001) == 0x0001)
	{
		wstrFlash = _T("开");
	}
	else
	{
		wstrFlash = _T("关");
	}

	strFlash = ultra::_T2UTF(wstrFlash);
	return TRUE;
}

BOOL CKKImageEXIF::GetWhiteBalance(std::string& strWhiteBalance)
{
	TSAUTO();

	if (m_exifs.find("WhiteBalance") == m_exifs.end())
	{
		TSINFO4CXX("There is no WhiteBalance Info!");
		return FALSE;
	}

	UINT16 nWhiteBalance;
	//注意这里不能用m_exifs["WhiteBalance"].getLength()，本来应该是2，但是在NIKON D3100上测是13，坑爹啊;
	memcpy(&nWhiteBalance, m_exifs["WhiteBalance"].getValue(), 2);

	TSINFO4CXX("nWhiteBalance:"<<nWhiteBalance);

	std::wstring wstrWhiteBalance;
	if (nWhiteBalance == 0)
	{
		wstrWhiteBalance = _T("自动");
	}
	else if (nWhiteBalance == 1)
	{
		wstrWhiteBalance = _T("手动");
	}
	else
	{
		wstrWhiteBalance = _T("");
	}

	strWhiteBalance = ultra::_T2UTF(wstrWhiteBalance);
	return TRUE;
}

BOOL CKKImageEXIF::GetExposureProgram(std::string& strExposureProgram)
{
	TSAUTO();

	if (m_exifs.find("ExposureProgram") == m_exifs.end() && m_exifs.find("CameraSettings:CanonExposureMode") == m_exifs.end())
	{
		TSINFO4CXX("There is no ExposureProgram Info!");
		return FALSE;
	}

	UINT16 nExposureProgram;
	if (m_exifs.find("ExposureProgram") != m_exifs.end())
	{
		TSINFO4CXX("ExposureProgram!");
		memcpy(&nExposureProgram, m_exifs["ExposureProgram"].getValue(), m_exifs["ExposureProgram"].getLength());
	}
	else
	{
		TSINFO4CXX("CameraSettings:CanonExposureMode!");
		memcpy(&nExposureProgram, m_exifs["CameraSettings:CanonExposureMode"].getValue(), m_exifs["CameraSettings:CanonExposureMode"].getLength());
	}
	TSINFO4CXX("nExposureProgram:"<<nExposureProgram);

	//不同的取值，对应不同的测光模式，参考exif文档exif2.2.pdf 38页
	/*
	0:未定义(标准）
	1:手动曝光
	2:自动曝光
	3:光圈优先
	4:快门优先
	5:创意模式
	6:运动模式
	7;人像模式
	8:风景模式
	other:reserved
	*/
	
	std::wstring wstrExposureProgram;
	if (nExposureProgram == 0)
	{
		wstrExposureProgram = _T("标准模式");
	}
	else if (nExposureProgram == 1)
	{
		wstrExposureProgram = _T("手动曝光");
	}
	else if (nExposureProgram == 2)
	{
		wstrExposureProgram = _T("自动曝光");
	}
	else if (nExposureProgram == 3)
	{
		wstrExposureProgram = _T("光圈优先");
	}
	else if (nExposureProgram == 4)
	{
		wstrExposureProgram = _T("快门优先");
	}
	else if (nExposureProgram == 5)
	{
		wstrExposureProgram = _T("创意模式");
	}
	else if (nExposureProgram == 6)
	{
		wstrExposureProgram = _T("运动模式");
	}
	else if (nExposureProgram == 7)
	{
		wstrExposureProgram = _T("人像模式");
	}
	else if (nExposureProgram == 8)
	{
		wstrExposureProgram = _T("风景模式");
	}
	else
	{
		wstrExposureProgram = _T("");
	}

	strExposureProgram = ultra::_T2UTF(wstrExposureProgram);
	return TRUE;
}

BOOL CKKImageEXIF::GetMeteringMode(std::string& strMeteringMode)
{
	TSAUTO();

	if (m_exifs.find("MeteringMode") == m_exifs.end())
	{
		TSINFO4CXX("There is no MeteringMode Info!");
		return FALSE;
	}

	UINT16 nMeteringMode;
	memcpy(&nMeteringMode, m_exifs["MeteringMode"].getValue(), m_exifs["MeteringMode"].getLength());

	//不同的取值，对应不同的测光模式，参考exif文档exif2.2.pdf 40页
	/*
	0:未知模式
	1:平均测光
	2:中央重点平均测光
	3:点测光
	4:多点测光
	5:加权测光（评价测光）（多分区测光）（矩阵测光）
	6:局部测光
	other:reserved
	255:other
	*/
	
	std::wstring wstrMeteringMode;
	if (nMeteringMode == 0)
	{
		wstrMeteringMode = _T("未知测光模式");
	}
	else if (nMeteringMode == 1)
	{
		wstrMeteringMode = _T("平均测光");
	}
	else if (nMeteringMode == 2)
	{
		wstrMeteringMode = _T("中央重点平均测光");
	}
	else if (nMeteringMode == 3)
	{
		wstrMeteringMode = _T("点测光");
	}
	else if (nMeteringMode == 4)
	{
		wstrMeteringMode = _T("多点测光");
	}
	else if (nMeteringMode == 5)
	{
		wstrMeteringMode = _T("加权测光");//这里保留老光魔的叫法
	}
	else if (nMeteringMode == 6)
	{
		wstrMeteringMode = _T("局部测光");
	}
	else if (nMeteringMode == 255)
	{
		wstrMeteringMode = _T("其他测光模式");
	}
	else
	{
		wstrMeteringMode = _T("");
	}

	strMeteringMode = ultra::_T2UTF(wstrMeteringMode);
	return TRUE;
}

BOOL CKKImageEXIF::GetShutterCount(std::string& strShutterCount)
{
	TSAUTO();

	std::wstring wstrShutterCount = m_szShutterCount;
	strShutterCount = ultra::_T2UTF(wstrShutterCount);
	TSINFO4CXX("wstrShutterCount:"<<wstrShutterCount<<" strShutterCount:"<<strShutterCount);

	return TRUE;
}

BOOL CKKImageEXIF::GetExposureBiasValue(std::string& strExposureBiasValue)
{
	TSAUTO();

	if (m_exifs.find("ExposureBiasValue") == m_exifs.end())
	{
		TSINFO4CXX("There is no ExposureBiasValue Info!");
		return FALSE;
	}

	INT32 nExposureBiasValue[2];
	memcpy(nExposureBiasValue, m_exifs["ExposureBiasValue"].getValue(), m_exifs["ExposureBiasValue"].getLength());

	float fExposureBiasValue = (float)nExposureBiasValue[0]/nExposureBiasValue[1];

	char szExposureBiasValue[100] = {0};
	::StringCchPrintfA(szExposureBiasValue, 100, "%.1f", fExposureBiasValue);

	strExposureBiasValue = szExposureBiasValue;

	TSINFO4CXX("ExposureBiasValue:"<<strExposureBiasValue);

	return TRUE;
}

void CKKImageEXIF::SetImageEXIF(fipImage* fImage)
{
	TSAUTO();

	for (std::map<std::string, fipTag>::iterator pi = m_exifs.begin(); pi != m_exifs.end(); pi++)
	{
		if (pi->first == "ExifRaw")
		{
			fImage->setMetadata(FIMD_EXIF_RAW, (pi->first).c_str(), pi->second);
			break;
		}		
	}
}

std::wstring CKKImageEXIF::ApplyExif(std::wstring wstrText, BOOL bIsReplaceSpecialChar, std::wstring wstrFormatChar)
{
	TSAUTO();

	std::wstring wstrRealText = wstrText;

	//替换相机厂商
	std::string strMake;
	if (GetMake(strMake) == FALSE)
	{
		strMake = "";
	}
	std::wstring wstrMake;
	wstrMake = ultra::_UTF2T(strMake);

	replace_all_distinct(wstrRealText, L"<Exif:相机厂商>", wstrMake);

	//替换相机型号
	std::string strModel;
	if (GetModel(strModel) == FALSE)
	{
		strModel = "";
	}
	std::wstring wstrModel;
	wstrModel = ultra::_UTF2T(strModel);
	replace_all_distinct(wstrRealText, L"<Exif:相机型号>", wstrModel);

	//替换拍摄日期
	std::string strDateDay;
	if (GetDateDay(strDateDay, 1) == FALSE)
	{
		strDateDay = "";
	}
	std::wstring wstrDateDay;
	wstrDateDay = ultra::_UTF2T(strDateDay);

	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（YYYY-MM-DD）>", wstrDateDay);

	if (GetDateDay(strDateDay, 2) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（YY-MM-DD）>", wstrDateDay);

	if (GetDateDay(strDateDay, 3) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（MM/DD/YYYY）>", wstrDateDay);

	if (GetDateDay(strDateDay, 4) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（MMM.DD.YY）>", wstrDateDay);

	if (GetDateDay(strDateDay, 5) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（YYYY）>", wstrDateDay);

	if (GetDateDay(strDateDay, 6) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（YY）>", wstrDateDay);

	if (GetDateDay(strDateDay, 7) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（M）>", wstrDateDay);

	if (GetDateDay(strDateDay, 8) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（MM）>", wstrDateDay);

	if (GetDateDay(strDateDay, 9) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（MMM）>", wstrDateDay);

	if (GetDateDay(strDateDay, 10) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（D）>", wstrDateDay);

	if (GetDateDay(strDateDay, 11) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（DD）>", wstrDateDay);

	if (GetDateDay(strDateDay, 12) == FALSE)
	{
		strDateDay = "";
	}
	if (strDateDay == "Monday")
	{
		wstrDateDay = L"星期一";
	}
	else if (strDateDay == "Tuesday")
	{
		wstrDateDay = L"星期二";
	}
	else if (strDateDay == "Wednesday")
	{
		wstrDateDay = L"星期三";
	}
	else if (strDateDay == "Thursday")
	{
		wstrDateDay = L"星期四";
	}
	else if (strDateDay == "Friday")
	{
		wstrDateDay = L"星期五";
	}
	else if (strDateDay == "Saturday")
	{
		wstrDateDay = L"星期六";
	}
	else if (strDateDay == "Sunday")
	{
		wstrDateDay = L"星期日";
	}
	replace_all_distinct(wstrRealText, L"<Exif:拍摄日期（DDD）>", wstrDateDay);

	//替换拍摄时间
	std::string strDateTime;
	std::wstring wstrDateTime;
	if (GetDateTime(strDateTime, 1) == FALSE)
	{
		strDateTime = "";
	}
	wstrDateTime = ultra::_UTF2T(strDateTime);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄时间（HH:MM AM/PM）>", wstrDateTime);

	if (GetDateTime(strDateTime, 2) == FALSE)
	{
		strDateTime = "";
	}
	wstrDateTime = ultra::_UTF2T(strDateTime);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄时间（HH:MM:SS）>", wstrDateTime);

	if (GetDateTime(strDateTime, 3) == FALSE)
	{
		strDateTime = "";
	}
	wstrDateTime = ultra::_UTF2T(strDateTime);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄时间（HH）>", wstrDateTime);

	if (GetDateTime(strDateTime, 4) == FALSE)
	{
		strDateTime = "";
	}
	wstrDateTime = ultra::_UTF2T(strDateTime);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄时间（MM）>", wstrDateTime);

	if (GetDateTime(strDateTime, 5) == FALSE)
	{
		strDateTime = "";
	}
	wstrDateTime = ultra::_UTF2T(strDateTime);
	replace_all_distinct(wstrRealText, L"<Exif:拍摄时间（SS）>", wstrDateTime);

	//替换光圈
	std::string strFNumber;
	if (GetFNumber(strFNumber) == FALSE)
	{
		strFNumber = "";
	}
	std::wstring wstrFNumber;
	wstrFNumber = ultra::_UTF2T(strFNumber);
	replace_all_distinct(wstrRealText, L"<Exif:光圈>", wstrFNumber);

	//替换快门
	std::string strExposureTime;
	if (GetExposureTime(strExposureTime) == FALSE)
	{
		strExposureTime = "";
	}
	std::wstring wstrExposureTime;
	wstrExposureTime = ultra::_UTF2T(strExposureTime);
	replace_all_distinct(wstrRealText, L"<Exif:快门>", wstrExposureTime);

	//替换ISO
	std::string strISO;
	if (GetISO(strISO) == FALSE)
	{
		strISO = "";
	}
	std::wstring wstrISO;
	wstrISO = ultra::_UTF2T(strISO);
	replace_all_distinct(wstrRealText, L"<Exif:ISO>", wstrISO);

	//替换曝光补偿
	std::string strExposureBiasValue;
	if (GetExposureBiasValue(strExposureBiasValue) == FALSE)
	{
		strExposureBiasValue = "";
	}
	std::wstring wstrExposureBiasValue;
	wstrExposureBiasValue = ultra::_UTF2T(strExposureBiasValue);
	replace_all_distinct(wstrRealText, L"<Exif:曝光补偿>", wstrExposureBiasValue);

	//替换焦距
	std::string strFocalLength;
	if (GetFocalLength(strFocalLength) == FALSE)
	{
		strFocalLength = "";
	}
	std::wstring wstrFocalLength;
	wstrFocalLength = ultra::_UTF2T(strFocalLength);
	replace_all_distinct(wstrRealText, L"<Exif:焦距>", wstrFocalLength);

	//替换镜头
	std::string strLens;
	if (GetLens(strLens) == FALSE)
	{
		strLens = "";
	}
	std::wstring wstrLens;
	wstrLens = ultra::_UTF2T(strLens);
	replace_all_distinct(wstrRealText, L"<Exif:镜头>", wstrLens);

	std::wstring wstrExifSummary = wstrModel + L" " + wstrFNumber + L" " + wstrExposureTime + L" " + wstrISO;
	replace_all_distinct(wstrRealText, L"<Exif:Exif摘要>", wstrExifSummary);

	if (bIsReplaceSpecialChar == TRUE)
	{
		replace_all_distinct(wstrRealText, L"\\", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"/", wstrFormatChar);
		replace_all_distinct(wstrRealText, L":", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"*", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"?", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"\"", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"<", wstrFormatChar);
		replace_all_distinct(wstrRealText, L">", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"|", wstrFormatChar);
	}

	return wstrRealText;
}

std::string CKKImageEXIF::CalculateWeekDay(int nYear, int nMonth, int nDay)
{
	int week = 0; 
	if(nMonth == 1)
	{
		nMonth = 13;
		nYear--;
	}
	if(nMonth == 2)
	{
		nMonth = 14;
		nYear--;
	}
	if((nYear < 1752) || (( nYear == 1752) && (nMonth < 9)) || ((nYear == 1752) && (nMonth == 9) && (nDay < 3))) //判断是否在1752年9月3日之前 
		week =(nDay + 2*nMonth + 3*(nMonth+1)/5 + nYear + nYear/4+5)%7; //1752年9月3日之前的公式 
	else 
		week =(nDay + 2*nMonth + 3*(nMonth + 1)/5 + nYear + nYear/4 - nYear/100 + nYear/400)%7; //1752年9月3日之后的公式 
	string weekstr = "";
	switch(week)
	{
	case 0: {weekstr="Monday"; break;}
	case 1: {weekstr="Tuesday"; break;}
	case 2: {weekstr="Wednesday"; break;}
	case 3: {weekstr="Thursday"; break;}
	case 4: {weekstr="Friday"; break;}
	case 5: {weekstr="Saturday"; break;}
	case 6: {weekstr="Sunday"; break;}
	}

	return weekstr; 
}
int CKKImageEXIF::GetOrientation()
{
	TSAUTO();

	if (m_exifs.find("Orientation") == m_exifs.end())
	{
		TSINFO4CXX("There is no Orientation Info!");
		return -1;
	}

	UINT16 nOrientation = 0;
	memcpy(&nOrientation, m_exifs["Orientation"].getValue(), m_exifs["Orientation"].getLength());
	return nOrientation;
}
