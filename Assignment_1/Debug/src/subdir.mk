################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Consumer.cpp \
../src/POSIXSemaphoreSet.cpp \
../src/POSIXSharedMemory.cpp \
../src/Producer.cpp \
../src/SignalRegister.cpp 

OBJS += \
./src/Consumer.o \
./src/POSIXSemaphoreSet.o \
./src/POSIXSharedMemory.o \
./src/Producer.o \
./src/SignalRegister.o 

CPP_DEPS += \
./src/Consumer.d \
./src/POSIXSemaphoreSet.d \
./src/POSIXSharedMemory.d \
./src/Producer.d \
./src/SignalRegister.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


