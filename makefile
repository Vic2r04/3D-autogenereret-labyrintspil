TARGET_EXEC = main
TARGET_TEST = testbin

SRC_DIR := ./src
BUILD_DIR := ./build
TEST_DIR := ./test

CC := gcc
CFLAGS := -Wall

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
	rm $(TEST_DIR)/$(TARGET_TEST)

.PHONY: test
test:
	$(CC) $(CPPFLAGS) -I/opt/homebrew/Cellar/cunit/2.1-3/include -L/opt/homebrew/Cellar/cunit/2.1-3/lib -lcunit -o ./test/$(TARGET_TEST) ./test/main.c $(patsubst ./src/main.c,,$(SRCS))
	@$(TEST_DIR)/$(TARGET_TEST)

-include $(DEPS)
