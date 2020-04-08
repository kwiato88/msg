#include <iostream>
#include "MsgClient.hpp"
#include "MsgTcpIpConnection.hpp"
#include "SockSocketUtils.hpp"

std::unique_ptr<msg::Connection> createConnectionToLocalTcpIpHost()
{
	std::cout << "Create connection to 127.0.0.1:1234" << std::endl;
	return std::make_unique<msg::TcpIpConnection>("127.0.0.1", "1234");
}
int main()
{
	try
	{
		sock::init();

		std::cout << "Create client to local host" << std::endl;
		msg::Client client(&createConnectionToLocalTcpIpHost);
		std::string req, resp;

		req = "req1";
		std::cout << "Send request [" << req << "]" << std::endl;
		resp = client.sendReq(req);
		std::cout << "Received response [" << resp << "]" << std::endl;

		req = "print";
		std::cout << "Send print ind [" << req << "]" << std::endl;
		client.sendInd(req);

		req = "dummy req";
		std::cout << "Send request [" << req << "]" << std::endl;
		resp = client.sendReq(req);
		std::cout << "Received response [" << resp << "]" << std::endl;

		try
		{
			req = "getPrinterStatus";
			std::cout << "Send request [" << req << "]" << std::endl;
			resp = client.sendReq(req);
			std::cout << "Received response [" << resp << "]" << std::endl;
		}
		catch (std::exception& e)
		{
			std::cout << "Error while geting printer status: " << e.what() << std::endl;
		}

		req = "toPrinter";
		std::cout << "Send ind [" << req << "]" << std::endl;
		client.sendInd(req);

		req = "stop";
		std::cout << "Send stop server ind" << std::endl;
		client.sendInd(req);

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
