################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../iimportandexport.cpp \
../ikiostream.cpp \
../interface.cpp 

OBJS += \
./iimportandexport.o \
./ikiostream.o \
./interface.o 

CPP_DEPS += \
./iimportandexport.d \
./ikiostream.d \
./interface.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/soci -I/usr/include/postgresql -I../../databaseLib/IKDatabaseLib/include -O0 -g3 -Wall -c -fmessage-length=0  -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


