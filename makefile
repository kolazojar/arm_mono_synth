# @file makefile
# @author Matt Krol

OBJ_DIR=obj
SRC_DIR=src
INC_DIR=inc
BIN_DIR=bin

TARGET=runme

SOURCES:=$(wildcard $(SRC_DIR)/*)
OBJECTS:=$(patsubst $(SRC_DIR)%.c, $(OBJ_DIR)%.o, $(SOURCES))
OBJECTS:=$(patsubst $(SRC_DIR)%.s, $(OBJ_DIR)%.o, $(OBJECTS))

LD_SCRIPT=STM32F407VGTx_FLASH.ld

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
STFLASH=st-flash

FLASH_ADDR=0x08000000

CFLAGS=-g -Wall -I$(INC_DIR)
LDFLAGS=-Wl,-Map=$(OBJ_DIR)/$(TARGET).map,-T$(LD_SCRIPT),-lnosys,-lm,-lc,-lgcc
CPU=-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPU) $(CFLAGS) -c -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	$(CC) $(CPU) $(CFLAGS) -c -o $@ $^

all: $(OBJ_DIR) $(BIN_DIR) $(INC_DIR)/lut.h $(BIN_DIR)/$(TARGET).bin $(BIN_DIR)/$(TARGET).elf

$(BIN_DIR)/$(TARGET).bin: $(BIN_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $^ $@

$(BIN_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(CPU) -o $@ $^ $(LDFLAGS)

$(INC_DIR)/lut.h:
	./scripts/lut.py > inc/lut.h

$(BIN_DIR):
	mkdir $(BIN_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

flash: all
	$(STFLASH) write $(BIN_DIR)/$(TARGET).bin $(FLASH_ADDR)

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	rm -f $(INC_DIR)/lut.h
