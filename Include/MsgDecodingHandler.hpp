#pragma once

#include <memory>
#include <functional>
#include "MsgHandler.hpp"

namespace msg
{

template <typename Req, typename Resp, typename CodecException>
class DecodingHandler : public Handler
{
public:
	DecodingHandler(std::unique_ptr<ReqHandler<Req, Resp>> p_handler,
					std::function<Req(const std::string&)> p_decoder,
					std::function<std::string(const Resp&)> p_encoder)
		: handler(std::move(p_handler)), decoder(p_decoder), encoder(p_encoder)
	{}

	std::string handle(const std::string& p_payload)
	{
		return encoder(handler->handle(decoder(p_payload)));
	}
	std::string onError(std::exception& e)
	{
		handler->onError(e);
		return std::string();
	}


private:
	std::unique_ptr<ReqHandler<Req, Resp>> handler;
	std::function<Req(const std::string&)> decoder;
	std::function<std::string(const Resp&)> encoder;
};

template <typename Ind, typename CodecException>
class IndicationDecondingHandler : public Handler
{
public:
	IndicationDecondingHandler(std::unique_ptr<IndicationHandler<Ind>> p_handler,
		std::function<Ind(const std::string&)> p_decoder)
		: handler(std::move(p_handler)), decoder(p_decoder)
	{}

	std::string handle(const std::string& p_payload)
	{
		handler->handle(decoder(p_payload));
		return std::string();
	}
	std::string onError(std::exception& e)
	{
		handler->onError(e);
		return std::string();
	}

private:
	std::unique_ptr<IndicationHandler<Ind>> handler;
	std::function<Ind(const std::string&)> decoder;
};

template<typename ReqHandlerType, typename CodecException>
std::unique_ptr<Handler> buildRequestDecodingHandler(
	std::unique_ptr<ReqHandlerType> p_handler,
	std::function<typename ReqHandlerType::RequestType(const std::string&)> p_decoder,
	std::function<std::string(const typename ReqHandlerType::ResponseType&)> p_encoder)
{
	return std::make_unique<DecodingHandler<typename ReqHandlerType::RequestType, typename ReqHandlerType::ResponseType, CodecException>>(std::move(p_handler), p_decoder, p_encoder);
}

template<typename IndHandlerType, typename CodecException>
std::unique_ptr<Handler> buildIndicationDecodingHandler(
	std::unique_ptr<IndHandlerType> p_handler,
	std::function<typename IndHandlerType::IndicationType(const std::string&)> p_decoder)
{
	return std::make_unique<IndicationDecondingHandler<typename IndHandlerType::IndicationType, CodecException>>(std::move(p_handler), p_decoder);
}

}
