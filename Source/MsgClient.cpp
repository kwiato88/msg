#include "MsgClient.hpp"

namespace msg
{

Client::Client(ConnectionFactory p_factory)
	: factory(p_factory)
{}

std::string Client::sendReq(const std::string& p_req)
{
	auto conn(std::move(factory()));
	conn->send(p_req);
	return conn->receive();
}

void Client::sendInd(const std::string& p_ind)
{
	auto conn(std::move(factory()));
	conn->send(p_ind);
}

}
