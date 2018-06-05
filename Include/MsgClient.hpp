#pragma once

#include <memory>
#include <functional>
#include "MsgConnection.hpp"

namespace msg
{

typedef std::function<std::unique_ptr<Connection>()> ConnectionFactory;

class Client
{
public:
	Client(ConnectionFactory p_factory);

	std::string sendReq(const std::string& p_req);
	void sendInd(const std::string& p_ind);

private:
	ConnectionFactory factory;
};

}
