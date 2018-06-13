#include <iostream>
#include "MsgService.hpp"
#include "MsgTcpIpServer.hpp"
#include "SockSocketUtils.hpp"

enum class MessageId
{
	Stop,
	Echo,
	Print
};

MessageId getId(const std::string& p_msg)
{
	if (p_msg == "stop")
		return MessageId::Stop;
	if (p_msg == "print")
		return MessageId::Print;
	return MessageId::Echo;
}

class EchoReqHandler : public msg::Handler
{
public:
	std::string handle(const std::string& p_req) override
	{
		std::cout << "Echo message. Received [" << p_req << "]. Reply the same" << std::endl;
		return p_req;
	}
};

class PrintIndHandler : public msg::Handler
{
public:
	std::string handle(const std::string& p_req) override
	{
		std::cout << "Print message. Received [" << p_req << "]. Reply nothing" << std::endl;
		return "";
	}
};

using BaseService = msg::Service<MessageId>;

std::unique_ptr<msg::Server> createLocalTcpIpServer()
{
	std::cout << "Create server on 127.0.0.1:1234" << std::endl;
	return std::make_unique<msg::TcpIpServer>("127.0.0.1", "1234");
}

class EchoService : public BaseService
{
	class StopHandler : public msg::Handler
	{
	public:
		StopHandler(BaseService& p_ser) : service(p_ser) {}
		std::string handle(const std::string& p_req) override
		{
			std::cout << "Stop message. Received [" << p_req << "]. Stoping service" << std::endl;
			service.stop();
			return "";
		}
	private:
		BaseService& service;
	};
public:
	EchoService()
		: BaseService(&createLocalTcpIpServer, &getId)
	{
		addHandler(MessageId::Stop, std::make_unique<StopHandler>(*this));
		addHandler(MessageId::Echo, std::make_unique<EchoReqHandler>());
		addHandler(MessageId::Print, std::make_unique<PrintIndHandler>());
	}
};


int main()
{
	try
	{
		sock::init();
		std::cout << "Create service" << std::endl;
		EchoService ser;
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
