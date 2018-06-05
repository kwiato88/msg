#include "MsgTcpIpServer.hpp"

namespace msg
{

TcpIpServer::TcpIpServer(const std::string& p_host, const std::string& p_port)
	: ser(p_host, p_port), conn(nullptr)
{}

std::string TcpIpServer::receiveReg()
{
	conn = std::move(ser.accept());
	return conn->receive();
}

void TcpIpServer::reply(const std::string& p_response)
{
	conn->send(p_response);
	conn.reset();
}

void TcpIpServer::reply(const boost::none_t&)
{
	conn.reset();
}

}
