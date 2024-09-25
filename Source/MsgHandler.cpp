#include "MsgHandler.hpp"

namespace msg
{

std::string NullHandler::handle(const std::string&)
{
	return "";
}

}
