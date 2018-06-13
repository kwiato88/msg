#pragma once

#include "MsgServer.hpp"
#include "SockServer.hpp"

namespace msg
{

class TcpIpServer : public Server
{
public:
	TcpIpServer(const std::string& p_host, const std::string& p_port);

	std::string receiveReq() override;
	void reply(const std::string& p_response) override;
	void reply(const boost::none_t&) override;

private:
	sock::Server ser;
	std::unique_ptr<sock::Connection> conn;
};

}
