ARCH   = aarch64

GCC   = aarch64-elf-gcc
LD    = aarch64-elf-ld
AS    = aarch64-elf-as
OC    = aarch64-elf-objcopy

BUILD = ./build
BIN   = ./bin

SRC_DIR      = ./src
ARCH_SRC_DIR = $(SRC_DIR)/$(ARCH)
INCLUDE_DIR = ./include

# C files that are arch-independent (still under ./src)
SRC_C  := $(wildcard $(SRC_DIR)/*.c)
OBJ_C  := $(patsubst $(SRC_DIR)/%.c, $(BUILD)/%.o, $(SRC_C))

# Arch-specific C sources
ARCH_SRC_C := $(wildcard $(ARCH_SRC_DIR)/*.c)
ARCH_OBJ_C := $(patsubst $(ARCH_SRC_DIR)/%.c, $(BUILD)/%.o, $(ARCH_SRC_C))

# Arch-specific assembly object
OBJ_S  := $(BUILD)/boot.o

OBJ    := $(OBJ_C) $(OBJ_S) $(ARCH_OBJ_C)

all: $(BIN)/kernel8.img

kernel8.bin: $(BIN)/kernel8.img

$(BIN)/kernel8.img: $(BUILD)/kernel.elf | $(BIN)
	$(OC) -O binary $(BUILD)/kernel.elf $(BIN)/kernel8.img

# NOTE: linker.ld is now under src/aarch64
$(BUILD)/kernel.elf: $(OBJ) $(ARCH_SRC_DIR)/linker.ld | $(BUILD)
	$(LD) -T $(ARCH_SRC_DIR)/linker.ld -o $(BUILD)/kernel.elf $(OBJ)

# boot.s is now under src/aarch64
$(BUILD)/boot.o: $(ARCH_SRC_DIR)/boot.s | $(BUILD)
	$(AS) $(ARCH_SRC_DIR)/boot.s -o $(BUILD)/boot.o

# C sources in ./src (arch-independent)
$(BUILD)/%.o: $(SRC_DIR)/%.c | $(BUILD)
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

