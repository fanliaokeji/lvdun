#pragma once

struct exif_struct
{
	int oriention;
	int color_space;
	char manufactor[256];
};

unsigned get_unsigned_int( unsigned char* data_ptr, bool is_little_endian );
int get_unsigned_word( unsigned char* data_ptr, bool is_little_endian );
int unencode_exif_data( unsigned char* data_ptr, unsigned data_len, exif_struct* result );
