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
	CT_TEXT_HTML,	// text/html
	CT_TEXT_XML,	// text/xml
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
private:
	static const std::size_t MAXIMUM_REQUEST_HEADER_LENGTH = 1048576;
	static const std::size_t MAXIMUM_RESPONSE_HEADER_LENGTH = 1048576;
};
