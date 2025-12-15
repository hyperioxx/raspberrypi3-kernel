ARCH   ?= aarch64

GCC   = $(ARCH)-elf-gcc
LD    = $(ARCH)-elf-ld
AS    = $(ARCH)-elf-as
OC    = $(ARCH)-elf-objcopy

BUILD = build
BIN   = bin

SRC_DIR      = src
KERNEL_DIR   = $(SRC_DIR)/kernel
ARCH_SRC_DIR = $(SRC_DIR)/arch/$(ARCH)
DRIVERS_DIR   = $(SRC_DIR)/drivers
INCLUDE_DIR = include

ALL_SRC_C :=  $(wildcard $(KERNEL_DIR)/*.c) \
  $(wildcard $(ARCH_SRC_DIR)/*.c) \
  $(wildcard $(DRIVERS_DIR)/*/*.c)

OBJ_S := $(BUILD)/arch/$(ARCH)/boot.o

OBJ := $(patsubst src/%.c,build/%.o,$(ALL_SRC_C)) $(OBJ_S)

all: $(BIN)/kernel8.img

kernel8.bin: $(BIN)/kernel8.img

$(BIN)/kernel8.img: $(BUILD)/kernel.elf | $(BIN)
	$(OC) -O binary $(BUILD)/kernel.elf $(BIN)/kernel8.img

$(BUILD)/kernel.elf: $(OBJ) $(ARCH_SRC_DIR)/linker.ld | $(BUILD)
	$(LD) -T $(ARCH_SRC_DIR)/linker.ld -o $(BUILD)/kernel.elf $(OBJ)

$(BUILD)/%.o: src/%.s
	mkdir -p $(dir $@)
	$(AS) $< -o $@

$(BUILD)/%.o : src/%.c
	mkdir -p $(dir $@)
	$(GCC) -I include -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

$(BIN):
	mkdir -p $(BIN)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD)

.PHONY: run
run:
	qemu-system-aarch64 -M raspi3b -kernel bin/kernel8.img -serial stdio -display none -dtb qemu/bcm2710-rpi-3-b.dtb
