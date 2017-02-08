//============================================================================
// Name        : Assignment_2.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <vector>
#include <fstream>
#include <sched.h>
#include <string>
#include <string.h>
#include <pthread.h>
#include <sstream>

#include "EventCounterAndSequencer.h"
#include "SignalRegister.h"

using namespace std;

#define		NUMFLAVORS		4
#define		NUMSLOTS       	575
#define		NUMCONSUMERS 	50
#define		NUMPRODUCERS	30
#define		NUMDOZENS		200

struct donut_ring
{
	//four ring buffers
	int flavor[NUMFLAVORS][NUMSLOTS];
	//Pointer to next item in each buffer for consumers
	int outptr[NUMFLAVORS];
	int in_ptr[NUMFLAVORS];
	int spaces[NUMFLAVORS];
	int donuts[NUMFLAVORS];
	int serial[NUMFLAVORS];
} donutBuffer;

/*****************************************************************/
/* SIGNAL WAITER, PRODUCER AND CONSUMER THREAD FUNCTIONS */
/*****************************************************************/
void signal_thread();
void producer();
void consumer(int);

EventCounterAndSequencer prodECSeq[NUMFLAVORS];
EventCounterAndSequencer conECSeq[NUMFLAVORS];
bool systemScope = false;

void signal_handler(int signal)
{
	cout << "Got Signal: " << signal << endl;
	exit(signal);
}

void setAffinityForCurrentThread()
{
	cpu_set_t cpuSet;
	CPU_ZERO(&cpuSet);
	unsigned cores = sysconf(_SC_NPROCESSORS_ONLN); //thread::hardware_concurrency();
	//sched_getaffinity(getpid(), sizeof(cpu_set_t), &cpuSet);
	CPU_SET(rand() % cores, &cpuSet);
	sched_setaffinity(0, sizeof(cpu_set_t), &cpuSet);
}

int main(int argc, char* argv[])
{
	try
	{
		pthread_attr_t thread_attr;
		struct sched_param sched_struct;
		int signals[] = { SIGBUS, SIGSEGV, SIGFPE };
		SignalRegister signalRegister(signals, sizeof(signals) / sizeof(*signals));
		signalRegister.blockAllOtherSignals();
		signalRegister.setSignalHandler(signal_handler);
		thread signalThread(signal_thread);

		std::fill(donutBuffer.spaces, donutBuffer.spaces + NUMFLAVORS, NUMSLOTS);

		srand(time(NULL));

		if (getopt(argc, argv, "s:") != -1)
		{
			systemScope = (strcmp("system", optarg) == 0) ? true : false;
		}

		struct timeval initialTime, finalTime;
		gettimeofday(&initialTime, NULL);

		if (!systemScope)
		{
			pthread_attr_init(&thread_attr);
			pthread_attr_setinheritsched(&thread_attr, PTHREAD_INHERIT_SCHED);
			setAffinityForCurrentThread();
			cout << "Running with Process Scope" << endl;
		}
		else
		{
			pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
			pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);
			sched_struct.sched_priority = sched_get_priority_max(SCHED_OTHER);
			pthread_attr_setschedparam(&thread_attr, &sched_struct);
			pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);
			cout << "Running with System Scope" << endl;
		}

		vector<thread*> producers, consumers;

		for (int i = 0; i < NUMPRODUCERS; i++)
		{
			thread *prodThread = new thread(producer);
			prodThread->detach();
			producers.push_back(prodThread);
		}

		for (int i = 0; i < NUMCONSUMERS; i++)
		{
			consumers.push_back(new thread(consumer, i + 1));
		}

		for (int i = 0; i < NUMCONSUMERS; i++)
		{
			consumers.at(i)->join();
			delete consumers.at(i);
		}

		gettimeofday(&finalTime, NULL);
		cout << "Seconds: " << finalTime.tv_sec - initialTime.tv_sec << " Microseconds: " << finalTime.tv_usec << endl;

		for (unsigned i = 0; i < producers.size(); i++)
		{
			delete producers.at(i);
		}

		exit(EXIT_SUCCESS);
	}
	catch (std::runtime_error& e)
	{
		perror(e.what());
		exit(EXIT_FAILURE);
	}
}

void producer()
{
	if (systemScope)
		setAffinityForCurrentThread();
	for (int x = 0; x < NUMCONSUMERS * NUMDOZENS * 12; x++)
	{
		int id = rand() % 4;
		int ticket = prodECSeq[id].ticket();
		conECSeq[id].await(ticket);
		prodECSeq[id].await(ticket - NUMSLOTS + 1);
		donutBuffer.flavor[id][/*donutBuffer.in_ptr[id]*/ticket % NUMSLOTS] = ticket + 1; //donutBuffer.serial[id]++;
//		donutBuffer.in_ptr[id] = (donutBuffer.in_ptr[id] + 1) % NUMSLOTS;
//		donutBuffer.spaces[id]--;
//		donutBuffer.donuts[id]++;
		conECSeq[id].advance();
	}
}

void consumer(int number)
{
	if (systemScope)
	{
		setAffinityForCurrentThread();
	}

	ostringstream ss;
	ss << number;
	fstream outputFile("c" + ss.str(), fstream::out | fstream::trunc);
	vector<int> currentDonuts[NUMFLAVORS];

	for (int dozen = 0; dozen < NUMDOZENS; ++dozen)
	{
		for (int x = 0; x < 12; x++)
		{
			int id = rand() % 4;
			int ticket = conECSeq[id].ticket();
			prodECSeq[id].await(ticket);
			conECSeq[id].await(ticket + 1);
			currentDonuts[id].push_back(donutBuffer.flavor[id][ticket % NUMSLOTS/*donutBuffer.outptr[id]*/]);
//			donutBuffer.outptr[id] = (donutBuffer.outptr[id] + 1) % NUMSLOTS;
//			donutBuffer.spaces[id]++;
//			donutBuffer.donuts[id]--;
			prodECSeq[id].advance();
			usleep(50);
		}

		if (dozen < 10)
		{
			outputFile << "Thread: " << number << "\tDozen: " << dozen + 1 << endl;
			outputFile << "Plain\t\t" << "Jelly\t\t" << "Coconut\t\t" << "Honey-dip" << endl;

			for (int i = 0; i < 12; i++)
			{
				for (int selection = 0; selection < NUMFLAVORS; selection++)
				{
					if (currentDonuts[selection].size() > i)
						outputFile << currentDonuts[selection].at(i);
					outputFile << "\t\t";
				}

				outputFile << endl;
			}
		}

		for (int i = 0; i < NUMFLAVORS; i++)
			currentDonuts[i].clear();
	}

	outputFile.close();
}

void signal_thread()
{
	sigset_t signals;
	int signal;

	sigaddset(&signals, SIGTERM);
	sigaddset(&signals, SIGINT);

	sigwait(&signals, &signal);

	cout << "Signal: " << signal << endl;

	exit(signal);
}
