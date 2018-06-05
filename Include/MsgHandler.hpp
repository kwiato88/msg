#pragma once

#include <string>

namespace msg
{

class Handler
{
public:
	virtual std::string handle(const std::string& p_req) = 0;
};

}
