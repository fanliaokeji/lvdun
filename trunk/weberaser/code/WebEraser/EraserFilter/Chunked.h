#pragma once

// RFC 2616 3.6.1 Chunked Transfer Coding

// Chunked-Body   = *chunk
//                         last-chunk
//                         trailer
//                         CRLF

// chunk          = chunk-size [ chunk-extension ] CRLF
//                  chunk-data CRLF
// chunk-size     = 1*HEX
// last-chunk     = 1*("0") [ chunk-extension ] CRLF

// chunk-extension= *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
// chunk-ext-name = token
// chunk-ext-val  = token | quoted-string
// chunk-data     = chunk-size(OCTET)
// trailer        = *(entity-header CRLF)

// token          = 1*<any CHAR except CTLs or separators>
// CHAR           = <any US-ASCII character (octets 0 - 127)>
// CTL            = <any US-ASCII control character (octets 0 - 31) and DEL (127)>
// separators     = "(" | ")" | "<" | ">" | "@"
//                      | "," | ";" | ":" | "\" | <">
//                      | "/" | "[" | "]" | "?" | "="
//                      | "{" | "}" | SP | HT
// quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
// qdtext         = <any TEXT except <">>
// quoted-pair    = "\" CHAR
// TEXT           = <any OCTET except CTLs, but including LWS>
// LWS            = [CRLF] 1*( SP | HT )
// OCTET          = <any 8-bit sequence of data>

// 暂时不支持校验带trailer的数据

#include <cstddef>
#include <cassert>
#include <list>
#include <utility>
#include <iterator>

class ChunkChecker {
	int state;
	std::size_t current_chunk_size;
	std::size_t current_chunk_size_of_read;
private:
	static bool is_ascii_char(char ch) {
		return static_cast<unsigned char>(ch) < 128;
	}

	static bool is_separator(char ch) {
		return ch == '(' || ch == ')'
			|| ch == '<' || ch == '>'
			|| ch == '@' || ch == ','
			|| ch == ';' || ch == ':'
			|| ch == '\\' || ch == '"'
			|| ch == '/' || ch == '['
			|| ch == ']' || ch == '?'
			|| ch == '=' || ch == '{'
			|| ch == '}' || ch == ' '
			|| ch == '\t';
	}

	static bool is_ascii_ctl(char ch) {
		return static_cast<unsigned char>(ch) < 32 || static_cast<unsigned char>(ch) == 127;
	}
public:
	ChunkChecker() : state(0), current_chunk_size(0), current_chunk_size_of_read(0) {}

	void Reset() {
		this->state = 0;
		this->current_chunk_size = 0;
		this->current_chunk_size_of_read = 0;
	}

	bool IsAllChunkRead() const {
		return this->state == 100;
	}

	bool IsFailed() const {
		return this->state == -1;
	}

	template<typename ForwordIterator>
	bool Check(ForwordIterator begin, ForwordIterator end) {
		assert(!this->IsFailed() && sizeof(typename std::iterator_traits<ForwordIterator>::value_type) == sizeof(char));
		for(ForwordIterator iter = begin; iter != end; ++iter) {
			switch(this->state) {
			case 0:
				if(std::isxdigit(static_cast<unsigned char>(*iter))) {
					this->current_chunk_size = (*iter) >= 'A' ? std::toupper(static_cast<unsigned char>(*iter)) - 'A' + 10 : *iter - '0';
					this->current_chunk_size_of_read = 0;
					this->state = 1;
					continue;
				}
				break;
			case 1:
				if(this->current_chunk_size != 0 && std::isxdigit(static_cast<unsigned char>(*iter))) {
					this->current_chunk_size = this->current_chunk_size * 16 + ((*iter) >= 'A' ? std::toupper(static_cast<unsigned char>(*iter)) - 'A' + 10 : *iter - '0');
					continue;
				}
				else if (*iter == ';') {
					this->state = 2;
					continue;
				}
				else if(*iter == '\r') {
					this->state = 11;
					continue;
				}
				break;
			case 2:
				if(is_ascii_char(*iter) && !is_ascii_ctl(*iter) && !is_separator(*iter)) {
					this->state = 3;
					continue;
				}
				break;
			case 3:
				if(*iter == '=') {
					this->state = 4;
					continue;
				}
				else if(*iter == '\r') {
					this->state = 11;
					continue;
				}
				else if(is_ascii_char(*iter) && !is_ascii_ctl(*iter) && !is_separator(*iter)) {
					continue;
				}
				break;
			case 4:
				if(is_ascii_char(*iter) && !is_ascii_ctl(*iter) && !is_separator(*iter)) {
					this->state = 5;
					continue;
				}
				else if(*iter == '"') {
					this->state = 6;
					continue;
				}
				break;
			case 5:
				if(*iter == '\r') {
					this->state = 11;
					continue;
				}
				else if(is_ascii_char(*iter) && !is_ascii_ctl(*iter) && !is_separator(*iter)) {
					continue;
				}
				break;
			case 6:
				if(*iter == '\\') {
					this->state = 7;
					continue;
				}
				else if(*iter == '"') {
					this->state = 8;
					continue;
				}
				else if(*iter == ' ' || *iter == '\t') {
					continue;
				}
				else if(*iter == '\r') {
					// [CRLF] 1*(SP | HT)
					this->state = 9;
					continue;
				}
				else if(!is_ascii_ctl(*iter)) {
					continue;
				}
				break;
			case 7:
				if(is_ascii_char(*iter)) {
					this->state = 6;
					continue;
				}
				break;
			case 8:
				if(*iter == '\r') {
					this->state = 11;
					continue;
				}
				break;
			case 9:
				if(*iter == '\n') {
					this->state = 10;
					continue;
				}
				break;
			case 10:
				if(*iter == ' ' || *iter == '\t') {
					this->state = 6;
					continue;
				}
				break;
			case 11:
				if(*iter == '\n') {
					if(this->current_chunk_size != 0) {
						this->state = 12;
					}
					else {
						this->state = 14;
					}
					continue;
				}
				break;
			case 12:
				if(this->current_chunk_size_of_read < this->current_chunk_size) {
					++this->current_chunk_size_of_read;
					continue;
				}
				else if (this->current_chunk_size_of_read == this->current_chunk_size){
					if(*iter == '\r') {
						this->state = 13;
						continue;
					}
				}
				break;
			case 13:
				if(*iter == '\n') {
					this->state = 0;
					continue;
				}
				break;
			case 14:
				if(*iter == '\r') {
					this->state = 15;
					continue;
				}
				break;
			case 15:
				if(*iter == '\n') {
					this->state = 100;
					continue;
				}
				break;
			}
			this->state = -1;
			return false;
		}
		return true;
	}

	template<typename ForwordIterator>
	static bool GetChunksRangeList(ForwordIterator begin, ForwordIterator end, std::list<std::pair<ForwordIterator, ForwordIterator> >& lst) {
		assert(sizeof(typename std::iterator_traits<ForwordIterator>::value_type) == sizeof(char));
		int state = 0;
		std::size_t current_chunk_size = 0;
		std::size_t current_chunk_size_of_read = 0;
		ForwordIterator range_begin = begin;
		for(ForwordIterator iter = begin; iter != end; ++iter) {
			switch(state) {
			case 0:
				if(std::isxdigit(static_cast<unsigned char>(*iter))) {
					current_chunk_size = (*iter) >= 'A' ? std::toupper(static_cast<unsigned char>(*iter)) - 'A' + 10 : *iter - '0';
					current_chunk_size_of_read = 0;
					state = 1;
					continue;
				}
				break;
			case 1:
				if(current_chunk_size != 0 && std::isxdigit(static_cast<unsigned char>(*iter))) {
					current_chunk_size = current_chunk_size * 16 + ((*iter) >= 'A' ? std::toupper(static_cast<unsigned char>(*iter)) - 'A' + 10 : *iter - '0');
					continue;
				}
				else if (*iter == ';') {
					state = 2;
					continue;
				}
				else if(*iter == '\r') {
					state = 11;
					continue;
				}
				break;
			case 2:
				if(is_ascii_char(*iter) && !is_ascii_ctl(*iter) && !is_separator(*iter)) {
					state = 3;
					continue;
				}
				break;
			case 3:
				if(*iter == '=') {
					state = 4;
					continue;
				}
				else if(*iter == '\r') {
					state = 11;
					continue;
				}
				else if(is_ascii_char(*iter) && !is_ascii_ctl(*iter) && !is_separator(*iter)) {
					continue;
				}
				break;
			case 4:
				if(is_ascii_char(*iter) && !is_ascii_ctl(*iter) && !is_separator(*iter)) {
					state = 5;
					continue;
				}
				else if(*iter == '"') {
					state = 6;
					continue;
				}
				break;
			case 5:
				if(*iter == '\r') {
					state = 11;
					continue;
				}
				else if(is_ascii_char(*iter) && !is_ascii_ctl(*iter) && !is_separator(*iter)) {
					continue;
				}
				break;
			case 6:
				if(*iter == '\\') {
					state = 7;
					continue;
				}
				else if(*iter == '"') {
					state = 8;
					continue;
				}
				else if(*iter == ' ' || *iter == '\t') {
					continue;
				}
				else if(*iter == '\r') {
					// [CRLF] 1*(SP | HT)
					state = 9;
					continue;
				}
				else if(!is_ascii_ctl(*iter)) {
					continue;
				}
				break;
			case 7:
				if(is_ascii_char(*iter)) {
					state = 6;
					continue;
				}
				break;
			case 8:
				if(*iter == '\r') {
					state = 11;
					continue;
				}
				break;
			case 9:
				if(*iter == '\n') {
					state = 10;
					continue;
				}
				break;
			case 10:
				if(*iter == ' ' || *iter == '\t') {
					state = 6;
					continue;
				}
				break;
			case 11:
				if(*iter == '\n') {
					if(current_chunk_size != 0) {
						state = 12;
					}
					else {
						state = 14;
					}
					continue;
				}
				break;
			case 12:
				if(current_chunk_size_of_read < current_chunk_size) {
					if(current_chunk_size_of_read == 0) {
						range_begin = iter;
					}
					++current_chunk_size_of_read;
					continue;
				}
				else if (current_chunk_size_of_read == current_chunk_size){
					if(*iter == '\r') {
						lst.push_back(std::make_pair(range_begin, iter));
						state = 13;
						continue;
					}
				}
				break;
			case 13:
				if(*iter == '\n') {
					state = 0;
					continue;
				}
				break;
			case 14:
				if(*iter == '\r') {
					state = 15;
					continue;
				}
				break;
			case 15:
				if(*iter == '\n') {
					state = 100;
					continue;
				}
				break;
			}
			state = -1;
			return false;
		}
		return true;
	}
};
