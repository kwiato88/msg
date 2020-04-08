#include <iostream>
#include <stdexcept>
#include "MsgService.hpp"
#include "MsgTcpIpServer.hpp"
#include "SockSocketUtils.hpp"

enum class MessageId
{
	Stop,
	Echo,
	Print,
	SendToPrinter,
	GetPrinterStatus,
	SetPrinterStatus
};

MessageId getId(const std::string& p_msg)
{
	if (p_msg == "stop")
		return MessageId::Stop;
	if (p_msg == "print")
		return MessageId::Print;
	if (p_msg == "toPrinter")
		return MessageId::SendToPrinter;
	if (p_msg == "getPrinterStatus")
		return MessageId::GetPrinterStatus;
	if (p_msg == "setPrinterStatus")
		return MessageId::SetPrinterStatus;
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

class SendToPrinterIndHandler : public msg::Handler
{
public:
	std::string handle(const std::string& p_req) override
	{
		std::cout << "Send to printer message. Received [" << p_req << "]." << std::endl;
		throw std::runtime_error("Failed to print");
		return "";
	}
	std::string onError(std::exception& e)
	{
		std::cout << "ERROR while handling send to printer. Detail: " << e.what() << std::endl;
		return "";
	}
};

class GetPrinterStatusReqHandler : public msg::Handler
{
public:
	std::string handle(const std::string& p_req) override
	{
		std::cout << "Get printer status message. Received [" << p_req << "]." << std::endl;
		throw std::runtime_error("FAILURE");
		return "";
	}
	std::string onError(std::exception& e)
	{
		std::cout << "ERROR while handling get printer status. Detail: " << e.what() << std::endl;
		return "";
	}
};

class LoggingHandler : public msg::Handler
{
public:
	std::string handle(const std::string& p_req) override
	{
		std::cout << "Log message. Received [" << p_req << "]." << std::endl;
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
		addHandler(MessageId::SendToPrinter, std::make_unique<SendToPrinterIndHandler>());
		addHandler(MessageId::GetPrinterStatus, std::make_unique<GetPrinterStatusReqHandler>());
		setDefaultHandler(std::make_unique<LoggingHandler>());
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
