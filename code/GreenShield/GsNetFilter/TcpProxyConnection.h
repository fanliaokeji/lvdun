#pragma once
#include <string>
#include <map>
#include <cctype>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

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
	CS_TRANSFERHTTPREQUESTLINE, // 传输请求行
	CS_TRANSFERHTTPREQUESTHEADER,	// 传输 实体头(entity header)和请求头(request header)
	CS_TRANSFERHTTPREQUESTCONTENT, // 传输 请求的消息主体(message body)
	CS_WAITFORNEXTQUERY	// 等待下一个请求
};

class HttpHeadersFieldNameCompare {
	struct CaseInsensitiveLess {
		bool operator() (char ch1, char ch2) const {
			return std::tolower(ch1) < std::tolower(ch2);
		}
	};
public:
	bool operator() (const std::string& str1, const std::string& str2) const {
		return std::lexicographical_compare(str1.begin(), str1.end(), str2.begin(), str2.end(), CaseInsensitiveLess());
	}
};

class TcpProxyConnection : public boost::enable_shared_from_this<TcpProxyConnection> {
	boost::asio::ip::tcp::socket m_userAgentSocket;
	boost::asio::ip::tcp::socket m_targetServerSocket;
	char m_upstreamBuffer[10240];
	char m_downstreamBuffer[10240];
	ConnectionState m_state;
	std::string m_targetServerAddress;
	unsigned short m_targetServerPort;
	std::string m_requestString;
	HttpRequestMethod m_httpRequestMethod;
	std::string m_relativeUrl;
	std::string m_absoluteUrl;
	bool m_isRequestHasContentLength;
	std::size_t m_bytesOfRequestContentHasRead;
	std::size_t m_bytesOfRequestContent;
private:
	typedef std::multimap<std::string, std::string, HttpHeadersFieldNameCompare> HttpHeadersContainerType;
	TcpProxyConnection(boost::asio::io_service& io_service);
public:
	static boost::shared_ptr<TcpProxyConnection> CreateConnection(boost::asio::io_service& io_service);
	boost::asio::ip::tcp::socket& GetUserAgentSocketRef();
	void AsyncStart();
private:
	void HandleConnect(const boost::system::error_code& error);
	void AsyncReadDataFromUserAgent();
	void AsyncReadDataFromTargetServer();
	void HandleReadDataFromUserAgent(const boost::system::error_code& error, std::size_t bytes_transferred);
	void HandleReadDataFromTargetServer(const boost::system::error_code& error, std::size_t bytes_transferred);
	void HandleWriteDataToUserAgent(const boost::system::error_code& error, std::size_t bytes_transferred);
	void HandleWriteDataToTargetServer(const boost::system::error_code& error, std::size_t bytes_transferred);
private:
	// 辅助函数
	bool CheckIfIsNonHttpRequestViaMethod(std::string::const_iterator begin, std::string::const_iterator end);
	std::string::const_iterator SplitRequestLine(std::string::const_iterator begin, std::string::const_iterator end, std::string& leftString, std::string& middleString, std::string& rightString);
	bool IsSeperators(char ch);
	bool ParseHttpRequestHeaders(std::string::const_iterator begin, std::string::const_iterator end, HttpHeadersContainerType& headers);
	// 重置状态等待下一次请求
	void PrepareForNextHttpQuery();
private:
	static const std::size_t MAXIMUM_REQUEST_HEADER_LENGTH = 1048576;
};
