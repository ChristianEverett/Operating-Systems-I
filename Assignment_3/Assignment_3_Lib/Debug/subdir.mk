################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Message.cpp \
../MutexConditionVariable.cpp \
../SignalRegister.cpp \
../TCPConnection.cpp \
../TCPServer.cpp 

OBJS += \
./Message.o \
./MutexConditionVariable.o \
./SignalRegister.o \
./TCPConnection.o \
./TCPServer.o 

CPP_DEPS += \
./Message.d \
./MutexConditionVariable.d \
./SignalRegister.d \
./TCPConnection.d \
./TCPServer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


