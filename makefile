TARGET_EXEC = main

SRC_DIR := ./src
BUILD_DIR := ./build

CC := gcc
CFLAGS := -g -Wall

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP

$(BUILD_DIR)/$(TARGET_EXEC) : $(OBJS)
	$(CC) $(OBJS) -o $@

$(BUILD_DIR)/%.c.o : %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ 

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

include $(DEPS)
