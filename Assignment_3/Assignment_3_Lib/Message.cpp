/*
 * Message.cpp
 *
 *  Created on: Oct 22, 2016
 *      Author: Christian Everett
 */

#include "Message.h"

Message::Message(int nodeID, int request, int operation, int bufferNumber, int clock, int dataProducer, int data, int replyClock)
{
	this->nodeID = htonl(nodeID);
	this->request = htonl(request);
	this->operation = htonl(operation);
	this->bufferNumber = htonl(bufferNumber);
	this->clock = htonl(clock);
	this->dataProducer = htonl(dataProducer);
	this->data = htonl(data);
	this->replyClock = htonl(replyClock);
}

Message::~Message()
{
}

Message Message::createEmtpyMessage()
{
	Message message(-1, -1, -1, -1, -1, -1, -1, -1);
	return message;
}

Message Message::createProduceRequest(int nodeID, int buffer, int clock, int data)
{
	Message message(nodeID, REQUEST, PRODUCE, buffer, clock, nodeID, data, -1);
	return message;
}

Message Message::createConsumeRequest(int nodeID, int buffer, int clock)
{
	Message message(nodeID, REQUEST, CONSUME, buffer, clock, -1, -1, -1);
	return message;
}

Message Message::createReply(int nodeID, int operation, int buffer, int clock, int replyClock)
{
	Message message(nodeID, REPLY, operation, buffer, clock, -1, -1, replyClock);
	return message;
}

Message Message::createBufferMangerReply(int operation, int buffer, int dataProducer, int data)
{
	Message message(-1, BUFFER_ACCESS , operation, buffer, -1, dataProducer, data, -1);
	return message;
}
