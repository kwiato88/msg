#pragma once

#include <string>
#include <boost/none.hpp>

namespace msg
{

class Server
{
public:
	virtual ~Server() {}

	virtual std::string receiveReg() = 0;
	virtual void reply(const std::string& p_response) = 0;
	virtual void reply(const boost::none_t&) = 0;
};

}
