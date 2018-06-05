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

class Service
{
public:
	typedef int MsgId;
	typedef std::function<MsgId(const std::string& p_payload)> GetMsgId;

	Service(ServerFacotry p_facotry, GetMsgId p_getMsgId);

	void start();
	void stop();
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

}
