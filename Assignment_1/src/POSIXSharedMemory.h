/*
 * POSIXSharedMemoryContainer.h
 *
 *  Created on: Sep 17, 2016
 *      Author: Christian Everett
 */

#ifndef POSIXSHAREDMEMORY_H_
#define POSIXSHAREDMEMORY_H_

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string>
#include <stdexcept>

class POSIXSharedMemory
{
public:
	POSIXSharedMemory(std::string, size_t, bool);
	virtual ~POSIXSharedMemory();
	size_t size();
	void *getMemory();
	void setMemory(void*);

private:
	int fd = -1;
	void *sharedMemory = NULL;
	bool deleteOnClose = false;
	std::string name;
};

#endif /* POSIXSHAREDMEMORY_H_ */
