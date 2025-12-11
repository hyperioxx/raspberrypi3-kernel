# Select architecture: aarch64 or x86_64
ARCH ?= aarch64

SUPPORTED_ARCHES := aarch64 x86_64

# Simple guard: error if ARCH is unsupported
ifeq (,$(filter $(ARCH),$(SUPPORTED_ARCHES)))
$(error Unsupported ARCH '$(ARCH)'. Supported: $(SUPPORTED_ARCHES))
endif

# Per-arch cross compiler prefixes
CROSS_COMPILE_aarch64 = aarch64-elf-
CROSS_COMPILE_x86_64  = x86_64-elf-

CROSS_COMPILE = $(CROSS_COMPILE_$(ARCH))

GCC = $(CROSS_COMPILE)gcc
LD  = $(CROSS_COMPILE)ld
AS  = $(CROSS_COMPILE)as
OC  = $(CROSS_COMPILE)objcopy

# Per-arch build/bin dirs
BUILD = ./build/$(ARCH)
BIN   = ./bin/$(ARCH)

SRC_DIR      = ./src
ARCH_SRC_DIR = $(SRC_DIR)/$(ARCH)

# Common C sources (in ./src)
SRC_C  := $(wildcard $(SRC_DIR)/*.c)
OBJ_C  := $(patsubst $(SRC_DIR)/%.c, $(BUILD)/%.o, $(SRC_C))

# Arch-specific asm object
OBJ_S  := $(BUILD)/boot.o

OBJ    := $(OBJ_C) $(OBJ_S)

# Top-level target
all: $(BIN)/kernel-$(ARCH).bin

# Convert ELF to flat binary
$(BIN)/kernel-$(ARCH).bin: $(BUILD)/kernel.elf | $(BIN)
	$(OC) -O binary $(BUILD)/kernel.elf $@

# Link kernel
$(BUILD)/kernel.elf: $(OBJ) $(ARCH_SRC_DIR)/linker.ld | $(BUILD)
	$(LD) -T $(ARCH_SRC_DIR)/linker.ld -o $@ $(OBJ)

# Assemble boot.s
$(BUILD)/boot.o: $(ARCH_SRC_DIR)/boot.s | $(BUILD)
	$(AS) $< -o $@

# Compile common C sources
$(BUILD)/%.o: $(SRC_DIR)/%.c | $(BUILD)
	$(GCC) -c $< -o $@

# Directories
$(BUILD):
	mkdir -p $(BUILD)

$(BIN):
	mkdir -p $(BIN)

.PHONY: clean
clean:
	rm -rf ./build ./bin

