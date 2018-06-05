#include "MsgService.hpp"

namespace msg
{

Service::Service(ServerFacotry p_facotry, GetMsgId p_getMsgId)
	: factory(p_facotry), getMsgId(p_getMsgId), server(nullptr), isRunning(false)
{}

void Service::setup()
{
	server = std::move(factory());
	isRunning = true;
}

void Service::start()
{
	setup();
	while (isRunning)
		handleRequest();
}

void Service::handleRequest()
{
	auto resp = handle(server->receiveReg());
	if (resp.empty())
		server->reply(boost::none);
	else
		server->reply(resp);
}

std::string Service::handle(const std::string& p_req)
{
	MsgId id = getMsgId(p_req);
	auto handler = handlers.find(id);
	if (handler != handlers.end())
		return handler->second->handle(p_req);
	return "";
}

void Service::stop()
{
	isRunning = false;
	server.reset();
}

void Service::addHandler(MsgId p_id, std::unique_ptr<Handler> p_handler)
{
	handlers[p_id] = std::move(p_handler);
}

}
