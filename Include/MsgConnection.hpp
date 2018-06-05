#pragma once

#include <string>

namespace msg
{

class Connection
{
public:
	virtual ~Connection() {}

	virtual void send(const std::string& p_data) = 0;
	virtual std::string receive() = 0;
};
}

