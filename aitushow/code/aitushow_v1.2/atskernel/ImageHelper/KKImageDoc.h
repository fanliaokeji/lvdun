#pragma once
#include "..\KKImageLoader\KKImageEXIF.h"
#include "..\KKImageLoader\KKImageInfo.h"


class CKKImageDoc
{
public:
	
	CKKImageDoc(void);
	~CKKImageDoc(void);	
	void SetGifObj(XLGP_GIF_HANDLE hGifObj);
	XLGP_GIF_HANDLE GetGifObj();
	
	void SetSrcBitmap(XL_BITMAP_HANDLE hXLBitmap);		// 设置原始位图
	XL_BITMAP_HANDLE GetSrcBitmap();					// 获取原始位图
	void GetSrcBitmapSize(int& nWidth, int& nHeight);	// 获取原始位图的大小
	void ReleaseSrcBitmap();		// 释放原始位图							

	
	void SetAdaptedBitmap(XL_BITMAP_HANDLE hXLBitmap);		// 设置适屏位图
	XL_BITMAP_HANDLE GetAdaptedBitmap();					// 获取适屏位图
	
	void SetExifInfo(CKKImageEXIF* pExifInfo);		// 设置Exif信息
	void SetImageInfo(CKKImageInfo* pImageInfo);	// 设置图片文件信息
	
	void SetExifInfoStatus(int nExifInfoStatus);
	int GetExifInfoStatus();			// 获取exif信息状态
	void SetDocType(int nType);		// 设置文档类型
	int GetDocType();				// 获取文档类型
	void ResetRotate();
	void ResetRotateAngle();
	void RotateBitmap(int nOffsetAngle);					// 旋转位图
	int GetRotateAngle();									// 获取旋转角度
	
	// 图片信息相关
	wstring GetFileName();
	void SetFileName(wstring& wstrFileName);
	wstring GetFilePath();
	void SetFilePath(wstring& wstrFilePath);
	wstring GetFileType();
	DWORD GetFileSize();
	void GetFileImageSize(int& nWidth, int& nHeight);
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

	void SetExposureCompensation(wstring& wstrExposureCompensation);
	wstring GetExposureCompensation();

	void SetFlashStatus(int nFlashStatus);
	int GetFlashStatus();

	void SetWhiteBalance(wstring& wstrWhiteBalance);
	wstring GetWhiteBalance();

	void SetExposureProgram(wstring& wstrExposureProgram);
	wstring GetExposureProgram();

	void SetMeteringMode(wstring& wstrMeteringMode);
	wstring GetMeteringMode();


	int CreateDocId();
	int GetDocId()
	{
		return m_nDocId;
	}
private:
	int m_nDocId;
	int m_nDocType;	//单图还是多图 0:未知 1:单图 2:多图
	// 普通位图信息
	XL_BITMAP_HANDLE m_hSrcBitmap;
	XL_BITMAP_HANDLE m_hAdaptedBitmap;	// 对原始图像适屏后的图像
	int m_nRotateAngle;		// 旋转角度

	// Gif
	XLGP_GIF_HANDLE m_hGifObj;

	// 图片信息相关
	CKKImageEXIF* m_KKImageEXIF; 
	CKKImageInfo* m_KKImageInfo;
private:
	//
};
