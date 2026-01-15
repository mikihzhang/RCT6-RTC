################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MQTT/MQTTConnectClient.c \
../MQTT/MQTTDeserializePublish.c \
../MQTT/MQTTFormat.c \
../MQTT/MQTTPacket.c \
../MQTT/MQTTSerializePublish.c \
../MQTT/MQTTSubscribeClient.c \
../MQTT/MQTTUnsubscribeClient.c 

C_DEPS += \
./MQTT/MQTTConnectClient.d \
./MQTT/MQTTDeserializePublish.d \
./MQTT/MQTTFormat.d \
./MQTT/MQTTPacket.d \
./MQTT/MQTTSerializePublish.d \
./MQTT/MQTTSubscribeClient.d \
./MQTT/MQTTUnsubscribeClient.d 

OBJS += \
./MQTT/MQTTConnectClient.o \
./MQTT/MQTTDeserializePublish.o \
./MQTT/MQTTFormat.o \
./MQTT/MQTTPacket.o \
./MQTT/MQTTSerializePublish.o \
./MQTT/MQTTSubscribeClient.o \
./MQTT/MQTTUnsubscribeClient.o 



# Each subdirectory must supply rules for building sources it contributes
MQTT/%.o: ../MQTT/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DDEBUG=2 -I"d:/2.WORK/002code/002-fiber-CH32V208GBU6/TCP/Startup" -I"d:/2.WORK/002code/002-fiber-CH32V208GBU6/TCP/NetLib" -I"d:/2.WORK/002code/002-fiber-CH32V208GBU6/TCP/Debug" -I"d:/2.WORK/002code/002-fiber-CH32V208GBU6/TCP/MQTT/inc" -I"d:/2.WORK/002code/002-fiber-CH32V208GBU6/TCP/Core" -I"d:/2.WORK/002code/002-fiber-CH32V208GBU6/TCP/User" -I"d:/2.WORK/002code/002-fiber-CH32V208GBU6/TCP/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
