/*
 * NodeController.cpp
 *
 *  Created on: Nov 4, 2016
 *      Author: Christian Everett
 */

#include "NodeController.h"

NodeController *NodeController::instance = nullptr;

NodeController::NodeController(int nodeID) :
		nodeID(nodeID)
{
	memset(producerBuffer, 0, sizeof(producerBuffer));
	if ((epollDescriptor = epoll_create1(0)) == -1)
		throw std::runtime_error("Could not create epoll Descriptor");

	for (int i = 0; i < nodeID; i++)
	{
		TCPConnection *client = new TCPConnection(TCPConnection::ID_IP_MAP[i], TCPServer::NODE_PORT);
		registerConnection(client, true, i);
	}

	for (int i = nodeID + 1; i < TCPConnection::ID_IP_MAP.size(); i++)
	{
		registerConnection(server.acceptTCPConnection(), true, i);
	}

	nodeControllerThread = new std::thread(&NodeController::run, this);
}

NodeController::~NodeController()
{
	running = false;
	//close(epollDescriptor);

	for(int i = 0; i < BUFFERS; i++)
	{
		produceMutex[i].signalAll();
		consumeMutex[i].signalAll();
	}

	nodeControllerThread->join();
	delete nodeControllerThread;
}

void NodeController::shutdown()
{
	if (instance != nullptr)
		delete instance;
}

void NodeController::run()
{
	try
	{
		const int REQUEST_LIMIT = 10;
		struct epoll_event events[REQUEST_LIMIT];

		while (running)
		{
			int requests = epoll_wait(epollDescriptor, events, REQUEST_LIMIT, -1);

			if (requests == -1)
			{
				if (!running)
					break;
				if (errno == EINTR)
					continue;
				throw std::runtime_error("Poll on file Descriptor set failed");
			}

			for (int i = 0; i < requests; i++)
			{
				if (!(events[i].events & EPOLLIN))
					throw std::runtime_error("Poll set returned non input event");

				TCPConnection *connection = hostConnectionMap.at(events[i].data.fd).first;
				Message message = connection->readMessage();
				handleRequest(message, connection);
			}
		}
	}
	catch (runtime_error& e)
	{
		cerr << e.what() << endl;
		cerr << errno;
		_exit(errno);
	}
}

void NodeController::handleRequest(Message message, TCPConnection *connection)
{
	getAndAdjustClock(message.getClockValue());

	switch (message.getRequestType())
	{
	case Message::REQUEST:
	{
		addRequestToQueue(message);
		replyToRequests(message);
		break;
	}
	case Message::REPLY:
	{
		handleReply(message);
		break;
	}
	case Message::BUFFER_ACCESS:
	{
		mapLock.lock();
		hostConnectionMap.erase(connection->getSocketDescripter());
		delete connection;
		mapLock.unlock();
		getMutexFor(message.getOperation(), message.getBufferNumber()).lock();
		getQueueFor(message.getOperation(), message.getBufferNumber()).front().second.push_back(message);
		getMutexFor(message.getOperation(), message.getBufferNumber()).unlock();
		getMutexFor(message.getOperation(), message.getBufferNumber()).signalAll();

		break;
	}
	default:
		throw std::runtime_error("Unknown Message Operation: " + message.getOperation());
	}
}

void NodeController::registerConnection(TCPConnection* connection, bool nodeConnection, int nodeID)
{
	int fd = connection->getSocketDescripter();

	pair<TCPConnection*, bool> typeConnection(connection, nodeConnection);
	hostConnectionMap.insert(pair<int, pair<TCPConnection*, bool>>(fd, typeConnection));
	nodeIDConnectionMap.insert(pair<int, TCPConnection*>(nodeID, connection));

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = fd;

	if (epoll_ctl(epollDescriptor, EPOLL_CTL_ADD, fd, &event) == -1)
		throw std::runtime_error("Can't add File Descriptor to poll set");
}

Message NodeController::requestOperation(int buffer, int operation)
{

	Message message = Message::createEmtpyMessage();
	getAndAdjustClock(getAndAdjustClock());

	if (operation == Message::PRODUCE)
		message = Message::createProduceRequest(nodeID, buffer, getAndAdjustClock(), ++producerBuffer[buffer]);
	else
		message = Message::createConsumeRequest(nodeID, buffer, getAndAdjustClock());

	addRequestToQueue(message);

	getMutexFor(operation, buffer).lock();
	mapLock.lock();
	for (std::map<int, pair<TCPConnection*, bool>>::iterator iter = hostConnectionMap.begin(); iter != hostConnectionMap.end(); iter++)
	{
		if (iter->second.second)
			iter->second.first->sendMessage(message);
	}
	mapLock.unlock();

	list<pair<Message, list<Message>>>&queue = getQueueFor(message.getOperation(), message.getBufferNumber());
	//Size of all other nodes plus buffer manger
	while (((queue.front().second.size() < TCPConnection::ID_IP_MAP.size()) || (queue.front().first.getClockValue() != message.getClockValue()))
			&& running)
		getMutexFor(operation, buffer).wait();

	Message result = getQueueFor(operation, buffer).front().second.back();
	getQueueFor(operation, buffer).pop_front();
	getMutexFor(operation, buffer).unlock();
	replyToRequests(message);

	return result;
}

void NodeController::addRequestToQueue(Message message)
{
	getMutexFor(message.getOperation(), message.getBufferNumber()).lock();
	list<pair<Message, list<Message>>>&queue = getQueueFor(message.getOperation(), message.getBufferNumber());

	if (queue.empty())
	{
		queue.push_back(make_pair(message, list<Message>()));
	}
	else
	{
		for (list<pair<Message, list<Message>>> ::iterator iter = queue.begin(); iter != queue.end(); ++iter)
		{
			if(iter->first.getClockValue() >= message.getClockValue())
			{
				if(iter->first.getClockValue() == message.getClockValue() && nodeID < message.getNodeId())
				iter++;
				queue.insert(iter, make_pair(message, list<Message>()));
				break;
			}
			else if(next(iter) == queue.end())
			{
				queue.push_back(make_pair(message, list<Message>()));
				break;
			}
		}
	}

	getMutexFor(message.getOperation(), message.getBufferNumber()).unlock();
}

void NodeController::replyToRequests(Message message)
{
	int clock = getAndAdjustClock();
	getMutexFor(message.getOperation(), message.getBufferNumber()).lock();

	list<pair<Message, list<Message>>>&queue = getQueueFor(message.getOperation(), message.getBufferNumber());
	for (list<pair<Message, list<Message>>> ::iterator iter = queue.begin(); iter != queue.end();)
	{
		if(iter->first.getNodeId() == nodeID)
		{
			break;
		}
		else
		{
			nodeIDConnectionMap.at(iter->first.getNodeId())->sendMessage(Message::createReply(nodeID, iter->first.getOperation(),
					iter->first.getBufferNumber(), clock, iter->first.getClockValue()));
			iter = queue.erase(iter);
		}
	}

	getMutexFor(message.getOperation(), message.getBufferNumber()).unlock();
}

void NodeController::handleReply(Message message)
{
	getMutexFor(message.getOperation(), message.getBufferNumber()).lock();
	list<pair<Message, list<Message>>>&queue = getQueueFor(message.getOperation(), message.getBufferNumber());

	for (list<pair<Message, list<Message>>> ::iterator iter = queue.begin(); iter != queue.end();)
	{
		if(iter->first.getNodeId() == nodeID && iter->first.getClockValue() < message.getClockValue()
				&& iter->first.getClockValue() == message.getReplyClock())
		{
			iter->second.push_back(message);

			//Is this the last message needed to run
			if(iter->second.size() == TCPConnection::ID_IP_MAP.size() - 1)
			{
				TCPConnection *bufferConnection = new TCPConnection(TCPConnection::BUFFER_MANGER_IP, TCPServer::BUFFER_MANGER_PORT);
				registerConnection(bufferConnection, false);
				bufferConnection->sendMessage(iter->first);
			}

			break;
		}
		else if(iter->first.getNodeId() == message.getNodeId())
		{
			iter = queue.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	getMutexFor(message.getOperation(), message.getBufferNumber()).unlock();
}
