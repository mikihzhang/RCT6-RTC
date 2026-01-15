################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../NetLib/eth_driver.c 

C_DEPS += \
./NetLib/eth_driver.d 

OBJS += \
./NetLib/eth_driver.o 



# Each subdirectory must supply rules for building sources it contributes
NetLib/%.o: ../NetLib/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/admin/Desktop/math+/Debug" -I"c:/Users/admin/Desktop/math+/Core" -I"c:/Users/admin/Desktop/math+/User" -I"c:/Users/admin/Desktop/math+/Peripheral/inc" -I"c:/Users/admin/Desktop/math+/NetLib" -I"c:/Users/admin/Desktop/math+/User/MQTT" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
