/*
 * TCPServer.h
 *
 *  Created on: Oct 23, 2016
 *      Author: Christian Everett
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <functional>
#include <thread>
#include <string.h>
#include <arpa/inet.h>
#include "TCPConnection.h"

class TCPServer
{
public:
	TCPServer(int port = NODE_PORT);
	virtual ~TCPServer();

	TCPConnection* acceptTCPConnection();
	void acceptAsynchronousTCPConnections(std::function<void(TCPConnection)>);
	int getServerDescriptor(){return serverSocketDescriptor;}

	static const int NODE_PORT = 8787;
	static const int BUFFER_MANGER_PORT = 8788;

private:

	int connectionQueue = 5;
	int serverSocketDescriptor;
};

#endif /* TCPSERVER_H_ */
