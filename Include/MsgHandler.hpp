#pragma once

#include <string>
#include <exception>

namespace msg
{

class Handler
{
public:
    virtual ~Handler() {}
	virtual std::string handleMsg(const std::string& p_req);
	virtual std::string handle(const std::string& p_req) = 0;
	virtual std::string onError(std::exception& e);
};

class EmptyHandler : public Handler
{
public:
	std::string handle(const std::string&);
};

}
