/*
 * Message.h
 *
 *  Created on: Oct 22, 2016
 *      Author: Christian Everett
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <arpa/inet.h>
#include <vector>

class Message
{
public:

	virtual ~Message();

	//int nodeID, int request, int buffer, int clock, int data
	static Message createProduceRequest(int, int, int, int);
	//int nodeID, int buffer, int clock
	static Message createConsumeRequest(int, int, int);
	//int nodeID, int operation, int buffer, int clock, int reply clock
	static Message createReply(int, int, int, int, int);
	//int operation, int buffer, int dataProducer, int data
	static Message createBufferMangerReply(int, int, int, int);
	static Message createEmtpyMessage();

	int getBufferNumber() const
	{
		return ntohl(bufferNumber);
	}

	int getNodeId() const
	{
		return ntohl(nodeID);
	}

	int getRequestType() const
	{
		return ntohl(request);
	}

	int getOperation() const
	{
		return ntohl(operation);
	}

	int getClockValue() const
	{
		return ntohl(clock);
	}

	int getDataProducer() const
	{
		return ntohl(dataProducer);
	}

	int getData() const
	{
		return ntohl(data);
	}

	int getReplyClock() const
	{
		return ntohl(replyClock);
	}

	static const int REQUEST = 0;
	static const int REPLY = 1;
	static const int BUFFER_ACCESS = 2;

	static const int PRODUCE = 3;
	static const int CONSUME = 4;


private:
	Message(int, int, int, int, int, int, int, int);

	int nodeID;
	int request;
	int operation;
	int bufferNumber;

	//Node - Node data
	int clock;
	int replyClock;

	//BufferManger - Node data
	int dataProducer;
	int data;

};

#endif /* MESSAGE_H_ */
