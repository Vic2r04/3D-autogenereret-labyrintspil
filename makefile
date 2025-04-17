TARGET_EXEC = main

SRC_DIR := ./src
BUILD_DIR := ./build

CC := gcc
CFLAGS := -g -Wall

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(TARGET_EXEC) : $(OBJS)
	$(CC) $(OBJS) -o $@

$(BUILD_DIR)/%.c.o : %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

