#pragma once
#include <Winsock2.h>
#include <set>

#include "Lock.h"

class WinsockHooker {
	typedef SOCKET (WSAAPI* socket_FuncType)(int, int, int);
	typedef SOCKET (WSAAPI* WSASocket_FuncType)(int, int, int, LPWSAPROTOCOL_INFO, GROUP, DWORD);
	typedef int (WSAAPI* connect_FuncType)(SOCKET, const struct sockaddr*, int);
	typedef int (WSAAPI* closesocket_FuncType)(SOCKET);
private:
	static socket_FuncType Real_socket;
	static WSASocket_FuncType Real_WSASocket;
	static connect_FuncType Real_connect;
	static closesocket_FuncType Real_closesocket;
private:
	static XMLib::CriticalSection cs;
	static std::set<SOCKET> TcpSocketSet;
	static bool IsHooked;
private:
	static bool IsEnable();
public:
	static SOCKET WSAAPI Hooked_socket(int af, int type, int protocol);
	static SOCKET WSAAPI Hooked_WSASocket(int af, int type, int protocol, LPWSAPROTOCOL_INFO lpProtocolInfo, GROUP g, DWORD dwFlags);
	static int WSAAPI Hooked_connect(SOCKET s, const struct sockaddr *name, int namelen);
	static int WSAAPI Hooked_closesocket(SOCKET s);
public:
	static bool AttachHook();
	static void DetachHook();
};
