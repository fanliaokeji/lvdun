#include "stdafx.h"
#include "TcpProxyConnection.h"

#include <string>
#include <sstream>
#include <iostream> // std::cerr
#include <cassert>
#include <limits>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <Sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>

#include "ScopeResourceHandle.h"
#include "HttpRequestFilter.h"
#include "Decompress.h"
#include "ABPRuleHelper.h"
#include "./ParseABP/FilterManager.h"

#define POPT_LOG_USE_CLOG

#ifdef POPT_LOG_USE_CLOG
#define SOCKET_ERROR_MSG_LOG(MSG) // std::clog << MSG << std::endl
#define IPC_ERROR_MSG_LOG(MSG) // std::clog << MSG << std::endl
#define CON_MSG_LOG(MSG) // std::clog << MSG << std::endl
#define FILTER_MSG_LOG(MSG) // std::clog << MSG << std::endl
#define ERROR_MSG_LOG(MSG) // std::clog << "Error: " << MSG << std::endl
#define INFO_LOG(MSG) // std::clog << "Info: " << MSG << std::endl;
#elif POPT_LOG_USE_TSLOG
#define SOCKET_ERROR_MSG_LOG(MSG) TSLOG(MSG)
#define IPC_ERROR_MSG_LOG(MSG) TSLOG(MSG)
#define CON_MSG_LOG TSLOG(MSG) TSLOG(MSG)
#define ERROR_MSG_LOG(MSG) TSLOG(MSG)
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
	m_responseStateCode(0),
	m_bytesOfRequestContentHasRead(0),
	m_bytesOfResponseContentHasRead(0),
	m_requestTransferEncoding(TE_NONE),
	m_responseTransferEncoding(TE_NONE),
	m_responseContentEncoding(CE_NONE),
	m_isThisRequestNeedModifyResponse(false)
{
}

TcpProxyConnection::~TcpProxyConnection()
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

void TcpProxyConnection::AsyncStart(unsigned short listen_port)
{
	boost::asio::ip::tcp::socket::endpoint_type userAgentEnpoint = this->m_userAgentSocket.remote_endpoint();
    unsigned short userAgentPort = userAgentEnpoint.port();
	boost::asio::ip::address userAgentIP = userAgentEnpoint.address();
	std::wstring eventName = L"Local\\{D3855530-84CD-4009-8920-793E798EEA87}WebEarserPDIPCSyncEvent_";
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

    std::wstring fileMappingName = L"Local\\{D3855530-84CD-4009-8920-793E798EEA87}WebEarserPDIPCSharedMemory_";
    fileMappingName += userAgentPortStr;

    HANDLE hFileMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4 * 1024, fileMappingName.c_str());
    if(hFileMapping == NULL) {
        IPC_ERROR_MSG_LOG("Failed to CreateFileMapping Error: " << ::GetLastError());
        return;
    }

	// 自动关闭FileMapping
	ScopeResourceHandle<HANDLE, BOOL(WINAPI*)(HANDLE)> autoCloseFileMapping(hFileMapping, ::CloseHandle);

    SetObjectToLowIntegrity(hFileMapping);
    std::wstring ackEventName = L"Local\\{D3855530-84CD-4009-8920-793E798EEA87}WebEarserPDIPCSyncAckEvent_";
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
    DWORD dwWaitResult = ::WaitForSingleObject(hAckEvent, 2000);
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
	if(remoteIPAddr == boost::asio::ip::address_v4::from_string("127.0.0.1") && remotePort == listen_port)
	{
		// 防止放大攻击
		boost::system::error_code ec;
		this->m_userAgentSocket.close(ec);
		return;
	}
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
		if(this->m_state == CS_TUNNELLING) {
			if(this->m_bufferedRequestData.empty()) {
				boost::asio::async_write(this->m_targetServerSocket, boost::asio::buffer(this->m_upstreamBuffer, bytes_transferred),
					boost::bind(&TcpProxyConnection::HandleWriteDataToTargetServer, this->shared_from_this(), _1, _2, false));
			}
			else {
				this->m_bufferedRequestData.append(&this->m_upstreamBuffer[0], &this->m_upstreamBuffer[0] + bytes_transferred);
				boost::asio::async_write(this->m_targetServerSocket, boost::asio::buffer(this->m_bufferedRequestData),
					boost::bind(&TcpProxyConnection::HandleWriteDataToTargetServer, this->shared_from_this(), _1, _2, true));
			}
		}
		else {
			if(this->m_state == CS_WRITE_HTTP_RESPONSE_CONTENT) {
				if(this->m_responseContentLength) {
					if(this->m_bytesOfResponseContentHasRead == *this->m_responseContentLength ) {
						this->PrepareForNextHttpQuery();
						this->m_state = CS_WAIT_FOR_NEXT_HTTP_QUERY;
					}
				}
				else if(this->m_responseTransferEncoding == TE_CHUNKED) {
					if(this->m_responseChunkChecker.IsAllChunkRead()) {
						this->PrepareForNextHttpQuery();
						this->m_state = CS_WAIT_FOR_NEXT_HTTP_QUERY;
					}
				}
			}

			if(this->m_state == CS_ESTABLISHED || this->m_state == CS_WAIT_FOR_NEXT_HTTP_QUERY || this->m_state == CS_READ_HTTP_REQUEST_LINE || this->m_state == CS_READ_HTTP_REQUEST_HEADER) {
				this->m_requestString.append(&this->m_upstreamBuffer[0], &this->m_upstreamBuffer[0] + bytes_transferred);
				if(this->m_state == CS_ESTABLISHED || this->m_state == CS_WAIT_FOR_NEXT_HTTP_QUERY) {
					if(this->CheckIfIsNonHttpRequestViaMethod(this->m_requestString.begin(), this->m_requestString.end())) {
						ERROR_MSG_LOG("Non-Http request");
						// 非HTTP请求 进入隧道传输模式
						this->m_state = CS_TUNNELLING;
					}
					else {
						// 将状态转换为读取请求行
						this->m_state = CS_READ_HTTP_REQUEST_LINE;
					}
				}

				if(this->m_state == CS_READ_HTTP_REQUEST_LINE) {
					if(this->CheckIfIsNonHttpRequestViaMethod(this->m_requestString.begin(), this->m_requestString.end())) {
						ERROR_MSG_LOG("Non-Http request");
						// 非HTTP请求 进入隧道传输模式
						this->m_state = CS_TUNNELLING;
					}
					else {
						// 通过寻找crlf判断请求行是否读取完毕
						std::size_t crlfPos = this->m_requestString.find("\r\n");
						if(crlfPos == std::string::npos) {
							if(this->m_requestString.size() > MAXIMUM_REQUEST_HEADER_LENGTH) {
								ERROR_MSG_LOG("Request header is too large");
								// 读取的请求行(头)长度超过MAXIMUM_REQUEST_HEADER_LENGTH
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
								}
								else if(methodString == "TRACE") {
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
								for(std::size_t index = 0; index < 5; ++index) {
									if(std::tolower(static_cast<unsigned char>(middleString[index])) != absoluteUrlPrefix[index]) {
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

							if(parseRequestLineFailed) {
								// 解析请求行失败或者不不支持的HTTP版本
								ERROR_MSG_LOG("Faile to parse request line");
								this->m_state = CS_TUNNELLING;
							}
							else {
								this->m_state = CS_READ_HTTP_REQUEST_HEADER;
							}
						}
					}
				}

				if(this->m_state == CS_READ_HTTP_REQUEST_HEADER) {
					// 读取请求头
					std::size_t crlfPos = this->m_requestString.find("\r\n");
					// 状态为CS_READHTTPREQUESTHEADER不可能找不到crlf除非代码逻辑有问题
					assert(crlfPos != std::string::npos);
					std::size_t doubleCrlfPos = this->m_requestString.find("\r\n\r\n");
					if(doubleCrlfPos == std::string::npos) {
						if(this->m_requestString.size() > MAXIMUM_REQUEST_HEADER_LENGTH) {
							ERROR_MSG_LOG("Request header is too large");
							// 读取的请求头长度超过MAXIMUM_REQUEST_HEADER_LENGTH
							this->m_state = CS_TUNNELLING;
						}
					}
					else {
						assert(this->m_requestHeader.empty());
						bool parseRequestHeaderSuccess = true;
						if(doubleCrlfPos != crlfPos) {
							parseRequestHeaderSuccess = this->ParseHttpRequestHeader(this->m_requestString.begin() + crlfPos + 2, this->m_requestString.begin() + doubleCrlfPos + 4, this->m_requestHeader);
						}

						if(!parseRequestHeaderSuccess) {
							ERROR_MSG_LOG("Failed to parse http request header");
							this->m_state = CS_TUNNELLING;
						}
						else {
							this->m_requestContentLength = this->GetRequestContentLength();
							this->m_requestTransferEncoding = this->GetRequestTransferEncoding();
							do {
								if(this->IsRequestHeaderHasContentLength() && !this->m_requestContentLength || this->m_requestTransferEncoding == TE_UNKNOWN) {
									// 请求头存在ContentLength但是不是有效的十进制数
									// 或未知的传输编码
									this->m_state = CS_TUNNELLING;
									break;
								}

								if(this->m_httpRequestMethod == HTTP_HEAD || this->m_httpRequestMethod == HTTP_GET || this->m_httpRequestMethod == HTTP_DELETE) {
									this->m_requestContentLength = 0;
									this->m_bytesOfRequestContentHasRead = this->m_requestString.size() - (doubleCrlfPos + 4);
									if(this->m_bytesOfRequestContentHasRead > *this->m_requestContentLength) {
										// HEAD GET DELETE不允许携带消息主体但却读到了消息主体
										ERROR_MSG_LOG("The request content data is larger than the Content-Length value");
										this->m_state = CS_TUNNELLING;
										break;
									}
								}
								else if(this->m_httpRequestMethod == HTTP_PUT || this->m_httpRequestMethod == HTTP_POST || this->m_httpRequestMethod == HTTP_OPTIONS) {
									if(!this->m_requestContentLength && this->m_requestTransferEncoding == TE_NONE
										&& this->m_httpRequestMethod == HTTP_OPTIONS) {
											// OPTIONS的消息主体是可选的
											this->m_requestContentLength = 0;
									}

									if(this->m_requestContentLength) {
										this->m_bytesOfRequestContentHasRead = this->m_requestString.size() - (doubleCrlfPos + 4);
										if(this->m_bytesOfRequestContentHasRead > *this->m_requestContentLength) {
											// 读取到的消息主体大于 Content-Length所指定的大小
											ERROR_MSG_LOG("The request content data is larger than the Content-Length value");
											this->m_state = CS_TUNNELLING;
											break;
										}
									}
									else if(this->m_requestTransferEncoding == TE_CHUNKED) {
										if(this->m_requestChunkChecker.Check(this->m_requestString.begin() + doubleCrlfPos + 4, this->m_requestString.end())) {
											// Chunked数据校验失败
											ERROR_MSG_LOG("Failed to check request chunked data");
											this->m_state = CS_TUNNELLING;
											break;
										}
									}
									else {
										// 没有Content-Length或没有指定传输编码
										assert(this->m_httpRequestMethod != HTTP_OPTIONS);
										ERROR_MSG_LOG("Content-Length or Transfer-Encoding is needed in request header");
										this->m_state = CS_TUNNELLING;
										break;
									}

									if(this->m_requestHeader.find("Expect") != this->m_requestHeader.end()) {
										// 带Expect的一个请求会产生两个响应 不处理
										this->m_state = CS_TUNNELLING;
										break;
									}
								}
								else {
									assert(this->m_httpRequestMethod == HTTP_TRACE);
									ERROR_MSG_LOG("The request method is TRACE");
									this->m_state = CS_TUNNELLING;
									break;
								}
							} while(false);

							if(this->m_absoluteUrl.empty()) {
								HttpHeaderContainerType::const_iterator iter = this->m_requestHeader.find("Host");

								if(iter != this->m_requestHeader.end()) {
									this->m_absoluteUrl = "http://" + iter->second + this->m_relativeUrl;
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

							if(!this->m_absoluteUrl.empty() && HttpRequestFilter::GetInstance().IsEnable()) {

								std::string referer = this->GetRequestReferer();

								bool blockUseForbiden = false;
								if(this->ShouldBlockRequest(this->m_absoluteUrl, referer, blockUseForbiden)) {
									this->SendNotify(this->m_absoluteUrl);
									if (blockUseForbiden) {
										this->m_requestString = "HTTP/1.1 403 Forbiden\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
									}
									else {
										std::string content_to_response;
										this->m_requestString = "HTTP/1.1 200 OK\r\n";
										if(this->IsJavaScriptUrl(this->m_absoluteUrl)) {
											this->m_requestString += "Content-Type: text/javascript\r\n";
											content_to_response = ";window.onerror=function(){return!0};";
										}
										else {
											this->m_requestString += "Content-Type: image/gif\r\n";
											const char* gif_data = "\x47\x49\x46\x38\x39\x61\x01\x00\x01\x00\x80\x00\x00\xFF\xFF\xFF\x00\x00\x00\x21\xF9\x04\x01\x00\x00\x00\x00\x2C\x00\x00\x00\x00\x01\x00\x01\x00\x00\x02\x02\x44\x01\x00\x3B";
											content_to_response.append(gif_data, gif_data + 43);
										}
										std::string content_length;
										{
											std::stringstream ss;
											ss << content_to_response.size();
											ss >> content_length;
										}
										this->m_requestString += "Content-Length: ";
										this->m_requestString += content_length;
										this->m_requestString += "\r\nConnection: close\r\n\r\n";
										this->m_requestString += content_to_response;
									}
									
									this->m_state = CS_WRITE_BLOCK_RESPONSE;
									boost::asio::async_write(this->m_userAgentSocket, boost::asio::buffer(this->m_requestString), 
										boost::bind(&TcpProxyConnection::HandleWriteDataToUserAgent, this->shared_from_this(), _1, _2, false));
									return;
								}
								else if(HttpRequestFilter::GetInstance().IsEnableRedirect()) {
									std::pair<bool, boost::optional<std::string> > redirectResult = this->ShouldRedirect(this->m_absoluteUrl, referer);
									if(redirectResult.first && redirectResult.second) {
										this->SendRedirectNotify(this->m_absoluteUrl);
										this->m_requestString = "HTTP/1.1 302 Found\r\n";
										this->m_requestString += "Connection: close\r\n";
										this->m_requestString += "Content-Length: 0\r\n";
										this->m_requestString += "Location: ";
										this->m_requestString += *(redirectResult.second);
										this->m_requestString += "\r\n\r\n";
										this->m_state = CS_WRITE_REDIRECT_RESPONSE;
										boost::system::error_code ec;
										if (this->m_targetServerSocket.is_open()) {
											this->m_targetServerSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
											this->m_targetServerSocket.close(ec);
										}
										boost::asio::async_write(this->m_userAgentSocket, boost::asio::buffer(this->m_requestString), 
											boost::bind(&TcpProxyConnection::HandleWriteDataToUserAgent, this->shared_from_this(), _1, _2, false));
										return;
									}
								}
								
								if(this->m_httpRequestMethod == HTTP_GET && this->NeedInsertCSSCode(this->m_absoluteUrl)) {
									this->m_isThisRequestNeedModifyResponse = true;
								}
								else if(this->m_httpRequestMethod == HTTP_GET && this->NeedReplaceContent(this->m_absoluteUrl)) {
									this->m_isThisRequestNeedModifyResponse = true;
								}

								// INFO_LOG(this->m_absoluteUrl);
							}

							// 如果需要在这里修改请求头
							
							// 请求的数据发往服务端
							this->m_state = CS_WRITE_HTTP_REQUEST_HEADER;
							// 清除缓存数据
							this->m_bufferedRequestData.clear();
							boost::asio::async_write(this->m_targetServerSocket, boost::asio::buffer(this->m_requestString), 
								boost::bind(&TcpProxyConnection::HandleWriteDataToTargetServer, this->shared_from_this(), _1, _2, false));
							return;
						}
					}
				}

				if(this->m_state != CS_TUNNELLING && this->m_state != CS_WRITE_HTTP_REQUEST_HEADER) {
					// 缓存读取到的数据
					this->m_bufferedRequestData.append(&this->m_upstreamBuffer[0], &this->m_upstreamBuffer[0] + bytes_transferred);
				}
			}
			else if(this->m_state == CS_TRANSFER_HTTP_REQUEST_CONTENT) {
				if(this->m_requestContentLength) {
					this->m_bytesOfRequestContentHasRead += bytes_transferred;
					if(this->m_bytesOfRequestContentHasRead > *this->m_requestContentLength) {
						// 读取到的消息主体大于 Content-Length所指定的大小
						ERROR_MSG_LOG("The request content data is larger than the Content-Length value");
						this->m_state = CS_TUNNELLING;
					}
				}
				else if(this->m_requestTransferEncoding == TE_CHUNKED) {
					if(!this->m_requestChunkChecker.Check(&this->m_upstreamBuffer[0], &this->m_upstreamBuffer[0] + bytes_transferred)) {
						// chunk数据校验失败
						ERROR_MSG_LOG("Failed to check request chunked data");
						this->m_state = CS_TUNNELLING;
					}
				}
				else {
					// 前面已经判断过了 所以不应该进入此分支
					assert(false);
				}

				if(this->m_state != CS_TUNNELLING) {
					// 转发数据到服务端
					boost::asio::async_write(this->m_targetServerSocket, boost::asio::buffer(this->m_upstreamBuffer, bytes_transferred), 
						boost::bind(&TcpProxyConnection::HandleWriteDataToTargetServer, this->shared_from_this(), _1, _2, false));
					return;
				}
			}
			else {
				ERROR_MSG_LOG("Read unexpected data from user-agent");
				// 收到意料之外的数据
				this->m_state = CS_TUNNELLING;
			}

			if(this->m_state == CS_TUNNELLING) {
				if(!this->m_bufferedRequestData.empty()) {
					this->m_bufferedRequestData.append(&this->m_upstreamBuffer[0], &this->m_upstreamBuffer[0] + bytes_transferred);
					boost::asio::async_write(this->m_targetServerSocket, boost::asio::buffer(this->m_bufferedRequestData), 
						boost::bind(&TcpProxyConnection::HandleWriteDataToTargetServer, this->shared_from_this(), _1, _2, true));
				}
				else {
					boost::asio::async_write(this->m_targetServerSocket, boost::asio::buffer(this->m_upstreamBuffer, bytes_transferred), 
						boost::bind(&TcpProxyConnection::HandleWriteDataToTargetServer, this->shared_from_this(), _1, _2, false));
				}
			}
			else {
				this->AsyncReadDataFromUserAgent();
			}
		}
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
	if(this->m_state == CS_WRITE_BLOCK_RESPONSE) {
		return;
	}
	else if(this->m_state == CS_WRITE_REDIRECT_RESPONSE) {
		return;
	}
	if(!error) {
		if(this->m_state == CS_TUNNELLING) {
			if(this->m_bufferedResponseData.empty()) {
				boost::asio::async_write(this->m_userAgentSocket, boost::asio::buffer(this->m_downstreamBuffer, bytes_transferred),
					boost::bind(&TcpProxyConnection::HandleWriteDataToUserAgent, this->shared_from_this(), _1, _2, false));
			}
			else {
				this->m_bufferedResponseData.append(&this->m_downstreamBuffer[0], &this->m_downstreamBuffer[0] + bytes_transferred);
				boost::asio::async_write(this->m_userAgentSocket, boost::asio::buffer(this->m_downstreamBuffer, bytes_transferred),
					boost::bind(&TcpProxyConnection::HandleWriteDataToUserAgent, this->shared_from_this(), _1, _2, false));
			}
		}
		else {
			if(this->m_state == CS_READ_HTTP_RESPONSE_LINE || this->m_state == CS_READ_HTTP_RESPONSE_HEADER) {
				this->m_responseString.append(&this->m_downstreamBuffer[0], &this->m_downstreamBuffer[0] + bytes_transferred);
				if(this->CheckIfIsNonHttpResponseViaResponseLine(this->m_responseString.begin(), this->m_responseString.end())) {
					ERROR_MSG_LOG("Invalid Http Response");
					this->m_state = CS_TUNNELLING;
				}

				if(this->m_state == CS_READ_HTTP_RESPONSE_LINE) {
					// 通过寻找crlf判断响应行是否读取完毕
					std::size_t crlfPos = this->m_responseString.find("\r\n");
					if(crlfPos == std::string::npos) {
						if(this->m_responseString.size() > MAXIMUM_RESPONSE_HEADER_LENGTH) {
							ERROR_MSG_LOG("Response header is too large");
							// 读取的响应行(头)长度超过MAXIMUM_RESPONSE_HEADER_LENGTH
							this->m_state = CS_TUNNELLING;
						}
					}
					else {
						// 处理HTTP响应行
						std::vector<std::pair<std::string::const_iterator, std::string::const_iterator> > splitedResponseLine = this->SplitHttpResponseLine(this->m_responseString.begin(), this->m_responseString.begin() + crlfPos);
						bool parseResponseLineSuccess = true;
						if(splitedResponseLine.size() < 2) {
							ERROR_MSG_LOG("Failed to split response line");
							parseResponseLineSuccess = false;
						}

						if(parseResponseLineSuccess) {
							std::string responseHttpVersion(splitedResponseLine[0].first, splitedResponseLine[0].second);
							if(responseHttpVersion != "HTTP/1.0" && responseHttpVersion != "HTTP/1.1") {
								ERROR_MSG_LOG("Unsupported http version of response");
								parseResponseLineSuccess = false;
							}
						}

						if(parseResponseLineSuccess) {
							std::string responseStateCode(splitedResponseLine[1].first, splitedResponseLine[1].second);
							if(responseStateCode.size() != 3) {
								ERROR_MSG_LOG("The Response state code length is invalid");
								parseResponseLineSuccess = false;
							}
							else {
								this->m_responseStateCode = 0;
								for(std::size_t i = 0; i < responseStateCode.size(); ++i) {
									if(std::isdigit(static_cast<unsigned char>(responseStateCode[i]))) {
										this->m_responseStateCode *= 10;
										this->m_responseStateCode += (responseStateCode[i] - '0');
									}
									else {
										parseResponseLineSuccess = false;
										break;
									}
								}
								if(parseResponseLineSuccess) {
									if((this->m_responseStateCode / 100) < 1 || (this->m_responseStateCode / 100) > 6) {
										parseResponseLineSuccess = false;
									}
								}
							}
						}

						if(!parseResponseLineSuccess) {
							ERROR_MSG_LOG("Failed to parse response line");
							this->m_state = CS_TUNNELLING;
						}
						else {
							// 只修改状态码为200的响应
							if(this->m_isThisRequestNeedModifyResponse && this->m_responseStateCode != 200) {
								this->m_isThisRequestNeedModifyResponse = false;
							}
							// 状态变更为读取请求头
							this->m_state = CS_READ_HTTP_RESPONSE_HEADER;
						}
					}
				}

				if(this->m_state == CS_READ_HTTP_RESPONSE_HEADER) {
					// 寻找换行
					std::size_t crlfPos = this->m_responseString.find("\r\n");
					assert(crlfPos != std::string::npos);
					std::size_t doubleCrlfPos = this->m_responseString.find("\r\n\r\n", crlfPos);
					if(doubleCrlfPos == std::string::npos) {
						if(this->m_responseString.size() > MAXIMUM_RESPONSE_HEADER_LENGTH) {
							ERROR_MSG_LOG("Response header is too large");
							// 读取的响应行(头)长度超过MAXIMUM_RESPONSE_HEADER_LENGTH
							this->m_state = CS_TUNNELLING;
						}
					}
					else {
						// 处理响应头
						assert(this->m_responseHeader.empty());
						this->m_responseHeader.clear();
						bool parseResponseHeaderSuccess = true;
						if(doubleCrlfPos != crlfPos) {
							parseResponseHeaderSuccess = this->ParseHttpResponseHeader(this->m_responseString.begin() + crlfPos + 2, this->m_responseString.begin() + doubleCrlfPos + 4, this->m_responseHeader);
						}

						if(!parseResponseHeaderSuccess) {
							ERROR_MSG_LOG("Failed to parse http resonse header");
							this->m_state = CS_TUNNELLING;
						}
						else {
							this->m_responseContentLength = this->GetResponseContentLength();
							this->m_responseTransferEncoding = this->GetResponseTransferEncoding();
							this->m_responseContentEncoding = this->GetResponseContentEncoding();
							
							do {
								if(this->IsResponseHeaderHasContentLength() && !this->m_responseContentLength
									|| this->m_responseContentEncoding == TE_UNKNOWN) {
										ERROR_MSG_LOG("Invalid Content-Length value or unknown Content-Encoding in response header");
										this->m_state = CS_TUNNELLING;
										break;
								}

								if(this->m_httpRequestMethod == HTTP_HEAD) {
									this->m_responseContentLength = 0;
								}

								if(!this->m_responseContentLength) {
									// 204 No Content
									// 205 Reset Content
									// 304 Not Modified
									if(this->m_responseStateCode == 204
										|| this->m_responseStateCode == 205
										|| this->m_responseStateCode == 304) {
										this->m_responseContentLength = 0;
									}
								}

								if(this->m_responseContentLength) {
									this->m_bytesOfResponseContentHasRead = this->m_responseString.size() - (doubleCrlfPos + 4);
									if(this->m_bytesOfResponseContentHasRead > *this->m_responseContentLength) {
										ERROR_MSG_LOG("The reponse content size is larger than the Content-Length value");
										this->m_state = CS_TUNNELLING;
										break;
									}
								}
								else if(this->m_responseTransferEncoding == TE_CHUNKED) {
									if(!this->m_responseChunkChecker.Check(this->m_responseString.begin() + doubleCrlfPos + 4, this->m_responseString.end())) {
										// 校验失败
										ERROR_MSG_LOG("Failed to check response chunked data");
										this->m_state = CS_TUNNELLING;
										break;
									}
								}
								else {
									// 既没有Content-Length也没有Transfer-Encoding
									// 可能Connection为false
									// 暂时不处理 进隧道
									this->m_state = CS_TUNNELLING;
									break;
								}

								this->m_state = CS_READ_HTTP_RESPONSE_CONTENT;
								
								if(this->m_isThisRequestNeedModifyResponse) {
									do {
										// 响应大于4MB的不修改
										if(this->m_responseContentLength && *this->m_responseContentLength > 4 * 1024 * 1024) {
											this->m_isThisRequestNeedModifyResponse = false;
											break;
										}

										// 只修改文本类的响应
										if(!this->ResponseContentTypeIsText()) {
											this->m_isThisRequestNeedModifyResponse = false;
											break;
										}
									} while(false);
								}
							} while(false);
						}
					}
				}
			}
			else if(this->m_state == CS_READ_HTTP_RESPONSE_CONTENT) {
				if(this->m_responseContentLength) {
					this->m_bytesOfResponseContentHasRead += bytes_transferred;
					if(this->m_bytesOfResponseContentHasRead > *this->m_responseContentLength) {
						ERROR_MSG_LOG("The reponse content size is larger than the Content-Length value");
						this->m_state = CS_TUNNELLING;
					}
				}
				else if(this->m_responseTransferEncoding == TE_CHUNKED) {
					if(!this->m_responseChunkChecker.Check(&this->m_downstreamBuffer[0], &this->m_downstreamBuffer[0] + bytes_transferred)) {
						ERROR_MSG_LOG("Failed to check response chunked data");
						this->m_state = CS_TUNNELLING;
					}
				}
				else {
					// 暂时不支持 不会到这个分支
					assert(false);
					this->m_state = CS_TUNNELLING;
				}
			}

			if(!this->m_bufferedResponseData.empty()) {
				// 缓冲区不为空 将新读到的数据加到缓冲区之后
				this->m_bufferedResponseData.append(&this->m_downstreamBuffer[0], &this->m_downstreamBuffer[0] + bytes_transferred);
				// 如果缓冲区大小太大则不修改内容
				if(this->m_bufferedResponseData.size() > 4 * 1024 * 1024) {
					this->m_isThisRequestNeedModifyResponse = false;
				}
			}

			if(this->m_state != CS_TUNNELLING && this->m_isThisRequestNeedModifyResponse) {
				if(this->m_bufferedResponseData.empty()) {
					// 缓冲区为空 将新读到的数据加到缓冲区
					this->m_bufferedResponseData.append(&this->m_downstreamBuffer[0], &this->m_downstreamBuffer[0] + bytes_transferred);
				}
				
				if(this->m_state == CS_READ_HTTP_RESPONSE_CONTENT) {
					if((this->m_responseContentLength && this->m_bytesOfResponseContentHasRead == *this->m_responseContentLength)
						|| (this->m_responseTransferEncoding == TE_CHUNKED && this->m_responseChunkChecker.IsAllChunkRead())) {
							do {
								std::size_t doubleCrlfPos = this->m_bufferedResponseData.find("\r\n\r\n");
								if(doubleCrlfPos == std::string::npos) {
									assert(false);
									this->m_isThisRequestNeedModifyResponse = false;
									break;
								}

								std::string decodedResponseContent;
								if(this->m_responseContentLength) {
									if(this->m_responseContentEncoding == CE_GZIP || this->m_responseContentEncoding == CE_DEFLATE) {
										bool decompressResult = false;
										// 预留内存 gzip对web页面的平均压缩率为25%(1/4)左右 预留5倍的大小
										decodedResponseContent.reserve((this->m_bufferedResponseData.size() - (doubleCrlfPos + 4)) * 5);
										if(this->m_responseContentEncoding == CE_GZIP) {
											GZipDecompressor gzipDecompressor;
											decompressResult = gzipDecompressor.Decompress(&this->m_bufferedResponseData[0] + doubleCrlfPos + 4, this->m_bufferedResponseData.size() - (doubleCrlfPos + 4), decodedResponseContent, false) == Z_STREAM_END;
										}
										else {
											assert(this->m_responseContentEncoding == CE_DEFLATE);
											DeflateDecompressor deflateDecompressor;
											decompressResult = deflateDecompressor.Decompress(&this->m_bufferedResponseData[0] + doubleCrlfPos + 4, this->m_bufferedResponseData.size() - (doubleCrlfPos + 4), decodedResponseContent, false) == Z_STREAM_END;
										}
										if(!decompressResult) {
											ERROR_MSG_LOG("Failed to decompress compressed data");
											this->m_isThisRequestNeedModifyResponse = false;
											break;
										}
									}
									else if(this->m_responseContentEncoding == CE_NONE) {
										decodedResponseContent.append(this->m_bufferedResponseData.begin() + doubleCrlfPos + 4, this->m_bufferedResponseData.end());
									}
									else {
										assert(false);
										this->m_isThisRequestNeedModifyResponse = false;
										break;
									}
								}
								else {
									assert(this->m_responseTransferEncoding == TE_CHUNKED);
									std::list<std::pair<std::string::iterator, std::string::iterator> > lst;
									if(!ChunkChecker::GetChunksRangeList(this->m_bufferedResponseData.begin() + doubleCrlfPos + 4, this->m_bufferedResponseData.end(), lst)
										|| lst.empty()) {
											ERROR_MSG_LOG("Failed to get chunked list");
											this->m_isThisRequestNeedModifyResponse = false;
											break;
									}
									std::size_t total_chunk_size = 0;
									for(std::list<std::pair<std::string::iterator, std::string::iterator> >::const_iterator iter = lst.begin(); iter != lst.end(); ++iter) {
										total_chunk_size += std::distance(iter->first, iter->second);
									}
									if(this->m_responseContentEncoding == CE_GZIP || this->m_responseContentEncoding == CE_DEFLATE) {
										bool decompressResult = false;
										// 预留内存 gzip对web页面的平均压缩率为25%(1/4)左右 预留5倍的大小
										decodedResponseContent.reserve(total_chunk_size * 5);
										if(this->m_responseContentEncoding == CE_GZIP) {
											GZipDecompressor gzipDecompressor;
											int ret = Z_OK;
											std::list<std::pair<std::string::iterator, std::string::iterator> >::const_iterator iter = lst.begin();
											for(; iter != lst.end(); ++iter) {
												std::string::const_iterator begin = iter->first;
												std::string::const_iterator end = iter->second;
												std::size_t chunk_length = std::distance(begin, end);
												ret = gzipDecompressor.Decompress(&(*begin), chunk_length, decodedResponseContent, true);
												if(ret != Z_OK || ret == Z_STREAM_END) {
													break;
												}
											}

											decompressResult = ret == Z_STREAM_END && iter != lst.end() && ++iter == lst.end();
										}
										else {
											assert(this->m_responseContentEncoding == CE_DEFLATE);
											DeflateDecompressor deflateDecompressor;
											int ret = Z_OK;
											std::list<std::pair<std::string::iterator, std::string::iterator> >::const_iterator iter = lst.begin();
											for(; iter != lst.end(); ++iter) {
												std::string::const_iterator begin = iter->first;
												std::string::const_iterator end = iter->second;
												std::size_t chunk_length = std::distance(begin, end);
												ret = deflateDecompressor.Decompress(&(*begin), chunk_length, decodedResponseContent, true);
												if(ret != Z_OK || ret == Z_STREAM_END) {
													break;
												}
											}
											decompressResult = ret == Z_STREAM_END && iter != lst.end() && ++iter == lst.end();
										}
										if(!decompressResult) {
											ERROR_MSG_LOG("Failed to decompress compressed data");
											this->m_isThisRequestNeedModifyResponse = false;
											break;
										}
									}
									else {
										// 预留内存
										 decodedResponseContent.reserve(total_chunk_size);
										for(std::list<std::pair<std::string::iterator, std::string::iterator> >::const_iterator iter = lst.begin(); iter != lst.end(); ++iter) {
											decodedResponseContent.append(iter->first, iter->second);
										}
									}
								}
								std::list<std::pair<std::pair<std::string::const_iterator, std::string::const_iterator>, bool> > content_list;
								content_list.push_back(std::make_pair(std::make_pair(decodedResponseContent.begin(), decodedResponseContent.end()), true));
								// 在</head>前面添加CSS代码
								std::size_t endHeadPos = this->GetHideElementCodeInsertPos(decodedResponseContent);
								std::string style;
								if(endHeadPos != std::string::npos) {
									std::string insertCode = this->GetInsertCSSCode(this->m_absoluteUrl);
									if(!insertCode.empty()) {
										style = "<style>";
										style += insertCode;
										style += "</style>";
										assert(content_list.size() == 1);
										content_list.clear();
										if(endHeadPos != 0) {
											content_list.push_back(std::make_pair(std::make_pair(decodedResponseContent.begin(), decodedResponseContent.begin() + endHeadPos), true));
										}
										content_list.push_back(std::make_pair(std::make_pair(style.begin(), style.end()), false));
										content_list.push_back(std::make_pair(std::make_pair(decodedResponseContent.begin() + endHeadPos, decodedResponseContent.end()), true));
									}
								}
								// 正则匹配替换
								std::vector<std::string> raw_rules = this->GetReplaceRule(this->m_absoluteUrl);
								std::vector<std::pair<std::pair<std::string, std::string>, bool> > rules;
								for(std::vector<std::string>::const_iterator raw_rules_iter = raw_rules.begin(); raw_rules_iter != raw_rules.end(); ++raw_rules_iter) {
									std::string pattern, replace;
									bool icase = false;
									if(ReplaceRuleToECMAScriptRegex(*raw_rules_iter, pattern, replace, icase)) {
										rules.push_back(std::make_pair(std::make_pair(pattern, replace), icase));
									}
								}

								for(std::vector<std::pair<std::pair<std::string, std::string>, bool> >::const_iterator rule_iter = rules.begin(); rule_iter != rules.end(); ++rule_iter) {
									try {
										boost::regex re(rule_iter->first.first);
										for(std::list<std::pair<std::pair<std::string::const_iterator, std::string::const_iterator>, bool> >::const_iterator content_list_iter = content_list.begin(); content_list_iter != content_list.end();) {
											if(!content_list_iter->second) {
												++content_list_iter;
												continue;
											}
											boost::match_results<std::string::const_iterator> match_results;
											if(!boost::regex_search(content_list_iter->first.first, content_list_iter->first.second, match_results, re)) {
												++content_list_iter;
												continue;
											}

											std::list<std::pair<std::pair<std::string::const_iterator, std::string::const_iterator>, bool> >::const_iterator insert_pos = content_list.erase(content_list_iter);
											if(match_results.suffix().matched) {
												insert_pos = content_list.insert(insert_pos, std::make_pair(std::make_pair(match_results.suffix().first, match_results.suffix().second), true));
											}
											content_list_iter = insert_pos;
											if(!rule_iter->first.second.empty()) {
												insert_pos = content_list.insert(insert_pos, std::make_pair(std::make_pair(rule_iter->first.second.begin(), rule_iter->first.second.end()), false));
											}
											if(match_results.prefix().matched) {
												content_list.insert(insert_pos, std::make_pair(std::make_pair(match_results.prefix().first, match_results.prefix().second), true));
											}
										}
									}
									catch(const boost::regex_error&) {
									}
								}

								if(content_list.size() == 1 && content_list.begin()->second) {
									this->m_isThisRequestNeedModifyResponse = false;
									break;
								}
								this->m_bufferedResponseData.resize(this->m_bufferedResponseData.find("\r\n") + 2);
								HttpHeaderContainerType header = this->m_responseHeader;
								header.erase("Transfer-Encoding");
								header.erase("Content-Encoding");
								header.erase("Content-Length");
								std::size_t modifiedContentLength = 0;
								for(std::list<std::pair<std::pair<std::string::const_iterator, std::string::const_iterator>, bool> >::const_iterator content_list_iter = content_list.begin(); content_list_iter != content_list.end(); ++ content_list_iter) {
									modifiedContentLength += std::distance(content_list_iter->first.first, content_list_iter->first.second);
								}
								std::string contentLengthValue;
								{
									std::stringstream ss;
									ss << modifiedContentLength;
									ss >> contentLengthValue;
								}
								header.insert(std::make_pair(std::string("Content-Length"), contentLengthValue));
								for(HttpHeaderContainerType::const_iterator iter = header.begin(); iter != header.end(); ++iter) {
									this->m_bufferedResponseData += iter->first;
									this->m_bufferedResponseData += ": ";
									this->m_bufferedResponseData += iter->second;
									this->m_bufferedResponseData += "\r\n";
								}
								this->m_bufferedResponseData += "\r\n";
								// 预留内存
								this->m_bufferedResponseData.reserve(this->m_bufferedResponseData.size() + modifiedContentLength);
								for(std::list<std::pair<std::pair<std::string::const_iterator, std::string::const_iterator>, bool> >::const_iterator content_list_iter = content_list.begin(); content_list_iter != content_list.end(); ++ content_list_iter) {
									this->m_bufferedResponseData.append(content_list_iter->first.first, content_list_iter->first.second);
								}

								// 修改属性
								this->m_bytesOfResponseContentHasRead = modifiedContentLength;
								this->m_responseTransferEncoding = TE_NONE;
								this->m_responseContentEncoding = CE_NONE;
								this->m_responseContentLength = this->m_bytesOfResponseContentHasRead;
								this->m_isThisRequestNeedModifyResponse = false;
								this->SendNotify(this->m_absoluteUrl);
							} while(false);
					}
				}
			}

			if(!this->m_isThisRequestNeedModifyResponse || this->m_state == CS_TUNNELLING) {
				if(this->m_state == CS_READ_HTTP_RESPONSE_CONTENT) {
					this->m_state = CS_WRITE_HTTP_RESPONSE_CONTENT;
				}
				if(!this->m_bufferedResponseData.empty()) {
					// 缓存的数据不为空 转发缓冲区的数据(含当前新读到的数据)
					boost::asio::async_write(this->m_userAgentSocket, boost::asio::buffer(this->m_bufferedResponseData),
						boost::bind(&TcpProxyConnection::HandleWriteDataToUserAgent, this->shared_from_this(), _1, _2, true));
				}
				else {
					// 转发当前新读到的数据
					boost::asio::async_write(this->m_userAgentSocket, boost::asio::buffer(this->m_downstreamBuffer, bytes_transferred),
						boost::bind(&TcpProxyConnection::HandleWriteDataToUserAgent, this->shared_from_this(), _1, _2, false));
				}
			}
			else {
				this->AsyncReadDataFromTargetServer();
			}
		}
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

void TcpProxyConnection::HandleWriteDataToUserAgent(const boost::system::error_code& error, std::size_t bytes_transferred, bool clearBuffer)
{
	if(!error) {
		if(clearBuffer) {
			this->m_bufferedResponseData.clear();
		}
		if(this->m_state == CS_WRITE_HTTP_RESPONSE_CONTENT) {
			if(this->m_responseContentLength) {
				if(this->m_bytesOfResponseContentHasRead == *this->m_responseContentLength ) {
					this->PrepareForNextHttpQuery();
					this->m_state = CS_WAIT_FOR_NEXT_HTTP_QUERY;
				}
				else {
					this->m_state = CS_READ_HTTP_RESPONSE_CONTENT;
				}
			}
			else if(this->m_responseTransferEncoding == TE_CHUNKED) {
				if(this->m_responseChunkChecker.IsAllChunkRead()) {
					this->PrepareForNextHttpQuery();
					this->m_state = CS_WAIT_FOR_NEXT_HTTP_QUERY;
				}
				else {
					this->m_state = CS_READ_HTTP_RESPONSE_CONTENT;
				}
			}
			else {
				this->m_state = CS_READ_HTTP_RESPONSE_CONTENT;
			}
		}
		else if(this->m_state == CS_WRITE_BLOCK_RESPONSE) {
			boost::system::error_code ec;
			if(this->m_targetServerSocket.is_open()) {
				this->m_targetServerSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
				this->m_targetServerSocket.close(ec);
			}
			if(this->m_userAgentSocket.is_open()) {
				this->m_userAgentSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
				this->m_userAgentSocket.close(ec);
			}
			return;
		}
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

void TcpProxyConnection::HandleWriteDataToTargetServer(const boost::system::error_code& error, std::size_t bytes_transferred, bool clearBuffer)
{
	if(!error) {
		if(clearBuffer) {
			this->m_bufferedRequestData.clear();
		}
		if(this->m_state == CS_WRITE_HTTP_REQUEST_HEADER) {
			// 更新状态
			if(this->m_requestContentLength) {
				if(this->m_bytesOfRequestContentHasRead == *this->m_requestContentLength) {
					this->m_state = CS_READ_HTTP_RESPONSE_LINE;
				}
				else {
					this->m_state = CS_TRANSFER_HTTP_REQUEST_CONTENT;
				}
			}
			else if(this->m_requestTransferEncoding == TE_CHUNKED) {
				if(this->m_requestChunkChecker.IsAllChunkRead()) {
					this->m_state = CS_READ_HTTP_RESPONSE_LINE;
				}
				else {
					this->m_state = CS_TRANSFER_HTTP_REQUEST_CONTENT;
				}
			}
			else {
				assert(false);
			}
		}
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

bool TcpProxyConnection::CheckIfIsNonHttpResponseViaResponseLine(std::string::const_iterator begin, std::string::const_iterator end)
{
	int state = 1;
	for(std::string::const_iterator iter = begin; iter != end && state != 0; ++iter) {
		switch(state) {
			case 1:
				if(*iter == 'H') {
					state = 2;
				}
				else {
					return true;
				}
				break;
			case 2:
				if(*iter == 'T') {
					state = 3;
				}
				else {
					return true;
				}
				break;
			case 3:
				if(*iter == 'T') {
					state = 4;
				}
				else {
					return true;
				}
				break;
			case 4:
				if(*iter == 'P') {
					state = 5;
				}
				else {
					return true;
				}
				break;
			case 5:
				if(*iter == '/') {
					state = 0;
				}
				else {
					return true;
				}
				break;
		}
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

std::vector<std::pair<std::string::const_iterator, std::string::const_iterator> > TcpProxyConnection::SplitHttpResponseLine(std::string::const_iterator begin, std::string::const_iterator end)
{
	std::vector<std::pair<std::string::const_iterator, std::string::const_iterator> > result;
	std::string::const_iterator iter = begin;
	for(;;) {
		for(;iter != end && std::isspace(static_cast<unsigned char>(*iter)); ++iter)
			;
		std::string::const_iterator range_begin = iter;
		for(;iter != end && !std::isspace(static_cast<unsigned char>(*iter)); ++iter)
			;
		std::string::const_iterator range_end = iter;
		if(range_begin == range_end) {
			break;
		}
		result.push_back(std::make_pair(range_begin, range_end));
	}
	return result;
}

bool TcpProxyConnection::IsSeperators(char ch) const
{
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

bool TcpProxyConnection::ParseHttpRequestHeader(std::string::const_iterator begin, std::string::const_iterator end, HttpHeaderContainerType& requestHeaders) const
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
				if(*iter == '\r') {
					state = 5;
				}
				else if(!std::isspace(static_cast<unsigned char>(*iter))) {
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

bool TcpProxyConnection::ParseHttpResponseHeader(std::string::const_iterator begin, std::string::const_iterator end, HttpHeaderContainerType& resonseHeader) const
{
	return this->ParseHttpRequestHeader(begin, end, resonseHeader);
}

void TcpProxyConnection::PrepareForNextHttpQuery()
{
	this->m_state = CS_WAIT_FOR_NEXT_HTTP_QUERY;
	this->m_requestString.clear();
	this->m_responseString.clear();
	this->m_bufferedRequestData.clear();
	this->m_httpRequestMethod = HTTP_UNKNOWN;
	this->m_responseStateCode = 0;
	this->m_relativeUrl.clear();
	this->m_absoluteUrl.clear();
	this->m_requestHeader.clear();
	this->m_responseHeader.clear();
	this->m_requestContentLength = boost::optional<std::size_t>();
	this->m_responseContentLength = boost::optional<std::size_t>();
	this->m_bytesOfRequestContentHasRead = 0;
	this->m_bytesOfResponseContentHasRead = 0;
	this->m_requestTransferEncoding = TE_NONE;
	this->m_responseTransferEncoding = TE_NONE;
	this->m_responseContentEncoding = CE_NONE;
	this->m_requestChunkChecker.Reset();
	this->m_responseChunkChecker.Reset();
	this->m_isThisRequestNeedModifyResponse = false;
}

bool TcpProxyConnection::IsCaseInsensitiveEqual(const std::string& lhs, const std::string& rhs) const
{
	if(lhs.size() != rhs.size()) {
		return false;
	}
	for(std::size_t index = 0; index < lhs.size(); ++index) {
		if(std::tolower(static_cast<unsigned char>(lhs[index])) != std::tolower(static_cast<unsigned char>(rhs[index]))) {
			return false;
		}
	}
	return true;
}

TransferEncoding TcpProxyConnection::GetTransferEncoding(const HttpHeaderContainerType& httpHeader) const
{
	HttpHeaderContainerType::const_iterator iter = httpHeader.find("Transfer-Encoding");
	if(iter == httpHeader.end()) {
		return TE_NONE;
	}
	else {
		std::size_t pos = iter->second.find(';');
		if(pos == std::string::npos) {
			pos = iter->second.size();
		}
		if(pos != 7) {
			return TE_UNKNOWN;
		}
		const char* szChunked = "chunked";
		for(std::size_t index = 0; index < 7; ++index) {
			if(std::tolower(static_cast<unsigned char>(iter->second[index])) != szChunked[index]) {
				return TE_UNKNOWN;
			}
		}
		return TE_CHUNKED;
	}
}

TransferEncoding TcpProxyConnection::GetRequestTransferEncoding() const
{
	return this->GetTransferEncoding(this->m_requestHeader);
}

TransferEncoding TcpProxyConnection::GetResponseTransferEncoding() const
{
	return this->GetTransferEncoding(this->m_responseHeader);
}

bool TcpProxyConnection::HasContentLength(const HttpHeaderContainerType& httpHeader) const
{
	HttpHeaderContainerType::const_iterator iter = httpHeader.find("Content-Length");
	return iter != httpHeader.end();
}

bool TcpProxyConnection::IsRequestHeaderHasContentLength() const
{
	return this->HasContentLength(this->m_requestHeader);
}

bool TcpProxyConnection::IsResponseHeaderHasContentLength() const
{
	return this->HasContentLength(this->m_responseHeader);
}

boost::optional<std::size_t> TcpProxyConnection::GetContentLength(const HttpHeaderContainerType& httpHeader) const
{
	boost::optional<std::size_t> result;
	HttpHeaderContainerType::const_iterator iter = httpHeader.find("Content-Length");
	if(iter != httpHeader.end()) {
		// 检查是否全部是十进制数
		bool convertable = true;
		for(std::size_t index = 0; index < iter->second.size(); ++index) {
			if(!std::isdigit(static_cast<unsigned char>(iter->second[index]))) {
				convertable = false;
				break;
			}
		}
		if(convertable) {
			std::size_t contentLength = 0;
			std::stringstream ss;
			ss << iter->second;
			ss >> contentLength;
			if(!ss.fail()) {
				result = contentLength;
			}
		}
	}
	return result;
}

boost::optional<std::size_t> TcpProxyConnection::GetRequestContentLength() const
{
	return this->GetContentLength(this->m_requestHeader);
}

boost::optional<std::size_t> TcpProxyConnection::GetResponseContentLength() const
{
	return this->GetContentLength(this->m_responseHeader);
}

ContentEncoding TcpProxyConnection::GetContentEncoding(const HttpHeaderContainerType& httpHeader) const
{
	HttpHeaderContainerType::const_iterator iter = httpHeader.find("Content-Encoding");
	if(iter == httpHeader.end()) {
		return CE_NONE;
	}
	else if(this->IsCaseInsensitiveEqual(iter->second, "gzip")) {
		return CE_GZIP;
	}
	else if(this->IsCaseInsensitiveEqual(iter->second, "deflate")) {
		return CE_DEFLATE;
	}
	else {
		return CE_UNKNOWN;
	}
}

ContentEncoding TcpProxyConnection::GetResponseContentEncoding() const
{
	return this->GetContentEncoding(this->m_responseHeader);
}

ContentType TcpProxyConnection::GetResponseContentType() const
{
	HttpHeaderContainerType::const_iterator iter = this->m_responseHeader.find("Content-Type");
	if(iter == this->m_responseHeader.end()) {
		return CT_NONE;
	}
	std::size_t semicolonPos = iter->second.find(';');
	if(semicolonPos == std::string::npos) {
		semicolonPos = iter->second.size();
	}
	std::string lower_type;
	for(std::size_t index = 0; index < semicolonPos; ++index) {
		lower_type.push_back(std::tolower(static_cast<unsigned char>(iter->second[index])));
	}

	if(lower_type == "application/atom+xml") {
		return CT_APPLICATION_ATOM_XML;
	}
	else if(lower_type == "application/dart") {
		return CT_APPLICATION_DART;
	}
	else if(lower_type == "application/ecmascript") {
		return CT_APPLICATION_ECMASCRIPT;
	}
	else if(lower_type == "application/edi-x12") {
		return CT_APPLICATION_EDI_X12;
	}
	else if(lower_type == "application/edifact") {
		return CT_APPLICATION_EDIFACT;
	}
	else if(lower_type == "application/json") {
		return CT_APPLICATION_JSON;
	}
	else if(lower_type == "application/javascript") {
		return CT_APPLICATION_JAVASCRIPT;
	}
	else if(lower_type == "application/octet-stream") {
		return CT_APPLICATION_OCTET_STREAM;
	}
	else if(lower_type == "application/ogg") {
		return CT_APPLICATION_OGG;
	}
	else if(lower_type == "application/pdf") {
		return CT_APPLICATION_PDF;
	}
	else if(lower_type == "application/postscript") {
		return CT_APPLICATION_POSTSCRIPT;
	}
	else if(lower_type == "application/rdf+xml") {
		return CT_APPLICATION_RDF_XML;
	}
	else if(lower_type == "application/rss+xml") {
		return CT_APPLICATION_RSS_XML;
	}
	else if(lower_type == "application/soap+xml") {
		return CT_APPLICATION_SOAP_XML;
	}
	else if(lower_type == "application/font-woff") {
		return CT_APPLICATION_FONT_WOFF;
	}
	else if(lower_type == "application/xhtml+xml") {
		return CT_APPLICATION_XHTML_XML;
	}
	else if(lower_type == "application/xml") {
		return CT_APPLICATION_XML;
	}
	else if(lower_type == "application/xml-dtd") {
		return CT_APPLICATION_XML_DTD;
	}
	else if(lower_type == "application/xop+xml") {
		return CT_APPLICATION_XOP_XML;
	}
	else if(lower_type == "application/zip") {
		return CT_APPLICATION_ZIP;
	}
	else if(lower_type == "application/gzip") {
		return CT_APPLICATION_GZIP;
	}
	else if(lower_type == "application/example") {
		return CT_APPLICATION_EXAMPLE;
	}
	else if(lower_type == "application/x-nacl") {
		return CT_APPLICATION_X_NACL;
	}
	else if(lower_type == "application/x-pnacl") {
		return CT_APPLICATION_X_PNACL;
	}
	else if(lower_type == "application/x-javascript") {
		return CT_APPLICATION_X_JAVASCRIPT; 
	}
	else if(lower_type == "audio/basic") {
		return CT_AUDIO_BASIC;
	}
	else if(lower_type == "audio/l24") {
		return CT_AUDIO_L24;
	}
	else if(lower_type == "audio/mp4") {
		return CT_AUDIO_MP4;
	}
	else if(lower_type == "audio/mpeg") {
		return CT_AUDIO_MPEG;
	}
	else if(lower_type == "audio/ogg") {
		return CT_AUDIO_OGG;
	}
	else if(lower_type == "audio/opus") {
		return CT_AUDIO_OPUS;
	}
	else if(lower_type == "audio/vorbis") {
		return CT_AUDIO_VORBIS;
	}
	else if(lower_type == "audio/vnd.rn-realaudio") {
		return CT_AUDIO_VND_RN_REALAUDIO;
	}
	else if(lower_type == "audio/vnd.wave") {
		return CT_AUDIO_VND_WAVE;
	}
	else if(lower_type == "audio/webm") {
		return CT_AUDIO_WEBM;
	}
	else if(lower_type == "audio/example") {
		return CT_AUDIO_EXAMPLE;
	}
	else if(lower_type == "image/gif") {
		return CT_IMAGE_GIF;
	}
	else if(lower_type == "image/jpeg") {
		return CT_IMAGE_JPEG;
	}
	else if(lower_type == "image/pjpeg") {
		return CT_IMAGE_PJPEG;
	}
	else if(lower_type == "image/png") {
		return CT_IMAGE_PNG;
	}
	else if(lower_type == "image/svg+xml") {
		return CT_IMAGE_SVG_XML;
	}
	else if(lower_type == "image/vnd.djvu") {
		return CT_IMAGE_VND_DJVU;
	}
	else if(lower_type == "image/example") {
		return CT_IMAGE_EXAMPLE;
	}
	else if(lower_type == "message/http") {
		return CT_MESSAGE_HTTP;
	}
	else if(lower_type == "message/imdn+xml") {
		return CT_MESSAGE_IMDN_XML;
	}
	else if(lower_type == "message/partial") {
		return CT_MESSAGE_PARTIAL;
	}
	else if(lower_type == "message/rfc822") {
		return CT_MESSAGE_RFC822;
	}
	else if(lower_type == "message/example") {
		return CT_MESSAGE_EXAMPLE;
	}
	else if(lower_type == "model/iges") {
		return CT_MODEL_IGES;
	}
	else if(lower_type == "model/mesh") {
		return CT_MODEL_MESH;
	}
	else if(lower_type == "model/vrml") {
		return CT_MODEL_VRML;
	}
	else if(lower_type == "model/x3d+binary") {
		return CT_MODEL_X3D_BINARY;
	}
	else if(lower_type == "model/x3d+fastinfoset") {
		return CT_MODEL_X3D_FASTINFOSET;
	}
	else if(lower_type == "model/x3d-vrml") {
		return CT_MODEL_X3D_VRML;
	}
	else if(lower_type == "model/x3d+xml") {
		return CT_MODEL_X3D_XML;
	}
	else if(lower_type == "model/example") {
		return CT_MODEL_EXAMPLE;
	}
	else if(lower_type == "multipart/mixed") {
		return CT_MULTIPART_MIXED;
	}
	else if(lower_type == "multipart/alternative") {
		return CT_MULTIPART_ALTERNATIVE;
	}
	else if(lower_type == "multipart/related") {
		return CT_MULTIPART_RELATED;
	}
	else if(lower_type == "multipart/form-data") {
		return CT_MULTIPART_FORM_DATA;
	}
	else if(lower_type == "multipart/signed") {
		return CT_MULTIPART_SIGNED;
	}
	else if(lower_type == "multipart/encrypted") {
		return CT_MULTIPART_ENCRYPTED;
	}
	else if(lower_type == "multipart/example") {
		return CT_MULTIPART_EXAMPLE;
	}
	else if(lower_type == "text/cmd") {
		return CT_TEXT_CMD;
	}
	else if(lower_type == "text/css") {
		return CT_TEXT_CSS;
	}
	else if(lower_type == "text/csv") {
		return CT_TEXT_CSV;
	}
	else if(lower_type == "text/example") {
		return CT_TEXT_EXAMPLE;
	}
	else if(lower_type == "text/html") {
		return CT_TEXT_HTML;
	}
	else if(lower_type == "text/javascript") {
		return CT_TEXT_JAVASCRIPT;
	}
	else if(lower_type == "text/plain") {
		return CT_TEXT_PLAIN;
	}
	else if(lower_type == "text/rtf") {
		return CT_TEXT_RTF;
	}
	else if(lower_type == "text/vcard") {
		return CT_TEXT_VCARD;
	}
	else if(lower_type == "text/vnd.abc") {
		return CT_TEXT_VND_ABC;
	}
	else if(lower_type == "text/xml") {
		return CT_TEXT_XML;
	}
	else if(lower_type == "video/avi") {
		return CT_VIDEO_AVI;
	}
	else if(lower_type == "video/example") {
		return CT_VIDEO_EXAMPLE;
	}
	else if(lower_type == "video/mpeg") {
		return CT_VIDEO_MPEG;
	}
	else if(lower_type == "video/mp4") {
		return CT_VIDEO_MP4;
	}
	else if(lower_type == "video/ogg") {
		return CT_VIDEO_OGG;
	}
	else if(lower_type == "video/quicktime") {
		return CT_VIDEO_QUICKTIME;
	}
	else if(lower_type == "video/webm") {
		return CT_VIDEO_WEBM;
	}
	else if(lower_type == "video/x-matroska") {
		return CT_VIDEO_X_MATROSKA;
	}
	else if(lower_type == "video/x-ms-wmv") {
		return CT_VIDEO_X_MS_WMV;
	}
	else if(lower_type == "video/x-flv") {
		return CT_VIDEO_X_FLV;
	}

	return CT_UNKNOWN;
}

bool TcpProxyConnection::ResponseContentTypeIsText() const
{
	switch(this->GetResponseContentType()) {
		case CT_APPLICATION_ATOM_XML:
		case CT_APPLICATION_DART:
		case CT_APPLICATION_ECMASCRIPT:
		// case CT_APPLICATION_EDI_X12:
		// case CT_APPLICATION_EDIFACT:
		case CT_APPLICATION_JSON:
		case CT_APPLICATION_JAVASCRIPT:
		// case CT_APPLICATION_OCTET_STREAM:
		// case CT_APPLICATION_OGG:
		// case CT_APPLICATION_PDF:
		case CT_APPLICATION_POSTSCRIPT:
		case CT_APPLICATION_RDF_XML:
		case CT_APPLICATION_RSS_XML:
		case CT_APPLICATION_SOAP_XML:
		// case CT_APPLICATION_FONT_WOFF:
		case CT_APPLICATION_XHTML_XML:
		case CT_APPLICATION_XML:
		case CT_APPLICATION_XML_DTD:
		case CT_APPLICATION_XOP_XML:
		// case CT_APPLICATION_ZIP:
		// case CT_APPLICATION_GZIP:
		// case CT_APPLICATION_EXAMPLE:
		// case CT_APPLICATION_X_NACL:
		// case CT_APPLICATION_X_PNACL:
		case CT_APPLICATION_X_JAVASCRIPT:
		// case CT_AUDIO_BASIC:
		// case CT_AUDIO_L24:
		// case CT_AUDIO_MP4:
		// case CT_AUDIO_MPEG:
		// case CT_AUDIO_OGG:
		// case CT_AUDIO_OPUS:
		// case CT_AUDIO_VORBIS:
		// case CT_AUDIO_VND_RN_REALAUDIO:
		// case CT_AUDIO_VND_WAVE:
		// case CT_AUDIO_WEBM:
		// case CT_AUDIO_EXAMPLE:
		// case CT_IMAGE_GIF:
		// case CT_IMAGE_JPEG:
		// case CT_IMAGE_PJPEG:
		// case CT_IMAGE_PNG:
		// case CT_IMAGE_SVG_XML:
		// case CT_IMAGE_VND_DJVU:
		// case CT_IMAGE_EXAMPLE:
		// case CT_MESSAGE_HTTP:
		// case CT_MESSAGE_IMDN_XML:
		// case CT_MESSAGE_PARTIAL:
		// case CT_MESSAGE_RFC822:
		// case CT_MESSAGE_EXAMPLE:
		// case CT_MODEL_IGES:
		// case CT_MODEL_MESH:
		// case CT_MODEL_VRML:
		// case CT_MODEL_X3D_BINARY:
		// case CT_MODEL_X3D_FASTINFOSET:
		// case CT_MODEL_X3D_VRML:
		// case CT_MODEL_X3D_XML:
		// case CT_MODEL_EXAMPLE:
		// case CT_MULTIPART_MIXED:
		// case CT_MULTIPART_ALTERNATIVE:
		// case CT_MULTIPART_RELATED:
		// case CT_MULTIPART_FORM_DATA:
		// case CT_MULTIPART_SIGNED:
		// case CT_MULTIPART_ENCRYPTED:
		// case CT_MULTIPART_EXAMPLE:
		case CT_TEXT_CMD:
		case CT_TEXT_CSS:
		case CT_TEXT_CSV:
		case CT_TEXT_EXAMPLE:
		case CT_TEXT_HTML:
		case CT_TEXT_JAVASCRIPT:
		case CT_TEXT_PLAIN:
		case CT_TEXT_RTF:
		case CT_TEXT_VCARD:
		case CT_TEXT_VND_ABC:
		case CT_TEXT_XML:
		// case CT_VIDEO_AVI:
		// case CT_VIDEO_EXAMPLE:
		// case CT_VIDEO_MPEG:
		// case CT_VIDEO_MP4:
		// case CT_VIDEO_OGG:
		// case CT_VIDEO_QUICKTIME:
		// case CT_VIDEO_WEBM:
		// case CT_VIDEO_X_MATROSKA:
		// case CT_VIDEO_X_MS_WMV:
		// case CT_VIDEO_X_FLV:
			return true;
		default:
			return false;
	}
}

std::size_t TcpProxyConnection::GetHideElementCodeInsertPos(const std::string& decodedContentData) const
{
	int state = 0;
	std::size_t insert_pos = 0;
	const std::string& data = decodedContentData;
	for(std::size_t index = 0; index < data.size() && state != 7; ++index) {
		switch(state) {
			case 0:
				if(data[index] == '<') {
					insert_pos = index;
					state = 1;
				}
				break;
			case 1:
				if(data[index] == ' ') {
					continue;
				}
				else if(data[index] == '/') {
					state = 2;
				}
				else {
					state = 0;
				}
				break;
			case 2:
				if(data[index] == ' ') {
					continue;
				}
				else if(std::tolower(static_cast<unsigned char>(data[index])) == 'h') {
					state = 3;
				}
				else {
					state = 0;
				}
				break;
			case 3:
				if(std::tolower(static_cast<unsigned char>(data[index])) == 'e') {
					state = 4;
				}
				else {
					state = 0;
				}
				break;
			case 4:
				if(std::tolower(static_cast<unsigned char>(data[index])) == 'a') {
					state = 5;
				}
				else {
					state = 0;
				}
				break;
			case 5:
				if(std::tolower(static_cast<unsigned char>(data[index])) == 'd') {
					state = 6;
				}
				else {
					state = 0;
				}
				break;
			case 6:
				if(data[index] == ' ') {
					continue;
				}
				else if(data[index] == '>') {
					state = 7;
				}
				else {
					state = 0;
				}
				break;
		}
	}
	if(state == 7) {
		return insert_pos;
	}
	else {
		return std::string::npos;
	}
}

std::string TcpProxyConnection::GetRequestReferer() const
{
	HttpHeaderContainerType::const_iterator iter = this->m_requestHeader.find("Referer");
	if(iter == this->m_requestHeader.end()) {
		return std::string();
	}
	else {
		return iter->second;
	}
}

bool TcpProxyConnection::IsJavaScriptUrl(const std::string& url) const
{
	std::size_t query_start_pos = url.find('?');
	if(query_start_pos == std::string::npos) {
		query_start_pos = url.size();
	}
	if(query_start_pos < 1) {
		return false;
	}
	std::size_t last_slash_pos = std::string::npos;
	for(std::size_t index = query_start_pos - 1; index != 0; --index) {
		if(url[index] == '/') {
			last_slash_pos = index;
			break;
		}
	}
	if(last_slash_pos == std::string::npos) {
		return false;
	}
	assert(last_slash_pos > 0);
	std::size_t dot_pos = std::string::npos;
	for(std::size_t index = query_start_pos - 1; index > last_slash_pos; --index) {
		if(url[index] == '.') {
			dot_pos = index;
			break;
		}
	}
	if(dot_pos == std::string::npos) {
		return false;
	}
	if(dot_pos + 3 == query_start_pos) {
		if(std::tolower(static_cast<unsigned char>(url[dot_pos + 1]) == 'j')
			&& std::tolower(static_cast<unsigned char>(url[dot_pos + 1]) == 'j')) {
				return true;
		}
	}
	return false;
}

bool TcpProxyConnection::ShouldBlockRequest(const std::string& url, const std::string& referer, bool& blockUseForbiden) const
{
	FilterManager* m = FilterManager::getManager();
	if(m == NULL) {
		return false;
	}
	else {
		Url requestUrl(url.c_str()), refererUrl(referer.c_str());
		int blockPolicy = m->shouldFilter(refererUrl, requestUrl);
		if (blockPolicy == 0) {
			return false;
		}
		else if (blockPolicy == 2) {
			blockUseForbiden = true;
		}
		return true;
	}
}

bool TcpProxyConnection::NeedInsertCSSCode(const std::string& url) const
{
	FilterManager* m = FilterManager::getManager();
	if(m == NULL) {
		return false;
	}
	else {
		Url requestUrl(url.c_str());
		std::string csscode = m->getcssRules(requestUrl);
		return !csscode.empty();
	}
}

std::string TcpProxyConnection::GetInsertCSSCode(const std::string& url) const
{
	FilterManager* m = FilterManager::getManager();
	if(m == NULL) {
		return "";
	}
	else {
		Url requestUrl(url.c_str());
		return m->getcssRules(requestUrl);
	}
}

bool TcpProxyConnection::NeedReplaceContent(const std::string& url) const
{
	FilterManager* m = FilterManager::getManager();
	if(m == NULL) {
		return false;
	}
	else {
		Url requestUrl(url.c_str());
		std::vector<std::string> v;
		m->getreplaceRules(requestUrl, v);
		return !v.empty();
	}
}

std::vector<std::string> TcpProxyConnection::GetReplaceRule(const std::string& url) const
{
	FilterManager* m = FilterManager::getManager();
	std::vector<std::string> v;
	if(m == NULL) {
		return v;
	}
	else {
		Url requestUrl(url.c_str());
		m->getreplaceRules(requestUrl, v);
		return v;
	}
}

std::pair<bool, boost::optional<std::string> > TcpProxyConnection::ShouldRedirect(const std::string& url, const std::string& referer) const
{
	FilterManager* m = FilterManager::getManager();
	if(m != NULL) {
		std::string redirectUrl;
		if(m->shouldRedirect(Url(referer.c_str()), Url(url.c_str()), redirectUrl)) {
			return std::make_pair(true, boost::optional<std::string>(redirectUrl));
		}
	}
	return std::make_pair(false, boost::optional<std::string>());
}

void TcpProxyConnection::SendNotify(const std::string& url) const
{
	static HWND hNotifyWnd = ::FindWindow(L"{D8BD00DC-74BF-45ad-B8CB-61CB31C2CE84}_weberaser", NULL);
	if(hNotifyWnd != NULL) 
	{
		char* szUrl = new char[url.size() + 1];
		std::copy(url.begin(), url.end(), szUrl);
		szUrl[url.size()] = '\0';
		if(::PostMessage(hNotifyWnd, WM_USER + 201, WPARAM(1), LPARAM(szUrl))== FALSE) {
			delete szUrl;
		}
	}
}

void TcpProxyConnection::SendRedirectNotify(const std::string& url) const
{
	HWND hNotifyWnd = ::FindWindow(L"{D8BD00DC-74BF-45ad-B8CB-61CB31C2CE84}_weberaser", NULL);
	if(hNotifyWnd != NULL) 
	{
		char* szUrl = new char[url.size() + 1];
		std::copy(url.begin(), url.end(), szUrl);
		szUrl[url.size()] = '\0';
		if(::PostMessage(hNotifyWnd, WM_USER + 203, WPARAM(1), LPARAM(szUrl))== FALSE) {
			delete szUrl;
		}
	}
}
