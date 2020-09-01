
#include <Network.h>
#include <iostream>

int main()
{
	try
	{
		//Will call destructor at end of try
		std::unique_ptr<Network> network( new Network() );

		network->Initialize();
// 		std::shared_ptr<TCPServer> server = network->CreateTCPServer();
// 		server->bind();
// 		server->accept();

		//std::unique_ptr<TCPServer> server = network->createTCPServer();
	}
	catch ( std::runtime_error& rt )
	{
		std::cerr << rt.what() << std::endl;
	}

	return 0;
}


