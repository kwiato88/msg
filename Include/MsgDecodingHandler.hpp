#pragma once

#include <memory>
#include <functional>
#include "MsgHandler.hpp"
#include "MsgReqHandler.hpp"

namespace msg
{

template <typename Req, typename Resp, typename Codec>
class DecodingHandler : public Handler
{
public:
	DecodingHandler(std::shared_ptr<ReqHandler<Req, Resp> > p_handler)
		: handler(p_handler)
	{}

	std::string handle(const std::string& p_payload)
	{
		try
		{
			return Codec::encode(handler->handle(Codec::template decode<Req>(p_payload)));
		} catch (typename Codec::ExceptionType& e)
		{
			std::cerr << "DecodingHandler: " << e.what() << std::endl;
			return "";
		}
	}

private:
	std::shared_ptr<ReqHandler<Req, Resp> > handler;
};

template <typename Ind, typename Codec>
class IndicationDecondingHandler : public Handler
{
public:
	IndicationDecondingHandler(std::shared_ptr<IndHandler<Ind> > p_handler)
		: handler(p_handler)
	{}

	std::string handle(const std::string& p_payload)
	{
		try
		{
			handler->handle(Codec::template decode<Ind>(p_payload));
			return std::string();
		}
		catch(typename Codec::ExceptionType& e)
		{
			std::cerr << "IndicationDecondingHandler: " << e.what() << std::endl;
			return std::string();
		}
	}

private:
	std::shared_ptr<IndHandler<Ind> > handler;
};

}
