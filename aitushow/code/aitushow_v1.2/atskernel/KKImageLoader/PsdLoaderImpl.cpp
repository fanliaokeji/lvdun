#include "StdAfx.h"
#include "PsdLoaderImpl.h"

CPsdLoaderImpl::CPsdLoaderImpl(void)
{
}

CPsdLoaderImpl::~CPsdLoaderImpl(void)
{
}
XL_BITMAP_HANDLE CPsdLoaderImpl::GetXLBitmap()
{
	return m_hBitmap;
}
void CPsdLoaderImpl::GetSrcBitmapSize(int& nWidth, int& nHeight)
{	
	nWidth = m_nWidth;
	nHeight = m_nHeight;
}
inline void
SwapShort(WORD *sp) {
	WORD Value = *sp;
	WORD temp = (*sp) & 0x00FF;
	(*sp) = (*sp) >> 8;
	Value = *sp;
	temp = temp << 8;
	(*sp) = (*sp) | temp;
}


int CPsdLoaderImpl::LoadImage(const wstring& wstrFilePath, bool* pbStop, bool bScale, int nWidth, int nHeight)
{
	HANDLE hFile = CreateFile(wstrFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return 3;
	}
	// 读取头部信息
	if (!m_PsdHeaderInfo.Read(hFile)) {
		return 5;
	}
	// 读取颜色Mode;
	if (!m_PsdColourModeData.Read(hFile))
	{
		return 6;
	}

	// 读取Image Resources
	if (! ReadImageResources(hFile))
	{
		return 7;
	}

	// 读取 LayerAndMask Information

	if (!ReadLayerAndMaskInfoSection(hFile))
	{
		return 8;
	}

	if(!ReadImageData(hFile))
	{
		return 9;
	}
	return 4;
}

psdHeaderInfo::psdHeaderInfo() : _Channels(-1), _Height(-1), _Width(-1), _BitsPerChannel(-1), _ColourMode(-1) {
}

psdHeaderInfo::~psdHeaderInfo() {
}


static inline int 
psdGetValue(const BYTE * iprBuffer, const int iBytes) {
	int v = iprBuffer[0];
	for (int i=1; i<iBytes; ++i) {
		v = (v << 8) | iprBuffer[i];
	}
	return v;
}

bool psdHeaderInfo::Read(HANDLE hFile) {
	psdHeader header;
	DWORD dwRead = 0;
	BOOL bRet = ReadFile(hFile, &header, sizeof(header), &dwRead, NULL);
	if (!bRet || dwRead != sizeof(header))
	{
		return false;
	}
	
	// check the signature
	int nSignature = psdGetValue(header.Signature, sizeof(header.Signature));
	if (PSD_SIGNATURE == nSignature) {
		// check the version
		int nVersion = psdGetValue( header.Version, sizeof(header.Version) );
		if (1 == nVersion) {
			// header.Reserved must be zero
			BYTE psd_reserved[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			if(memcmp(header.Reserved, psd_reserved, 6) != 0) {
				return false;
			}
			// read the header
			_Channels = (short)psdGetValue( header.Channels, sizeof(header.Channels) );
			_Height = psdGetValue( header.Rows, sizeof(header.Rows) );
			_Width = psdGetValue( header.Columns, sizeof(header.Columns) );
			_BitsPerChannel = (short)psdGetValue( header.Depth, sizeof(header.Depth) );
			_ColourMode = (short)psdGetValue( header.Mode, sizeof(header.Mode) );

			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

psdColourModeData::psdColourModeData() : _Length(-1), _plColourData(NULL) {
}

psdColourModeData::~psdColourModeData() { 
	
}

bool psdColourModeData::Read(HANDLE hFile) {
	BYTE Length[4];
	DWORD dwRead = 0;
	BOOL bRet = ReadFile(hFile, Length, 4, &dwRead, NULL);
	if (!bRet || dwRead != 4)
	{
		return false;
	}
	_Length = psdGetValue( Length, sizeof(_Length) );
	if (_Length > 0)
	{
		LARGE_INTEGER liDistanceToMove;
		liDistanceToMove.LowPart = _Length;
		liDistanceToMove.HighPart = 0;
		SetFilePointerEx(hFile, liDistanceToMove, NULL, FILE_CURRENT);
	}
	return true;
}

bool CPsdLoaderImpl::ReadImageResources(HANDLE hFile, LONG length) {
	psdImageResource oResource;
	bool bSuccess = false;

	if(length > 0) {
		oResource._Length = length;
	} else {
		BYTE Length[4];
		DWORD dwRead = 0;
		BOOL bRet = ReadFile(hFile, Length, 4, &dwRead, NULL);
		if (!bRet || dwRead != 4)
		{
			return false;
		}
		oResource._Length = psdGetValue( Length, sizeof(oResource._Length) );
	}
	if (oResource._Length > 0)
	{
		LARGE_INTEGER liDistanceToMove;
		liDistanceToMove.LowPart = oResource._Length;
		liDistanceToMove.HighPart = 0;
		SetFilePointerEx(hFile, liDistanceToMove, NULL, FILE_CURRENT);
	}
	return true;
} 

psdImageResource::psdImageResource() : _plName (0) { 
	Reset(); 
}

psdImageResource::~psdImageResource() { 
}

void psdImageResource::Reset() {
	_Length = -1;
	memset( _OSType, '\0', sizeof(_OSType) );
	_ID = -1;
	_plName = NULL;
	_Size = -1;
}

bool CPsdLoaderImpl::ReadLayerAndMaskInfoSection(HANDLE hFile)
{
	BYTE DataLength[4];
	DWORD dwRead = 0;
	BOOL bRet = ReadFile(hFile, DataLength, 4, &dwRead, NULL);
	if (!bRet || dwRead != 4)
	{
		return false;
	}
	int nTotalBytes = psdGetValue( DataLength, sizeof(DataLength) );
	// 跳过
	if (nTotalBytes)
	{
		LARGE_INTEGER liDistanceToMove;
		liDistanceToMove.LowPart = nTotalBytes;
		liDistanceToMove.HighPart = 0;
		SetFilePointerEx(hFile, liDistanceToMove, NULL, FILE_CURRENT);
	}
	return true;
}	

XL_BITMAP_HANDLE CPsdLoaderImpl::ReadImageData(HANDLE hFile)
{
	WORD nCompression = 0;
	DWORD dwRead = 0;
	BOOL bRet = ReadFile(hFile, &nCompression, sizeof(nCompression), &dwRead, NULL);
	if(!bRet || dwRead != dwRead)
	{
		return NULL;
	}
	SwapShort(&nCompression);


	if((nCompression != PSDP_COMPRESSION_NONE && nCompression != PSDP_COMPRESSION_RLE))	{
		return NULL;
	}

	const unsigned nWidth = m_PsdHeaderInfo._Width;
	const unsigned nHeight = m_PsdHeaderInfo._Height;
	const unsigned nChannels = m_PsdHeaderInfo._Channels;
	const unsigned depth = m_PsdHeaderInfo._BitsPerChannel;
	const unsigned bytes = (depth == 1) ? 1 : depth / 8;

	// channel(plane) line (BYTE aligned)
	const unsigned lineSize = (m_PsdHeaderInfo._BitsPerChannel == 1) ? (nWidth + 7) / 8 : nWidth * bytes;

	if(nCompression == PSDP_COMPRESSION_RLE && depth > 16) {
		return NULL;
	}

	// build output buffer

	XL_BITMAP_HANDLE bitmap = NULL;
	unsigned dstCh = 0;

	short mode = m_PsdHeaderInfo._ColourMode;

	if(mode == PSDP_MULTICHANNEL && nChannels < 3) {
		// CM 
		mode = PSDP_GRAYSCALE; // C as gray, M as extra channel
	}

	bool needPalette = false;
	switch (mode) {
		case PSDP_BITMAP:
		case PSDP_DUOTONE:	
		case PSDP_INDEXED:
		case PSDP_GRAYSCALE:
			dstCh = 1;
			switch(depth) {
				case 16:
				//	bitmap = FreeImage_AllocateHeaderT(header_only, FIT_UINT16, nWidth, nHeight, depth*dstCh);
					bitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
					break;
				case 32:
				//	bitmap = FreeImage_AllocateHeaderT(header_only, FIT_FLOAT, nWidth, nHeight, depth*dstCh);
					bitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
					break;
				default: // 1-, 8-
					needPalette = true;
				//	bitmap = FreeImage_AllocateHeader(header_only, nWidth, nHeight, depth*dstCh);
					bitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
					break;
				}
			break;
		case PSDP_RGB:	
		case PSDP_LAB:		
		case PSDP_CMYK	:
		case PSDP_MULTICHANNEL	:
			// force PSDP_MULTICHANNEL CMY as CMYK
			dstCh = (mode == PSDP_MULTICHANNEL) ? 4 : min(nChannels, 4);
			if(dstCh < 3) {
				throw "Invalid number of channels";
			}

			switch(depth) {
				case 16:
				//	bitmap = FreeImage_AllocateHeaderT(header_only, dstCh < 4 ? FIT_RGB16 : FIT_RGBA16, nWidth, nHeight, depth*dstCh);
					bitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
					break;
				case 32:
				//	bitmap = FreeImage_AllocateHeaderT(header_only, dstCh < 4 ? FIT_RGBF : FIT_RGBAF, nWidth, nHeight, depth*dstCh);
					bitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
					break;
				default:
				//	bitmap = FreeImage_AllocateHeader(header_only, nWidth, nHeight, depth*dstCh);
					bitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
					break;
			}
			break;
		default:
			break;
	}
	if(!bitmap) {
		return NULL;
	}

	// Load pixels data

	const unsigned dstChannels = dstCh;
//
//	const unsigned dstBpp =  (depth == 1) ? 1 : FreeImage_GetBPP(bitmap)/8;
//	const unsigned dstLineSize = FreeImage_GetPitch(bitmap);	
//	BYTE* const dst_first_line = FreeImage_GetScanLine(bitmap, nHeight - 1);//<*** flipped
//
//	BYTE* line_start = new BYTE[lineSize]; //< fileline cache
//
	switch ( nCompression )
	{
		//case PSDP_COMPRESSION_NONE: // raw data	
		//	{			
		//		for(unsigned c = 0; c < nChannels; c++) {
		//			if(c >= dstChannels) {
		//				// @todo write extra channels
		//				break; 
		//			}

		//			const unsigned channelOffset = c * bytes;

		//			BYTE* dst_line_start = dst_first_line;
		//			for(unsigned h = 0; h < nHeight; ++h, dst_line_start -= dstLineSize) {//<*** flipped

		//				io->read_proc(line_start, lineSize, 1, handle);

		//				for (BYTE *line = line_start, *dst_line = dst_line_start; line < line_start + lineSize; 
		//					line += bytes, dst_line += dstBpp) 
		//				{
		//					memcpy(dst_line + channelOffset, line, bytes);
		//				}
		//			} //< h
		//		}//< ch

		//		SAFE_DELETE_ARRAY(line_start);

		//	}
		//	break;
//
//		case PSDP_COMPRESSION_RLE: // RLE compression	
//			{			
//
//				// The RLE-compressed data is preceeded by a 2-byte line size for each row in the data,
//				// store an array of these
//
//				// later use this array as WORD rleLineSizeList[nChannels][nHeight];
//				WORD *rleLineSizeList = new (std::nothrow) WORD[nChannels*nHeight];
//
//				if(!rleLineSizeList) {
//					FreeImage_Unload(bitmap);
//					SAFE_DELETE_ARRAY(line_start);
//					throw std::bad_alloc();
//				}	
//
//				io->read_proc(rleLineSizeList, 2, nChannels * nHeight, handle);
//
//				WORD largestRLELine = 0;
//				for(unsigned ch = 0; ch < nChannels; ++ch) {
//					for(unsigned h = 0; h < nHeight; ++h) {
//						const unsigned index = ch * nHeight + h;
//
//#ifndef FREEIMAGE_BIGENDIAN 
//						SwapShort(&rleLineSizeList[index]);
//#endif
//						if(largestRLELine < rleLineSizeList[index]) {
//							largestRLELine = rleLineSizeList[index];
//						}
//					}
//				}
//
//				BYTE* rle_line_start = new (std::nothrow) BYTE[largestRLELine];
//				if(!rle_line_start) {
//					FreeImage_Unload(bitmap);
//					SAFE_DELETE_ARRAY(line_start);
//					SAFE_DELETE_ARRAY(rleLineSizeList);
//					throw std::bad_alloc();
//				}
//
//				// Read the RLE data (assume 8-bit)
//
//				const BYTE* const line_end = line_start + lineSize;
//
//				for (unsigned ch = 0; ch < nChannels; ch++) {
//					const unsigned channelOffset = ch * bytes;
//
//					BYTE* dst_line_start = dst_first_line;
//					for(unsigned h = 0; h < nHeight; ++h, dst_line_start -= dstLineSize) {//<*** flipped
//						const unsigned index = ch * nHeight + h;
//
//						// - read and uncompress line -
//
//						const WORD rleLineSize = rleLineSizeList[index];
//
//						io->read_proc(rle_line_start, rleLineSize, 1, handle);
//
//						for (BYTE* rle_line = rle_line_start, *line = line_start; 
//							rle_line < rle_line_start + rleLineSize, line < line_end;) {
//
//								int len = *rle_line++;
//
//								// NOTE len is signed byte in PackBits RLE
//
//								if ( len < 128 ) { //<- MSB is not set
//									// uncompressed packet
//
//									// (len + 1) bytes of data are copied
//
//									++len;
//
//									// assert we don't write beyound eol
//									memcpy(line, rle_line, line + len > line_end ? line_end - line : len);
//									line += len;
//									rle_line += len;
//								}
//								else if ( len > 128 ) { //< MSB is set
//
//									// RLE compressed packet
//
//									// One byte of data is repeated (–len + 1) times
//
//									len ^= 0xFF; // same as (-len + 1) & 0xFF 
//									len += 2;    //
//
//									// assert we don't write beyound eol
//									memset(line, *rle_line++, line + len > line_end ? line_end - line : len);							
//									line += len;
//
//								}
//								else if ( 128 == len ) {
//									// Do nothing
//								}
//						}//< rle_line
//
//						// - write line to destination -
//
//						if(ch >= dstChannels) {
//							// @todo write to extra channels
//							break; 
//						}
//
//						// byte by byte copy a single channel to pixel
//						for (BYTE *line = line_start, *dst_line = dst_line_start; line < line_start + lineSize; 
//							line += bytes, dst_line += dstBpp) {
//
//#ifdef FREEIMAGE_BIGENDIAN
//								memcpy(dst_line + channelOffset, line, bytes);
//#else
//								// reverse copy bytes
//								for (unsigned b = 0; b < bytes; ++b) {
//									dst_line[channelOffset + b] = line[(bytes-1) - b];							
//								}
//#endif // FREEIMAGE_BIGENDIAN
//						}	
//					}//< h
//				}//< ch
//
//				SAFE_DELETE_ARRAY(line_start);
//				SAFE_DELETE_ARRAY(rleLineSizeList);
//				SAFE_DELETE_ARRAY(rle_line_start);
//			}
//			break;
//
		case 2: // ZIP without prediction, no specification
//			break;
//
		case 3: // ZIP with prediction, no specification
//			break;
//
		default: // Unknown format
			break;
//
	}
//
//	// --- Further process the bitmap ---
//
//	if((mode == PSDP_CMYK || mode == PSDP_MULTICHANNEL)) {	
//		// CMYK values are "inverted", invert them back		
//
//		if(mode == PSDP_MULTICHANNEL) {
//			invertColor(bitmap);
//		} else {
//			FreeImage_Invert(bitmap);
//		}
//
//		if((_fi_flags & PSD_CMYK) == PSD_CMYK) {
//			// keep as CMYK
//
//			if(mode == PSDP_MULTICHANNEL) {
//				//### we force CMY to be CMYK, but CMY has no ICC. 
//				// Create empty profile and add the flag.
//				FreeImage_CreateICCProfile(bitmap, NULL, 0);
//				FreeImage_GetICCProfile(bitmap)->flags |= FIICC_COLOR_IS_CMYK;
//			}
//		}
//		else { 
//			// convert to RGB
//
//			ConvertCMYKtoRGBA(bitmap);
//
//			// The ICC Profile is no longer valid
//			_iccProfile.clear();
//
//			// remove the pending A if not present in source 
//			if(nChannels == 4 || nChannels == 3 ) {
//				FIBITMAP* t = RemoveAlphaChannel(bitmap);
//				if(t) {
//					FreeImage_Unload(bitmap);
//					bitmap = t;
//				} // else: silently fail
//			}
//		}
//	}
//	else if ( mode == PSDP_LAB && !((_fi_flags & PSD_LAB) == PSD_LAB)) {
//		ConvertLABtoRGB(bitmap);
//	}
//	else {
//		if (needPalette && FreeImage_GetPalette(bitmap)) {
//
//			if(mode == PSDP_BITMAP) {
//				CREATE_GREYSCALE_PALETTE_REVERSE(FreeImage_GetPalette(bitmap), 2);
//			}
//			else if(mode == PSDP_INDEXED) {
//				if(!_colourModeData._plColourData || _colourModeData._Length != 768 || _ColourCount < 0) {
//					FreeImage_OutputMessageProc(_fi_format_id, "Indexed image has no palette. Using the default grayscale one.");
//				} else {
//					_colourModeData.FillPalette(bitmap);
//				}
//			}
//			// GRAYSCALE, DUOTONE - use default grayscale palette
//		}
//
//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
//		if(FreeImage_GetImageType(bitmap) == FIT_BITMAP) {
//			SwapRedBlue32(bitmap);
//		}
//#endif
//	}
	return NULL;
}