/*
 * MutexConditionVariable.h
 *
 *  Created on: Sep 24, 2016
 *      Author: Christian Everett
 */

#ifndef MUTEXCONDITIONVARIABLE_H_
#define MUTEXCONDITIONVARIABLE_H_

#include <pthread.h>
#include <stdexcept>

class MutexConditionVariable
{
public:
	MutexConditionVariable();
	virtual ~MutexConditionVariable();

	void lock();
	void unlock();
	bool isLocked();

	void signalAll();
	void signal();
	void wait();

	void close();

private:
	pthread_mutex_t mutex;
	pthread_cond_t conditionVariable;
};

#endif /* MUTEXCONDITIONVARIABLE_H_ */
