/*
 * EventCounterAndSequencer.cpp
 *
 *  Created on: Sep 24, 2016
 *      Author: Christian Everett
 */

#include "EventCounterAndSequencer.h"
#include <iostream>
EventCounterAndSequencer::EventCounterAndSequencer() : counter(0), sequence(0), blockedThreads(0), reverseAwait(false), close(false)
{

}

EventCounterAndSequencer::~EventCounterAndSequencer()
{

}

void EventCounterAndSequencer::await(int ticket)
{
	mutex_cond.lock();
	blockedThreads++;

	while((counter < ticket && !this->reverseAwait)
			|| (counter > ticket && this->reverseAwait))
	{
		mutex_cond.wait();
	}

	blockedThreads--;
	mutex_cond.unlock();
}

int EventCounterAndSequencer::awaitWithTicket()
{
	int ticket = this->ticket();
	await(ticket);

	return ticket;
}

void EventCounterAndSequencer::advance()
{
	mutex_cond.lock();
	counter++;

	//handle roleover
	if(counter == INT_MAX)
	{
		counter = 0;
		this->reverseAwait = !this->reverseAwait;
	}
	mutex_cond.unlock();
	mutex_cond.signalAll();
}

int EventCounterAndSequencer::ticket()
{
	mutex_cond.lock();
	sequence++;

	//Handle roleover
	if(sequence == INT_MAX)
	{
		sequence = 0;
		this->reverseAwait = !this->reverseAwait;

	}

	int returnValue = sequence - 1;

	mutex_cond.unlock();

	return returnValue;
}

