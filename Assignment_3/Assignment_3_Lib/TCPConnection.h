/*
 * AbstractTCPConnection.h
 *
 *  Created on: Oct 22, 2016
 *      Author: Christian Everett
 */

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <unistd.h>
#include <string>
#include <vector>

#include "Message.h"

class TCPConnection
{
public:
	TCPConnection(std::string, int);
	TCPConnection(int, std::string, int);
	TCPConnection(const TCPConnection &);
	virtual ~TCPConnection();

	int getSocketDescripter(){return socketDescriptor;}
	Message readMessage();
	void sendMessage(Message);

	static const std::vector<std::string> ID_IP_MAP;
	static const std::string BUFFER_MANGER_IP;

private:

	int socketDescriptor;
	std::string host;
	int port;
};

#endif /* TCPCONNECTION_H_ */
