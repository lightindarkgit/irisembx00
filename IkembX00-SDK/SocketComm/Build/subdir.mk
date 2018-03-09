################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../autoMachine.cpp \
../socketAPI.cpp \
../socketBase.cpp \
../socketCon.cpp \
../socketInterfaceAPI.cpp \
../stdWait.cpp 

OBJS += \
./autoMachine.o \
./socketAPI.o \
./socketBase.o \
./socketCon.o \
./socketInterfaceAPI.o \
./stdWait.o 

CPP_DEPS += \
./autoMachine.d \
./socketAPI.d \
./socketBase.d \
./socketCon.d \
./socketInterfaceAPI.d \
./stdWait.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


