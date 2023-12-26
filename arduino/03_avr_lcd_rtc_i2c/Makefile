# Project name, it worck automatically after the enclosing directory.
TARGET = $(lastword $(subst /, ,$(CURDIR)))
BUILD_DIR:= build

#Target MCU:
MCU:= atmega328p

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

C_SOURCES:= \
serial.c \
adc.c \
lcd.c \
rtc.c \
twi.c \
main.c \

C_INCLUDE_DIRS:= \
.

COMP_OPT:= -Og
LINK_OPT:= -Os

TARGET_ARCH = -mmcu=$(MCU)

CFLAGS:= -g $(COMP_OPT) -std=c11 -Wall
LFLAGS:= $(LINK_OPT) $(TARGET_ARCH)

HFLAGS:= -j .text -j .data -O ihex
BFLAGS:= -j .text -j .data -O binary

PROGRAMMER_TYPE = arduino  # usbasp
PROGRAMMER_ARGS = -P COM13 -b 115200

vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS:= $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))

.PHONY: all flash_eeprom clean flash fuses show_fuses

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(addprefix -I, $(C_INCLUDE_DIRS)) $(CFLAGS) $(TARGET_ARCH) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile | $(BUILD_DIR)
	$(CC) $(OBJECTS) $(INCLUDE) $(TARGET_ARCH) $(LFLAGS) -o $@
	@echo
	@$(AVRSIZE) -C --mcu=$(MCU) $(BUILD_DIR)/$(TARGET).elf

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf | $(BUILD_DIR)
	@$(OBJCOPY) $(HFLAGS) $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf | $(BUILD_DIR)
	@$(OBJCOPY) $(BFLAGS) $< $@

$(BUILD_DIR):
	mkdir $@

clean:
	rm -r $(BUILD_DIR)

flash: $(BUILD_DIR)/$(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

flash_eeprom: $(TARGET).eeprom
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U eeprom:w:$<

# FUSE, don't use it if you don't understand what you are doing

## Mega 48, 88, 168, 328 default values
LFUSE = 0x62
HFUSE = 0xdf
EFUSE = 0x00

## Generic 
FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m 

fuses: 
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) \
	           $(PROGRAMMER_ARGS) $(FUSE_STRING)
show_fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nv	