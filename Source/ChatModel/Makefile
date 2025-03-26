.PHONY: all clean

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/ChatModel/%.o)

all: $(OBJS)

$(BUILD_DIR)/ChatModel/%.o: %.c
	@[ -d $(BUILD_DIR)/ChatModel ] || mkdir -p $(BUILD_DIR)/ChatModel
ifeq ($(build_libchat),true)
	$(CC) $(OBJ_CFLAGS) -c $< -o $@
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

clean:
	@rm -rf $(BUILD_DIR)/ChatModel