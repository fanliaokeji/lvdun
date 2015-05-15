#pragma once
#include <string>
#include <cstring>

#include <zlib/zlib.h>

class GZipDecompressor {
	z_stream strm;
	char out_buffer[1024];
	int error;
	std::size_t total_out;
public:
	GZipDecompressor() : error(Z_OK), total_out(0) {
		std::memset(&strm, 0, sizeof(strm));
		strm.next_in = NULL;
		strm.next_out = NULL;
		strm.avail_in = 0;
		strm.avail_out = 0;
		strm.total_in = 0;
		strm.total_out = 0;
		strm.zalloc = Z_NULL;
		strm.zfree  = Z_NULL;
		strm.opaque = Z_NULL;
		this->error = inflateInit2(&strm, MAX_WBITS + 16);
	}

	int Decompress(const char* input_buffer, std::size_t input_size, std::string& out, bool append = true) {
		if(input_buffer == NULL || input_size == 0) {
			return Z_BUF_ERROR;
		}

		if(this->error != Z_OK) {
			return this->error;
		}

		if(!append) {
			out.clear();
		}

		strm.next_in = (Bytef*)(input_buffer);
		strm.avail_in = input_size;

		int err = Z_OK;

		while(strm.avail_in != 0) {
			strm.next_out = (Bytef*)this->out_buffer;
			strm.avail_out = sizeof(this->out_buffer);
			err = inflate(&strm, Z_NO_FLUSH);
			if(err != Z_OK && err != Z_STREAM_END) {
				break;
			}

			std::size_t output_size = static_cast<std::size_t>(strm.total_out) - this->total_out;
			if(output_size > 0) {
				total_out += output_size;
				out.append(out_buffer, out_buffer + output_size);
			}

			if(err == Z_STREAM_END) {
				if(strm.avail_in != 0) {
					// err = Z_DATA_ERROR;
				}
				break;
			}
		}

		return err;
	}

	~GZipDecompressor() {
		inflateEnd(&this->strm);
	}
};

class DeflateDecompressor {
	z_stream strm;
	char out_buffer[1024];
	int error;
	std::size_t total_out;
public:
	DeflateDecompressor() : error(Z_OK), total_out(0) {
		std::memset(&strm, 0, sizeof(strm));
		strm.next_in = NULL;
		strm.next_out = NULL;
		strm.avail_in = 0;
		strm.avail_out = 0;
		strm.total_in = 0;
		strm.total_out = 0;
		strm.zalloc = Z_NULL;
		strm.zfree  = Z_NULL;
		strm.opaque = Z_NULL;
		this->error = inflateInit2(&strm, -15);
	}

	int Decompress(const char* input_buffer, std::size_t input_size, std::string& out, bool append = true) {
		if(input_buffer == NULL || input_size == 0) {
			return Z_BUF_ERROR;
		}

		if(this->error != Z_OK) {
			return this->error;
		}

		if(!append) {
			out.clear();
		}

		strm.next_in = (Bytef*)(input_buffer);
		strm.avail_in = input_size;

		int err = Z_OK;

		while(strm.avail_in != 0) {
			strm.next_out = (Bytef*)this->out_buffer;
			strm.avail_out = sizeof(this->out_buffer);
			err = inflate(&strm, Z_NO_FLUSH);
			if(err != Z_OK && err != Z_STREAM_END) {
				break;
			}

			std::size_t output_size = static_cast<std::size_t>(strm.total_out) - this->total_out;
			if(output_size > 0) {
				total_out += output_size;
				out.append(out_buffer, out_buffer + output_size);
			}

			if(err == Z_STREAM_END) {
				if(strm.avail_in != 0) {
					// err = Z_DATA_ERROR;
				}
				break;
			}
		}

		return err;
	}

	~DeflateDecompressor() {
		inflateEnd(&this->strm);
	}
};
