/*
 * POSIXSharedMemoryContainer.cpp
 *
 *  Created on: Sep 17, 2016
 *      Author: Christian Everett
 */

#include "POSIXSharedMemory.h"

POSIXSharedMemory::POSIXSharedMemory(std::string name, size_t size, bool deleteOnClose)
{
	this->name = name;
	this->deleteOnClose = deleteOnClose;

	if((fd = shm_open(name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
	{
		throw std::runtime_error("Cannot open shared memory: " + name);
	}
	if(ftruncate(fd, size) == -1)
	{
		this->~POSIXSharedMemory();
		throw std::runtime_error("Cannot size shared memory: " + name);
	}
	if((sharedMemory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		this->~POSIXSharedMemory();
		throw std::runtime_error("Cannot map shared memory: " + name);
	}
}

POSIXSharedMemory::~POSIXSharedMemory()
{
	if(sharedMemory != NULL)
	{
		if(munmap(sharedMemory, size()) == -1)
			throw std::runtime_error("Cannot unmap shared memory: " + name);
	}
	if(fd != -1)
	{
		close(fd);
	}
	if(deleteOnClose)
	{
		shm_unlink(name.c_str());
	}
}

size_t POSIXSharedMemory::size()
{
	if(fd != -1)
	{
		struct stat stats;
		if(fstat(fd, &stats) != -1)
			return stats.st_size;

		throw std::runtime_error("Cannot read stats of shared memory: " + name);
	}

	return 0;
}

void * POSIXSharedMemory::getMemory()
{
	if(sharedMemory == NULL)
		throw std::runtime_error("Null Pointer on shared memory: " + name);
	return sharedMemory;
}

//void * POSIXSharedMemory::getMemory()
//{
//	char buf[5];
//
//	lseek(fd, 0, SEEK_SET);
//	int result = read(fd, &buf, 5);
//
//	return NULL;
//}

//void POSIXSharedMemory::setMemory(void * data)
//{
//	char buf[5] = "test";
//
//	int result = write(fd, &buf, 5);
//}

