/*
 * NodeController.h
 *
 *  Created on: Nov 4, 2016
 *      Author: Christian Everett
 */

#ifndef NODECONTROLLER_H_
#define NODECONTROLLER_H_

#include <vector>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <map>
#include <array>
#include <thread>
#include <list>
#include <utility>
#include <queue>
#include <sys/epoll.h>
#include <fcntl.h>
#include "TCPConnection.h"
#include "TCPServer.h"
#include "Message.h"
#include "MutexConditionVariable.h"

using namespace std;

/**
 * Node Controller Singleton
 */
class NodeController
{
public:

	static void createNodeController(int id)
	{
		(instance == nullptr) ? instance = new NodeController(id) : throw std::runtime_error("Node Controller already exists");
	}

	static NodeController* getNodeController()
	{
		return instance == NULL ? throw std::runtime_error("Node Controller does'nt exist") : instance;
	}
	Message requestOperation(int, int);
	int getNodeID()
	{
		return nodeID;
	}
	static void shutdown();

private:
	NodeController(int);
	virtual ~NodeController();
	void run();
	void registerConnection(TCPConnection*, bool nodeConnection, int nodeID = -1);
	void handleRequest(Message, TCPConnection*);
	void addRequestToQueue(Message);
	void replyToRequests(Message);
	void handleReply(Message);

	MutexConditionVariable& getMutexFor(int operation, int buffer)
	{
		return operation == Message::PRODUCE ? produceMutex[buffer] : consumeMutex[buffer];
	}
	list<pair<Message, list<Message>>>& getQueueFor(int operation, int buffer)
	{
		return operation == Message::PRODUCE ? producerQueues[buffer] : consumerQueues[buffer];
	}
	int getAndAdjustClock(int reference = -1)
	{
		clockLock.lock();
		if(lamportClock <= reference)
		lamportClock = ++reference;
		clockLock.unlock();

		return lamportClock;
	}

	static NodeController *instance;

	TCPServer server;
	const int nodeID;
	int epollDescriptor;
	int lamportClock = 0;
	static const int BUFFERS = 4;
	int producerBuffer[BUFFERS];

	//Connection, flag true if this is a node connection
	map<int, pair<TCPConnection*, bool>> hostConnectionMap;
	//Map nodeIDs to TCPConnections
	map<int, TCPConnection*> nodeIDConnectionMap;

	bool running = true;
	thread *nodeControllerThread;

	//Each buffer has a list of pairs containing the request message, and the list of reply messages
	array<list<pair<Message, list<Message>>>, BUFFERS> producerQueues;
	array<list<pair<Message, list<Message>>>, BUFFERS> consumerQueues;

	MutexConditionVariable produceMutex[BUFFERS];
	MutexConditionVariable consumeMutex[BUFFERS];
	MutexConditionVariable clockLock;
	MutexConditionVariable mapLock;
};

#endif /* NODECONTROLLER_H_ */
