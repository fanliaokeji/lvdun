#pragma once

#include <string>

class CKKImageInfo
{
public:
	CKKImageInfo();
	~CKKImageInfo();

public:
	void Init();

	void SetFilePath(std::wstring strFilePath);
	std::wstring GetFilePath();

	void SetFileSize(DWORD nFileSize);
	DWORD GetFileSize();

	void SetFileName(std::wstring strFileName);
	std::wstring GetFileName();

	void SetCreateTime(std::wstring strCreateTime);
	std::wstring GetCreateTime();

	void SetLastEditTime(std::wstring strLastEditTime);
	std::wstring GetLastEditTime();

	void SetImageSize(int nWidth, int nHeight);
	void GetImageSize(int& nWidth, int& nHeight);

	void SetExifInfoStatus(int nExifInfoStatus);
	int GetExifInfoStatus();

	void SetDateTimeOriginal(wstring& wstrDateTimeOriginal);
	wstring GetDateTimeOriginal();

	void SetMake(wstring& wstrMake);
	wstring GetMake();

	void SetModel(wstring& wstrModel);
	wstring GetModel();

	void SetLensType(wstring& wstrLensType);
	wstring GetLensType();

	void SetShutterCount(wstring& wstrShutterCount);
	wstring GetShutterCount();

	void SetShutterSpeed(wstring& wstrLensType);
	wstring GetShutterSpeed();

	void SetFNumber(wstring& wstrFNumber);
	wstring GetFNumber();
	
	void SetFocalLength(wstring& wstrFocalLength);
	wstring GetFocalLength();

	void SetISO(wstring& wstrISO);
	wstring GetISO();
	
	void SetExposureCompensation(wstring& wstrfExposureCompensation);
	wstring GetExposureCompensation();

	void SetFlashStatus(int nFlashStatus);
	int GetFlashStatus();

	void SetWhiteBalance(wstring& wstrWhiteBalance);
	wstring GetWhiteBalance();

	void SetExposureProgram(wstring& wstrExposureProgram);
	wstring GetExposureProgram();

	void SetMeteringMode(wstring& wstrMeteringMode);
	wstring GetMeteringMode();

private:
	std::wstring m_strFilePath;	// 图片文件路径
	DWORD m_nFileSize;			//	图片文件大小
	std::wstring m_strFileName;	// 图片文件名称
	std::wstring m_strCreateTime;	// 图片文件创建时间
	std::wstring m_strLastEditTime;	// 图片文件最后修改时间
	int m_nWidth;					// 图片的原始宽度
	int m_nHeight;					// 图片的原始高度
	int m_nExifInfoStatus;				// 图片的Exif信息状态，-1：不知道有没有 0：没有 1：有，但是还没有获取到，2 有，且已经获取到了
	wstring m_wstrDateTimeOriginal;	// 拍摄时间
	wstring m_wstrMake;					// 相机厂商
	wstring m_wstrModel;				// 相机型号
	wstring m_wstrLensType;				// 相机镜头
	wstring m_wstrShutterCount;				// 快门次数
	wstring m_wstrShutterSpeed;			// 快门速度
	wstring m_wstrFNumber;					// 光圈值
	wstring m_wstrFocalLength;			// 焦距
	wstring m_wstrISO;							// ISO
	wstring m_wstrExposureCompensation;		// 曝光补偿
	int m_nFlashStatus;					// 闪光灯状态；-1 不知道 0 关，1开
	wstring m_wstrWhiteBalance;			// 白平衡
	wstring m_wstrExposureProgram;		// 拍摄模式
	wstring m_wstrMeteringMode;			// 测光模式
private:
	//
};
