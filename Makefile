#Output files
EXECUTABLE=main.elf
BIN_IMAGE=main.bin

#======================================================================#
#Cross Compiler
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
ST_LIB = ./Libraries/STM32F4xx_StdPeriph_Driver
PROGRAM_ALG = ./math
PROGRAM_DIR = ./driver
PROGRAM_MOD = ./module
PROGRAM_SYS = ./system

#PLATFORM
COMPILER = GCC
PLAT = ARM_CM4F

FREERTOS = ./Libraries/freertos
FREERTOS_INC = $(FREERTOS)/include
FREERTOS_PTB = $(FREERTOS)/portable/$(COMPILER)/$(PLAT)	

TOOLCHAIN_PATH ?= /usr/local/csl/arm-2012.03/arm-none-eabi
C_LIB= $(TOOLCHAIN_PATH)/lib/thumb2

CFLAGS=-g -mlittle-endian -mthumb
CFLAGS+=-mcpu=cortex-m4
CFLAGS+=-mfpu=fpv4-sp-d16 -mfloat-abi=softfp
CFLAGS+=-ffreestanding -nostdlib
#CFLAGS+=-Wall -Wno-unused-function

#Flash
CFLAGS+=-Wl,-T,stm32.ld

CFLAGS+=-I./

#STM32 CMSIS
CFLAGS+= \
	-D STM32F40XX \
	-D USE_STDPERIPH_DRIVER \
	-D __FPU_PRESENT=1 \
	-D ARM_MATH_CM4
	# __FPU_USED=1
	#__CC_ARM
CFLAGS+=-I./Libraries/CMSIS
#STM32F4xx Peripherys including
CFLAGS+=-I./Libraries/STM32F4xx_StdPeriph_Driver/inc

#FreeRTOS including
CFLAGS+=-I$(FREERTOS_INC)
CFLAGS+=-I$(FREERTOS_PTB)

#Major programs including
CFLAGS+=-I./system
CFLAGS+=-I./math
CFLAGS+=-I./driver
CFLAGS+=-I./module

LDFLAGS += -lm -lc -lgcc

#STM32 CMSIS
SRC=$(wildcard ./Libraries/CMSIS/*.c)
#STM32F4xx Peripherys source code
#SRC+=$(wildcard ./Libraries/STM32F4xx_StdPeriph_Driver/src/*.c)
#SRC+= ./Libraries/CMSIS/system_stm32f4xx.c 
SRC+= $(ST_LIB)/src/misc.c \
	$(ST_LIB)/src/stm32f4xx_rcc.c \
	$(ST_LIB)/src/stm32f4xx_dma.c \
	$(ST_LIB)/src/stm32f4xx_flash.c \
	$(ST_LIB)/src/stm32f4xx_gpio.c \
	$(ST_LIB)/src/stm32f4xx_usart.c \
	$(ST_LIB)/src/stm32f4xx_tim.c\
	$(ST_LIB)/src/stm32f4xx_spi.c\
	$(ST_LIB)/src/stm32f4xx_i2c.c \

#FreeRTOS source file

SRC += 	$(FREERTOS)/tasks.c \
	$(FREERTOS)/queue.c \
	$(FREERTOS)/list.c \
	$(FREERTOS)/croutine.c \
	$(FREERTOS)/timers.c\
	$(FREERTOS)/portable/MemMang/heap_2.c \
	$(FREERTOS)/portable/GCC/ARM_CM4F/port.c\
	

#Major programs source code
SRC +=  $(wildcard $(PROGRAM_ALG)/algorithm_*.c) \
		$(wildcard $(PROGRAM_DIR)/stm32f4_*.c) \
		$(wildcard $(PROGRAM_MOD)/module_*.c) \
		$(wildcard $(PROGRAM_SYS)/QCopterFC_*.c) \
		$(wildcard $(PROGRAM_SYS)/QCopterFC.c) \
		Libraries/CMSIS/FastMathFunctions/arm_cos_f32.c \
		Libraries/CMSIS/FastMathFunctions/arm_sin_f32.c \
		sensor.c \
		pwm.c \
		usartIO.c \
		string-util.c \
		shell.c \
		main.c

INC_HEADERS =	$(wildcard $(PROGRAM_ALG)/*.h) \
		$(wildcard $(PROGRAM_DIR)/*.h) \
		$(wildcard $(PROGRAM_MOD)/*.h) \
		$(wildcard $(PROGRAM_SYS)/*.h) \
		$(wildcard *.h)



#STM32 startup file
STARTUP=./Libraries/CMSIS/startup_stm32f4xx.s
#======================================================================#

#Make all
all:$(BIN_IMAGE)

$(BIN_IMAGE):$(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@

STARTUP_OBJ = startup_stm32f4xx.o

$(STARTUP_OBJ): $(STARTUP) 
	$(CC) $(CFLAGS) $^ -c $(STARTUP)

$(EXECUTABLE):$(SRC) $(STARTUP_OBJ) ${INC_HEADERS}
	$(CC) $(CFLAGS) $(SRC) $(STARTUP_OBJ) -o $@ $(LDFLAGS)

#Make clean
clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)

#Make flash
flash:
	st-flash write $(BIN_IMAGE) 0x8000000
#======================================================================#
openocd: flash
	openocd -s /opt/openocd/share/openocd/scripts/
gdbauto:
	arm-none-eabi-gdbtui -x openocd_gdb.gdb

.PHONY:all clean flash openocd gdbauto
