/*
 * AbstractTCPConnection.cpp
 *
 *  Created on: Oct 22, 2016
 *      Author: Christian Everett
 */

#include "TCPConnection.h"

const std::vector<std::string> TCPConnection::ID_IP_MAP /*{"10.0.0.24", "10.0.0.105"};*/{"192.168.8.161", "192.168.8.162", "192.168.8.163"};
const std::string TCPConnection::BUFFER_MANGER_IP = /*"10.0.0.105";*/"192.168.8.164";

TCPConnection::TCPConnection(std::string host, int port) : host(host) ,port(port)
{
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);

	if(inet_pton(AF_INET, host.c_str(), &serverAddress.sin_addr) < 1)
		throw std::runtime_error("Could not format address: " + host);

	if((socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error("Could not create socket");

	if(connect(socketDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0)
		throw std::runtime_error("Could not connect to server");
}

TCPConnection::TCPConnection(int fd, std::string host, int port) : host(host) ,port(port)
{
	this->socketDescriptor = fd;
}

TCPConnection::TCPConnection(const TCPConnection & object): host(object.host) ,port(object.port)
{
	this->socketDescriptor = dup(object.socketDescriptor);
}

TCPConnection::~TCPConnection()
{
	if(close (socketDescriptor) == -1)
		throw std::runtime_error("Could not close socket");
}

Message TCPConnection::readMessage()
{
	Message message = Message::createEmtpyMessage();
	int status = read(socketDescriptor, &message, sizeof(message));

	if(status == -1)
	{
		throw std::runtime_error("Could not read socket");
	}
	else if(status < sizeof(Message))
	{
		throw std::runtime_error("EOF reached or full message read failed");
	}

	return message;
}

void TCPConnection::sendMessage(Message message)
{
	int status = write(socketDescriptor, &message, sizeof(Message));

	if(status == -1)
	{
		throw std::runtime_error("Could not write socket");
	}
}
