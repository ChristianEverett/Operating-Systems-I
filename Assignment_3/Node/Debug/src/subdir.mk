################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/NodeController.cpp \
../src/main_node.cpp 

OBJS += \
./src/NodeController.o \
./src/main_node.o 

CPP_DEPS += \
./src/NodeController.d \
./src/main_node.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -I"C:\Computer Science\UML\Operating Systems I\Assignments\Assignment_3\Assignment_3_Lib" -O0 -g3 -Wall -c -fmessage-length=0 --target=arm-linux-gnueabihf -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


