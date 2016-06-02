#pragma once
#include "..\ImageHelper\KKImageDoc.h"


#define KKIMAGE_LUADOC_CLASSNAME "KKImage.Doc.Class"

class CLuaKKImageDoc
{
public:
	CLuaKKImageDoc(void);
	~CLuaKKImageDoc(void);

	static void RegisterLuaClass(XL_LRT_ENV_HANDLE hEnv);
	static int AddRef(lua_State* luaState);
	static int Release(lua_State* luaState);
	static int GetGifObj(lua_State* luaState);			// 获取Gif对象
	static int GetAdaptedBitmap(lua_State* luaState);	// 获取适屏大小的位图
	static int SetSrcBitmap(lua_State* luaState);		// 设置原始位图
	static int GetSrcBitmapSize(lua_State* luaState);	// 获取原始位图
	static int ReleaseSrcBitmap(lua_State* luaState);	// 释放原始位图
	static int GetSrcBitmap(lua_State* luaState);		// 获取图像原始大小的位图

	static int GetDocId(lua_State* luaState);			// 获取文档对象ID
	static int GetDocType(lua_State* luaState);			// 获取文档类型，用于判断是单图文档还是多图文档
	static int SetExifInfoStatus(lua_State* luaState);	
	static int GetExifInfoStatus(lua_State* luaState);

	static int LeftRotate(lua_State* luaState);			// 左旋
	static int RightRotate(lua_State* luaState);		// 右旋
	static int ResetRotate(lua_State* luaState);		// 重置旋转角度
	static int GetRotateAngle(lua_State* luaState);
	// 设置获取Exif信息
	static int GetFileName(lua_State* luaState);
	static int SetFileName(lua_State* luaState);
	static int GetFilePath(lua_State* luaState);
	static int SetFilePath(lua_State* luaState);
	static int GetFileType(lua_State* luaState);
	static int GetFileSize(lua_State* luaState);
	static int GetFileImageSize(lua_State* luaState);
	static int SetDateTimeOriginal(lua_State* luaState);	// 设置拍摄时间
	static int GetDateTimeOriginal(lua_State* luaState);	// 获取拍摄时间

	static int SetMake(lua_State* luaState);				// 设置设备厂商
	static int GetMake(lua_State* luaState);				// 获取设备厂商

	static int SetModel(lua_State* luaState);				// 设置设备型号
	static int GetModel(lua_State* luaState);				// 获取设备型号

	static int SetLensType(lua_State* luaState);			// 设置镜头信息
	static int GetLensType(lua_State* luaState);			// 获取镜头信息

	static int SetShutterCount(lua_State* luaState);		// 设置快门次数
	static int GetShutterCount(lua_State* luaState);		// 获取快门次数

	static int SetShutterSpeed(lua_State* luaState);		// 设置快门速度
	static int GetShutterSpeed(lua_State* luaState);		// 获取快门速度

	static int SetFNumber(lua_State* luaState);				// 设置光圈值
	static int GetFNumber(lua_State* luaState);				// 获取光圈值

	static int SetFocalLength(lua_State* luaState);			// 设置焦距
	static int GetFocalLength(lua_State* luaState);			// 获取焦距

	static int SetISO(lua_State* luaState);					// 设置ISO
	static int GetISO(lua_State* luaState);					// 获取ISO

	static int SetExposureCompensation(lua_State* luaState);	// 设置曝光补偿
	static int GetExposureCompensation(lua_State* luaState);	// 获取曝光补偿

	static int SetFlashStatus(lua_State* luaState);			// 设置闪光灯状态
	static int GetFlashStatus(lua_State* luaState);			// 获取闪光灯状态

	static int SetWhiteBalance(lua_State* luaState);			// 设置白平衡
	static int GetWhiteBalance(lua_State* luaState);			// 获取白平衡

	static int SetExposureProgram(lua_State* luaState);			// 设置拍摄模式
	static int GetExposureProgram(lua_State* luaState);			// 获取拍摄模式

	static int SetMeteringMode(lua_State* luaState);			// 设置测光模式
	static int GetMeteringMode(lua_State* luaState);			// 获取测光模式

public:
	void SetKKImageDoc(CKKImageDoc* pKKImageDoc)	// 设置内部文档对象
	{
		m_pKKImageDoc = pKKImageDoc;
	}
	CKKImageDoc* m_pKKImageDoc;
private:
	// 引用相关
	int m_nRef;
	int AddRef()
	{
		m_nRef++;
		return m_nRef;
	}
	int Release()
	{
		m_nRef--;
		int nTempRef = m_nRef;
		if (m_nRef == 0)
		{
			delete this;
			return 0;
		}
		return nTempRef;
	}

private:
	//
};
