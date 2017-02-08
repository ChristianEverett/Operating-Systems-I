//============================================================================
// Name        : Assignment_1.cpp
// Author      : Christian Everett
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

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
		int producerPointers[NUMFLAVORS], nextDonut[NUMFLAVORS];
		int sigs[] = { SIGHUP, SIGINT, SIGQUIT, SIGBUS,
		SIGTERM, SIGSEGV, SIGFPE };

		std::fill(producerPointers, producerPointers+NUMFLAVORS, 0);
		std::fill(nextDonut, nextDonut+NUMFLAVORS, 0);

		SignalRegister signalRegister(sigs, sizeof(sigs) / sizeof(*sigs));
		signalRegister.setSignalHandler(handler);

		producerSemSet = new POSIXSemaphoreSet(PRODSEM, NUMFLAVORS, NUMSLOTS, true);
		consumerSemSet = new POSIXSemaphoreSet(CONSEM, NUMFLAVORS, 0, true);
		outptrSemSet = new POSIXSemaphoreSet(OUTPTRSEM, NUMFLAVORS, 1, true);
		memoryMap = new POSIXSharedMemory(SHAREDMEMORY, sizeof(struct donut_ring), true);

		sharedRing = (struct donut_ring*) memoryMap->getMemory();

		srand(time(NULL));

		while(true)
		{
			int id = rand() % 4;
			producerSemSet->wait(id);
			sharedRing->flavor[id][producerPointers[id]] = ++nextDonut[id];
			producerPointers[id] = ++producerPointers[id] % NUMSLOTS;
			consumerSemSet->signal(id);
		}

		handler(EXIT_SUCCESS);
	}
	catch (std::runtime_error& e)
	{
		cerr << "Exception: " << e.what() << endl;
		handler(-1);
	}
}

