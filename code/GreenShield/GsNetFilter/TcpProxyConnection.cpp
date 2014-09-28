#include "stdafx.h"
#include "TcpProxyConnection.h"

#include <string>
#include <sstream>
#include <cassert>
#include <limits>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <Sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>

#include "ScopeResourceHandle.h"
#include "HttpRequestFilter.h"

#ifdef POPT_LOG_USE_CLOG
#include <iostream>
#define SOCKET_ERROR_MSG_LOG(MSG) std::clog << MSG << std::endl
#define IPC_ERROR_MSG_LOG(MSG) std::clog << MSG << std::endl
#define CON_MSG_LOG(MSG) std::clog << MSG << std::endl;
#elif POPT_LOG_USE_TSLOG
#define SOCKET_ERROR_MSG_LOG(MSG) TSLOG(MSG)
#define IPC_ERROR_MSG_LOG(MSG) TSLOG(MSG)
#define CON_MSG_LOG TSLOG(MSG) TSLOG(MSG)
#else
#define SOCKET_ERROR_MSG_LOG(MSG)
#define IPC_ERROR_MSG_LOG(MSG)
#define CON_MSG_LOG(MSG)
#endif

TcpProxyConnection::TcpProxyConnection(boost::asio::io_service& io_service) : 
	m_userAgentSocket(io_service),
	m_targetServerSocket(io_service),
	m_state(CS_ESTABLISHING),
	m_targetServerPort(80),
	m_httpRequestMethod(HTTP_UNKNOWN),
	m_isRequestHasContentLength(false),
	m_bytesOfRequestContentHasRead(0),
	m_bytesOfRequestContent(0)
{
}

boost::shared_ptr<TcpProxyConnection> TcpProxyConnection::CreateConnection(boost::asio::io_service& io_service)
{
	return boost::shared_ptr<TcpProxyConnection>(new TcpProxyConnection(io_service));
}

boost::asio::ip::tcp::socket& TcpProxyConnection::GetUserAgentSocketRef()
{
	return this->m_userAgentSocket;
}

namespace {

bool SetObjectToLowIntegrity(HANDLE hObject, SE_OBJECT_TYPE type = SE_KERNEL_OBJECT)
{
	bool bRet = false;
	DWORD dwErr = ERROR_SUCCESS;
	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pSacl = NULL;
	BOOL fSaclPresent = FALSE;
	BOOL fSaclDefaulted = FALSE;

	if(ConvertStringSecurityDescriptorToSecurityDescriptor(L"S:(ML;;NW;;;LW)", SDDL_REVISION_1, &pSD, NULL)) {
		if (GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted)) {
			dwErr = SetSecurityInfo(hObject, type, LABEL_SECURITY_INFORMATION,NULL, NULL, NULL, pSacl);
			bRet = (ERROR_SUCCESS == dwErr);
		}
		LocalFree (pSD);
    }
	return bRet;
}

}

void TcpProxyConnection::AsyncStart()
{
	boost::asio::ip::tcp::socket::endpoint_type userAgentEnpoint = this->m_userAgentSocket.remote_endpoint();
    unsigned short userAgentPort = userAgentEnpoint.port();
	boost::asio::ip::address userAgentIP = userAgentEnpoint.address();
	std::wstring eventName = L"Local\\GreenSheildPDIPCSyncEvent_";
	std::wstring userAgentPortStr;
    {
        std::wstringstream wss;
        wss << userAgentPort;
        wss >> userAgentPortStr;
    }
    eventName += userAgentPortStr;
    HANDLE hEvent = ::OpenEvent(EVENT_MODIFY_STATE, FALSE, eventName.c_str());
    if(hEvent == NULL) {
        IPC_ERROR_MSG_LOG("Failed to open Event Error: " << ::GetLastError());
        return;
    }

	// 自动关闭事件
	ScopeResourceHandle<HANDLE, BOOL(WINAPI*)(HANDLE)> autoCloseEvent(hEvent, ::CloseHandle);

    std::wstring fileMappingName = L"Local\\GreenSheildPDIPCSharedMemory_";
    fileMappingName += userAgentPortStr;

    HANDLE hFileMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4 * 1024, fileMappingName.c_str());
    if(hFileMapping == NULL) {
        IPC_ERROR_MSG_LOG("Failed to CreateFileMapping Error: " << ::GetLastError());
        return;
    }

	// 自动关闭FileMapping
	ScopeResourceHandle<HANDLE, BOOL(WINAPI*)(HANDLE)> autoCloseFileMapping(hFileMapping, ::CloseHandle);

    SetObjectToLowIntegrity(hFileMapping);
    std::wstring ackEventName = L"Local\\GreenSheildPDIPCSyncAckEvent_";
    ackEventName += userAgentPortStr;

    HANDLE hAckEvent = ::CreateEvent(NULL, TRUE, FALSE, ackEventName.c_str());
    if(hAckEvent == NULL) {
        IPC_ERROR_MSG_LOG("Failed to open Create Event Error: " << ::GetLastError());
        return;
    }
    SetObjectToLowIntegrity(hAckEvent);

	// 自动关闭ackEvent
	ScopeResourceHandle<HANDLE, BOOL(WINAPI*)(HANDLE)> autoCloseAckEvent(hAckEvent, ::CloseHandle);

    ::SetEvent(hEvent);
    DWORD dwWaitResult = ::WaitForSingleObject(hAckEvent, 5000);
    if(dwWaitResult != WAIT_OBJECT_0) {
        IPC_ERROR_MSG_LOG("Failed to wait AckEvent Error:" << ::GetLastError());
        return;
    }
    char* sharedMemoryBuffer = reinterpret_cast<char*>(::MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 256));
	if(sharedMemoryBuffer == NULL) {
		IPC_ERROR_MSG_LOG("Failed to MapViewOfFile Error:" << ::GetLastError());
		return;
	}
	// 自动Unmap
	ScopeResourceHandle<HANDLE, BOOL(WINAPI*)(LPCVOID)> autoUnmapViewOfFile(sharedMemoryBuffer, ::UnmapViewOfFile);

    unsigned long remoteIP = *reinterpret_cast<unsigned long*>((sharedMemoryBuffer));
    unsigned short remotePort = *reinterpret_cast<unsigned short*>(sharedMemoryBuffer + 4);

    boost::asio::ip::address_v4 remoteIPAddr(htonl(remoteIP));
	this->m_targetServerAddress = remoteIPAddr.to_string();
	this->m_targetServerPort = remotePort;

	this->m_targetServerSocket.async_connect(boost::asio::ip::tcp::endpoint(remoteIPAddr, remotePort),
		boost::bind(&TcpProxyConnection::HandleConnect, this->shared_from_this(), _1));
}

void TcpProxyConnection::HandleConnect(const boost::system::error_code& error)
{
	if(!error) {
		this->m_state = CS_ESTABLISHED;
		this->AsyncReadDataFromUserAgent();
		this->AsyncReadDataFromTargetServer();
	}
}

void TcpProxyConnection::AsyncReadDataFromUserAgent()
{
	boost::asio::async_read(this->m_userAgentSocket, boost::asio::buffer(this->m_upstreamBuffer), boost::asio::transfer_at_least(1),
		boost::bind(&TcpProxyConnection::HandleReadDataFromUserAgent, this->shared_from_this(), _1, _2));
}

void TcpProxyConnection::AsyncReadDataFromTargetServer()
{
	boost::asio::async_read(this->m_targetServerSocket, boost::asio::buffer(this->m_downstreamBuffer), boost::asio::transfer_at_least(1),
		boost::bind(&TcpProxyConnection::HandleReadDataFromTargetServer, this->shared_from_this(), _1, _2));
}

void TcpProxyConnection::HandleReadDataFromUserAgent(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if(!error) {
		if(this->m_state != CS_TUNNELLING) {
			if(this->m_state == CS_ESTABLISHED || this->m_state == CS_WAITFORNEXTQUERY || this->m_state == CS_TRANSFERHTTPREQUESTLINE || this->m_state == CS_TRANSFERHTTPREQUESTHEADER) {
				this->m_requestString.append(&this->m_upstreamBuffer[0], &this->m_upstreamBuffer[0] + bytes_transferred);
				if(this->m_state == CS_ESTABLISHED || this->m_state == CS_WAITFORNEXTQUERY) {
					if(this->CheckIfIsNonHttpRequestViaMethod(this->m_requestString.begin(), this->m_requestString.end())) {
						// 非HTTP请求 进入隧道传输模式
						this->m_state = CS_TUNNELLING;
					}
					else {
						// 将状态转换为读取请求行
						this->m_state = CS_TRANSFERHTTPREQUESTLINE;
					}
				}

				if(this->m_state == CS_TRANSFERHTTPREQUESTLINE) {
					if(this->CheckIfIsNonHttpRequestViaMethod(this->m_requestString.begin(), this->m_requestString.end())) {
						// 非HTTP请求 进入隧道传输模式
						this->m_state = CS_TUNNELLING;
					}
					else {
						// 通过寻找crlf判断请求行是否读取完毕
						std::size_t crlfPos = this->m_requestString.find("\r\n");
						if(crlfPos == std::string::npos) {
							if(this->m_requestString.size() > MAXIMUM_REQUEST_HEADER_LENGTH) {
								this->m_state = CS_TUNNELLING;
							}
						}
						else {
							// 处理HTTP请求行
							std::string methodString, middleString, httpVersionString;
							bool parseRequestLineFailed = (this->m_requestString.begin() + crlfPos != this->SplitRequestLine(this->m_requestString.begin(), this->m_requestString.begin() + crlfPos, methodString, middleString, httpVersionString))
								|| methodString.empty() || middleString.empty() || httpVersionString.empty();

							// 处理请求方法
							if(!parseRequestLineFailed) {
								this->m_httpRequestMethod = HTTP_UNKNOWN;
								if(methodString == "OPTIONS") {
									this->m_httpRequestMethod = HTTP_OPTIONS;
								}
								else if(methodString == "GET") {
									this->m_httpRequestMethod = HTTP_GET;
								}
								else if(methodString == "HEAD") {
									this->m_httpRequestMethod = HTTP_HEAD;
								}
								else if(methodString == "POST") {
									this->m_httpRequestMethod = HTTP_POST;
								}
								else if(methodString == "PUT") {
									this->m_httpRequestMethod = HTTP_PUT;
								}
								else if(methodString == "DELETE") {
									this->m_httpRequestMethod = HTTP_DELETE;
								}else if(methodString == "TRACE") {
									this->m_httpRequestMethod = HTTP_TRACE;
								}
								else {
									// CONNECT方法是HTTP代理才需要处理的 TCP代理对CONNECT方法不视为HTTP请求
									assert(false);
								}
								if(this->m_httpRequestMethod == HTTP_UNKNOWN) {
									parseRequestLineFailed = true;
								}
							}

							// 处理请求uri
							if(!parseRequestLineFailed) {
								bool isAbsoluteUrl = true;
								const char* absoluteUrlPrefix = "http:";
								for(std::size_t index = 0; 0 < 5; ++index) {
									if(std::tolower(middleString[index]) != absoluteUrlPrefix[index]) {
										isAbsoluteUrl = false;
										break;
									}
								}

								if(isAbsoluteUrl || middleString[0] == '/') {
									if(isAbsoluteUrl) {
										this->m_absoluteUrl = middleString;
									}
									else {
										this->m_relativeUrl = middleString;
									}
								}
								else {
									// 不正确的uri
									parseRequestLineFailed = true;
								}
							}

							// 处理HTTP版本
							if(httpVersionString != "HTTP/1.1" && httpVersionString != "HTTP/1.0") {
								// HTTP版本高于1.1或低于1.0的直接进入隧道
								parseRequestLineFailed = true;
							}

							this->m_state = parseRequestLineFailed ? CS_TUNNELLING : CS_TRANSFERHTTPREQUESTHEADER;
						}
					}
				}

				if(this->m_state == CS_TRANSFERHTTPREQUESTHEADER) {
					// 读取请求头
					std::size_t crlfPos = this->m_requestString.find("\r\n");
					// 状态为CS_TRANSFERHTTPREQUESTHEADER不可能找不到crlf除非代码逻辑有问题
					assert(crlfPos != std::string::npos);
					std::size_t doubleCrlfPos = this->m_requestString.find("\r\n\r\n");
					if(doubleCrlfPos == std::string::npos) {
						if(this->m_requestString.size() > MAXIMUM_REQUEST_HEADER_LENGTH) {
							this->m_state = CS_TUNNELLING;
						}
					}
					else {
						HttpHeadersContainerType requestHeaders;
						bool parseRequestHeaderSuccess = true;
						if(doubleCrlfPos != crlfPos) {
							parseRequestHeaderSuccess = this->ParseHttpRequestHeaders(this->m_requestString.begin() + crlfPos + 2, this->m_requestString.begin() + doubleCrlfPos + 4, requestHeaders);
						}

						if(!parseRequestHeaderSuccess) {
							this->m_state = CS_TUNNELLING;
						}
						else {
							if(this->m_absoluteUrl.empty()) {
								HttpHeadersContainerType::const_iterator iter = requestHeaders.find("Host");
								
								if(iter != requestHeaders.end()) {
									if(this->m_targetServerPort == 80) {
										this->m_absoluteUrl = "http://" + iter->second + this->m_relativeUrl;
									}
									else {
										std::string portStr;
										{
											std::stringstream ss;
											ss << this->m_targetServerPort;
											ss >> portStr;
										}
										this->m_absoluteUrl = "http://" + iter->second + ":" + portStr + this->m_relativeUrl;
									}
								}
								else {
									// 找不到host则使用ip作为host
									if(this->m_targetServerPort == 80) {
										this->m_absoluteUrl = "http://" + this->m_targetServerAddress + this->m_relativeUrl;
									}
									else {
										std::string portStr;
										{
											std::stringstream ss;
											ss << this->m_targetServerPort;
											ss >> portStr;
										}
										this->m_absoluteUrl = "http://" + this->m_targetServerAddress + ":" + portStr + this->m_relativeUrl;
									}
								}
							}
							if(!this->m_absoluteUrl.empty()) {
								// test
								if(HttpRequestFilter::GetInstance().FilterUrl(this->m_absoluteUrl)) {
									if(this->m_targetServerSocket.is_open()) {
										this->m_targetServerSocket.close();
									}
									if(this->m_userAgentSocket.is_open()) {
										this->m_userAgentSocket.close();
									}
									return;
								}
								CON_MSG_LOG(this->m_absoluteUrl);
							}

							if(this->m_httpRequestMethod == HTTP_GET || this->m_httpRequestMethod == HTTP_HEAD || this->m_httpRequestMethod == HTTP_DELETE) {
								// 这几个方法是没有消息主体的
								if(this->m_requestString.size() - 4 == doubleCrlfPos) {
									this->m_state = CS_WAITFORNEXTQUERY;
								}
								else {
									this->m_state = CS_TUNNELLING;
								}
							}
							else if(this->m_httpRequestMethod == HTTP_POST || this->m_httpRequestMethod == HTTP_PUT) {
								// POST和PUT带有消息主体
								// 第一个版本不支持chuncked传输编码
								HttpHeadersContainerType::const_iterator iter = requestHeaders.find("Content-Length");
								bool hasContentLength = true;
#ifdef max
#undef max
#endif
								std::size_t contentLength = std::numeric_limits<std::size_t>::max();
								if(iter == requestHeaders.end()) {
									hasContentLength = false;
								}
								else {
									// 检查是否全部是十进制数
									bool convertable = true;
									for(std::size_t index = 0; index < iter->second.size(); ++index) {
										if(!std::isdigit(static_cast<unsigned char>(iter->second[index]))) {
											convertable = false;
											break;
										}
									}
									if(convertable) {
										std::stringstream ss;
										ss << iter->second;
										ss >> contentLength;
										if(ss.fail()) {
											contentLength = std::numeric_limits<std::size_t>::max();
										}
									}
								}
								if(hasContentLength && contentLength != std::numeric_limits<std::size_t>::max()) {
									this->m_isRequestHasContentLength = true;
									this->m_bytesOfRequestContent = contentLength;
									this->m_bytesOfRequestContentHasRead = this->m_requestString.size() - (doubleCrlfPos + 4);
									if(this->m_bytesOfRequestContentHasRead == this->m_bytesOfRequestContent) {
										this->PrepareForNextHttpQuery();
										this->m_state = CS_WAITFORNEXTQUERY;
									}
									else if (this->m_bytesOfRequestContentHasRead > this->m_bytesOfRequestContent){
										this->m_state = CS_TUNNELLING;
									}
								}
								else {
									this->m_state = CS_TUNNELLING;
								}
							}
							else {
								// OPTIONS和TRACE进入隧道模式
								this->m_state = CS_TUNNELLING;
							}
						}
					}
				}
			}
			else if(this->m_state == CS_TRANSFERHTTPREQUESTCONTENT) {
				assert(this->m_isRequestHasContentLength);
				this->m_bytesOfRequestContentHasRead += bytes_transferred;
				if(this->m_bytesOfRequestContentHasRead == this->m_bytesOfRequestContent) {
					this->PrepareForNextHttpQuery();
					this->m_state = CS_WAITFORNEXTQUERY;
				}
				else if (this->m_bytesOfRequestContentHasRead > this->m_bytesOfRequestContent){
					this->m_state = CS_TUNNELLING;
				}
			}
		}
		boost::asio::async_write(this->m_targetServerSocket, boost::asio::buffer(this->m_upstreamBuffer, bytes_transferred),
			boost::bind(&TcpProxyConnection::HandleWriteDataToTargetServer, this->shared_from_this(), _1, _2));
	}
	else {
		if(this->m_targetServerSocket.is_open()) {
			if(error == boost::asio::error::eof) {
				boost::system::error_code ec;
				this->m_targetServerSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			}
			this->m_targetServerSocket.close();
		}
		else {
			SOCKET_ERROR_MSG_LOG("HandleReadDataFromUserAgent error: " << error.message());
		}
	}
}

void TcpProxyConnection::HandleReadDataFromTargetServer(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if(!error) {
		if(this->m_state == CS_ESTABLISHED) {
			this->m_state = CS_TUNNELLING;
		}
		boost::asio::async_write(this->m_userAgentSocket, boost::asio::buffer(this->m_downstreamBuffer, bytes_transferred),
			boost::bind(&TcpProxyConnection::HandleWriteDataToUserAgent, this->shared_from_this(), _1, _2));
	}
	else {
		if(this->m_userAgentSocket.is_open()) {
			if(error == boost::asio::error::eof) {
				boost::system::error_code ec;
				this->m_userAgentSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			}
			this->m_userAgentSocket.close();
		}
		else {
			SOCKET_ERROR_MSG_LOG("HandleReadDataFromTargetServer error: " << error.message());
		}
	}
}

void TcpProxyConnection::HandleWriteDataToUserAgent(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if(!error) {
		this->AsyncReadDataFromTargetServer();
	}
	else {
		if(this->m_targetServerSocket.is_open()) {
			if(error == boost::asio::error::eof) {
				boost::system::error_code ec;
				this->m_targetServerSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			}
			this->m_targetServerSocket.close();
		}
		else {
			SOCKET_ERROR_MSG_LOG("HandleWriteDataToUserAgent error: " << error.message());
		}
	}
}

void TcpProxyConnection::HandleWriteDataToTargetServer(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if(!error) {
		this->AsyncReadDataFromUserAgent();
	}
	else {
		if(this->m_userAgentSocket.is_open()) {
			if(error == boost::asio::error::eof) {
				boost::system::error_code ec;
				this->m_userAgentSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			}
			this->m_userAgentSocket.close();
		}
		else {
			SOCKET_ERROR_MSG_LOG("HandleWriteDataToTargetServer error: " << error.message());
		}
	}
}

bool TcpProxyConnection::CheckIfIsNonHttpRequestViaMethod(std::string::const_iterator begin, std::string::const_iterator end)
{
	int state = 1;
	for(std::string::const_iterator iter = begin; iter != end && state != 0; ++iter) {
		switch(state) {
			case 1:
				if(*iter == 'O') {
					state = 10;
					continue;
				}
				else if(*iter == 'G') {
					state = 20;
					continue;
				}
				else if(*iter == 'H') {
					state = 30;
					continue;
				}
				else if(*iter == 'P') {
					state = 40;
					continue;
				}
				else if(*iter == 'D') {
					state = 50;
					continue;
				}
				else if(*iter == 'T') {
					state = 60;
					continue;
				}
				break;
			case 10:
				// OPTIONS
				if(*iter == 'P') {
					++state; // 11
					continue;
				}
				break;
			case 11:
				if(*iter == 'T') {
					++state; // 12
					continue;
				}
				break;
			case 12:
				if(*iter == 'I') {
					++state; // 13
					continue;
				}
				break;
			case 13:
				if(*iter == 'O') {
					++state; // 14
					continue;
				}
				break;
			case 14:
				if(*iter == 'N') {
					++state; // 15
					continue;
				}
				break;
			case 15:
				if(*iter == 'S') {
					state = 100; // 100
					continue;
				}
				break;
			case 20:
				// GET
				if(*iter == 'E') {
					++state; // 21
					continue;
				}
				break;
			case 21:
				if(*iter == 'T') {
					state = 100; // 100
					continue;
				}
				break;
			case 30:
				// HEAD
				if(*iter == 'E') {
					++state; // 31
					continue;
				}
				break;
			case 31:
				if(*iter == 'A') {
					++state; // 32
					continue;
				}
				break;
			case 32:
				if(*iter == 'D') {
					state = 100; // 100
					continue;
				}
				break;
			case 40:
				// POST PUT
				if(*iter == 'O') {
					++state; // 41
					continue;
				}
				else if(*iter == 'U') {
					state = 42; // 42
					continue;
				}
				break;
			case 41:
				if(*iter == 'S') {
					++state; // 42
					continue;
				}
				break;
			case 42:
				if(*iter == 'T') {
					state = 100; // 100
					continue;
				}
				break;
			case 50:
				// DELETE
				if(*iter == 'E') {
					++state; // 51
					continue;
				}
				break;
			case 51:
				if(*iter == 'L') {
					++state; // 52
					continue;
				}
				break;
			case 52:
				if(*iter == 'E') {
					++state; // 53
					continue;
				}
				break;
			case 53:
				if(*iter == 'T') {
					++state; // 54
					continue;
				}
				break;
			case 54:
				if(*iter == 'E') {
					state = 100; // 100
					continue;
				}
				break;
			case 60:
				if(*iter == 'R') {
					++state; // 61
					continue;
				}
				break;
			case 61:
				if(*iter == 'A') {
					++state; // 62
					continue;
				}
				break;
			case 62:
				if(*iter == 'C') {
					++state; // 63
					continue;
				}
				break;
			case 63:
				if(*iter == 'E') {
					state = 100; // 100
					continue;
				}
				break;
			case 100:
				if(std::isspace(static_cast<unsigned char>(*iter))) {
					state = 0; // 0
					continue;
				}
				break;
		}
		return true;
	}
	return false;
}

std::string::const_iterator TcpProxyConnection::SplitRequestLine(std::string::const_iterator begin, std::string::const_iterator end, std::string& leftString, std::string& middleString, std::string& rightString)
{
	assert(leftString.empty() && middleString.empty() && rightString.empty());
	std::string::const_iterator iter = begin;
	for(;iter != end && !std::isspace(static_cast<unsigned char>(*iter)); ++iter) {
		leftString.push_back(*iter);
	}
	for(;iter != end && std::isspace(static_cast<unsigned char>(*iter)); ++iter)
		;

	for(;iter != end && !std::isspace(static_cast<unsigned char>(*iter)); ++iter) {
		middleString.push_back(*iter);
	}
	for(;iter != end && std::isspace(static_cast<unsigned char>(*iter)); ++iter)
		;

	for(;iter != end && !std::isspace(static_cast<unsigned char>(*iter)); ++iter) {
		rightString.push_back(*iter);
	}
	return iter;
}

bool TcpProxyConnection::IsSeperators(char ch)
{
	return ch == '(' || ch == ')'
		|| ch == '<' || ch == '>'
		|| ch == '@' || ch == ','
		|| ch == ';' || ch == ':'
		|| ch == '\\' || ch == '"'
		|| ch == '/' || ch == '['
		|| ch == ']' || ch == '?'
		|| ch == '=' || ch == '{'
		|| ch == '}' || std::isspace(static_cast<unsigned char>(ch))
		|| ch == '\t';
}

bool TcpProxyConnection::ParseHttpRequestHeaders(std::string::const_iterator begin, std::string::const_iterator end, HttpHeadersContainerType& requestHeaders)
{
	int state = 0;
	std::string key;
	std::string value;
	for(std::string::const_iterator iter = begin; iter != end && state != 7; ++iter) {
		switch(state) {
			case 0:
				if(*iter == '\r') {
					state = 6;
				}
				else if((0 <= *iter && *iter <= 31) || *iter == 127) {
					return false;
				}
				else if(!std::isspace(static_cast<unsigned char>(*iter))) {
					if(this->IsSeperators(*iter)) {
						return false;
					}
					++state; // 1
					key.push_back(*iter);
				}
				break;
			case 1:
				if(std::isspace(static_cast<unsigned char>(*iter))) {
					++state; // 2
				}
				else if(*iter == ':') {
					state = 3;
				}
				else if((0 <= *iter && *iter <= 31) || *iter == 127 || this->IsSeperators(*iter)) {
					return false;
				}
				else {
					key.push_back(*iter);
				}
				break;
			case 2:
				if(*iter == ':') {
					++state; // 3
				}
				else if(!std::isspace(static_cast<unsigned char>(*iter))) {
					return false;
				}
				break;
			case 3:
				if(!std::isspace(static_cast<unsigned char>(*iter))) {
					++state; // 4
					value.push_back(*iter);
				}
				break;
			case 4:
				if(*iter == '\r') {
					++state; // 5
				}
				else {
					value.push_back(*iter);
				}
				break;
			case 5:
				if(*iter == '\n') {
					if(!value.empty()) {
						for(std::size_t index = value.size() - 1; index != 0 && std::isspace(static_cast<unsigned char>(value[index])); --index) {
							value.erase(value.begin() + index);
						}
					}
					requestHeaders.insert(std::make_pair(key, value));
					key.clear();
					value.clear();
					state = 0;
				}
				else {
					return false;
				}
				break;
			case 6:
				if(*iter == '\n') {
					state = 7;
				}
				else {
					return false;
				}
			break;
			default:
				assert(false);
				return false;
			break;
		}
	}
	return state == 7;
}

void TcpProxyConnection::PrepareForNextHttpQuery()
{
	this->m_state = CS_WAITFORNEXTQUERY;
	this->m_httpRequestMethod = HTTP_UNKNOWN;
	this->m_requestString.clear();
	this->m_relativeUrl.clear();
	this->m_absoluteUrl.clear();
	this->m_isRequestHasContentLength = false;
	this->m_bytesOfRequestContentHasRead = 0;
	this->m_bytesOfRequestContent = 0;
}
