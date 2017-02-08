//============================================================================
// Name        : BufferManger.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <thread>
#include <algorithm>
#include <errno.h>
#include "BufferManger.h"
#include "SignalRegister.h"

using namespace std;

void signal_handler(int signal)
{
	exit(signal);
}

int main()
{
	try
	{
		int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGBUS,
		SIGTERM, SIGSEGV, SIGFPE };

		std::setvbuf(stdout, NULL, _IONBF, 0);
		SignalRegister signalRegister(signals, sizeof(signals) / sizeof(*signals));
		signalRegister.setSignalHandler(signal_handler);

		BufferManger bufferManger;
		TCPServer server(TCPServer::BUFFER_MANGER_PORT);

		while (true)
		{
			TCPConnection *client = server.acceptTCPConnection();
			thread task(&BufferManger::processRequest, &bufferManger, client);
			task.detach();
		}

		exit(EXIT_SUCCESS);
	}
	catch (std::runtime_error& e)
	{
		cerr << "Exception: " << e.what() << endl;
		cerr << errno << endl;
		exit(EXIT_FAILURE);
	}
}
