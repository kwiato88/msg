#pragma once

#include "MsgConnection.hpp"
#include "SockConnection.hpp"

namespace msg
{

class TcpIpConnection : public Connection
{
public:
	TcpIpConnection(const std::string& p_host, const std::string& p_port);

	void send(const std::string& p_data) override;
	std::string receive() override;

private:
	sock::Connection conn;
};

}
