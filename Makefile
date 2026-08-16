CC = riscv64-unknown-elf-gcc
CFLAGS = -march=rv64g -mabi=lp64 -mcmodel=medany -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T linker.ld -nostdlib

PROJECT = kernel.elf
BUILD_DIR = build
OBJECTS = entry.o main.o

_BUILDDIR_OBJS = $(addprefix $(BUILD_DIR)/,$(OBJECTS))

.PHONY: all clean run
all: $(BUILD_DIR)/$(PROJECT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(PROJECT): $(_BUILDDIR_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/entry.o: entry.S | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c entry.S -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BUILD_DIR)/$(PROJECT)
	qemu-system-riscv64 -machine virt -nographic -kernel $<

clean:
	rm -rf $(BUILD_DIR)/*
