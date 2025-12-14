ARCH   = aarch64

GCC   = $(ARCH)-elf-gcc
LD    = $(ARCH)-elf-ld
AS    = $(ARCH)-elf-as
OC    = $(ARCH)-elf-objcopy

BUILD = ./build
BIN   = ./bin

SRC_DIR      = ./src
KERNEL_DIR   = $(SRC_DIR)/kernel
ARCH_SRC_DIR = $(SRC_DIR)/arch/$(ARCH)
INCLUDE_DIR = ./include

SRC_C  := $(wildcard $(SRC_DIR)/kernel/*.c)
OBJ_C  := $(patsubst $(SRC_DIR)/kernel/%.c, $(BUILD)/%.o, $(SRC_C))

ARCH_SRC_C := $(wildcard $(ARCH_SRC_DIR)/*.c)
ARCH_OBJ_C := $(patsubst $(ARCH_SRC_DIR)/%.c, $(BUILD)/%.o, $(ARCH_SRC_C))

OBJ_S  := $(BUILD)/boot.o

OBJ    := $(OBJ_C) $(OBJ_S) $(ARCH_OBJ_C) 

all: $(BIN)/kernel8.img

kernel8.bin: $(BIN)/kernel8.img

$(BIN)/kernel8.img: $(BUILD)/kernel.elf | $(BIN)
	$(OC) -O binary $(BUILD)/kernel.elf $(BIN)/kernel8.img

$(BUILD)/kernel.elf: $(OBJ) $(ARCH_SRC_DIR)/linker.ld | $(BUILD)
	$(LD) -T $(ARCH_SRC_DIR)/linker.ld -o $(BUILD)/kernel.elf $(OBJ)

$(BUILD)/boot.o: $(ARCH_SRC_DIR)/boot.s | $(BUILD)
	$(AS) $(ARCH_SRC_DIR)/boot.s -o $(BUILD)/boot.o

$(BUILD)/%.o: $(KERNEL_DIR)/%.c | $(BUILD)
	$(GCC) -I $(INCLUDE_DIR) -c $< -o $@

$(BUILD)/%.o: $(ARCH_SRC_DIR)/%.c | $(BUILD)
	$(GCC) -I $(INCLUDE_DIR) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

$(BIN):
	mkdir -p $(BIN)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD)

