#include <iostream>
#include <stdexcept>
#include "MsgService.hpp"
#include "MsgReqHandler.hpp"
#include "MsgTcpIpServer.hpp"
#include "SockSocketUtils.hpp"

namespace Printer
{

enum class MessageId
{
	Stop,
	Echo,
	Print
};

struct Stop {};
struct Echo
{
	std::string message;
};
struct Print
{
	std::string message;
};

// "Id:[message]"
struct Codec
{
	using ExceptionType = typename std::runtime_error;
	using IdType = typename Printer::MessageId;

	static IdType idFromString(const std::string& p_id)
	{
		if (p_id == "stop")
			return IdType::Stop;
		if (p_id == "print")
			return IdType::Print;
		if (p_id == "echo")
			return IdType::Echo;
		throw ExceptionType(std::string("Printer::codec: unknown id ") + p_id);
	}
	static std::string idToString(IdType p_id)
	{
		switch(p_id)
		{
			case IdType::Stop : return "stop";
			case IdType::Print : return "print";
			case IdType::Echo : return "echo";
		}
		throw ExceptionType("Printer::codec: unknown id");
	}
	static IdType getId(const std::string& p_data)
	{
		auto separatorPos = p_data.find(':');
		if(separatorPos == std::string::npos)
			throw ExceptionType("Printer::codec: no separator(:) in payload");
		return idFromString(p_data.substr(0, separatorPos));
	}

	template <typename T>
	static T decode(const std::string&)
	{
		throw ExceptionType("Printer::codec: decode not defined");
	}

	static std::string encode(const Stop&)
	{
		return idToString(IdType::Stop) + ":";
	}
/*
	template <>
	static Stop decode<Stop>(const std::string&)
	{
		return Stop{};
	}
*/
	static std::string encode(const Echo& p_msg)
	{
		return idToString(IdType::Echo) + ":" + p_msg.message;
	}
/*
	template <>
	static Echo decode<Echo>(const std::string& p_msg)
	{
		auto separatorPos = p_msg.find(':');
		if(separatorPos == std::string::npos)
			throw ExceptionType("Printer::codec: no separator(:) in payload");
		Echo msg;
		msg.message = p_msg.substr(separatorPos+1);
		return msg;
	}
*/
	static std::string encode(const Print& p_msg)
	{
		return idToString(IdType::Print) + ":" + p_msg.message;
	}
	/*
	template <>
	static Print decode<Print>(const std::string& p_msg)
	{
		auto separatorPos = p_msg.find(':');
		if(separatorPos == std::string::npos)
			throw ExceptionType("Printer::codec: no separator(:) in payload");
		Print msg;
		msg.message = p_msg.substr(separatorPos+1);
		return msg;
	}
*/
};

template <>
Stop Codec::decode<Stop>(const std::string&)
{
	return Stop{};
}
template <>
Echo Codec::decode<Echo>(const std::string& p_msg)
{
	auto separatorPos = p_msg.find(':');
	if(separatorPos == std::string::npos)
		throw ExceptionType("Printer::codec: no separator(:) in payload");
	Echo msg;
	msg.message = p_msg.substr(separatorPos+1);
	return msg;
}
template <>
Print Codec::decode<Print>(const std::string& p_msg)
{
	auto separatorPos = p_msg.find(':');
	if(separatorPos == std::string::npos)
		throw ExceptionType("Printer::codec: no separator(:) in payload");
	Print msg;
	msg.message = p_msg.substr(separatorPos+1);
	return msg;
}

class EchoReqHandler : public msg::ReqHandler<Echo, Echo>
{
public:
	Echo handle(const Echo& p_req) override
	{
		std::cout << "Echo message. Received [" << p_req.message << "]. Reply the same" << std::endl;
		return p_req;
	}
};

class PrintIndHandler : public msg::IndHandler<Print>
{
public:
	void handle(const Print& p_req) override
	{
		std::cout << "Print message. Received [" << p_req.message << "]. Reply nothing" << std::endl;
	}
};

class UnexpectedMsgHandler : public msg::Handler
{
public:
	std::string handle(const std::string& p_req) override
	{
		std::cout << "Unexpected message. Received [" << p_req << "]. Reply nothing" << std::endl;
		return "";
	}
};

using BaseService = msg::Service<Codec>;

std::unique_ptr<msg::Server> createLocalTcpIpServer()
{
	std::cout << "Create server on 127.0.0.1:1234" << std::endl;
	return std::make_unique<msg::TcpIpServer>("127.0.0.1", "1234");
}

class Service : public BaseService
{
	class StopHandler : public msg::IndHandler<Stop>
	{
	public:
		StopHandler(BaseService& p_service)
			: service(p_service)
		{}
		void handle(const Stop&) override
		{
			std::cout << "Stop message. Stop service" << std::endl;
			service.stop();
		}
	private:
		BaseService& service;
	};
public:
	Service()
		: BaseService(&createLocalTcpIpServer)
	{
		addHandler<Stop>(MessageId::Stop, std::make_unique<StopHandler>(*this));
		addHandler<Echo, Echo>(MessageId::Echo, std::make_unique<EchoReqHandler>());
		addHandler<Print>(MessageId::Print, std::make_unique<PrintIndHandler>());
		setDefaultHandler(std::make_unique<UnexpectedMsgHandler>());
	}
};

}

int main()
{
	try
	{
		sock::init();
		std::cout << "Create service" << std::endl;
		Printer::Service ser;
		std::cout << "Start service" << std::endl;
		ser.start();
		std::cout << "Service finished" << std::endl;
		sock::cleanup();
		return 0;
	}
	catch (std::exception& e)
	{
		std::cout << "Something went wrong. Detail: " << e.what() << std::endl;
		sock::cleanup();
		return 1;
	}
}
