/*
 * MutexConditionVariable.cpp
 *
 *  Created on: Sep 24, 2016
 *      Author: Christian Everett
 */

#include "MutexConditionVariable.h"

MutexConditionVariable::MutexConditionVariable()
{
	if (pthread_mutex_init(&mutex, NULL) != 0)
		throw std::runtime_error("Could not create mutex");
	if (pthread_cond_init(&conditionVariable, NULL) != 0)
		throw std::runtime_error("Could not create condition variable");
}

MutexConditionVariable::~MutexConditionVariable()
{

}

void MutexConditionVariable::close()
{
	if (pthread_mutex_destroy(&mutex) != 0)
		throw std::runtime_error("Could not destroy mutex");
	if (pthread_cond_destroy(&conditionVariable) != 0)
		throw std::runtime_error("Could not destroy condition variable");
}

void MutexConditionVariable::lock()
{
	if (pthread_mutex_lock(&mutex) != 0)
		throw std::runtime_error("Failed to lock mutex");
}

void MutexConditionVariable::unlock()
{
	if (pthread_mutex_unlock(&mutex) != 0)
		throw std::runtime_error("Failed to unlock mutex");
}

bool MutexConditionVariable::isLocked()
{
	if (pthread_mutex_trylock(&mutex) == 0)
	{
		MutexConditionVariable::unlock();
		return false;
	}
	else
	{
		return true;
	}
}

void MutexConditionVariable::signalAll()
{
	if (pthread_cond_broadcast(&conditionVariable) != 0)
		throw std::runtime_error("Could not signal all condition variables");
}

void MutexConditionVariable::signal()
{
	if (pthread_cond_signal(&conditionVariable) != 0)
		throw std::runtime_error("Could not signal condition variable");
}

void MutexConditionVariable::wait()
{
	if (pthread_cond_wait(&conditionVariable, &mutex) != 0)
		throw std::runtime_error("Could not wait on condition variable");
}
