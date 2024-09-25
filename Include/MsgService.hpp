#pragma once

#include <iostream>
#include <memory>
#include <map>
#include "MsgHandler.hpp"
#include "MsgServer.hpp"
#include "MsgReqHandler.hpp"
#include "MsgDecodingHandler.hpp"

namespace msg
{

typedef std::function<std::unique_ptr<Server>()> ServerFacotry;

// Definitions inside Codec
//   typename Codec::ExceptionType
//   typename Codec::IdType
//   IdType Codec::getId(const std::string& p_data)
//   std::string Codec::encode(const MsgType& msg)
//   MsgType Codec::decode(const std::string& msg)
template <typename Codec>
class Service
{
public:
	using IdType = typename Codec::IdType;

	Service(ServerFacotry p_facotry);
	void start();
	void stop();
	void cleanup();
	template <typename Req, typename Resp>
	void addHandler(IdType p_id, std::unique_ptr<ReqHandler<Req, Resp> > p_handler);
	template <typename Ind>
	void addHandler(IdType p_id, std::unique_ptr<IndHandler<Ind> > p_handler);
	void setDefaultHandler(std::unique_ptr<Handler> p_handler);

private:
	void setup();
	void handle();
	void reply(const std::string& p_response);

	ServerFacotry factory;
	std::unique_ptr<Server> server;
	bool isRunning;
	std::map<IdType, std::shared_ptr<Handler> > handlers;
	std::unique_ptr<Handler> defaultHandler;
};

template<typename Codec>
Service<Codec>::Service(ServerFacotry p_facotry)
	: factory(p_facotry), server(nullptr), isRunning(false), defaultHandler(std::make_unique<NullHandler>())
{}

template<typename Codec>
void Service<Codec>::start()
{
	setup();
	while (isRunning)
		handle();
}

template<typename Codec>
void Service<Codec>::stop()
{
	isRunning = false;
}

template<typename Codec>
void Service<Codec>::cleanup()
{
	server.reset();
}

template<typename Codec>
void Service<Codec>::setup()
{
	server = std::move(factory());
	isRunning = true;
}

template<typename Codec>
void Service<Codec>::reply(const std::string& p_response)
{
	if(p_response.empty())
		server->reply(boost::none);
	else
		server->reply(p_response);
}

template<typename Codec>
void Service<Codec>::handle()
{
	try
	{
		std::string msg = server->receiveReq();
		auto handler = handlers.find(Codec::getId(msg));
		if (handler == handlers.end())
			reply(defaultHandler->handle(msg));
		else
			reply(handler->second->handle(msg));
	}
	catch (std::exception& e)
	{
		std::cerr << "Service: error while handling message. " << e.what() << std::endl;
		server->reply(boost::none);
	}
}

template <typename Codec>
template <typename Req, typename Resp>
void Service<Codec>::addHandler(IdType p_id, std::unique_ptr<ReqHandler<Req, Resp> > p_handler)
{
	handlers.insert_or_assign(p_id, std::make_unique<DecodingHandler<Req, Resp, Codec> >(std::move(p_handler)));
}

template <typename Codec>
template <typename Ind>
void Service<Codec>::addHandler(IdType p_id, std::unique_ptr<IndHandler<Ind> > p_handler)
{
	handlers.insert_or_assign(p_id, std::make_unique<IndicationDecondingHandler<Ind, Codec> >(std::move(p_handler)));
}

template <typename Codec>
void Service<Codec>::setDefaultHandler(std::unique_ptr<Handler> p_handler)
{
	defaultHandler = std::move(p_handler);
}

}