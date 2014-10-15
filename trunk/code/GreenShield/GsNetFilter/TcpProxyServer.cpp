#include "stdafx.h"
#include "TcpProxyServer.h"
#include <boost/bind.hpp>

TcpProxyServer::TcpProxyServer()
	: acceptor(io_service),
	listen_port(0)
{
}

bool TcpProxyServer::Bind(boost::asio::ip::address address, unsigned short listen_port)
{
	boost::system::error_code ec;
	this->listen_port = listen_port;
	boost::asio::ip::tcp::acceptor::endpoint_type endpoint(address, listen_port);
	if(!this->acceptor.is_open()) {
		this->acceptor.open(endpoint.protocol(), ec);
		if(ec) {
			return false;	
		}
	}
	this->acceptor.bind(endpoint, ec);
	if(ec) {
		return false;
	}
	this->acceptor.listen(boost::asio::ip::tcp::acceptor::max_connections, ec);
	return !ec;
}

void TcpProxyServer::Run()
{
	this->AsyncAccept();
	this->io_service.run();
}

void TcpProxyServer::AsyncAccept()
{
	boost::shared_ptr<TcpProxyConnection> connection_ptr = TcpProxyConnection::CreateConnection(this->io_service);
	this->acceptor.async_accept(connection_ptr->GetUserAgentSocketRef(), 
		boost::bind(&TcpProxyServer::HandleAccept, this, connection_ptr, _1));
}

void TcpProxyServer::HandleAccept(boost::shared_ptr<TcpProxyConnection> connection_ptr, const boost::system::error_code& error)
{
	if(!error) {
        connection_ptr->AsyncStart(this->listen_port);
        this->AsyncAccept();
    }
}
