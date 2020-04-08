#include "MsgHandler.hpp"

namespace msg
{

std::string Handler::handleMsg(const std::string& p_req)
{
	try
	{
		return handle(p_req);
	}
	catch (std::exception& e)
	{
		return onError(e);
	}
}

std::string Handler::onError(std::exception& e)
{
	return "";
}

std::string EmptyHandler::handle(const std::string&)
{
	return "";
}

}
