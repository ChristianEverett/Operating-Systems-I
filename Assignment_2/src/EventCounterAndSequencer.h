/*
 * EventCounterAndSequencer.h
 *
 *  Created on: Sep 24, 2016
 *      Author: Christian Everett
 */

#ifndef EVENTCOUNTERANDSEQUENCER_H_
#define EVENTCOUNTERANDSEQUENCER_H_

#include <climits>
#include "MutexConditionVariable.h"

class EventCounterAndSequencer
{
public:
	EventCounterAndSequencer();
	virtual ~EventCounterAndSequencer();

	void await(int);
	int awaitWithTicket();
	int ticket();
	void advance();

private:
	bool waitForTicket();

	int counter;
	int sequence;
	bool reverseAwait;
	bool close;
	unsigned blockedThreads;
	MutexConditionVariable mutex_cond;
};

#endif /* EVENTCOUNTERANDSEQUENCER_H_ */
