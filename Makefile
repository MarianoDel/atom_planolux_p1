#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!
#
##############################################################################################
#
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
#
# Included originally in the yagarto projects. Original Author : Michael Fischer
# Modified to suit our purposes by Hussam Al-Hertani
# Use at your own risk!!!!!
##############################################################################################
# Start of default section
#
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S
MCU  = cortex-m0

# List all default C defines here, like -D_DEBUG=1
#DDEFS = -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DUSE_STM3210E_EVAL
#DDEFS = -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DUSE_STM3210E_EVAL -D__GNUC__
# List all default ASM defines here, like -D_DEBUG=1
DADEFS =

# List all default directories to look for include files here
DINCDIR = ./src

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS =

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

#
# Define project name and Ram = 0/Flash = 1 mode here
PROJECT        = Template_F050

# List all user C define here, like -D_DEBUG=1
UDEFS =

# Define ASM defines here
UADEFS =

# List C source files here
LIBSDIR    = ../STM32F0xx_StdPeriph_Lib_V1.3.1/Libraries/STM32F0xx_StdPeriph_Driver
CORELIBDIR = ./cmsis_core
DEVDIR  =	./cmsis_boot


STMSPDDIR    = ./stm_lib

STMSPSRCDDIR = $(LIBSDIR)/src
STMSPINCDDIR = $(LIBSDIR)/inc
#STMSPSRCDDIR = $(STMSPDDIR)/src
#STMSPINCDDIR = $(STMSPDDIR)/inc

#DISCOVERY    = ../STM32F0-Discovery_FW_V1.0.0/Utilities/STM32F0-Discovery

LINKER = ./cmsis_boot/startup

SRC  = ./src/main.c
SRC += $(DEVDIR)/system_stm32f0xx.c
SRC += $(DEVDIR)/syscalls/syscalls.c
## Libs de ST V1.3 o V1.5
#SRC += $(STMSPSRCDDIR)/stm32f0xx_adc.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_can.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_cec.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_comp.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_crc.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_crs.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_dac.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_dbgmcu.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_dma.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_exti.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_flash.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_gpio.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_i2c.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_iwdg.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_misc.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_pwr.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_rcc.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_rtc.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_spi.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_syscfg.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_tim.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_usart.c
#SRC += $(STMSPSRCDDIR)/stm32f0xx_wwdg.c
SRC += ./src/it.c
SRC += ./src/gpio.c
SRC += ./src/tim.c
SRC += ./src/spi.c
SRC += ./src/adc.c
SRC += ./src/uart.c
SRC += ./src/flash_program.c
SRC += ./src/programs_functions.c
## System Support
#SRC += ./cmsis_boot/system_stm32f0xx.c
#SRC += $(DISCOVERY)/stm32f0_discovery.c
#SRC += ./src/Sim900.c
## Core Support
#SRC += ./startup_src/syscalls.c
SRC += $(CORELIBDIR)/core_cm0.c
## used parts of the STM-Library

# List ASM source files here
ASRC = ./cmsis_boot/startup/startup_stm32f0xx.s

# List all user directories here
UINCDIR = $(DEVDIR) \
          $(CORELIBDIR) \
          $(STMSPINCDDIR) \
          $(DISCOVERY)    \
          ./inc  \
          ./cmsis_boot
# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

# Define optimisation level here
# O1 optimiza size no significativo
# O2 size mas fuerte
# Os (size mas fuerte que O2)
# O3 el mas fuerte, seguro despues no corre
# O0 (no optimiza casi nada, mejor para debug)
OPT = -O0

#
# End of user defines
##############################################################################################
#
# Define linker script file here
#
LDSCRIPT = $(LINKER)/stm32_flash.ld
FULL_PRJ = $(PROJECT)_rom

INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))

ADEFS   = $(DADEFS) $(UADEFS)

LIBS    = $(DLIBS) $(ULIBS)
MCFLAGS = -mcpu=$(MCU)

ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb  -Wa,-amhls=$(<:.s=.lst) $(ADEFS)

# SIN INFO DEL DEBUGGER
#CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb   -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)

# CON INFO PARA DEBUGGER
#CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)

# CON INFO PARA DEBUGGER + STRIP CODE
CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fdata-sections -ffunction-sections -fverbose-asm -Wa,-ahlms=$(<:.c=.lst)

# SIN DEAD CODE, hace el STRIP
LDFLAGS = $(MCFLAGS) -mthumb --specs=nano.specs -Wl,--gc-sections -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)
# CON DEAD CODE
#LDFLAGS = $(MCFLAGS) -mthumb --specs=nano.specs -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)
#LDFLAGS = $(MCFLAGS) -mthumb -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)

#
# OPENOCD Command Options
#
OCDCMN = -c "program $(FULL_PRJ).bin verify reset exit"
#OCDCMN = -c "flash probe 0"
# OCDCMN += -c "stm32f1x mass_erase 0"
# OCDCMN += -c "flash write_bank 0 $(FULL_PRJ).bin 0"
# OCDCMN += -c "reset run"

#
# makefile rules
#

assemblersources = $(ASRC)
sources = $(SRC)
OBJS  = $(SRC:.c=.o) $(ASRC:.s=.o)

objects = $(sources:.c=.o)
assobjects = $(assemblersources:.s=.o)

all: $(objects) $(assobjects) $(FULL_PRJ).elf $(FULL_PRJ).bin
	arm-none-eabi-size $(FULL_PRJ).elf

$(objects): %.o: %.c
	$(CC) -c $(CPFLAGS) -I. $(INCDIR) $< -o $@

$(assobjects): %.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

%elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(HEX) $< $@

%bin: %elf
	$(BIN)  $< $@

flash:
	sudo openocd -f stm32f0_flash.cfg

gdb:
	sudo openocd -f stm32f0_gdb.cfg

reset:
	sudo openocd -f stm32f0_reset.cfg

clean:
	rm -f $(OBJS)
	rm -f $(FULL_PRJ).elf
	rm -f $(FULL_PRJ).map
	rm -f $(FULL_PRJ).hex
	rm -f $(FULL_PRJ).bin
#	rm $(SRC:.c=.c.bak)
	rm -f $(SRC:.c=.lst)
#   rm $(ASRC:.s=.s.bak)
	rm -f $(ASRC:.s=.lst)

# *** EOF ***
