/*
 * POSIXSemaphoreSet.cpp
 *
 *  Created on: Sep 16, 2016
 *      Author: Christian Everett
 */

#include "POSIXSemaphoreSet.h"

POSIXSemaphoreSet::POSIXSemaphoreSet(std::string name, int numberOfSemaphores, int value, bool unlinkOnClose)
{
	for (int x = 0; x < numberOfSemaphores; x++)
	{
		sem_t *semaphore = sem_open((name + std::to_string((x + 1))).c_str(),
		O_CREAT, S_IRUSR | S_IWUSR, value);
		if (semaphore == SEM_FAILED)
			throw std::runtime_error("Failed to create semaphore: " + name + std::to_string(x));
		semaphores.push_back(semaphore);
	}

	this->name = name;
	this->unlinkOnClose = unlinkOnClose;
}

POSIXSemaphoreSet::~POSIXSemaphoreSet()
{
	for (int x = 0; x < semaphores.size(); x++)
	{
		sem_close(semaphores.at(x));
	}

	if(unlinkOnClose)
	{
		deleteSemaphores();
	}
}

void POSIXSemaphoreSet::signal(int semaphoreID)
{
	if (sem_post(semaphores.at(semaphoreID )) == -1)
	{
		throw std::runtime_error("Failed to signal semaphore: " + semaphoreID);
	}
}

void POSIXSemaphoreSet::wait(int semaphoreID)
{
	if (sem_wait(semaphores.at(semaphoreID)) == -1)
	{
		throw std::runtime_error("Failed to wait for semaphore: " + semaphoreID);
	}
}

int POSIXSemaphoreSet::getSemaphoreValue(int semaphoreID)
{
	int value = -1;

	if (sem_getvalue(semaphores.at(semaphoreID), &value) == -1)
	{
		throw std::runtime_error("Failed to check value for semaphore: " + semaphoreID);
	}

	return value;
}

bool POSIXSemaphoreSet::nonBlockingWait(int semaphoreID)
{
	if (sem_trywait(semaphores.at(semaphoreID)) == -1)
	{
		if(errno == EAGAIN)
			return false;

		throw std::runtime_error("Failed to wait for semaphore: " + semaphoreID);
	}

	return true;
}

void POSIXSemaphoreSet::deleteSemaphores()
{
	if(!unlinkOnClose)
		return;

	for (int x = 0; x < semaphores.size(); x++)
	{
		sem_unlink((name + std::to_string(x + 1)).c_str());
	}
}

