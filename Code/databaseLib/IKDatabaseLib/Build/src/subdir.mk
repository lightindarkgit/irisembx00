################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/IKDatabaseLib.cpp \
../src/IKDatabaseSession.cpp \
../src/IKPsqlSession.cpp \
../src/IKSqliteSession.cpp \
../src/IKTransaction.cpp 

OBJS += \
./src/IKDatabaseLib.o \
./src/IKDatabaseSession.o \
./src/IKPsqlSession.o \
./src/IKSqliteSession.o \
./src/IKTransaction.o 

CPP_DEPS += \
./src/IKDatabaseLib.d \
./src/IKDatabaseSession.d \
./src/IKPsqlSession.d \
./src/IKSqliteSession.d \
./src/IKTransaction.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../include -I/usr/include/postgresql -I/usr/local/include/soci -O0 -g3 -pedantic -Wall -c -fmessage-length=0 -std=c++11 -Wno-long-long -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


