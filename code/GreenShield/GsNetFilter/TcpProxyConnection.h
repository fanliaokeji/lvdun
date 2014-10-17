#pragma once
#include <string>
#include <map>
#include <cctype>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/optional.hpp>

#include "Chunked.h"

enum HttpRequestMethod {
	HTTP_UNKNOWN,
	HTTP_OPTIONS,
	HTTP_GET,
	HTTP_HEAD,
	HTTP_POST,
	HTTP_PUT,
	HTTP_DELETE,
	HTTP_TRACE,
	HTTP_CONNECT
};

enum ConnectionState {
	CS_ESTABLISHING,	// 正在建立连接
	CS_ESTABLISHED,	// 连接已经建立
	CS_TUNNELLING,	// 盲目隧道传输
	CS_READ_HTTP_REQUEST_LINE, // 读取 请求行
	CS_READ_HTTP_REQUEST_HEADER, // 读取 实体头(entity header)和请求头(request header)
	CS_WRITE_HTTP_REQUEST_HEADER, // 发送 实体头(entity header)和请求头(request header)
	CS_TRANSFER_HTTP_REQUEST_CONTENT, // 传输 请求的消息主体(request message body)
	CS_READ_HTTP_RESPONSE_LINE,	// 读取 响应行
	CS_READ_HTTP_RESPONSE_HEADER,	// 读取 实体头(entity header)和响应头(response header)
	CS_READ_HTTP_RESPONSE_CONTENT,	// 读取 响应的消息主体(response message body)
	CS_WRITE_HTTP_RESPONSE_CONTENT, // 发送 响应
	CS_WAIT_FOR_NEXT_HTTP_QUERY, // 等待下一个请求
	CS_WRITE_BLOCK_RESPONSE // 发送阻止响应
};

enum TransferEncoding {
	TE_NONE,	// 没有TransferEncoding
	TE_UNKNOWN,	// 未知传输编码
	TE_CHUNKED	// 分块传输编码
};

enum ContentEncoding {
	CE_NONE,	// 没有内容编码
	CE_UNKNOWN,	// 未知的内容编码
	CE_GZIP,	// gzip
	CE_DEFLATE	// deflate
};

enum ContentType {
	CT_NONE,	// 没有指明
	// application
	CT_APPLICATION_ATOM_XML, // application/atom+xml
	CT_APPLICATION_DART, // application/dart
	CT_APPLICATION_ECMASCRIPT, // application/ecmascript
	CT_APPLICATION_EDI_X12, // application/EDI-X12
	CT_APPLICATION_EDIFACT, // application/EDIFACT
	CT_APPLICATION_JSON, // application/json
	CT_APPLICATION_JAVASCRIPT, // application/javascript
	CT_APPLICATION_OCTET_STREAM, // application/octet-stream
	CT_APPLICATION_OGG, // application/ogg
	CT_APPLICATION_PDF, // application/pdf
	CT_APPLICATION_POSTSCRIPT, // application/postscript
	CT_APPLICATION_RDF_XML, // application/rdf+xml
	CT_APPLICATION_RSS_XML, // application/rss+xml
	CT_APPLICATION_SOAP_XML, // application/soap+xml
	CT_APPLICATION_FONT_WOFF, // application/font-woff
	CT_APPLICATION_XHTML_XML, // application/xhtml+xml
	CT_APPLICATION_XML, // application/xml
	CT_APPLICATION_XML_DTD, // application/xml-dtd
	CT_APPLICATION_XOP_XML, // application/xop+xml
	CT_APPLICATION_ZIP, // application/zip
	CT_APPLICATION_GZIP, // application/gzip
	CT_APPLICATION_EXAMPLE, // application/example
	CT_APPLICATION_X_NACL, // application/x-nacl
	CT_APPLICATION_X_PNACL, // application/x-pnacl

	// audio
	CT_AUDIO_BASIC, // audio/basic
	CT_AUDIO_L24, // audio/L24
	CT_AUDIO_MP4, // audio/mp4
	CT_AUDIO_MPEG, // audio/mpeg
	CT_AUDIO_OGG, // audio/ogg
	CT_AUDIO_OPUS, // audio/opus
	CT_AUDIO_VORBIS, // audio/vorbis
	CT_AUDIO_VND_RN_REALAUDIO, // audio/vnd.rn-realaudio
	CT_AUDIO_VND_WAVE, // audio/vnd.wave
	CT_AUDIO_WEBM, // audio/webm
	CT_AUDIO_EXAMPLE, // audio/example

	// image
	CT_IMAGE_GIF, // image/gif
	CT_IMAGE_JPEG, // image/jpeg
	CT_IMAGE_PJPEG, // image/pjpeg
	CT_IMAGE_PNG, // image/png
	CT_IMAGE_SVG_XML, // image/svg+xml
	CT_IMAGE_VND_DJVU, // image/vnd.djvu
	CT_IMAGE_EXAMPLE, // image/example

	// message
	CT_MESSAGE_HTTP, // message/http
	CT_MESSAGE_IMDN_XML, // message/imdn+xml
	CT_MESSAGE_PARTIAL, // message/partial
	CT_MESSAGE_RFC822, // message/rfc822
	CT_MESSAGE_EXAMPLE, // message/example

	// model
	CT_MODEL_IGES, // model/iges
	CT_MODEL_MESH, // model/mesh
	CT_MODEL_VRML, // model/vrml
	CT_MODEL_X3D_BINARY, // model/x3d+binary
	CT_MODEL_X3D_FASTINFOSET, // model/x3d+fastinfoset
	CT_MODEL_X3D_VRML, // model/x3d-vrml
	CT_MODEL_X3D_XML, // model/x3d+xml
	CT_MODEL_EXAMPLE, // model/example

	// multipart
	CT_MULTIPART_MIXED, // multipart/mixed
	CT_MULTIPART_ALTERNATIVE, // multipart/alternative
	CT_MULTIPART_RELATED, // multipart/related
	CT_MULTIPART_FORM_DATA, // multipart/form-data
	CT_MULTIPART_SIGNED, // multipart/signed
	CT_MULTIPART_ENCRYPTED, // multipart/encrypted
	CT_MULTIPART_EXAMPLE, // multipart/example

	// text
	CT_TEXT_CMD, // text/cmd
	CT_TEXT_CSS, // text/css
	CT_TEXT_CSV, // text/csv
	CT_TEXT_EXAMPLE, // text/example
	CT_TEXT_HTML, // text/html
	CT_TEXT_JAVASCRIPT, // text/javascript
	CT_TEXT_PLAIN, // text/plain
	CT_TEXT_RTF, // text/rtf
	CT_TEXT_VCARD, // text/vcard
	CT_TEXT_VND_ABC, // text/vnd.abc
	CT_TEXT_XML, // text/xml

	// video
	CT_VIDEO_AVI, // video/avi
	CT_VIDEO_EXAMPLE, // video/example
	CT_VIDEO_MPEG, // video/mpeg
	CT_VIDEO_MP4, // video/mp4
	CT_VIDEO_OGG, // video/ogg
	CT_VIDEO_QUICKTIME, // video/quicktime
	CT_VIDEO_WEBM, // video/webm
	CT_VIDEO_X_MATROSKA, // video/x-matroska
	CT_VIDEO_X_MS_WMV, // video/x-ms-wmv
	CT_VIDEO_X_FLV, // video/x-flv

	CT_UNKNOWN	// 其他
};

class HttpHeadersFieldNameCompare {
	struct CaseInsensitiveLess {
		bool operator() (char ch1, char ch2) const {
			return std::tolower(static_cast<unsigned char>(ch1)) < std::tolower(static_cast<unsigned char>(ch2));
		}
	};
public:
	bool operator() (const std::string& str1, const std::string& str2) const {
		return std::lexicographical_compare(str1.begin(), str1.end(), str2.begin(), str2.end(), CaseInsensitiveLess());
	}
};

class TcpProxyConnection : public boost::enable_shared_from_this<TcpProxyConnection> {
	typedef std::multimap<std::string, std::string, HttpHeadersFieldNameCompare> HttpHeaderContainerType;
private:
	boost::asio::ip::tcp::socket m_userAgentSocket;
	boost::asio::ip::tcp::socket m_targetServerSocket;
	char m_upstreamBuffer[10240];
	char m_downstreamBuffer[10240];
	ConnectionState m_state;
	std::string m_targetServerAddress;
	unsigned short m_targetServerPort;
	std::string m_requestString;
	std::string m_responseString;
	std::string m_bufferedRequestData;
	std::string m_bufferedResponseData;
	HttpRequestMethod m_httpRequestMethod;
	unsigned short m_responseStateCode;
	std::string m_relativeUrl;
	std::string m_absoluteUrl;
	HttpHeaderContainerType m_requestHeader;
	HttpHeaderContainerType m_responseHeader;
	boost::optional<std::size_t> m_requestContentLength;
	boost::optional<std::size_t> m_responseContentLength;
	std::size_t m_bytesOfRequestContentHasRead;
	std::size_t m_bytesOfResponseContentHasRead;
	TransferEncoding m_requestTransferEncoding;
	TransferEncoding m_responseTransferEncoding;
	ContentEncoding m_responseContentEncoding;
	ChunkChecker m_requestChunkChecker;
	ChunkChecker m_responseChunkChecker;
	bool m_isThisRequestNeedModifyResponse;
	std::string m_responseContentDecoded;
private:
	TcpProxyConnection(boost::asio::io_service& io_service);
public:
	static boost::shared_ptr<TcpProxyConnection> CreateConnection(boost::asio::io_service& io_service);
	boost::asio::ip::tcp::socket& GetUserAgentSocketRef();
	void AsyncStart(unsigned short listen_port);
private:
	void HandleConnect(const boost::system::error_code& error);
	void AsyncReadDataFromUserAgent();
	void AsyncReadDataFromTargetServer();
	void HandleReadDataFromUserAgent(const boost::system::error_code& error, std::size_t bytes_transferred);
	void HandleReadDataFromTargetServer(const boost::system::error_code& error, std::size_t bytes_transferred);
	void HandleWriteDataToUserAgent(const boost::system::error_code& error, std::size_t bytes_transferred, bool clearBuffer);
	void HandleWriteDataToTargetServer(const boost::system::error_code& error, std::size_t bytes_transferred, bool clearBuffer);
private:
	// 辅助函数
	bool CheckIfIsNonHttpRequestViaMethod(std::string::const_iterator begin, std::string::const_iterator end);
	bool CheckIfIsNonHttpResponseViaResponseLine(std::string::const_iterator begin, std::string::const_iterator end);
	std::string::const_iterator SplitRequestLine(std::string::const_iterator begin, std::string::const_iterator end, std::string& leftString, std::string& middleString, std::string& rightString);
	std::vector<std::pair<std::string::const_iterator, std::string::const_iterator> > SplitHttpResponseLine(std::string::const_iterator begin, std::string::const_iterator end);
	bool IsSeperators(char ch) const;
	bool ParseHttpRequestHeader(std::string::const_iterator begin, std::string::const_iterator end, HttpHeaderContainerType& requestHeader) const;
	bool ParseHttpResponseHeader(std::string::const_iterator begin, std::string::const_iterator end, HttpHeaderContainerType& resonseHeader) const;
	bool IsCaseInsensitiveEqual(const std::string& lhs, const std::string& rhs) const;
	TransferEncoding GetTransferEncoding(const HttpHeaderContainerType& httpHeader) const;
	TransferEncoding GetRequestTransferEncoding() const;
	TransferEncoding GetResponseTransferEncoding() const;
	bool HasContentLength(const HttpHeaderContainerType& httpHeader) const;
	bool IsRequestHeaderHasContentLength() const;
	bool IsResponseHeaderHasContentLength() const;
	boost::optional<std::size_t> GetContentLength(const HttpHeaderContainerType& httpHeader) const;
	boost::optional<std::size_t> GetRequestContentLength() const;
	boost::optional<std::size_t> GetResponseContentLength() const;
	ContentEncoding GetContentEncoding(const HttpHeaderContainerType& httpHeader) const;
	ContentEncoding GetResponseContentEncoding() const;
	ContentType GetResponseContentType() const;
	bool ResponseContentTypeIsText() const;
	std::size_t GetHideElementCodeInsertPos(const std::string& decodedContentData) const;
	std::string GetRequestReferer() const;
	// 重置状态等待下一次请求
	void PrepareForNextHttpQuery();
private:
	bool ShouldBlockRequest(const std::string& url, const std::string& referer) const;
	bool NeedInsertCSSCode(const std::string& url) const;
	std::string GetInsertCSSCode(const std::string& url) const;
	bool NeedReplaceContent(const std::string& url) const;
	std::vector<std::string> GetReplaceRule(const std::string& url) const;
	void SendNotify(const std::string& url) const;
private:
	static const std::size_t MAXIMUM_REQUEST_HEADER_LENGTH = 1048576;
	static const std::size_t MAXIMUM_RESPONSE_HEADER_LENGTH = 1048576;
};
