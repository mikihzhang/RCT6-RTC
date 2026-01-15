################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/MQTT/MQTTConnectClient.c \
../User/MQTT/MQTTDeserializePublish.c \
../User/MQTT/MQTTFormat.c \
../User/MQTT/MQTTPacket.c \
../User/MQTT/MQTTSerializePublish.c \
../User/MQTT/MQTTSubscribeClient.c \
../User/MQTT/MQTTUnsubscribeClient.c 

C_DEPS += \
./User/MQTT/MQTTConnectClient.d \
./User/MQTT/MQTTDeserializePublish.d \
./User/MQTT/MQTTFormat.d \
./User/MQTT/MQTTPacket.d \
./User/MQTT/MQTTSerializePublish.d \
./User/MQTT/MQTTSubscribeClient.d \
./User/MQTT/MQTTUnsubscribeClient.d 

OBJS += \
./User/MQTT/MQTTConnectClient.o \
./User/MQTT/MQTTDeserializePublish.o \
./User/MQTT/MQTTFormat.o \
./User/MQTT/MQTTPacket.o \
./User/MQTT/MQTTSerializePublish.o \
./User/MQTT/MQTTSubscribeClient.o \
./User/MQTT/MQTTUnsubscribeClient.o 



# Each subdirectory must supply rules for building sources it contributes
User/MQTT/%.o: ../User/MQTT/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/admin/Desktop/math+/Debug" -I"c:/Users/admin/Desktop/math+/Core" -I"c:/Users/admin/Desktop/math+/User" -I"c:/Users/admin/Desktop/math+/Peripheral/inc" -I"c:/Users/admin/Desktop/math+/NetLib" -I"c:/Users/admin/Desktop/math+/User/MQTT" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
