#pragma once
#include "imageloaderimpl.h"

// PSD signature (= '8BPS')
#define PSD_SIGNATURE	0x38425053
// PSD compression schemes
#define PSDP_COMPRESSION_NONE	0	// Raw data
#define PSDP_COMPRESSION_RLE	1	// RLE compression (same as TIFF packed bits)

// PSD color modes
#define PSDP_BITMAP			0
#define PSDP_GRAYSCALE		1
#define PSDP_INDEXED		2
#define PSDP_RGB			3
#define PSDP_CMYK			4
#define PSDP_MULTICHANNEL	7
#define PSDP_DUOTONE		8
#define PSDP_LAB			9

typedef struct psdHeader {
	BYTE Signature[4];	//! Always equal 8BPS, do not try to read the file if the signature does not match this value.
	BYTE Version[2];	//! Always equal 1, do not read file if the version does not match this value.
	char Reserved[6];	//! Must be zero.
	BYTE Channels[2];	//! Number of channels including any alpha channels, supported range is 1 to 24.
	BYTE Rows[4];		//! The height of the image in pixels. Supported range is 1 to 30,000.
	BYTE Columns[4];	//! The width of the image in pixels. Supported range is 1 to 30,000.
	BYTE Depth[2];		//! The number of bits per channel. Supported values are 1, 8, and 16.
	BYTE Mode[2];		//! Colour mode of the file, Bitmap=0, Grayscale=1, Indexed=2, RGB=3, CMYK=4, Multichannel=7, Duotone=8, Lab=9. 
} psdHeader;

class psdHeaderInfo {
public:
	short _Channels;	//! Numer of channels including any alpha channels, supported range is 1 to 24.
	int   _Height;		//! The height of the image in pixels. Supported range is 1 to 30,000.
	int   _Width;		//! The width of the image in pixels. Supported range is 1 to 30,000.
	short _BitsPerChannel;//! The number of bits per channel. Supported values are 1, 8, and 16.
	short _ColourMode;	//! Colour mode of the file, Bitmap=0, Grayscale=1, Indexed=2, RGB=3, CMYK=4, Multichannel=7, Duotone=8, Lab=9. 

public:
	psdHeaderInfo();
	~psdHeaderInfo();
	bool Read(HANDLE hFile);
};


class psdColourModeData {
public:
	int _Length;			//! The length of the following color data
	BYTE * _plColourData;	//! The color data

public:
	psdColourModeData();
	~psdColourModeData();
	bool Read(HANDLE hFile);
	//bool FillPalette(FIBITMAP *dib);
};

class psdImageResource {
public:
	int     _Length;
	char    _OSType[4];	//! Photoshop always uses its signature, 8BIM
	short   _ID;		//! Unique identifier. Image resource IDs on page 8
	BYTE * _plName;		//! A pascal string, padded to make size even (a null name consists of two bytes of 0)
	int     _Size;		//! Actual size of resource data. This does not include the Type, ID, Name or Size fields.

public:
	psdImageResource();
	~psdImageResource();
	void Reset();
};




class CPsdLoaderImpl :
	public CImageLoaderImpl
{
public:
	CPsdLoaderImpl(void);
	~CPsdLoaderImpl(void);
	virtual int LoadImage(const wstring& wstrFilePath, bool* pbStop = NULL, bool bScale = false, int nWidth = 0, int nHeight = 0);
	virtual void GetSrcBitmapSize(int& nWidth, int& nHeight);
	virtual XL_BITMAP_HANDLE GetXLBitmap();

private:
	XL_BITMAP_HANDLE m_hBitmap;
	int	m_nWidth;
	int m_nHeight;
	
	// 读取Image资源信息
	bool ReadImageResources( HANDLE hFile, LONG length=0);
	// 读取图层信息
	bool ReadLayerAndMaskInfoSection(HANDLE hFile);
	// 读取位图信息
	XL_BITMAP_HANDLE ReadImageData(HANDLE hFile);

	psdHeaderInfo m_PsdHeaderInfo;	// 文件头部信息
	psdColourModeData	m_PsdColourModeData;	// 颜色mode信息
};
