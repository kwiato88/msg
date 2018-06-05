#include "MsgTcpIpConnection.hpp"

namespace msg
{

TcpIpConnection::TcpIpConnection(const std::string& p_host, const std::string& p_port)
	: conn(p_host, p_port)
{}

void TcpIpConnection::send(const std::string& p_data)
{
	conn.send(p_data);
}

std::string TcpIpConnection::receive()
{
	return conn.receive();
}

}
