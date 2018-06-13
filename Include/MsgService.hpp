#pragma once

#include <memory>
#include <map>
#include <functional>
#include "MsgHandler.hpp"
#include "MsgServer.hpp"

#include "MsgReqHandler.hpp"
#include "MsgDecodingHandler.hpp"

namespace msg
{

typedef std::function<std::unique_ptr<Server>()> ServerFacotry;

template<typename MsgId>
class Service
{
public:
	typedef std::function<MsgId(const std::string& p_payload)> GetMsgId;

	Service(ServerFacotry p_facotry, GetMsgId p_getMsgId);

	void start();
	void stop();
	void cleanup();
	void addHandler(MsgId p_id, std::unique_ptr<Handler> p_handler);

private:
	void setup();
	void handleRequest();
	std::string handle(const std::string& p_req);

	ServerFacotry factory;
	GetMsgId getMsgId;
	std::unique_ptr<Server> server;
	bool isRunning;
	std::map<MsgId, std::unique_ptr<Handler>> handlers;
};

template<typename MsgId>
Service<MsgId>::Service(ServerFacotry p_facotry, GetMsgId p_getMsgId)
	: factory(p_facotry), getMsgId(p_getMsgId), server(nullptr), isRunning(false)
{}

template<typename MsgId>
void Service<MsgId>::setup()
{
	server = std::move(factory());
	isRunning = true;
}

template<typename MsgId>
void Service<MsgId>::start()
{
	setup();
	while (isRunning)
		handleRequest();
}

template<typename MsgId>
void Service<MsgId>::handleRequest()
{
	auto resp = handle(server->receiveReq());
	if (resp.empty())
		server->reply(boost::none);
	else
		server->reply(resp);
}

template<typename MsgId>
std::string Service<MsgId>::handle(const std::string& p_req)
{
	MsgId id = getMsgId(p_req);
	auto handler = handlers.find(id);
	if (handler != handlers.end())
		return handler->second->handle(p_req);
	return "";
}

template<typename MsgId>
void Service<MsgId>::stop()
{
	isRunning = false;
}

template<typename MsgId>
void Service<MsgId>::cleanup()
{
	server.reset();
}

template<typename MsgId>
void Service<MsgId>::addHandler(MsgId p_id, std::unique_ptr<Handler> p_handler)
{
	handlers[p_id] = std::move(p_handler);
}

}
