#include "stdafx.h"
#include "exif_thumbnail_length.h"

unsigned get_unsigned_int( const unsigned char*  data_ptr, bool is_little_endian )
{
	unsigned ret;
	if( is_little_endian )
	{
		ret = data_ptr[3];
		ret <<= 8;
		ret += data_ptr[2];
		ret <<= 8;
		ret += data_ptr[1];
		ret <<= 8;
		ret += data_ptr[0];
	}
	else
	{
		ret = data_ptr[0];
		ret <<= 8;
		ret += data_ptr[1];
		ret <<= 8;
		ret += data_ptr[2];
		ret <<= 8;
		ret += data_ptr[3];
	}
	return ret;
}

int get_unsigned_word( const unsigned char*  data_ptr, bool is_little_endian )
{
	int ret;
	if( is_little_endian )
	{
		ret = data_ptr[1];
		ret <<= 8;
		ret += data_ptr[0];
	}
	else
	{
		ret = data_ptr[0];
		ret <<= 8;
		ret += data_ptr[1];
	}
	return ret;
}


int get_thumbnail_length( const unsigned char* data_ptr, unsigned data_len )
{
	if( data_len < 14 )
		return -1;
	char exif_id[6];
	::memcpy(exif_id,data_ptr,6);
	if( (::strcmp(exif_id,"Exif") !=0) || (exif_id[5]!='\0') )
		return 0;
	const unsigned tiff_header_offset=6;
	bool is_little_endian;
	if( (data_ptr[tiff_header_offset] == 'I') && (data_ptr[tiff_header_offset+1] == 'I') )
		is_little_endian = true;
	else if( (data_ptr[tiff_header_offset] == 'M') && (data_ptr[tiff_header_offset+1] == 'M') )
		is_little_endian = false;
	else
		return 0;
	int magic_number = get_unsigned_word( data_ptr+tiff_header_offset+2, is_little_endian );
	if( magic_number != 42 )
		return 0;
	unsigned offset = get_unsigned_int( data_ptr+tiff_header_offset+4,is_little_endian );
	offset += tiff_header_offset;
	if( offset +2 > data_len )
		return 0;
	unsigned number_of_tags = get_unsigned_word( data_ptr+offset,is_little_endian) ;
	if( number_of_tags == 0 )
		return 0;
	offset += 2;
	unsigned next_IFD_offset_index = offset + 12*number_of_tags;
	if( next_IFD_offset_index+4 > data_len )
		return 0;
	unsigned next_IFD_offset = get_unsigned_int( data_ptr+next_IFD_offset_index, is_little_endian );
	if( next_IFD_offset == 0 )
		return 0;
	next_IFD_offset += tiff_header_offset;
	if( data_len > next_IFD_offset )
		return data_len - next_IFD_offset;
	else
		return 0;
}