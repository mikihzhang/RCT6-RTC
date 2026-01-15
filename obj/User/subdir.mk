################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/GPIO.c \
../User/UDP.c \
../User/app_logic.c \
../User/ch32v20x_it.c \
../User/main.c \
../User/system_ch32v20x.c \
../User/tim.c 

C_DEPS += \
./User/GPIO.d \
./User/UDP.d \
./User/app_logic.d \
./User/ch32v20x_it.d \
./User/main.d \
./User/system_ch32v20x.d \
./User/tim.d 

OBJS += \
./User/GPIO.o \
./User/UDP.o \
./User/app_logic.o \
./User/ch32v20x_it.o \
./User/main.o \
./User/system_ch32v20x.o \
./User/tim.o 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/admin/Desktop/math+/Debug" -I"c:/Users/admin/Desktop/math+/Core" -I"c:/Users/admin/Desktop/math+/User" -I"c:/Users/admin/Desktop/math+/Peripheral/inc" -I"c:/Users/admin/Desktop/math+/NetLib" -I"c:/Users/admin/Desktop/math+/User/MQTT" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
