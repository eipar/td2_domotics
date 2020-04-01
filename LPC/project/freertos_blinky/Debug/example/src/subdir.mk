################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cr_startup_lpc175x_6x.c \
../example/src/ff.c \
../example/src/fs.c \
../example/src/hardware.c \
../example/src/lcd.c \
../example/src/lpc17xx_spi.c \
../example/src/main.c \
../example/src/sd.c \
../example/src/sysinit.c \
../example/src/uart2.c \
../example/src/uart3.c 

OBJS += \
./example/src/cr_startup_lpc175x_6x.o \
./example/src/ff.o \
./example/src/fs.o \
./example/src/hardware.o \
./example/src/lcd.o \
./example/src/lpc17xx_spi.o \
./example/src/main.o \
./example/src/sd.o \
./example/src/sysinit.o \
./example/src/uart2.o \
./example/src/uart3.o 

C_DEPS += \
./example/src/cr_startup_lpc175x_6x.d \
./example/src/ff.d \
./example/src/fs.d \
./example/src/hardware.d \
./example/src/lcd.d \
./example/src/lpc17xx_spi.d \
./example/src/main.d \
./example/src/sd.d \
./example/src/sysinit.d \
./example/src/uart2.d \
./example/src/uart3.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M3 -I"C:\Users\Eugenia\Desktop\TDII\lpc_chip_175x_6x\inc" -I"C:\Users\Eugenia\Desktop\TDII\lpc_board_nxp_lpcxpresso_1769\inc" -I"C:\Users\Eugenia\Desktop\TDII\freertos_blinky\example\inc" -I"C:\Users\Eugenia\Desktop\TDII\freertos_blinky\freertos\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


