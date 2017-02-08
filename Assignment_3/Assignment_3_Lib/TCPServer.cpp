/*
 * TCPServer.cpp
 *
 *  Created on: Oct 23, 2016
 *      Author: Christian Everett
 */

#include "TCPServer.h"

TCPServer::TCPServer(int port)
{
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	if ((serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error("Could not create socket");

	int optval = 1;
	if (setsockopt(serverSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		throw std::runtime_error("Could set socket options");

	if (bind(serverSocketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1)
		throw std::runtime_error("Could not bind socket");

	if (listen(serverSocketDescriptor, connectionQueue) != 0)
		throw std::runtime_error("Could not listen on socket");
}

TCPServer::~TCPServer()
{
	if (close(serverSocketDescriptor) == -1)
		throw std::runtime_error("Could not close socket");
}

TCPConnection* TCPServer::acceptTCPConnection()
{
	int fd;
	struct sockaddr_in clientAddress;
	socklen_t length = sizeof(struct sockaddr_storage);

	if ((fd = accept(serverSocketDescriptor, (struct sockaddr *) &clientAddress, &length)) == -1)
		throw std::runtime_error("accept failed");

	return new TCPConnection(fd, inet_ntoa(clientAddress.sin_addr), ntohl(clientAddress.sin_port));
}

void TCPServer::acceptAsynchronousTCPConnections(std::function<void(TCPConnection)> callback)
{
	auto runnable = [](std::function<void(TCPConnection)> callback, int serverSocketDescriptor) -> void
	{
		int fd;
		struct sockaddr_in clientAddress;
		socklen_t length;

		while ((fd = accept(serverSocketDescriptor, (struct sockaddr *) &clientAddress, &length)) == -1)
		{
			callback(TCPConnection(fd, inet_ntoa(clientAddress.sin_addr), ntohl(clientAddress.sin_port)));
		}
	};

	std::thread pollThread(runnable, callback, serverSocketDescriptor);
}
//	server.acceptAsynchronousTCPConnections([=](TCPConnection connection)
//	{
//		this->registerNode(connection);
//	});
