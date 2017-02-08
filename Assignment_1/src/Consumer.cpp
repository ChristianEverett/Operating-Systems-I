/*
 * Consumer.cpp
 *
 *  Created on: Sep 13, 2016
 *      Author: Christian Everett
 */
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "Donuts.h"
#include "POSIXSemaphoreSet.h"
#include "POSIXSharedMemory.h"
#include "SignalRegister.h"

using namespace std;

POSIXSemaphoreSet *producerSemSet;
POSIXSemaphoreSet *consumerSemSet;
POSIXSemaphoreSet *outptrSemSet;
POSIXSharedMemory *memoryMap;

void printColumn(vector<int> currentlyHeldDonuts, int number)
{
	if(currentlyHeldDonuts.size() > number)
		cout << currentlyHeldDonuts.at(number);
	cout << "\t\t";
}

void printHeader(vector<int> currentlyHeldDonuts[], int dozen)
{
	time_t _time = time(NULL);
	struct tm *localTime = localtime(&_time);
	cout << "Running Consumer Process: " << getpid() << "\ttime: " << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec
			<< " dozen: " << dozen << endl;

	cout << "plain\tjelly\t\tcoconut\t\thoney-dip" << endl;

	for(int x = 0; x < 12; x++)
	{
		printColumn(currentlyHeldDonuts[0], x);
		printColumn(currentlyHeldDonuts[1], x);
		printColumn(currentlyHeldDonuts[2], x);
		printColumn(currentlyHeldDonuts[3], x);
		cout << endl;
	}
}

void handler(int signalNumber)
{
	cout << "Handled Signal: " << signalNumber << endl;
	delete producerSemSet;
	delete consumerSemSet;
	delete outptrSemSet;
	delete memoryMap;

	exit(signalNumber);
}

int main()
{
	try
	{
		struct donut_ring *sharedRing = NULL;
		vector<int> currentlyHeldDonuts[NUMFLAVORS];
		int sigs[] = { SIGHUP, SIGINT, SIGQUIT, SIGBUS,
		SIGTERM, SIGSEGV, SIGFPE };

		SignalRegister signalRegister(sigs, sizeof(sigs) / sizeof(*sigs));
		signalRegister.setSignalHandler(handler);

		producerSemSet = new POSIXSemaphoreSet(PRODSEM, NUMFLAVORS, NUMSLOTS, false);
		consumerSemSet = new POSIXSemaphoreSet(CONSEM, NUMFLAVORS, 0, false);
		outptrSemSet = new POSIXSemaphoreSet(OUTPTRSEM, NUMFLAVORS, 1, false);
		memoryMap = new POSIXSharedMemory(SHAREDMEMORY, sizeof(struct donut_ring), false);

		sharedRing = (struct donut_ring*) memoryMap->getMemory();

		srand(time(NULL));

		for (int var = 0; var < 10; var++)
		{
			for (int x = 0; x < 12; x++)
			{
				int id = rand() % 4;
				consumerSemSet->wait(id);
				outptrSemSet->wait(id);
				currentlyHeldDonuts[id].push_back(sharedRing->flavor[id][sharedRing->outptr[id]]);
				sharedRing->outptr[id] =  (sharedRing->outptr[id] + 1) % NUMSLOTS;
				outptrSemSet->signal(id);
				producerSemSet->signal(id);
			}

			printHeader(currentlyHeldDonuts, var + 1);
			for(int i = 0; i < NUMFLAVORS; i++)
			{
				currentlyHeldDonuts[i].clear();
			}
		}

		handler(EXIT_SUCCESS);
	}
	catch (std::runtime_error& e)
	{
		cerr << "Exception: " << e.what() << endl;
		handler(-1);
	}
}

