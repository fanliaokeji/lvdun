#include "stdafx.h"
#include "TcpProxyServer.h"
#include <boost/bind.hpp>

TcpProxyServer::TcpProxyServer()
	: acceptor(io_service),
	listen_port(0)
{
}

bool TcpProxyServer::Open(const boost::asio::ip::tcp::acceptor::protocol_type& protocol)
{
	boost::system::error_code ec;
	if(!this->acceptor.is_open()) {
		try {
			this->acceptor.open(protocol);
		}
		catch(const boost::system::system_error&) {
			return false;
		}
	}
	return true;
}

bool TcpProxyServer::Bind(boost::asio::ip::address address, unsigned short listen_port)
{
	this->listen_port = listen_port;
	if(!this->acceptor.is_open()) {
		return false;	
	}
	boost::asio::ip::tcp::acceptor::endpoint_type endpoint(address, listen_port);
	boost::system::error_code ec;
	this->acceptor.bind(endpoint, ec);
	return !ec;
}

bool TcpProxyServer::Listen(int backlog)
{
	boost::system::error_code ec;
	this->acceptor.listen(backlog, ec);
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
