#pragma once
#include <Winsock2.h>
#include <Mswsock.h>
#include <set>
#include <map>

#include "Lock.h"

class WinsockHooker {
	typedef SOCKET (WSAAPI* socket_FuncType)(int, int, int);
	typedef SOCKET (WSAAPI* WSASocket_FuncType)(int, int, int, LPWSAPROTOCOL_INFO, GROUP, DWORD);
	typedef int (WSAAPI* connect_FuncType)(SOCKET, const struct sockaddr*, int);
	typedef int (WSAAPI* closesocket_FuncType)(SOCKET);
	typedef int (WSAAPI* WSAIoctl_FuncType)(SOCKET, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
private:
	static socket_FuncType Real_socket;
	static WSASocket_FuncType Real_WSASocket;
	static connect_FuncType Real_connect;
	static closesocket_FuncType Real_closesocket;
	static WSAIoctl_FuncType Real_WSAIoctl;
private:
	static XMLib::CriticalSection cs;
	static std::set<SOCKET> TcpSocketSet;
	static std::map<SOCKET, LPFN_CONNECTEX> Connect_Ex_Funcs;
	static bool IsHooked;
private:
	static bool IsEnable(unsigned short* proxy_port);
public:
	static SOCKET WSAAPI Hooked_socket(int af, int type, int protocol);
	static SOCKET WSAAPI Hooked_WSASocket(int af, int type, int protocol, LPWSAPROTOCOL_INFO lpProtocolInfo, GROUP g, DWORD dwFlags);
	static int WSAAPI Hooked_connect(SOCKET s, const struct sockaddr *name, int namelen);
	static int WSAAPI Hooked_closesocket(SOCKET s);
	static int WSAAPI Hooked_WSAIoctl(SOCKET s, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	static BOOL WSAAPI Hooked_ExtendConnectEx(SOCKET s, const struct sockaddr *name, int namelen, PVOID lpSendBuffer, DWORD dwSendDataLength, LPDWORD lpdwBytesSent, LPOVERLAPPED lpOverlapped);
public:
	static bool AttachHook();
	static void DetachHook();
};
