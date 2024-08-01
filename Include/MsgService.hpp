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
	enum class Processing
	{
		Sync,
		Async
	};
	typedef std::function<MsgId(const std::string& p_payload)> GetMsgId;

	Service(ServerFacotry p_facotry, GetMsgId p_getMsgId);

	void start();
	void stop();
	void cleanup();
	void addHandler(MsgId p_id, std::unique_ptr<Handler> p_handler, Processing p_type = Processing::Sync);
	void setDefaultHandler(std::unique_ptr<Handler> p_handler, Processing p_type = Processing::Sync);

	template<typename HandlerType, typename Codec>
	void addReqHandler(std::unique_ptr<HandlerType> p_handler);
	template<typename HandlerType, typename Codec>
	void addIndHandler(std::unique_ptr<HandlerType> p_handler, Processing p_type = Processing::Sync);
	template<typename Codec, typename ReqType, typename RespType>
	void addFunReqHandler(std::function<RespType(const ReqType&)> p_handler);
	template<typename Codec, typename IndType>
	void addFunIndHandler(std::function<void(const IndType&)> p_handler, Processing p_type = Processing::Sync);

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
	struct HandlingData
	{
		HandlingData(Processing p_type, std::unique_ptr<Handler> p_handler)
			: processingType(p_type), handler(std::move(p_handler))
		{}
		Processing processingType;
		std::unique_ptr<Handler> handler;
	};
	void setup();
	void handleRequest();
	void handle(const std::string& p_msg, const HandlingData& p_context);
	void reply(const std::string& p_resp);

	ServerFacotry factory;
	GetMsgId getMsgId;
	std::unique_ptr<Server> server;
	bool isRunning;
	std::map<MsgId, HandlingData> handlers;
	HandlingData defaultHandler;
};

template<typename MsgId>
Service<MsgId>::Service(ServerFacotry p_facotry, GetMsgId p_getMsgId)
	: factory(p_facotry), getMsgId(p_getMsgId), server(nullptr), isRunning(false), defaultHandler(Processing::Sync, std::make_unique<EmptyHandler>())
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
		std::string msg = server->receiveReq();
		MsgId id = getMsgId(msg);
		auto handler = handlers.find(id);
		if (handler == handlers.end())
			handle(msg, defaultHandler);
		else
			handle(msg, handler->second);
	}
	catch (std::exception& e)
	{
		std::cerr << "ERROR while handling message. " << e.what() << std::endl;
	}
}

template<typename MsgId>
void Service<MsgId>::handle(const std::string& p_msg, const HandlingData& p_context)
{
	if (p_context.processingType == Processing::Sync)
		reply(p_context.handler->handleMsg(p_msg));
	else
	{
		server->reply(boost::none);
		p_context.handler->handleMsg(p_msg);
	}
}

template<typename MsgId>
void Service<MsgId>::reply(const std::string& p_resp)
{
	if (p_resp.empty())
		server->reply(boost::none);
	else
		server->reply(p_resp);
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
void Service<MsgId>::addHandler(MsgId p_id, std::unique_ptr<Handler> p_handler, Processing p_type)
{
	handlers.insert_or_assign(p_id, std::move(HandlingData(p_type, std::move(p_handler))));
}

template<typename MsgId>
void Service<MsgId>::setDefaultHandler(std::unique_ptr<Handler> p_handler, Processing p_type)
{
	defaultHandler = std::move(HandlingData(p_type, std::move(p_handler)));
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
			std::move(p_handler), &Codec::template decode<ReqT> , &Codec::template encode<RespT>)),
		Processing::Sync
	);
}

template <typename MsgId>
template<typename HandlerType, typename Codec>
void Service<MsgId>::addIndHandler(std::unique_ptr<HandlerType> p_handler, Processing p_type)
{
	using IndT = typename HandlerType::IndicationType;
	using ExceptionT = typename Codec::ExceptionType;

	addHandler(
		IndT::id,
		std::move(msg::buildIndicationDecodingHandler<HandlerType, ExceptionT>(
			std::move(p_handler), &Codec::template decode<IndT>)),
		p_type
	);
}

template <typename MsgId>
template<typename Codec, typename ReqType, typename RespType>
void Service<MsgId>::addFunReqHandler(std::function<RespType(const ReqType&)> p_handler)
{
	addHandler(
		ReqType::id,
		std::make_unique<HandlerWtihFunctor<ReqType, RespType>>(p_handler, &Codec::template decode<ReqType>, &Codec::template encode<RespType>),
		Processing::Sync);
}

template <typename MsgId>
template<typename Codec, typename IndType>
void Service<MsgId>::addFunIndHandler(std::function<void(const IndType&)> p_handler, Processing p_type)
{
	addHandler(
		IndType::id,
		std::make_unique<IndHandlerWtihFunctor<IndType>>(p_handler, &Codec::template decode<IndType>),
		p_type);
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
	return "";
}

}
