#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "TcpProxyConnection.h"

class TcpProxyServer {
	boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor;
public:
	TcpProxyServer();
	bool Bind(boost::asio::ip::address address, unsigned short listen_port);
    void Run();
private:
	void AsyncAccept();
	void HandleAccept(boost::shared_ptr<TcpProxyConnection> connection_ptr, const boost::system::error_code& error);
};
