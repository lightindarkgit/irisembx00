################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/IKXmlApi.cpp \
../source/NoneCopyable.cpp \
../source/OPutBuffer.cpp \
../source/OStreamBuffer.cpp \
../source/Xml.cpp \
../source/XmlAttribute.cpp \
../source/XmlComment.cpp \
../source/XmlContent.cpp \
../source/XmlDocument.cpp \
../source/XmlElement.cpp \
../source/XmlNode.cpp \
../source/XmlText.cpp 

OBJS += \
./source/IKXmlApi.o \
./source/NoneCopyable.o \
./source/OPutBuffer.o \
./source/OStreamBuffer.o \
./source/Xml.o \
./source/XmlAttribute.o \
./source/XmlComment.o \
./source/XmlContent.o \
./source/XmlDocument.o \
./source/XmlElement.o \
./source/XmlNode.o \
./source/XmlText.o 

CPP_DEPS += \
./source/IKXmlApi.d \
./source/NoneCopyable.d \
./source/OPutBuffer.d \
./source/OStreamBuffer.d \
./source/Xml.d \
./source/XmlAttribute.d \
./source/XmlComment.d \
./source/XmlContent.d \
./source/XmlDocument.d \
./source/XmlElement.d \
./source/XmlNode.d \
./source/XmlText.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../libxml2/ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


