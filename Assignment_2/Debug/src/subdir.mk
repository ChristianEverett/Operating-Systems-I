################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/EventCounterAndSequencer.cpp \
../src/MutexConditionVariable.cpp \
../src/SignalRegister.cpp \
../src/main.cpp 

OBJS += \
./src/EventCounterAndSequencer.o \
./src/MutexConditionVariable.o \
./src/SignalRegister.o \
./src/main.o 

CPP_DEPS += \
./src/EventCounterAndSequencer.d \
./src/MutexConditionVariable.d \
./src/SignalRegister.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


