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
		try
		{
			return encoder(handler->handle(decoder(p_payload)));
		}
		catch (const CodecException&)
		{
			return std::string();
		}
	}

private:
	std::unique_ptr<ReqHandler<Req, Resp>> handler;
	std::function<Req(const std::string&)> decoder;
	std::function<std::string(const Resp&)> encoder;
};

}
