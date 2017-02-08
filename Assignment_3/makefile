CC=gcc
CXX=g++
CPPFLAGS=-g -Wall -std=c++11
LDFLAGS=-g
LDLIBS=-lpthread

build: Message.o MutexConditionVariable.o SignalRegister.o TCPConnection.o TCPServer.o BufferManger.o NodeController.o main_node.o NodeController.o main_bufferManger.o
	ar r libAssignment_3_Lib.a Message.o MutexConditionVariable.o SignalRegister.o TCPConnection.o TCPServer.o
	$(CXX) $(CPPFLAGS) -o BufferManger main_bufferManger.o BufferManger.o ./libAssignment_3_Lib.a $(LDLIBS)
	$(CXX) $(CPPFLAGS) -o Node main_node.o NodeController.o ./libAssignment_3_Lib.a $(LDLIBS)
Message.o: Message.cpp
	$(CXX) $(CPPFLAGS) -c $(LDLIBS) Message.cpp
SignalRegister.o:  SignalRegister.cpp
	$(CXX) $(CPPFLAGS) -c $(LDLIBS) SignalRegister.cpp
MutexConditionVariable.o:  MutexConditionVariable.cpp
	$(CXX) $(CPPFLAGS) -c $(LDLIBS) MutexConditionVariable.cpp
TCPConnection.o:  TCPConnection.cpp
	$(CXX) $(CPPFLAGS) -c $(LDLIBS) TCPConnection.cpp
TCPServer.o:  TCPServer.cpp
	$(CXX) $(CPPFLAGS) -c $(LDLIBS) TCPServer.cpp
main_bufferManger.o: main_bufferManger.cpp
	$(CXX) $(CPPFLAGS) -c $(LDLIBS) main_bufferManger.cpp
BufferManger.o:  BufferManger.cpp
	$(CXX) $(CPPFLAGS) -c $(LDLIBS) BufferManger.cpp
main_node.o: main_node.cpp
	$(CXX) $(CPPFLAGS) -c $(LDLIBS) main_node.cpp
NodeController.o:  NodeController.cpp
	$(CXX) $(CPPFLAGS) -c $(LDLIBS) NodeController.cpp
	
clean:
	rm *.o BufferManger Node libAssignment_3_Lib.a
