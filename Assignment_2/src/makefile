build: main.o SignalRegister.o MutexConditionVariable.o EventCounterAndSequencer.o
	g++ -Wall -o Assignment_2 main.o MutexConditionVariable.o EventCounterAndSequencer.o SignalRegister.o -lpthread
main.o: main.cpp
	g++ -Wall -c -g -std=c++0x -lpthread main.cpp
SignalRegister.o:  SignalRegister.cpp
	g++ -Wall -c -g -std=c++0x -lpthread SignalRegister.cpp
MutexConditionVariable.o:  MutexConditionVariable.cpp
	g++ -Wall -c -g -std=c++0x -lpthread MutexConditionVariable.cpp
EventCounterAndSequencer.o:  EventCounterAndSequencer.cpp
	g++ -Wall -c -g -std=c++0x -lpthread EventCounterAndSequencer.cpp
	
clean:
	rm *.o Assignment_2
