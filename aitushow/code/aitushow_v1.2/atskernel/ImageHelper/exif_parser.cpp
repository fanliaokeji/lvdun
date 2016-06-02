#include "StdAfx.h"
#include "exif_parser.h"

unsigned get_unsigned_int( unsigned char* data_ptr, bool is_little_endian )
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

int get_unsigned_word( unsigned char* data_ptr, bool is_little_endian )
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



/*
	type:
	1	BYTE		1byte;
	2	ASCII		1byte;
	3	SHORT		2byte;
	4	LONG		4byte;
	5	RATIONAL	8byte;
	7	UNDEFINED	1byte;
	9	SLONG		4byte;
	10	SRATIONAL	8byte;
	*/
//返回0表示解析成功，其他值失败。data_ptr指向的是0XFFE1(APP1)+2字节段长度之后的数据
int unencode_exif_data( unsigned char* data_ptr, unsigned data_len, exif_struct* result )
{
	::memset( result, 0 ,sizeof(exif_struct) );
	result->color_space = 1;
	result->oriention = 1;
	if( data_len < 14 )
		return -1;
	char exif_id[6];
	::memcpy(exif_id,data_ptr,6);
	if( (::strcmp(exif_id,"Exif") !=0) || (exif_id[5]!='\0') )
		return -1;
	const unsigned tiff_header_offset=6;
	bool is_little_endian;
	if( (data_ptr[tiff_header_offset] == 'I') && (data_ptr[tiff_header_offset+1] == 'I') )
		is_little_endian = true;
	else if( (data_ptr[tiff_header_offset] == 'M') && (data_ptr[tiff_header_offset+1] == 'M') )
		is_little_endian = false;
	else
		return -1;
	int magic_number = get_unsigned_word( data_ptr+tiff_header_offset+2, is_little_endian );
	if( magic_number != 42 )
		return -1;
	unsigned offset = get_unsigned_int( data_ptr+tiff_header_offset+4,is_little_endian );
	offset += tiff_header_offset;
	if( offset +2 > data_len )
		return -1;
	unsigned number_of_tags = get_unsigned_word( data_ptr+offset,is_little_endian) ;
	if( number_of_tags == 0 )
		return -1;
	offset += 2;
	unsigned next_IFD_offset_index = offset + 12*number_of_tags;
	if( next_IFD_offset_index+4 > data_len )
		return -1;
	unsigned next_IFD_offset = get_unsigned_int( data_ptr+next_IFD_offset_index, is_little_endian );
	unsigned thumbnail_ifd_offset = next_IFD_offset;
	unsigned temp=0;
	do
	{
		//print_one_filed(data_ptr,offset,is_little_endian,tiff_header_offset);
		int tag = get_unsigned_word( data_ptr+offset,is_little_endian);
		if( tag == 0X8769 )
		{
			//记住exif field的offset
			temp = offset;
		}
		if( tag == 274 ) // Oriention
		{
			int type = get_unsigned_word( data_ptr+offset+2,is_little_endian);
			int count = get_unsigned_int( data_ptr+offset+4,is_little_endian);
			if( (type == 3) && (count == 1) )
			{
				result->oriention = get_unsigned_word( data_ptr+offset+8,is_little_endian );
			}	
		}
		if( tag == 271 ) //make
		{
			int type = get_unsigned_word( data_ptr+offset+2,is_little_endian);
			int count = get_unsigned_int( data_ptr+offset+4,is_little_endian);
			if( type == 2 )
			{
				unsigned int value_offset;
				if( count <=4 )
					value_offset = offset+8;
				else
				{
					value_offset = get_unsigned_int( data_ptr+offset+8,is_little_endian );
					value_offset += tiff_header_offset;
				}
				unsigned str_len = 255;
				if( str_len > count )
					str_len = count;
				::strncpy( result->manufactor, (char*)(data_ptr+value_offset),str_len );
				result->manufactor[str_len] = 0;
			}
		}
		offset += 12;
		--number_of_tags;
	}while(number_of_tags);

	
	if( temp == 0 )
		return -1;
	offset = temp;
	offset += 8;
	offset = get_unsigned_int(data_ptr+offset,is_little_endian);
	offset += tiff_header_offset;
	if( offset+2 > data_len )
		return -1;
	number_of_tags = get_unsigned_word(data_ptr+offset,is_little_endian);
	if( number_of_tags == 0 )
		return -1;
	offset += 2;

	next_IFD_offset_index = offset + 12*number_of_tags;
	if( next_IFD_offset_index+4 > data_len )
		return -1;
	next_IFD_offset = get_unsigned_int( data_ptr+next_IFD_offset_index, is_little_endian );
	
	do
	{
		int tag = get_unsigned_word( data_ptr+offset,is_little_endian);
		if( tag == 40961 ) //color space
		{
			int type = get_unsigned_word( data_ptr+offset+2,is_little_endian);
			int count = get_unsigned_int( data_ptr+offset+4,is_little_endian);
			if( (type == 3) && (count == 1) )
			{
				result->color_space = get_unsigned_word( data_ptr+offset+8,is_little_endian );
			}	
		}
		offset += 12;
		--number_of_tags;
	}while(number_of_tags);
	
	return 0;
}