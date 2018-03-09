################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Client/netClient.cpp \
../Client/netClientPoll.cpp \
../Client/netClientSelect.cpp 

OBJS += \
./Client/netClient.o \
./Client/netClientPoll.o \
./Client/netClientSelect.o 

CPP_DEPS += \
./Client/netClient.d \
./Client/netClientPoll.d \
./Client/netClientSelect.d 


# Each subdirectory must supply rules for building sources it contributes
Client/%.o: ../Client/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


