################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cr_startup_lpc175x_6x.c \
../example/src/freertos_blinky.c \
../example/src/fs.c \
../example/src/sysinit.c \
../example/src/uart3.c 

OBJS += \
./example/src/cr_startup_lpc175x_6x.o \
./example/src/freertos_blinky.o \
./example/src/fs.o \
./example/src/sysinit.o \
./example/src/uart3.o 

C_DEPS += \
./example/src/cr_startup_lpc175x_6x.d \
./example/src/freertos_blinky.d \
./example/src/fs.d \
./example/src/sysinit.d \
./example/src/uart3.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M3 -I"C:\Users\Eugenia\Desktop\test_fs\lpc_chip_175x_6x\inc" -I"C:\Users\Eugenia\Desktop\test_fs\lpc_board_nxp_lpcxpresso_1769\inc" -I"C:\Users\Eugenia\Desktop\test_fs\freertos_blinky\example\inc" -I"C:\Users\Eugenia\Desktop\test_fs\freertos_blinky\freertos\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


