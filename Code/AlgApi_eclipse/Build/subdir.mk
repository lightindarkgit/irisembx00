################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AlgApi.cpp \
../ApiBase.cpp \
../ApiGetFlag.cpp \
../CameraBase.cpp 

OBJS += \
./AlgApi.o \
./ApiBase.o \
./ApiGetFlag.o \
./CameraBase.o 

CPP_DEPS += \
./AlgApi.d \
./ApiBase.d \
./ApiGetFlag.d \
./CameraBase.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C++ 编译器'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -fpermissive -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


