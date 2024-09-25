#pragma once

#include <string>
#include <exception>

namespace msg
{

class Handler
{
public:
    virtual ~Handler() {}
	virtual std::string handle(const std::string& p_req) = 0;
};


class NullHandler : public Handler
{
public:
	std::string handle(const std::string& p_req) override;
};

}