/*
 * BufferManger.h
 *
 *  Created on: Nov 5, 2016
 *      Author: Christian Everett
 */

#ifndef BUFFERMANGER_H_
#define BUFFERMANGER_H_

#include <iostream>
#include <errno.h>
#include "TCPServer.h"
#include "Message.h"
#include "MutexConditionVariable.h"

class BufferManger
{
private:

	static const int BUFFERS = 4;
	static const int BUFFER_SIZE = 70;

	MutexConditionVariable produceLock;
	MutexConditionVariable consumeLock;

	typedef struct
	{
		int producerID;
		int data;
	}BufferElement;

	BufferElement buffer[BUFFERS][BUFFER_SIZE];
	int spaces[BUFFERS], objects[BUFFERS];
	int in[BUFFERS], out[BUFFERS];

public:
	BufferManger();
	void processRequest(TCPConnection*);
	void produce(int, int, int);
	BufferElement consume(int);

};

#endif /* BUFFERMANGER_H_ */
