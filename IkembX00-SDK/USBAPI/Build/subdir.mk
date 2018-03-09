################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../asyncusbapi.cpp \
../dealwithusbtransferbuf.cpp \
../ikusbapibase.cpp \
../imagemanger.cpp \
../iusbimage.cpp \
../simplelock.cpp \
../syncusbapi.cpp 

OBJS += \
./asyncusbapi.o \
./dealwithusbtransferbuf.o \
./ikusbapibase.o \
./imagemanger.o \
./iusbimage.o \
./simplelock.o \
./syncusbapi.o 

CPP_DEPS += \
./asyncusbapi.d \
./dealwithusbtransferbuf.d \
./ikusbapibase.d \
./imagemanger.d \
./iusbimage.d \
./simplelock.d \
./syncusbapi.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -Iusb -O0 -g3 -Wall -c -fmessage-length=0  -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


