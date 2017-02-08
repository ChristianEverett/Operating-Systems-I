//============================================================================
// Name        : Node.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <errno.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <thread>
#include <list>
#include <stdlib.h>
#include <time.h>
#include <sstream>

#include "SignalRegister.h"
#include "NodeController.h"
#include "Message.h"

using namespace std;

const int PROUDCERS = 1;
const int CONSUMERS = 10;

const int DOZENS_TO_COLLECT = 10;

bool done = false;

void producer();
void consumer(int id);

void signal_handler(int signal)
{
	cout << "Got Signal: " << signal << endl;
	exit(signal);
}

int main(int argc, char * argv[])
{
	try
	{
		std::setvbuf(stdout, NULL, _IONBF, 0);
		int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGBUS,
		SIGTERM, SIGSEGV, SIGFPE, SIGIO, SIGPIPE, SIGTSTP};
		vector<thread*> threads;

		if (argc < 2)
			cerr << "Need an ID" << endl;

		SignalRegister signalRegister(signals, sizeof(signals) / sizeof(*signals));
		signalRegister.setSignalHandler(signal_handler);

		srand(time(NULL));

		int id = atoi(argv[1]);
		NodeController::createNodeController(id);

		for(int x = 0; x < PROUDCERS; x++)
		{
			thread producerThread(producer);
			producerThread.detach();
		}

		for(int x = 0; x < CONSUMERS; x++)
		{
			threads.push_back(new thread(consumer, x));
		}

		for(int x = 0; x < CONSUMERS; x++)
		{
			threads.at(x)->join();
			delete threads.at(x);
		}

		cout << "Completed" << endl;
		sleep(6);
		done = true;
		NodeController::shutdown();

		exit(EXIT_SUCCESS);
	}
	catch (runtime_error& e)
	{
		cerr << e.what() << endl;
		cerr << errno;
	}
}

void producer()
{
	int id = NodeController::getNodeController()->getNodeID();

	while (!done)
	{
		int buffer = rand() % 4;
		Message data = NodeController::getNodeController()->requestOperation(buffer, Message::PRODUCE);
	}
}

void consumer(int id)
{
	vector<Message> dozenList[4];
	ostringstream ss;
	ss << id;
	fstream outputFile("c" + ss.str(), fstream::out | fstream::trunc);

	for (int dozen = 0; dozen < DOZENS_TO_COLLECT; dozen++)
	{
		for (int number = 0; number < 12; number++)
		{
			int buffer = rand() % 4;
			Message data = NodeController::getNodeController()->requestOperation(buffer, Message::CONSUME);
			dozenList[buffer].push_back(data);
		}

		outputFile << "Node: " << NodeController::getNodeController()->getNodeID() << "\tDozen: " << dozen << endl;
		outputFile << "Plain\t\tJelly\t\tCoconut\t\tHoney-dip" << endl;
		for (int index = 0; index < 12; index++)
		{
			for (int buffer = 0; buffer < 4; buffer++)
			{
				if (dozenList[buffer].size() > index)
				{
					outputFile << dozenList[buffer].at(index).getDataProducer() << " ";
					outputFile << dozenList[buffer].at(index).getData();
				}
				outputFile << "\t\t";
			}

			outputFile << endl;
		}

		dozenList[0].clear();
		dozenList[1].clear();
		dozenList[2].clear();
		dozenList[3].clear();
	}
}
