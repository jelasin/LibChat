.PHONY: all clean

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/MessageInput/%.o)

all: $(OBJS)

$(BUILD_DIR)/MessageInput/%.o: %.c
	@[ -d $(BUILD_DIR)/MessageInput ] || mkdir -p $(BUILD_DIR)/MessageInput
ifeq ($(build_libchat),true)
	$(CC) $(OBJ_CFLAGS) -c $< -o $@
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

clean:
	@rm -rf $(BUILD_DIR)/MessageInput