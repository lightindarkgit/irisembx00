################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/asyncserial.cpp \
../source/asyncserialbase.cpp \
../source/basicserial.cpp \
../source/basictypes.cpp \
../source/cserial.cpp \
../source/innercommon.cpp \
../source/serialbase.cpp \
../source/serialframe.cpp \
../source/syncserial.cpp \
../source/syncserialbase.cpp 

OBJS += \
./source/asyncserial.o \
./source/asyncserialbase.o \
./source/basicserial.o \
./source/basictypes.o \
./source/cserial.o \
./source/innercommon.o \
./source/serialbase.o \
./source/serialframe.o \
./source/syncserial.o \
./source/syncserialbase.o 

CPP_DEPS += \
./source/asyncserial.d \
./source/asyncserialbase.d \
./source/basicserial.d \
./source/basictypes.d \
./source/cserial.d \
./source/innercommon.d \
./source/serialbase.d \
./source/serialframe.d \
./source/syncserial.d \
./source/syncserialbase.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/c++/4.8 -O0 -g3 -Wall -c -fmessage-length=0  -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


