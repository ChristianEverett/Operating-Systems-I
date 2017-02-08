/*
 * BufferManger.cpp
 *
 *  Created on: Nov 5, 2016
 *      Author: Christian Everett
 */

#include "BufferManger.h"

const int BufferManger::BUFFER_SIZE;

BufferManger::BufferManger()
{
	memset(buffer, 0, sizeof(buffer));
	std::fill(spaces, spaces + BUFFERS, BUFFER_SIZE);
	memset(objects, 0, sizeof(objects));
	memset(in, 0, sizeof(in));
	memset(out, 0, sizeof(out));
}

void BufferManger::produce(int id, int node, int data)
{
	produceLock.lock();

	while (spaces[id] == 0)
		produceLock.wait();

	buffer[id][in[id]].producerID = node;
	buffer[id][in[id]].data = data;
	in[id] = (in[id] + 1) % BUFFER_SIZE;
	spaces[id]--;
	produceLock.unlock();

	consumeLock.lock();
	objects[id]++;
	consumeLock.unlock();

	consumeLock.signal();
}

BufferManger::BufferElement BufferManger::consume(int id)
{
	consumeLock.lock();

	while (objects[id] == 0)
		consumeLock.wait();

	BufferElement object = buffer[id][out[id]];
	out[id] = (out[id] + 1) % BUFFER_SIZE;
	objects[id]--;

	consumeLock.unlock();

	produceLock.lock();
	spaces[id]++;
	produceLock.unlock();

	produceLock.signal();
	return object;
}

void BufferManger::processRequest(TCPConnection* client)
{
	try
	{
		Message message = client->readMessage();
		Message response = Message::createEmtpyMessage();
		std::cout << "Connected client - Buffer:" << message.getBufferNumber() << " P: " << (message.getOperation() == Message::PRODUCE) << std::endl;

		switch (message.getOperation())
		{
		case Message::CONSUME:
		{
			BufferElement result = consume(message.getBufferNumber());
			response = Message::createBufferMangerReply(message.getOperation(), message.getBufferNumber(), result.producerID, result.data);
			break;
		}
		case Message::PRODUCE:
		{
			produce(message.getBufferNumber(), message.getNodeId(), message.getData());
			response = Message::createBufferMangerReply(message.getOperation(), message.getBufferNumber(), message.getNodeId(), message.getData());
			break;
		}
		default:
			throw std::runtime_error("Invalid operation");
		}

		client->sendMessage(response);
		std::cout << "Responded to Client - " << "Spaces: " << spaces[0] << " " << spaces[1] << " " << spaces[2] << " " << spaces[3] <<
				" Objects:" << objects[0] << " " << objects[1] << " " << objects[2] << " " << objects[3] << std::endl;
		delete client;
	}
	catch (std::runtime_error& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		std::cerr << errno;
		exit(EXIT_FAILURE);
	}
}
