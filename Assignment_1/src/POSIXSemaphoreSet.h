/*
 * POSIXSemaphoreSet.h
 *
 *  Created on: Sep 16, 2016
 *      Author: Christian Everett
 */

#ifndef POSIXSEMAPHORESET_H_
#define POSIXSEMAPHORESET_H_

#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <stdexcept>

class POSIXSemaphoreSet
{
public:
	POSIXSemaphoreSet(std::string, int, int, bool);
	virtual ~POSIXSemaphoreSet();
	void signal(int);
	void wait(int);
	bool nonBlockingWait(int);
	int getSemaphoreValue(int);
	void deleteSemaphores();
private:
	std::string name;
	std::vector<sem_t*> semaphores;
	bool unlinkOnClose = false;
};

#endif /* POSIXSEMAPHORESET_H_ */
