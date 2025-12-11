GCC   = aarch64-elf-gcc
LD    = aarch64-elf-ld
AS    = aarch64-elf-as
OC    = aarch64-elf-objcopy

BUILD = ./build
BIN   = ./bin

SRC_C  := $(wildcard ./src/*.c)
OBJ_C  := $(patsubst ./src/%.c, $(BUILD)/%.o, $(SRC_C))
OBJ_S  := $(BUILD)/boot.o
OBJ    := $(OBJ_C) $(OBJ_S)

all: $(BIN)/kernel8.img

kernel8.bin: $(BIN)/kernel8.img

$(BIN)/kernel8.img: $(BUILD)/kernel.elf | $(BIN)
	$(OC) -O binary $(BUILD)/kernel.elf $(BIN)/kernel8.img

$(BUILD)/kernel.elf: $(OBJ) ./src/linker.ld | $(BUILD)
	$(LD) -T ./src/linker.ld -o $(BUILD)/kernel.elf $(OBJ)

$(BUILD)/boot.o: ./src/boot.s | $(BUILD)
	$(AS) ./src/boot.s -o $(BUILD)/boot.o

$(BUILD)/%.o: ./src/%.c | $(BUILD)
	$(GCC) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

$(BIN):
	mkdir -p $(BIN)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD)

