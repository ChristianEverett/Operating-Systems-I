proddonuts consdonuts: Donuts.h Producer.o Consumer.o POSIXSemaphoreSet.o POSIXSharedMemory.o SignalRegister.o
	g++ -Wall -o proddonuts Producer.o POSIXSemaphoreSet.o POSIXSharedMemory.o SignalRegister.o -lpthread -lrt
	g++ -Wall -o consdonuts Consumer.o POSIXSemaphoreSet.o POSIXSharedMemory.o SignalRegister.o -lpthread -lrt
Consumer.o: Donuts.h Consumer.cpp
	g++ -Wall -c -g -std=c++0x -lpthread Consumer.cpp
Producer.o: Donuts.h Producer.cpp
	g++ -Wall -c -g -std=c++0x -lpthread Producer.cpp
SignalRegister.o:  SignalRegister.cpp
	g++ -Wall -c -g -std=c++0x -lpthread SignalRegister.cpp
POSIXSharedMemory.o:  POSIXSharedMemory.cpp
	g++ -Wall -c -g -std=c++0x -lpthread POSIXSharedMemory.cpp
POSIXSemaphoreSet.o:  POSIXSemaphoreSet.cpp
	g++ -Wall -c -g -std=c++0x -lpthread POSIXSemaphoreSet.cpp
	
clean:
	rm *.o proddonuts consdonuts
