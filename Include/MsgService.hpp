#pragma once

#include <iostream>
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
	void setDefaultHandler(std::unique_ptr<Handler> p_handler);

	template<typename HandlerType, typename Codec>
	void addReqHandler(std::unique_ptr<HandlerType> p_handler);
	template<typename HandlerType, typename Codec>
	void addIndHandler(std::unique_ptr<HandlerType> p_handler);

	template<typename StopMessage>
	class StopHandler : public msg::IndicationHandler<StopMessage>
	{
	public:
		StopHandler(Service<MsgId>& p_service);
		StopHandler(Service<MsgId>& p_service, std::function<void(void)> p_callback);
		void handle(const StopMessage& p_msg);
	private:
		Service<MsgId>& service;
		std::function<void(void)> callback;
	};

	class NativeStopHandler : public msg::Handler
	{
	public:
		NativeStopHandler(Service<MsgId>& p_service);
		NativeStopHandler(Service<MsgId>& p_service, std::function<void(void)> p_callback);
		std::string handle(const std::string&);
	private:
		Service<MsgId>& service;
		std::function<void(void)> callback;
	};

private:
	void setup();
	void handleRequest();
	std::string handle(const std::string& p_req);

	ServerFacotry factory;
	GetMsgId getMsgId;
	std::unique_ptr<Server> server;
	bool isRunning;
	std::map<MsgId, std::unique_ptr<Handler>> handlers;
	std::unique_ptr<Handler> defaultHandler;
};

template<typename MsgId>
Service<MsgId>::Service(ServerFacotry p_facotry, GetMsgId p_getMsgId)
	: factory(p_facotry), getMsgId(p_getMsgId), server(nullptr), isRunning(false), defaultHandler(std::make_unique<EmptyHandler>())
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
	try
	{
		auto resp = handle(server->receiveReq());
		if (resp.empty())
			server->reply(boost::none);
		else
			server->reply(resp);
	}
	catch (std::exception& e)
	{
		std::cerr << "ERROR while handling message. " << e.what() << std::endl;
	}
}

template<typename MsgId>
std::string Service<MsgId>::handle(const std::string& p_req)
{
	MsgId id = getMsgId(p_req);
	auto handler = handlers.find(id);
	if (handler != handlers.end())
		return handler->second->handleMsg(p_req);
	return defaultHandler->handleMsg(p_req);
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

template<typename MsgId>
void Service<MsgId>::setDefaultHandler(std::unique_ptr<Handler> p_handler)
{
	defaultHandler = std::move(p_handler);
}

template <typename MsgId>
template<typename HandlerType, typename Codec>
void Service<MsgId>::addReqHandler(std::unique_ptr<HandlerType> p_handler)
{
	using ReqT = typename HandlerType::RequestType;
	using RespT = typename HandlerType::ResponseType;
	using ExceptionT = typename Codec::ExceptionType;

	addHandler(
		ReqT::id,
		std::move(msg::buildRequestDecodingHandler<HandlerType, ExceptionT>(
			std::move(p_handler), &Codec::decode<ReqT> , &Codec::encode<RespT>))
	);
}

template <typename MsgId>
template<typename HandlerType, typename Codec>
void Service<MsgId>::addIndHandler(std::unique_ptr<HandlerType> p_handler)
{
	using IndT = typename HandlerType::IndicationType;
	using ExceptionT = typename Codec::ExceptionType;

	addHandler(
		IndT::id,
		std::move(msg::buildIndicationDecodingHandler<HandlerType, ExceptionT>(
			std::move(p_handler), &Codec::decode<IndT>))
	);
}

template <typename MsgId>
template<typename StopMessage>
Service<MsgId>::StopHandler<StopMessage>::StopHandler(Service<MsgId>& p_service)
	: service(p_service)
{}

template <typename MsgId>
template<typename StopMessage>
Service<MsgId>::StopHandler<StopMessage>::StopHandler(Service<MsgId>& p_service, std::function<void(void)> p_callback)
	: service(p_service), callback(p_callback)
{}

template <typename MsgId>
template<typename StopMessage>
void Service<MsgId>::StopHandler<StopMessage>::handle(const StopMessage& p_msg)
{
	service.stop();
	if (callback)
		callback();
}

template <typename MsgId>
Service<MsgId>::NativeStopHandler::NativeStopHandler(Service<MsgId>& p_service)
	: service(p_service)
{}

template <typename MsgId>
Service<MsgId>::NativeStopHandler::NativeStopHandler(Service<MsgId>& p_service, std::function<void(void)> p_callback)
	: service(p_service), callback(p_callback)
{}

template <typename MsgId>
std::string Service<MsgId>::NativeStopHandler::handle(const std::string& p_msg)
{
	service.stop();
	if (callback)
		callback();
}

}
