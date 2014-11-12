#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "TcpProxyConnection.h"

class TcpProxyServer {
	boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor;
	unsigned short listen_port;
public:
	TcpProxyServer();
	bool Open(const boost::asio::ip::tcp::acceptor::protocol_type& protocol);
	bool Bind(boost::asio::ip::address address, unsigned short listen_port);
	bool Listen(int backlog);
    void Run();
private:
	void AsyncAccept();
	void HandleAccept(boost::shared_ptr<TcpProxyConnection> connection_ptr, const boost::system::error_code& error);
};
